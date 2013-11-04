// Name:  a2300_impl.cpp
//
// Copyright 2013 Loctronix Corporation
// http://www.loctronix.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "a2300_impl.hpp"
#include <A2300/A2300_Defs.h>
#include <A2300/A2300InterfaceDefs.h>
#include <A2300/libusb_ext.h>

#include <uhd/utils/log.hpp>
#include <uhd/transport/usb_control.hpp>
#include <uhd/utils/msg.hpp>
#include <uhd/exception.hpp>
#include <uhd/utils/static.hpp>
#include <uhd/utils/images.hpp>
#include <uhd/utils/safe_call.hpp>
#include <uhd/usrp/dboard_eeprom.hpp>
#include <uhd/types/ranges.hpp>

#include "DciProperty.h"
#include <boost/format.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>
//#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp>
#include <cstdio>
#include <ctime>

// DCI Messages.
#define PROPERTY_ID_GAIN  (0x1)

using namespace uhd;
using namespace uhd::usrp;
using namespace uhd::transport;

// A2300 USB information.
const boost::uint16_t A2300_VENDOR_ID  = 0xffff;
const boost::uint16_t A2300_PRODUCT_ID = 0x00ff;
const boost::uint16_t A2300_ADDR_UNDEF = 0xffff;

const boost::uint16_t A2300_DciIdc0_EpIn  = 0x82;
const boost::uint16_t A2300_DciIdc0_EpOut = 0x1;
const boost::uint16_t A2300_DciIdc1_EpIn  = 0x84;
const boost::uint16_t A2300_DciIdc1_EpOut = 0x3;

static const boost::posix_time::milliseconds REENUMERATION_TIMEOUT_MS(3000);


/***********************************************************************
 * Discovery
 **********************************************************************/
static device_addrs_t a2300_find(const device_addr_t &hint)
{
    device_addrs_t a2300_addrs;

    //return an empty list of addresses when type is set to non-a2300
    if( hint.has_key("type") and hint["type"] != "a2300") return a2300_addrs;
	
    //Return an empty list of addresses when an address is specified,
    //since an address is intended for a different, non-USB, device.
    if (hint.has_key("addr")) {
		printf("You can't use \"addr\" on a USB device - bye!\n");
		return a2300_addrs;    // Return empty list.
	}
    
	// Set defaults here.
    unsigned int vid = A2300_VENDOR_ID;
    unsigned int pid = A2300_PRODUCT_ID;

    if(hint.has_key("vid") && hint.has_key("pid")) {
        sscanf(hint.get("vid").c_str(), "%x", &vid);
        sscanf(hint.get("pid").c_str(), "%x", &pid);
    }

    // Important note:
    // The get device list calls are nested inside the for loop.
    // This allows the usb guts to deconstruct when not in use,
    // so that re-enumeration after fw load can occur successfully.
    // This requirement is a courtesy of libusb1.0 on windows.

    //get descriptors again with serial number, but using the initialized VID/PID now since we have firmware
    const int buffSize = 256;
    char buff[buffSize];
    vid = A2300_VENDOR_ID;
    pid = A2300_PRODUCT_ID;

	//search for the device until found or timeout
	int usbAddress;

	libusb_context *ctx = NULL;
	int rc = libusb_init(&ctx);
	if( rc == 0x0)
	{
		libusb_set_debug(ctx, 3);
		// Get a list of USB devices
		libusb_device** devList;
		int ctDevices = libusb_get_device_list(ctx, &devList);
		for (int i = 0; i < ctDevices; i++)
		{
			libusb_device *pDevice = devList[i];

			struct libusb_device_descriptor desc;
			int retval = libusb_get_device_descriptor( pDevice, &desc );
			if (retval < 0) throw std::runtime_error("libusb_get_device_descriptor() failed: " + std::string(libusb_error_name(retval)));

			if( desc.idVendor == vid && desc.idProduct == pid )
			{
				device_addr_t new_addr;
				new_addr["type"] = "a2300";
				new_addr["name"] = "ASR-2300";
				new_addr["mcr"]  = "32000000.0";  // CJC: Master Clock Rate used by some functions.

				// Add a serial string of information (you need the open to do this).
				libusb_device_handle *handle;
				rc=libusb_open(pDevice, &handle);
				if( rc == 0x0)
				{
					int iSerialNumber = desc.iSerialNumber;
					rc = libusb_get_string_descriptor_ascii(handle, iSerialNumber, (byte *)buff, buffSize);
					if( rc >= 0)
						new_addr["serial"] = std::string((char*)buff);
					else
						new_addr["serial"] = "1234";
					libusb_close(handle);
				}

				// Get the address of the device on the bus.
				// Note: "address" should only be used to support TCP/IP interfaces
				usbAddress = libusb_get_device_address(pDevice);
				sprintf((char *)buff, "%d", usbAddress);
				new_addr["addr"] = std::string(buff);
				printf("new_addr = usbAddr:%d [buff:%s]\n", usbAddress, buff);

				// Save
				a2300_addrs.push_back( new_addr );
			}
		}
		libusb_free_device_list(devList, 1);  // Free the list, unref the devices in it.
		libusb_exit(ctx);					   // close session.
	}
    return a2300_addrs;
}

