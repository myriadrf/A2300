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
#include <A2300/A2300InterfaceDefs.h>

#include <uhd/utils/log.hpp>
#include <uhd/utils/safe_call.hpp>
#include <uhd/transport/usb_control.hpp>
#include <uhd/utils/msg.hpp>
#include <uhd/exception.hpp>
#include <uhd/utils/static.hpp>
#include <uhd/utils/images.hpp>

#include <libusb-1.0/libusb.h>

#include "Dci/DciMsg.h"
#include "Dci/DciConversation.h"
#include "Dci/InfrastructureMsgs.h"
#include "Dci/WcaMsgs.h"
#include "Dci/StandardMsgs.h"

#include <boost/format.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp>
#include <cstdio>

// DCI Messages.
#define PROPERTY_ID_GAIN  (0x1)

using namespace uhd;
using namespace uhd::usrp;
using namespace uhd::transport;

// A2300 USB informaiton.
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

/***********************************************************************
 * Discovery
 **********************************************************************/
static device_addrs_t a2300_find(const device_addr_t &hint)
{
    device_addrs_t a2300_addrs;

    //return an empty list of addresses when type is set to non-usrp1
    if (hint.has_key("type") and hint["type"] != "usrp1") return a2300_addrs;

    //Return an empty list of addresses when an address is specified,
    //since an address is intended for a different, non-USB, device.
    if (hint.has_key("addr")) return a2300_addrs;

    unsigned int vid = A2300_VENDOR_ID;
    unsigned int pid = A2300_PRODUCT_ID;

    if(hint.has_key("vid") && hint.has_key("pid") && hint.has_key("type") && hint["type"] == "usrp1") {
        sscanf(hint.get("vid").c_str(), "%x", &vid);
        sscanf(hint.get("pid").c_str(), "%x", &pid);
    }

    // Important note:
    // The get device list calls are nested inside the for loop.
    // This allows the usb guts to decontruct when not in use,
    // so that re-enumeration after fw load can occur successfully.
    // This requirement is a courtesy of libusb1.0 on windows.

    //get descriptors again with serial number, but using the initialized VID/PID now since we have firmware
    const int buffSize = 256;
    char buff[buffSize];
    vid = A2300_VENDOR_ID;
    pid = A2300_PRODUCT_ID;
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
				new_addr["name"] = "a2300";

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
						new_addr["serial"] = "??";
					libusb_close(handle);
				}

				// Get the address of the device on the bus.
				usbAddress = libusb_get_device_address(pDevice);
				sprintf((char *)buff, "%d", usbAddress);
				new_addr["addr"] = std::string(buff);

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
{
    UHD_MSG(status) << "Opening the A2300 device..." << std::endl;

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
    handle = device_list[0];
    UHD_ASSERT_THROW(handle.get() != NULL); //better be found


    ////////////////////////////////////////////////////////////////////
    // Create standard UHD controller objects
    ////////////////////////////////////////////////////////////////////

    //Bind Bulk Data Interfaces to WcaPorts 0 Read and Write.
    //This is NOT dci.
    _data_transport = usb_zero_copy::make(
    		handle,     // identifier
			0, 0x86,    // IN interface, endpoint
			0, 0x05,    // OUT interface, endpoint
			device_addr);  // param hints


    _soft_time_ctrl = soft_time_ctrl::make(
    		boost::bind(&a2300_impl::rx_stream_on_off, this, _1));

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

    //Bind the DCI command and control interface to the transport.
    _dci0_ctrl = A2300_iface::make(	_idc0_transport,  0);

    ////////////////////////////////////////////////////////////////////
    // Initialize the properties tree
    ////////////////////////////////////////////////////////////////////
	InitializePropertyTree();

    ////////////////////////////////////////////////////////////////////
    // create clock control objects
    ////////////////////////////////////////////////////////////////////
#if defined(FUTURE_RELEASE)
    _master_clock_rate = 64e6;
    if (device_addr.has_key("mcr")){
        try{
            _master_clock_rate = boost::lexical_cast<double>(device_addr["mcr"]);
        }
        catch(const std::exception &e){
            UHD_MSG(error) << "Error parsing FPGA clock rate from device address: " << e.what() << std::endl;
        }
    }
#endif

    //initialize io handling
    this->io_init();
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

////////////////////////////////////////////////////////////////////
// create user-defined control objects
////////////////////////////////////////////////////////////////////
void a2300_impl::InitializePropertyTree()
{
    ////////////////////////////////////////////////////////////////////
    // Initialize the properties tree
    ////////////////////////////////////////////////////////////////////
	m_tree = property_tree::make();
	m_tree->create<std::string>("/name").set("a2300");
	m_tree->create<std::string>("/fpga_revision").publish(boost::bind(&a2300_impl::get_fpga_version, this));

	//Create Branch for Sensors.
	m_tree->create<int>("/sensors"); //phony property so this dir exists

	//Create Branch for RF components.
	const fs_path rf_path = "/rf/0";
	m_tree->create<std::string>(rf_path); //phony property so this dir exists
	m_tree->create<std::string>(rf_path / "name").set("ASR-2300");
	m_tree->create<std::string>(rf_path / "codename").set("Beta");

	const fs_path rx0_path = rf_path / "rx0";
	InitializePropertyTreeExt(rx0_path, WCACOMP_RF0, RFCH_RX);

	const fs_path tx0_path = rf_path / "tx0";
	InitializePropertyTreeExt(tx0_path, WCACOMP_RF0, RFCH_TX);

	const fs_path rx1_path = rf_path / "rx1";
	InitializePropertyTreeExt(rx1_path, WCACOMP_RF1, RFCH_RX);

	const fs_path tx1_path = rf_path / "tx1";
	InitializePropertyTreeExt(tx1_path, WCACOMP_RF1, RFCH_TX);

    //TODO: Create FPGA Waveform Components Properties.
    // includes IO Ports, sampling rates, etc.
}

/*
 * Create RX / TX Channel Controls (gain, BW, Frequency, Path)
 */
void a2300_impl::InitializePropertyTreeExt(const fs_path path, int idComponent, int rxTxFlag)
{
	m_tree->create<byte>(path).set(idComponent); // Set the component Id.
	m_tree->create<byte>(path / "pathid");

	m_tree->create<int>(path / "bandwidth");
	m_tree->create<int>(path / "bandwidth/value")
			.subscribe(boost::bind(&a2300_impl::set_bandwidth, this, idComponent, rxTxFlag, _1))
			.publish(boost::bind(&a2300_impl::get_bandwidth, this, idComponent, rxTxFlag));

	m_tree->create<int>(path / "frequency");
	m_tree->create<int>(path / "frequency/value")
			.subscribe(boost::bind(&a2300_impl::set_frequency, this, idComponent, rxTxFlag, _1))
			.publish(boost::bind(&a2300_impl::get_frequency, this, idComponent, rxTxFlag));

	m_tree->create<byte>(path / "gain");
	m_tree->create<int>(path / "gain/value")
			.subscribe(boost::bind(&a2300_impl::set_gain, this, idComponent, rxTxFlag, _1))
			.publish(boost::bind(&a2300_impl::get_gain, this, idComponent, rxTxFlag));
}

/***********************************************************************
 * Properties callback methods below
 **********************************************************************/
int a2300_impl::get_gain(int idComponent, int idCh)
{
	// Get work buffer.
	managed_send_buffer::sptr txbuff = _dci0_ctrl->GetSendDciMessageBuffer(WAIT_TIME);

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
int a2300_impl::get_bandwidth(int idComponent, int isRx)
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
				return (int)(m_bandwidthTableMHz[index] * 1000.0 * 1000.0);
			}
		}
		else
		{
			printf( "Bandwidth: received invalid DCI Message.\n");
		}
	}
	return(-1);  // Error.
}

