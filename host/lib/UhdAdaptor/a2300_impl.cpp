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

#include <uhd/utils/log.hpp>
#include <uhd/transport/usb_control.hpp>
#include <uhd/utils/msg.hpp>
#include <uhd/exception.hpp>
#include <uhd/utils/static.hpp>
#include <uhd/utils/images.hpp>
#include <uhd/utils/safe_call.hpp>
#include <uhd/usrp/dboard_eeprom.hpp>
#include <uhd/types/ranges.hpp>

#include <libusb-1.0/libusb.h>

#include "Dci/DciMsg.h"
#include "Dci/DciConversation.h"
#include "Dci/InfrastructureMsgs.h"
#include "Dci/WcaMsgs.h"
#include "Dci/StandardMsgs.h"

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

// Wait time (seconds)
static double WAIT_TIME = 0.1;

// Look-up table to map bandwidth (MHz) to A2300 entry.
const double a2300_impl::m_bandwidthTableMHz[a2300_impl::BW_TSIZE] =
	{ 14.0, 10.0, 7.0, 6.0, 5.0, 4.375, 3.5, 3.0, 2.75, 2.5, 1.92, 1.5, 1.375, 1.25, 0.875, 0.75 };

//! mapping of frontend to radio perif index
static const size_t FE1 = 0x0;  // TODO: Make sure 0's based is ok.
static const size_t FE2 = 0x1;

// DSP Systems {DUC / DDC }