/***********************************************************************
 * Make
 **********************************************************************/
static device::sptr a2300_make(const device_addr_t &device_addr)
{
    return device::sptr( new a2300_impl(device_addr) );
}

UHD_STATIC_BLOCK(register_a2300_device){
    device::register_device(&a2300_find, &a2300_make);
}

/***********************************************************************
 * Structors
 **********************************************************************/
a2300_impl::a2300_impl(const device_addr_t &device_addr)
: _is_setup(false)
{
    UHD_MSG(status) << "Opening the A2300 device..." << std::endl;
	m_tree = property_tree::make();
    const fs_path mb_path = "/mboards/0";
	
    //try to match the given device address with something on the USB bus
    std::vector<usb_device_handle::sptr> device_list =
        usb_device_handle::get_device_list(A2300_VENDOR_ID, A2300_PRODUCT_ID);

    //locate the matching handle in the device list
    usb_device_handle::sptr handle;
    BOOST_FOREACH(usb_device_handle::sptr dev_handle, device_list) {
        if (dev_handle->get_serial() == device_addr["serial"]){
            handle = dev_handle;
            break;
        }
    }
	// Temporary: Default to the first handle found.
    handle = device_list[0];
    UHD_ASSERT_THROW(handle.get() != NULL); //better be found

    //create control objects
    // usb_control::sptr control = usb_control::make(handle, 0);
    // _iface = b200_iface::make(control);
    // this->check_fw_compat(); //check after making

	 ////////////////////////////////////////////////////////////////////
	 // setup the mboard eeprom
	 ////////////////////////////////////////////////////////////////////
	 const mboard_eeprom_t mb_eeprom; // (*_iface, "B200");
	 m_tree->create<mboard_eeprom_t>(mb_path / "eeprom")
			.set(mb_eeprom)
			.subscribe(boost::bind(&a2300_impl::set_mb_eeprom, this, _1));
			//.publish(boost::bind(&a2300_impl::get_mb_eeprom, this));

    ////////////////////////////////////////////////////////////////////
    // Create control transport
    ////////////////////////////////////////////////////////////////////
    // boost::uint8_t usb_speed = _iface->get_usb_speed();
    // UHD_MSG(status) << "Operating over USB " << (int) usb_speed << "." << std::endl;
    // const std::string min_frame_size = (usb_speed == 3) ? "1024" : "512";

    ////////////////////////////////////////////////////////////////////
    // Create DCI Interface Objects to access ASR-2300 Command and Control Interface
    ////////////////////////////////////////////////////////////////////
    device_addr_t dci0_hints( device_addr);
    dci0_hints["num_recv_frames"] = "3";
    dci0_hints["recv_frame_size"] = "512";
    dci0_hints["num_send_frames"] = "2";
    dci0_hints["send_frame_size"] = "512";
    _idc0_transport = usb_zero_copy::make(
    		handle,     // identifier
			0, A2300_DciIdc0_EpIn,    // IN interface, endpoint
			0, A2300_DciIdc0_EpOut,    // OUT interface, endpoint
			dci0_hints);  // param hints
    while (_idc0_transport->get_recv_buff(0.0)){} //flush ctrl xport

    ////////////////////////////////////////////////////////////////////
    // Async task structure
    ////////////////////////////////////////////////////////////////////
    //_async_task_data.reset(new AsyncTaskData());
    //_async_task_data->async_md.reset(new async_md_type(1000/*messages deep*/));
    //_async_task = uhd::task::make(boost::bind(&b200_impl::handle_async_task, this, _ctrl_transport, _async_task_data));
    //Bind the DCI command and control interface to the transport.
    _dci0_ctrl = A2300_iface::make(	_idc0_transport,  0);

    ////////////////////////////////////////////////////////////////////
    // Initialize the properties tree
    ////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////
    // create user-defined control objects
    ////////////////////////////////////////////////////////////////////

	std::string product_name = "ASR-2300";

	m_tree->create<std::string>("/name").set("A2300 USB Device");
	m_tree->create<std::string>(mb_path / "name").set(product_name);
	m_tree->create<std::string>(mb_path / "codename").set("Woodinville");

	m_tree->create<std::string>(mb_path / "fw_version").publish(boost::bind(&a2300_impl::get_fw_version, this));
	m_tree->create<std::string>(mb_path / "fpga_version").publish(boost::bind(&a2300_impl::get_fpga_version, this));

	//Create Branch for Sensors.
	m_tree->create<int>("/sensors"); //phony property so this dir exists

	////////////////////////////////////////////////////////////////////
	// Create data transport
	// This happens after FPGA ctrl instantiated so any junk that might
	// be in the FPGAs buffers doesn't get pulled into the transport
	// before being cleared.
	////////////////////////////////////////////////////////////////////

	device_addr_t data_xport_args;
	data_xport_args["recv_frame_size"] = device_addr.get("recv_frame_size", "8192");
	data_xport_args["num_recv_frames"] = device_addr.get("num_recv_frames", "16");
	data_xport_args["send_frame_size"] = device_addr.get("send_frame_size", "8192");
	data_xport_args["num_send_frames"] = device_addr.get("num_send_frames", "16");

	//Bind Bulk Data Interfaces to WcaPorts 0 Read and Write.
	//This is NOT dci.
	//Note: A failure here will output a message of the form: "usb Rx8 submit failed"
	_data_transport = usb_zero_copy::make(
			handle,        // identifier
			0, 0x88,       // IN interface,  endpoint
			0, 0x08,       // OUT interface, endpoint
			data_xport_args);  // param hints

	while (_data_transport->get_recv_buff(0.0)){} //flush data xport

	_soft_time_ctrl = soft_time_ctrl::make(
			boost::bind(&a2300_impl::rx_stream_on_off, this, _1));

	////////////////////////////////////////////////////////////////////
	// create codec control objects
	////////////////////////////////////////////////////////////////////
	{
		const fs_path codec_path = mb_path / ("rx_codecs") / "A";
		m_tree->create<std::string>(codec_path / "name").set(product_name+" RX dual ADC");
		m_tree->create<int>(codec_path / "gains"); //empty cuz gains are in frontend
	}
	{
		const fs_path codec_path = mb_path / ("tx_codecs") / "A";
		m_tree->create<std::string>(codec_path / "name").set(product_name+" TX dual DAC");
		m_tree->create<int>(codec_path / "gains"); //empty cuz gains are in frontend
	}

	////////////////////////////////////////////////////////////////////
	// create clock control objects
	////////////////////////////////////////////////////////////////////
	m_tree->create<double>(mb_path / "tick_rate")
	   .coerce(boost::bind(&a2300_impl::set_tick_rate, this, _1))
	   .subscribe(boost::bind(&a2300_impl::set_tick_rate, this, _1))
	   .publish(boost::bind(&a2300_impl::get_tick_rate, this))
	   .subscribe(boost::bind(&a2300_impl::update_tick_rate, this, _1));
	m_tree->create<time_spec_t>(mb_path / "time" / "cmd");

	////////////////////////////////////////////////////////////////////
	// and do the misc mboard sensors
	////////////////////////////////////////////////////////////////////
	m_tree->create<sensor_value_t>(mb_path / "sensors" / "ref_locked")
			.set( sensor_value_t("ref", true, "locked", "unlocked"));

	////////////////////////////////////////////////////////////////////
	// create frontend mapping
	////////////////////////////////////////////////////////////////////
	m_tree->create<subdev_spec_t>(mb_path / "rx_subdev_spec")
		   .set(subdev_spec_t());
		   //.subscribe(boost::bind(&a2300_impl::update_rx_subdev_spec, this, _1));
	m_tree->create<subdev_spec_t>(mb_path / "tx_subdev_spec")
			.set(subdev_spec_t());
		   //.subscribe(boost::bind(&a2300_impl::update_tx_subdev_spec, this, _1));

	////////////////////////////////////////////////////////////////////
	// setup radio control
	////////////////////////////////////////////////////////////////////
	UHD_MSG(status) << "Initialize Radio Control..." << std::endl;

	//Define two Peripherals covering the 4 RF Inputs and 4 DSP Channels.
	const RadioPeripheral::InitArgs _iargs[] =
	{
		{0, 0,0,0,  WCACOMP_DSP_DUC0, WCACOMP_DSP_DDC0, WCACOMP_RF0},
		{1, 1,1,1,  WCACOMP_DSP_DUC1, WCACOMP_DSP_DDC1, WCACOMP_RF1}
	};

	m_perifs.resize(2);
	for( size_t i = 0; i < m_perifs.size(); i++)
	{
		m_perifs[i].Initialize( _iargs[i], mb_path,_dci0_ctrl,	m_tree);
	}


	#if defined(FUTURE)
		//now test each radio module's connection to the codec interface
		_codec_ctrl->data_port_loopback(true);
		BOOST_FOREACH(radio_perifs_t &perif, m_perifs)
		{
		  this->codec_loopback_self_test(perif.ctrl);
		}
		_codec_ctrl->data_port_loopback(false);
	#endif
	////////////////////////////////////////////////////////////////////
	// create time and clock control objects
	////////////////////////////////////////////////////////////////////
	//register time now and pps onto available radio cores
	m_tree->create<time_spec_t>(mb_path / "time" / "now");
		//.publish(boost::bind(&time_core_3000::get_time_now, m_perifs[0].time64));
	m_tree->create<time_spec_t>(mb_path / "time" / "pps");
		//.publish(boost::bind(&time_core_3000::get_time_last_pps, m_perifs[0].time64));
	for (size_t i = 0; i < m_perifs.size(); i++)
	{
		m_tree->access<time_spec_t>(mb_path / "time" / "now");
		//    .subscribe(boost::bind(&time_core_3000::set_time_now, m_perifs[i].time64, _1));
		m_tree->access<time_spec_t>(mb_path / "time" / "pps");
		//    .subscribe(boost::bind(&time_core_3000::set_time_next_pps, m_perifs[i].time64, _1));
	}

	//setup time source props
	m_tree->create<std::string>(mb_path / "time_source" / "value")
		.subscribe(boost::bind(&a2300_impl::update_time_source, this, _1));

	static const std::vector<std::string> time_sources = boost::assign::list_of("none")("external")("gpsdo");
	m_tree->create<std::vector<std::string> >(mb_path / "time_source" / "options").set(time_sources);

	//setup reference source props
	m_tree->create<std::string>(mb_path / "clock_source" / "value")
		.subscribe(boost::bind(&a2300_impl::update_clock_source, this, _1));

	static const std::vector<std::string> clock_sources = boost::assign::list_of("internal")("external")("gpsdo");
	m_tree->create<std::vector<std::string> >(mb_path / "clock_source" / "options").set(clock_sources);

	////////////////////////////////////////////////////////////////////
	// dboard eeproms but not really
	////////////////////////////////////////////////////////////////////
	dboard_eeprom_t db_eeprom;
	m_tree->create<dboard_eeprom_t>(mb_path / "dboards" / "A" / "rx_eeprom").set(db_eeprom);
	m_tree->create<dboard_eeprom_t>(mb_path / "dboards" / "A" / "tx_eeprom").set(db_eeprom);
	m_tree->create<dboard_eeprom_t>(mb_path / "dboards" / "A" / "gdb_eeprom").set(db_eeprom);


	////////////////////////////////////////////////////////////////////
	// do some post-init tasks
	////////////////////////////////////////////////////////////////////

	//init the clock rate to something reasonable
	m_tree->access<double>(mb_path / "tick_rate").set(A2300_DEFAULT_TICK_RATE);
	//      device_addr.cast<double>("master_clock_rate", A2300_DEFAULT_TICK_RATE));


	//_tree->access<subdev_spec_t>(mb_path / "rx_subdev_spec"); //.set(rx_spec);
	//_tree->access<subdev_spec_t>(mb_path / "tx_subdev_spec"); //.set(tx_spec);

	//init to internal clock and time source
	m_tree->access<std::string>(mb_path / "clock_source/value").set("internal");
	m_tree->access<std::string>(mb_path / "time_source/value").set("none");


    ////////////////////////////////////////////////////////////////////
    // create clock control objects
    ////////////////////////////////////////////////////////////////////
    _master_clock_rate = A2300_SYSTEM_CLOCK;
    if (device_addr.has_key("mcr")){
        try{
            _master_clock_rate = boost::lexical_cast<double>(device_addr["mcr"]);
        }
        catch(const std::exception &e){
            UHD_MSG(error) << "Error parsing FPGA clock rate from device address: " << e.what() << std::endl;
        }
    }

    //initialize io handling
    this->io_init();

    _is_setup = true;
}