/*
 * Read Bandwidth (Hz)
 */
void a2300_impl::set_bandwidth(int idComponent, int isRx, int bwHz)
{
	//Convert "from" db for instrument
	double bwMHz = (double)bwHz / (1000 * 1000);
	int iValue = GetBandwidthIndex( bwMHz );
	if( iValue < 0 )
	{
		printf("BW Error: Value %d not in range [%.2lf,%.2lf]\n", bwHz, m_bandwidthTableMHz[0], m_bandwidthTableMHz[BW_TSIZE-1]);
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

/*
 * Read Frequency (Hz)
 */
int a2300_impl::get_frequency(int idComponent, int isRx)
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
				return( (int)pProp->value.vUint32 * 1000 );   // Return Hz.
			}
			else
			{
				printf( "Frequency: received invalid DCI Message.\n");
			}
		}
	}
	return(-1);  // Error.
}


/*
 * Frequency in KHz.
 */
void a2300_impl::set_frequency(int idComponent, int isRx, int freqHz)
{
	// Round to KHz.
	unsigned freqKHz = (unsigned)( (freqHz + 500) / 1000 );

	managed_send_buffer::sptr mbuff = _dci0_ctrl->GetSendDciMessageBuffer(WAIT_TIME);
	if( mbuff != NULL )
	{
		//Create a DCI formatted message.
		Dci_Property property;
		property.idprop = (isRx) ? 0x03 : 0x07;  // RxFreq, TxFreq
		property.idtype = PT_UINT32;
		property.value.vUint32 = freqKHz;
		int len = Dci_TypedProperties_Init(mbuff->cast<void*>(), DCI_MAX_MSGSIZE, idComponent, 1, &property );

		//Send the DCI command.
		_dci0_ctrl->CommitDciMessageBuffer(mbuff, len, false);
		mbuff = NULL;  // Release Buffer.
	}
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
 * This function is for debugging DCI message response.
 * It demonstrates a method of passing multiple properties in one packet.
 */
bool a2300_impl::ProcessDciMessage(byte *buff, int msgLen)
{
	bool bHandled = false;

	int msgId = Dci_Hdr_MessageId( (Dci_Hdr *)buff);
	switch(msgId)
	{
		case 0x0200: //Handle device identification Messages.
 		{
 			Dci_IdentifyDevice* pid = (Dci_IdentifyDevice*) buff;
 			printf("  ID:\t\t\t%s\n  S/N:\t\t\t%s\n  Model:\t\t%s\n", pid->DeviceId, pid->SerialNumber, pid->Model);
 			bHandled = true;
 		}
 		break;

		case 0x2093:			// Type Data Query Response.
 		{
 			Dci_TypedProperties* ptp = (Dci_TypedProperties*) buff;
 			Dci_Property *pProp = Dci_TypedProperties_GetProperties(ptp);
 			int iValue;

 			for(int i=0; i< ptp->ctProperties; i++, pProp++)
 			{
 				switch(pProp->idprop)
 				{
 				case 0x02:			// RxGain.
 					iValue = (int)pProp[i].value.vByte;
 					printf("2093 Prop:%d %d\n", pProp->idprop, iValue);
 					break;
 				case 0x06:			// TxGain.
 					iValue = (int)pProp[i].value.vByte;
 					printf("2093 Prop:%d %d\n", pProp->idprop, iValue);
 					break;
 				default:			// Not Implemented or error.
 					iValue = (int) pProp[i].value.vUint32;
 					printf("2093 Prop:%d %d\n", pProp->idprop, iValue);
 					break;
 				}
 			}
 			bHandled = true;
 		}
 		break;

		case 0x2101:			// Typed Properties (21,01) for string query.
		{
			int iValue = 0x0;

			Dci_TypedProperties* ptp = (Dci_TypedProperties*) buff;
			Dci_Property *pProp = Dci_TypedProperties_GetProperties(ptp);

			for(int i=0; i< ptp->ctProperties; i++, pProp++)
			{
				switch(pProp->idprop)
				{
				case(0x1):			// ID
					iValue = (int) pProp->value.vUint16;
					printf("  2101 FPGA ID:\t\t%04X\n", iValue);
					break;
				case(0x2):			// Version
					iValue = (int) pProp->value.vUint16;
					printf("  2101 FPGA Version:\t\t%04X\n", iValue);
					break;
				default:
					printf(" Unknown property ID: %d value:%u\n", pProp->idprop, (unsigned) pProp->value.vUint32);
					break;
				}
			}
			bHandled = true;
		}
		break;

		case Dci_MessageError_Id:
		{
			Dci_MessageError* perr = (Dci_MessageError*) buff;
			printf( "Unrecognized message: category=%02X, id=%02X\n", perr->UnrecognizedCategoryId, perr->UnrecognizedTypeId);
			bHandled = true;
		}
		break;

	 	default: //Handle All other messages.
		{
			printf("Unknown message ID: 0x%04X\n", msgId);
			Dci_Hdr* phdr = (Dci_Hdr *)buff;
			printf( "Received unhandled message: category=%02X, id=%02X\n", phdr->idCategory, phdr->idType);
		}
		break;

	}
	return(bHandled);
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