static const int WCACOMP_DSP_DUC0  = 0x10; 	// Digital Up Converter
static const int WCACOMP_DSP_DDC0  = 0x20; 	// Digital Down Converter
static const int WCACOMP_DSP_DUC1  = 0x30; 	// Digital Up Converter
static const int WCACOMP_DSP_DDC1  = 0x40; 	// Digital Down Converter.

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
a2300_impl::a2300_impl(const device_addr_t &device_addr) :
		_is_setup(false),
		_continuous_streaming(false)
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
    dci0_hints["num_recv_frames"] = "3";   // CJC 16 ?
    dci0_hints["recv_frame_size"] = "512";
    dci0_hints["num_send_frames"] = "2";   // CJC 16 ?
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

	m_tree->create<std::string>("/fpga_revision").publish(boost::bind(&a2300_impl::get_fpga_version, this));

	//Create Branch for Sensors.
	m_tree->create<int>("/sensors"); //phony property so this dir exists

	////////////////////////////////////////////////////////////////////
	// Create data transport
	// This happens after FPGA ctrl instantiated so any junk that might
	// be in the FPGAs buffers doesn't get pulled into the transport
	// before being cleared.
	////////////////////////////////////////////////////////////////////
	device_addr_t data_xport_args;
	data_xport_args["recv_frame_size"] = device_addr.get("recv_frame_size", "512");  // 8192
	data_xport_args["num_recv_frames"] = device_addr.get("num_recv_frames", "3");    // 16
	data_xport_args["send_frame_size"] = device_addr.get("send_frame_size", "512");
	data_xport_args["num_send_frames"] = device_addr.get("num_send_frames", "3");

	//Bind Bulk Data Interfaces to WcaPorts 0 Read and Write.
	//This is NOT dci.
	//Note: A failure here will output a message of the form: "usb Rx8 submit failed"
	_data_transport = usb_zero_copy::make(
			handle,        // identifier
			0, 0x88,       // IN interface,  endpoint
			0, 0x08,       // OUT interface, endpoint
			data_xport_args);  // param hints

	while (_data_transport->get_recv_buff(0.0)){} //flush ctrl xport

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
	   //.coerce(boost::bind(&a2300_impl::set_tick_rate, this, _1))
	   .subscribe(boost::bind(&a2300_impl::set_tick_rate, this, _1))
	   .publish(boost::bind(&a2300_impl::get_tick_rate, this));
	   //.subscribe(boost::bind(&a2300_impl::update_tick_rate, this, _1));
	m_tree->create<time_spec_t>(mb_path / "time" / "cmd");

	////////////////////////////////////////////////////////////////////
	// and do the misc mboard sensors
	////////////////////////////////////////////////////////////////////
	m_tree->create<sensor_value_t>(mb_path / "sensors" / "ref_locked")
	   .publish(boost::bind(&a2300_impl::get_ref_locked, this));

	////////////////////////////////////////////////////////////////////
	// create frontend mapping
	////////////////////////////////////////////////////////////////////
	m_tree->create<subdev_spec_t>(mb_path / "rx_subdev_spec")
		   .set(subdev_spec_t())
		   .subscribe(boost::bind(&a2300_impl::update_rx_subdev_spec, this, _1));
	m_tree->create<subdev_spec_t>(mb_path / "tx_subdev_spec")
			.set(subdev_spec_t())
		   .subscribe(boost::bind(&a2300_impl::update_tx_subdev_spec, this, _1));

	////////////////////////////////////////////////////////////////////
	// setup radio control
	////////////////////////////////////////////////////////////////////
	UHD_MSG(status) << "Initialize Radio control..." << std::endl;
	const size_t num_radio_chains = 2; // CJC ((_local_ctrl->peek32(RB32_CORE_STATUS) >> 8) & 0xff);
	UHD_ASSERT_THROW(num_radio_chains > 0);
	UHD_ASSERT_THROW(num_radio_chains <= 2);
	_radio_perifs.resize(num_radio_chains);
	for (size_t i = 0; i < _radio_perifs.size(); i++) this->setup_radio(m_tree, i);

	#if defined(FUTURE)
		//now test each radio module's connection to the codec interface
		_codec_ctrl->data_port_loopback(true);
		BOOST_FOREACH(radio_perifs_t &perif, _radio_perifs)
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
		//.publish(boost::bind(&time_core_3000::get_time_now, _radio_perifs[0].time64));
	m_tree->create<time_spec_t>(mb_path / "time" / "pps");
		//.publish(boost::bind(&time_core_3000::get_time_last_pps, _radio_perifs[0].time64));
	for (size_t i = 0; i < _radio_perifs.size(); i++)
	{
		m_tree->access<time_spec_t>(mb_path / "time" / "now");
		//    .subscribe(boost::bind(&time_core_3000::set_time_now, _radio_perifs[i].time64, _1));
		m_tree->access<time_spec_t>(mb_path / "time" / "pps");
		//    .subscribe(boost::bind(&time_core_3000::set_time_next_pps, _radio_perifs[i].time64, _1));
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

	//subdev spec contains full width of selections
	//subdev_spec_t rx_spec, tx_spec;
	//BOOST_FOREACH(const std::string &fe, _tree->list(mb_path / "dboards" / "A" / "rx_frontends"))
	//{
	//    rx_spec.push_back(subdev_spec_pair_t("A", fe));
	//}
	//BOOST_FOREACH(const std::string &fe, _tree->list(mb_path / "dboards" / "A" / "tx_frontends"))
	//{
	//   tx_spec.push_back(subdev_spec_pair_t("A", fe));
	//}
	//_tree->access<subdev_spec_t>(mb_path / "rx_subdev_spec"); //.set(rx_spec);
	//_tree->access<subdev_spec_t>(mb_path / "tx_subdev_spec"); //.set(tx_spec);

	//init to internal clock and time source
	m_tree->access<std::string>(mb_path / "clock_source/value").set("internal");
	m_tree->access<std::string>(mb_path / "time_source/value").set("none");

	//Create Branch for RF components.
	const fs_path rf_path = mb_path / "rf/0";
	m_tree->create<std::string>(rf_path); //phony property so this dir exists
	m_tree->create<std::string>(rf_path / "name").set("ASR-2300");
	m_tree->create<std::string>(rf_path / "codename").set("Beta");

	const fs_path rx0_path = rf_path / "rx0";
	InitializePropertyTreeExt(m_tree, rx0_path, WCACOMP_RF0, RFCH_RX);

	const fs_path tx0_path = rf_path / "tx0";
	InitializePropertyTreeExt(m_tree, tx0_path, WCACOMP_RF0, RFCH_TX);

	const fs_path rx1_path = rf_path / "rx1";
	InitializePropertyTreeExt(m_tree, rx1_path, WCACOMP_RF1, RFCH_RX);

	const fs_path tx1_path = rf_path / "tx1";
	InitializePropertyTreeExt(m_tree, tx1_path, WCACOMP_RF1, RFCH_TX);

	//TODO: Create FPGA Waveform Components Properties.
	// includes IO Ports, sampling rates, etc.

    ////////////////////////////////////////////////////////////////////
    // create clock control objects
    ////////////////////////////////////////////////////////////////////
    _master_clock_rate = A2300_CLOCK_HZ;
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


/*
 * Create RX / TX Channel Controls (gain, BW, Frequency, Path)
 * This was part of the original interface to show a demonstration access to read/write parameters.
 */
void a2300_impl::InitializePropertyTreeExt(uhd::property_tree::sptr _tree, const fs_path path, int idComponent, int rxTxFlag)
{
	m_tree->create<byte>(path).set(idComponent); // Set the component Id.
	m_tree->create<byte>(path / "pathid");

	m_tree->create<int>(path / "bandwidth");
	m_tree->create<int>(path / "bandwidth/value")
			.subscribe(boost::bind(&a2300_impl::set_bandwidth, this, idComponent, rxTxFlag, _1))
			.publish(boost::bind(&a2300_impl::get_bandwidth, this, idComponent, rxTxFlag));

	m_tree->create<int>(path / "frequency");
	m_tree->create<double>(path / "frequency/value")
			.subscribe(boost::bind(&a2300_impl::set_freq, this, idComponent, rxTxFlag, _1))
			.publish(boost::bind(&a2300_impl::get_freq, this, idComponent, rxTxFlag));

	m_tree->create<byte>(path / "gain");
	m_tree->create<int>(path / "gain/value")
			.subscribe(boost::bind(&a2300_impl::set_gain, this, idComponent, rxTxFlag, _1))
			.publish(boost::bind(&a2300_impl::get_gain, this, idComponent, rxTxFlag))
			.set(A2300_DEFAULT_GAIN);
}

/***********************************************************************
 * setup radio control objects
 **********************************************************************/
void a2300_impl::setup_radio(uhd::property_tree::sptr _tree, const size_t dspno)
{
	printf("a2300 setup_radio(%lu)\n", dspno);

    radio_perifs_t &perif = _radio_perifs[dspno];

    const fs_path mb_path = "/mboards/0";

    // Testing only - remove.
    int idComponent = 0x81;  // 0x81, 0x82.
    int isRx = 0;		 // 1=Rx, 0=Tx
    ////////////////////////////////////////////////////////////////////
    // radio control
    ////////////////////////////////////////////////////////////////////
    //CJC const boost::uint32_t sid = (dspno == 0)? B200_CTRL0_MSG_SID : B200_CTRL1_MSG_SID;
    //CJC perif.ctrl = radio_ctrl_core_3000::make(false/*lilE*/, _ctrl_transport, zero_copy_if::sptr()/*null*/, sid);
    //CJC perif.ctrl->hold_task(_async_task);
    //CJC _async_task_data->radio_ctrl[dspno] = perif.ctrl; //weak
    _tree->access<time_spec_t>(mb_path / "time" / "cmd");
    //CJC    	.subscribe(boost::bind(&a2300_impl::set_time, _1));
    //CJC this->register_loopback_self_test(perif.ctrl);
    //CJC perif.atr = gpio_core_200_32wo::make(perif.ctrl, TOREG(SR_ATR));

    ////////////////////////////////////////////////////////////////////
    // create rx dsp control objects
    ////////////////////////////////////////////////////////////////////
    isRx = 1;
    idComponent = 0x81;
    perif.framer = 0x81; // rx_vita_core_3000::make(perif.ctrl, TOREG(SR_RX_CTRL));
    perif.ddc = dsp_core_a2300::make(); // perif.ctrl, TOREG(SR_RX_DSP));
    perif.ddc->set_link_rate(10e9/8); //whatever
    _tree->access<double>(mb_path / "tick_rate")
        // .subscribe(boost::bind(&a2300_impl::set_tick_rate, _1))
        .subscribe(boost::bind(&dsp_core_a2300::set_tick_rate, perif.ddc, _1));
    const fs_path rx_dsp_path = mb_path / "rx_dsps" / str(boost::format("%u") % dspno);
    _tree->create<meta_range_t>(rx_dsp_path / "rate" / "range");
    //CJC    .publish(boost::bind(&a2300_impl::get_host_rates));
    _tree->create<double>(rx_dsp_path / "rate" / "value")
    //		.coerce(boost::bind(&dsp_core_a2300::set_host_rate, _1))
    		.subscribe(boost::bind(&a2300_impl::update_rx_samp_rate, this, dspno, _1))
        	 .set(1e6);
    _tree->create<double>(rx_dsp_path / "freq" / "value")
					.subscribe(boost::bind(&a2300_impl::set_freq, this, idComponent, isRx, _1))
					.publish(boost::bind(&a2300_impl::get_freq, this, idComponent, isRx))
					.set(A2300_DEFAULT_FREQ);
        //.coerce(boost::bind(&dsp_core_a2300::set_freq, perif.ddc, _1))
        //.subscribe(boost::bind(&a2300_impl::set_freq, this, idComponent, isRx, _1))
        //.set(A2300_DEFAULT_FREQ);  // 1.946MHz
    printf("here 3\n");
	_tree->create<meta_range_t>(rx_dsp_path / "freq" / "range")
        .publish(boost::bind(&dsp_core_a2300::get_freq_range, perif.ddc));
    printf("here 4\n");

	_tree->create<stream_cmd_t>(rx_dsp_path / "stream_cmd")
        .subscribe(boost::bind(&a2300_impl::issue_stream_command, this, idComponent, isRx, _1));
      //.subscribe(boost::bind(&rx_vita_core_3000::issue_stream_command, perif.framer, _1));
    printf("here 5\n");

    ////////////////////////////////////////////////////////////////////
    // create tx dsp control objects
    ////////////////////////////////////////////////////////////////////
	isRx = 0;
	idComponent = 0x81;
    perif.deframer = 0x81; // tx_vita_core_3000::make(perif.ctrl, TOREG(SR_TX_CTRL));
    perif.duc = dsp_core_a2300::make(); // tx_dsp_core_3000::make(perif.ctrl, TOREG(SR_TX_DSP));
    perif.duc->set_link_rate(10e9/8); //whatever
    _tree->access<double>(mb_path / "tick_rate");
    //CJC .subscribe(boost::bind(&tx_vita_core_3000::set_tick_rate, _1))
    //CJC     .subscribe(boost::bind(&tx_dsp_core_3000::set_tick_rate, _1));
    const fs_path tx_dsp_path = mb_path / "tx_dsps" / str(boost::format("%u") % dspno);
    _tree->create<meta_range_t>(tx_dsp_path / "rate" / "range")
        .publish(boost::bind(&dsp_core_a2300::get_host_rates, perif.duc));
    _tree->create<double>(tx_dsp_path / "rate" / "value")
        .coerce(boost::bind(&dsp_core_a2300::set_host_rate, perif.duc, _1))
    //CJC     .subscribe(boost::bind(&b200_impl::update_tx_samp_rate, this, dspno, _1))
        .set(1e6);

    _tree->create<double>(tx_dsp_path / "freq" / "value")
							.subscribe(boost::bind(&a2300_impl::set_freq, this, idComponent, isRx, _1))
							.publish(boost::bind(&a2300_impl::get_freq, this, idComponent, isRx))
							.set(A2300_DEFAULT_FREQ);

	_tree->create<meta_range_t>(tx_dsp_path / "freq" / "range")
        .publish(boost::bind(&dsp_core_a2300::get_freq_range, perif.duc));

    ////////////////////////////////////////////////////////////////////
    // create time control objects
    ////////////////////////////////////////////////////////////////////
    //CJC time_core_3000::readback_bases_type time64_rb_bases;
    //CJC time64_rb_bases.rb_now = RB64_TIME_NOW;
    //CJC time64_rb_bases.rb_pps = RB64_TIME_PPS;
    //CJC perif.time64 = time_core_3000::make(perif.ctrl, TOREG(SR_TIME), time64_rb_bases);

    ////////////////////////////////////////////////////////////////////
    // create RF frontend interfacing
    ////////////////////////////////////////////////////////////////////
    for(size_t direction = 0; direction < 2; direction++)
    {
    	// RX1, RX2, TX1, TX2
        const std::string x = direction? "rx" : "tx";
        const std::string key = std::string((direction? "RX" : "TX")) + std::string(((dspno == FE1)? "1" : "2"));
        const fs_path rf_fe_path = mb_path / "dboards" / "A" / (x+"_frontends") / (dspno? "B" : "A");
        isRx = (direction == 0);
        int idComponent = (dspno == FE1) ? (0x81):(0x82);
        _tree->create<std::string>(rf_fe_path / "name").set("FE-"+key);
        _tree->create<int>(rf_fe_path / "sensors"); //empty TODO

        BOOST_FOREACH(const std::string &name, get_gain_names(key))
        {
            _tree->create<meta_range_t>(rf_fe_path / "gains" / name / "range")
                .set(a2300_impl::get_gain_range());

            _tree->create<double>(rf_fe_path / "gains" / name / "value")
                //.coerce(boost::bind(&a2300_impl::set_gain, codec_ctrl, key, _1))
                .subscribe(boost::bind(&a2300_impl::set_gain, this, idComponent, isRx, _1))
                .set(0.0);
        }

        _tree->create<std::string>(rf_fe_path / "connection").set("IQ");
        _tree->create<bool>(rf_fe_path / "enabled").set(true);
        _tree->create<bool>(rf_fe_path / "use_lo_offset").set(false);

        _tree->create<double>(rf_fe_path / "bandwidth" / "value")
            //.coerce(boost::bind(&set_bw_filter, _codec_ctrl, key, _1))
            .set(40e6);

        _tree->create<meta_range_t>(rf_fe_path / "bandwidth" / "range")
            .publish(boost::bind(&get_bw_filter_range)); // , key));

        _tree->create<double>(rf_fe_path / "freq" / "value")
            .set(0.0);
            //.coerce(boost::bind(&a2300_impl::tune, _codec_ctrl, key, _1))
            //.subscribe(boost::bind(&a2300_impl::update_bandsel, this, key, _1));

        _tree->create<meta_range_t>(rf_fe_path / "freq" / "range")
            .publish(boost::bind(&a2300_impl::get_rf_freq_range));

        //setup antenna stuff
        if (key[0] == 'R')
        {
            static const std::vector<std::string> ants = boost::assign::list_of("TX/RX")("RX2");
            _tree->create<std::vector<std::string> >(rf_fe_path / "antenna" / "options").set(ants);
            _tree->create<std::string>(rf_fe_path / "antenna" / "value")
                .subscribe(boost::bind(&a2300_impl::update_antenna_sel, this, dspno, _1))
                .set("RX2");
        }

        if (key[0] == 'T')
        {
            static const std::vector<std::string> ants(1, "TX/RX");
            _tree->create<std::vector<std::string> >(rf_fe_path / "antenna" / "options").set(ants);
            _tree->create<std::string>(rf_fe_path / "antenna" / "value").set("TX/RX");
        }

    }
}

/***********************************************************************
 * loopback tests
 **********************************************************************/
// Not Used.

/***********************************************************************
 * Sample and tick rate comprehension below
 **********************************************************************/
double a2300_impl::set_tick_rate(const double rate)
{
    UHD_MSG(status) << "Asking for clock rate " << rate/1e6 << " MHz\n";
    _tick_rate = rate; // _codec_ctrl->set_clock_rate(rate);
    UHD_MSG(status) << "Actually got clock rate " << _tick_rate/1e6 << " MHz\n";
    return(rate);
}

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

/***********************************************************************
 * Properties callback methods below
 **********************************************************************/
int a2300_impl::get_gain(int idComponent, int idCh)
{
	// Get work buffer.
	managed_send_buffer::sptr txbuff =
			_dci0_ctrl->GetSendDciMessageBuffer(WAIT_TIME);

	//Query Identify Device.
	byte idProperty = (idCh == RFCH_RX) ? 0x02 : 0x06;	// RxGain, TxGain
	int len = Dci_TypedPropertiesQuery_Init(txbuff->cast<void*>(), DCI_MAX_MSGSIZE, idComponent, 1, &idProperty );

	//Send the DCI command.
	_dci0_ctrl->CommitDciMessageBuffer(txbuff, len, false);
	txbuff = NULL;

	//Process DCI Response.
	managed_recv_buffer::sptr rxbuff = _dci0_ctrl->ReceiveDciMessage(WAIT_TIME);
	if( rxbuff != NULL)
	{
		//_dci0_ctrl->Test();
		byte* buff = rxbuff->cast<byte*>();
		Dci_TypedProperties* ptp = (Dci_TypedProperties*) buff;

		//If we got back what we expected.
		if( Dci_Hdr_MatchesId( (Dci_Hdr *)buff, Dci_WcaCategoryId, Dci_TypedProperties_Id)
			&& ptp->idComponent == idComponent)
		{
			Dci_Property *pProp = Dci_TypedProperties_GetProperties(ptp);
			return(pProp->value.vByte * 3);
		}
		else
		{
			printf( "Gain: received invalid DCI Message.\n");
		}
	}
	return(-1);  // Error.
}

void a2300_impl::set_gain(int idComponent, int idCh, int gainDb)
{
	managed_send_buffer::sptr mbuff = _dci0_ctrl->GetSendDciMessageBuffer(WAIT_TIME);

	//Convert "from" db for instrument.
	int gain = (gainDb + 2) / 3;  // Round to closest step.

	//Create a DCI formatted message.
	Dci_Property property;
	property.idprop = (idCh == RFCH_RX) ? 0x02 : 0x06;  // RxGain, TxGain
	property.idtype = PT_BYTE;
	property.value.vByte = (byte) gain;
	int len = Dci_TypedProperties_Init(mbuff->cast<void*>(), DCI_MAX_MSGSIZE, idComponent, 1, &property );

	//Send the DCI command.
	_dci0_ctrl->CommitDciMessageBuffer(mbuff, len, false);
	mbuff = NULL;

}

/*
 * Read Bandwidth (Hz)
 */
double a2300_impl::get_bandwidth(int idComponent, int isRx)
{
	// Get work buffer.
	managed_send_buffer::sptr txbuff = _dci0_ctrl->GetSendDciMessageBuffer(WAIT_TIME);

	//Query Identify Device.
	byte idProperty = isRx ? 0x05 : 0x09;
	int len = Dci_TypedPropertiesQuery_Init(txbuff->cast<void*>(), DCI_MAX_MSGSIZE, idComponent, 1, &idProperty );

	//Send the DCI command.
	_dci0_ctrl->CommitDciMessageBuffer(txbuff, len, false);
	txbuff = NULL;

	//Process DCI Response.
	managed_recv_buffer::sptr rxbuff = _dci0_ctrl->ReceiveDciMessage(WAIT_TIME);
	if( rxbuff != NULL)
	{
		//_dci0_ctrl->Test();
		byte* buff = rxbuff->cast<byte*>();
		Dci_TypedProperties* ptp = (Dci_TypedProperties*) buff;

		//If we got back what we expected.
		if( Dci_Hdr_MatchesId( (Dci_Hdr *)buff, Dci_WcaCategoryId, Dci_TypedProperties_Id)
			&& ptp->idComponent == idComponent)
		{
			Dci_Property *pProp = Dci_TypedProperties_GetProperties(ptp);
			int index = (int)pProp->value.vByte;
			if( index >= 0 && index < BW_TSIZE)
			{
				return (double)(m_bandwidthTableMHz[index] * 1000.0 * 1000.0);
			}
		}
		else
		{
			printf( "Bandwidth: received invalid DCI Message.\n");
		}
	}
	return(-1.0);  // Error.
}

/*
 * Read Bandwidth (Hz)
 */
void a2300_impl::set_bandwidth(int idComponent, int isRx, double bwHz)
{
	//Convert "from" db for instrument
	double bwMHz = bwHz / (1000.0 * 1000.0);
	int iValue = GetBandwidthIndex( bwMHz );
	if( iValue < 0 )
	{
		printf("BW Error: Value %.2lf not in range [%.2lf,%.2lf]\n", bwHz, m_bandwidthTableMHz[0], m_bandwidthTableMHz[BW_TSIZE-1]);
		return;
	}

	managed_send_buffer::sptr mbuff = _dci0_ctrl->GetSendDciMessageBuffer(WAIT_TIME);
	if( mbuff != NULL )
	{
		//Create a DCI formatted message.
		Dci_Property property;
		property.idprop = (isRx) ? 0x05 : 0x09;
		property.idtype = PT_BYTE;
		property.value.vByte = (byte) iValue;

		int len = Dci_TypedProperties_Init(mbuff->cast<void*>(), DCI_MAX_MSGSIZE, idComponent, 1, &property );

		//Send the DCI command.
		_dci0_ctrl->CommitDciMessageBuffer(mbuff, len, false);
		mbuff = NULL;  // Release Buffer.
	}
}

double a2300_impl::get_freq(int idComponent, int isRx)
{
	// Get work buffer.
	managed_send_buffer::sptr txbuff = _dci0_ctrl->GetSendDciMessageBuffer(WAIT_TIME);
	if( txbuff != NULL)
	{
		//Query Identify Device.
		byte idProperty = isRx ? 0x03 : 0x07;	// RxGain, TxGain
		int len = Dci_TypedPropertiesQuery_Init(txbuff->cast<void*>(), DCI_MAX_MSGSIZE, idComponent, 1, &idProperty );

		//Send the DCI command.
		_dci0_ctrl->CommitDciMessageBuffer(txbuff, len, false);
		txbuff = NULL;

		//Process DCI Response.
		managed_recv_buffer::sptr rxbuff = _dci0_ctrl->ReceiveDciMessage(WAIT_TIME);
		if( rxbuff != NULL)
		{
			//_dci0_ctrl->Test();
			byte* buff = rxbuff->cast<byte*>();
			Dci_TypedProperties* ptp = (Dci_TypedProperties*) buff;

			//If we got back what we expected.
			if( Dci_Hdr_MatchesId( (Dci_Hdr *)buff, Dci_WcaCategoryId, Dci_TypedProperties_Id)
				&& ptp->idComponent == idComponent)
			{
				Dci_Property *pProp = Dci_TypedProperties_GetProperties(ptp);
				return( (double)pProp->value.vUint32 * 1000.0 );   // Return Hz.
			}
			else
			{
				printf( "Frequency: received invalid DCI Message.\n");
			}
		}
	}
	return(-1.0);  // Error.
}


/*
 * Frequency in KHz.
 */
double a2300_impl::set_freq(int idComponent, int isRx, double freqHz)
{
	unsigned iFreqKHz = 0;
	printf("a2300 set_freq(%x,%d,%.2lf)\n", idComponent, isRx, freqHz);
	if( freqHz > 0.0 )
	{
		// Round to KHz.
		iFreqKHz = (unsigned)( ((unsigned long)freqHz) / 1000L );

		managed_send_buffer::sptr mbuff = _dci0_ctrl->GetSendDciMessageBuffer(WAIT_TIME);
		if( mbuff != NULL )
		{
			//Create a DCI formatted message.
			Dci_Property property;
			property.idprop = (isRx) ? 0x03 : 0x07;  // RxFreq, TxFreq
			property.idtype = PT_UINT32;
			property.value.vUint32 = iFreqKHz;
			int len = Dci_TypedProperties_Init(mbuff->cast<void*>(), DCI_MAX_MSGSIZE, idComponent, 1, &property );

			//Send the DCI command.
			_dci0_ctrl->CommitDciMessageBuffer(mbuff, len, false);
			mbuff = NULL;  // Release Buffer.
		}
	}
	return(iFreqKHz * 1000.0);
}

/*
 * Board Alive Test.
 */
void a2300_impl::send_idle_msg()
{
	// Get work buffer.
	managed_send_buffer::sptr txBuff = _dci0_ctrl->GetSendDciMessageBuffer(WAIT_TIME);
	if( txBuff != NULL )
	{
		int len = Dci_IdleMsg_Init(txBuff->cast<void *>());
		_dci0_ctrl->CommitDciMessageBuffer(txBuff, len, true);
		txBuff = NULL;
	}
}


/*
 * FPGA Version Query
 */
std::string a2300_impl::get_fpga_version()
{
	// Get work buffer.
	managed_send_buffer::sptr txbuff = _dci0_ctrl->GetSendDciMessageBuffer(1);

	// Request FPGA firmware ID & version information.
	byte ids[] = {0x2};
	int len = Dci_TypedPropertiesQuery_Init(txbuff->cast<void*>(), DCI_MAX_MSGSIZE, 0x00, 1, ids );

	//Send the DCI command.
	_dci0_ctrl->CommitDciMessageBuffer(txbuff, len, false);
	txbuff = NULL;

	//Receive information back.
	managed_recv_buffer::sptr rxBuff = _dci0_ctrl->ReceiveDciMessage(WAIT_TIME);
	if( rxBuff != NULL)
	{
		//_dci0_ctrl->Test();
		byte* buff = rxBuff->cast<byte*>();
		Dci_TypedProperties* ptp = (Dci_TypedProperties*) buff;

		//If we got back what we expected.
		if( Dci_Hdr_MatchesId( (Dci_Hdr *)buff, Dci_WcaCategoryId, Dci_TypedProperties_Id)
			&& ptp->idComponent == 0x0)
		{
			char buffout[32];
			Dci_Property *pProp = Dci_TypedProperties_GetProperties(ptp);
			sprintf(buffout, "%X", pProp->value.vUint16);		// ASR-2300 uses a Hex format.
			return std::string(buffout);
		}
		else
		{
			printf( "received invalid DCI Message.\n");
		}
		//rxBuff->release();
	}
	return(std::string("??"));
}


/*
 * Method to Map Frequency (MHz) to A2300 Lookup-Table Entry.
 */
int a2300_impl::GetBandwidthIndex( double bwMHz)
{
	// Verify range.
	if( bwMHz < m_bandwidthTableMHz[0] ||
		bwMHz > m_bandwidthTableMHz[BW_TSIZE-1] )
	{
		return -1;
	}

	// Find best match (below).
	int iFound = 0;
	for( int i=1; i<BW_TSIZE; i++ )
	{
		if( bwMHz <= m_bandwidthTableMHz[i] )
			iFound = i;
		else
			break;
	}
	return(iFound);
}

void a2300_impl::update_antenna_sel(const size_t which, const std::string &ant)
{
	printf("update_antenna_sel(%s)\n", ant.c_str());
    if (ant != "TX/RX" and ant != "RX2") throw uhd::value_error("a2300: unknown RX antenna option: " + ant);
    _radio_perifs[which].ant_rx2 = (ant == "RX2");
    //this->update_atrs();
}

void a2300_impl::update_enables(void)
{
    //extract settings from state variables
    const bool enb_tx1 = (_radio_perifs.size() > FE1) and bool(_radio_perifs[FE1].tx_streamer.lock());
    const bool enb_rx1 = (_radio_perifs.size() > FE1) and bool(_radio_perifs[FE1].rx_streamer.lock());
    const bool enb_tx2 = (_radio_perifs.size() > FE2) and bool(_radio_perifs[FE2].tx_streamer.lock());
    const bool enb_rx2 = (_radio_perifs.size() > FE2) and bool(_radio_perifs[FE2].rx_streamer.lock());
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

sensor_value_t a2300_impl::get_ref_locked(void)
{
    const bool lock = false; // CJC (_local_ctrl->peek32(RB32_CORE_MISC) & 0x1) == 0x1;
    return sensor_value_t("Ref", lock, "locked", "unlocked");
}