a2300_impl::~a2300_impl(void)
{
    _soft_time_ctrl->stop(); //stops cmd task before proceeding
    _io_impl.reset(); //stops vandal before other stuff gets deconstructed
}

//device properties interface
uhd::property_tree::sptr a2300_impl::get_tree(void) const
{
    return m_tree;
}

/*
 * More Device Discovery
 */
void * a2300_impl::FindDevice(uint16 vid, uint16 pid, int usbAddress)
{
	libusb_device *pDevice = 0;

	libusb_context *ctx = NULL;
	int rc = libusb_init(&ctx);
	if( rc == 0x0)
	{
		libusb_set_debug(ctx, 3);
		// Get a list of USB devices
		libusb_device** devList;
		int ctDevices = libusb_get_device_list(ctx, &devList);
		for (int i = 0; i < ctDevices; i++)
		{
			pDevice = devList[i];

			struct libusb_device_descriptor desc;
			int retval = libusb_get_device_descriptor( pDevice, &desc );
			if (retval < 0) throw std::runtime_error("libusb_get_device_descriptor() failed: " + std::string(libusb_error_name(retval)));

			if( desc.idVendor == vid && desc.idProduct == pid )
			{
				break;
			}
		}

		libusb_free_device_list(devList, 1);  // Free the list, unref the devices in it.
		libusb_exit(ctx);					   // close session.
	}
	return( pDevice );
}


/***********************************************************************
 * loopback tests
 **********************************************************************/
// Not Used.





/***********************************************************************
 * Reference time and clock
 **********************************************************************/

void a2300_impl::update_clock_source(const std::string &source)
{
	printf("update_clock_source()\n");
}

void a2300_impl::update_time_source(const std::string &source)
{
	printf("update_time_source(%s)\n", source.c_str());

    if (source == "none"){}
    else if (source == "external"){}
    else if (source == "gpsdo"){}
    else
    {
    	printf("Unknown time source <%s>\n", source.c_str());
    	throw uhd::key_error("update_time_source: unknown source: " + source);
    }
//    _local_ctrl->poke32(TOREG(SR_CORE_PPS_SEL), (source == "external")? 1 : 0);
    printf("update_time_source() - done\n");
}

//! get the clock rate range for the frontend
//uhd::meta_range_t a2300_impl::get_clock_rate_range(void)
//{
//	 // (32 / 4) to (32,4096) in powers of 2,
//	 return uhd::meta_range_t(A2300_MIN_CLOCK, A2300_MAX_CLOCK);
//}

/*******************************************************************
* Update methods for time
******************************************************************/
void a2300_impl::set_time(const uhd::time_spec_t &time)
{
	printf("set_time\n");
	//boost::mutex::scoped_lock lock(_mutex);
	//_time = time;
	//_use_time = _time != uhd::time_spec_t(0.0);
	//if (_use_time) _timeout = MASSIVE_TIMEOUT; //permanently sets larger timeout
}


std::string a2300_impl::get_fw_version()
{
	//
	return std::string("TODO");

	//Send DCI Infrastructure Message to get Firmware information.
}



/*
 * FPGA Version Query
 */
std::string a2300_impl::get_fpga_version()
{
	DciProperty prop(0x00, _dci0_ctrl, A2300_WAIT_TIME);
	uint16 id =0, ver =0;

	prop.GetProperty<uint16, PT_UINT16>( 0x01, id);
	prop.GetProperty<uint16, PT_UINT16>(0x02, ver);

	char buffout[32];
	sprintf(buffout, "%04X - %02X.%02X", id, (ver>>8), ver & 0x00FF);		// ASR-2300 uses a Hex format.
	return std::string(buffout);

}


void a2300_impl::update_enables(void)
{
    //extract settings from state variables

    const bool enb_tx1 = (_is_setup) and bool(m_perifs[0].TxStreamer().lock());
    const bool enb_rx1 = (_is_setup) and bool(m_perifs[0].RxStreamer().lock());
    const bool enb_tx2 = (_is_setup) and bool(m_perifs[1].TxStreamer().lock());
    const bool enb_rx2 = (_is_setup) and bool(m_perifs[1].RxStreamer().lock());
    const size_t num_rx = (enb_rx1?1:0) + (enb_rx2?1:0);
    const size_t num_tx = (enb_tx1?1:0) + (enb_tx2?1:0);
    const bool mimo = num_rx == 2 or num_tx == 2;

    //setup the active chains in the codec
    // _codec_ctrl->set_active_chains(enb_tx1, enb_tx2, enb_rx1, enb_rx2);
    // if ((num_rx + num_tx) == 0) _codec_ctrl->set_active_chains(true, false, true, false); //enable something
    // this->reset_codec_dcm(); //set_active_chains could cause a clock rate change - reset dcm

    //figure out if mimo is enabled based on new state
    // CJC Not Used _gpio_state.mimo = (mimo)? 1 : 0;
    // CJC Not Used update_gpio_state();

    //atrs change based on enables
    // CJC ?? this->update_atrs();
}

const uhd::usrp::mboard_eeprom_t & a2300_impl::get_mb_eeprom(void)
{
	static const mboard_eeprom_t mb_eeprom;
    printf("get_mb_eeprom()\n");
    return mb_eeprom;
}

void a2300_impl::set_mb_eeprom(const uhd::usrp::mboard_eeprom_t &mb_eeprom)
{
    printf("set_mb_eeprom()\n");
//    mb_eeprom.commit(*_iface, "B200");
}

/***********************************************************************
 * Reference time and clock
 **********************************************************************/
