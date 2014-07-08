/** Name: ConfigDevice.h
*
* Copyright(c) 2013 Loctronix Corporation
* http://www.loctronix.com
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef CONFIGDEVICE_H_
#define CONFIGDEVICE_H_

#include <A2300/UsbDevice.h>
#include <A2300/BulkDataPort.h>
#include <A2300/TransportDci.h>
#include <A2300/A2300InterfaceDefs.h>
#include <A2300/ConfigRf.h>

namespace A2300 {

typedef std::list<IConfigComponent*> ConfigComponentList;

/**
 * Class implements configuration accessors for the ASR-2300 Device.  This is
 * the root object for working with the ASR 2300 configuration.
 */
class ConfigDevice {
public:
	static const int FindFirstDevice = -1;

public:
	ConfigDevice();
	virtual ~ConfigDevice();

	/**
	 * Returns a reference to the attached ASR-2300 UsbDevice object.
	 */
	UsbDevice&  Device() { return *m_pDevice;}

	ConfigRf&   RF0()	 { return m_rf0;}
	ConfigRf&   RF1()	 { return m_rf1;}

	/**
	 * Property gets the DCI Conversation ID 0 transport.  Use to communicate
	 * with the ASR-2300 device controller.
	 */
	TransportDci& Dci0Transport() { return m_dci0.transport;}
	
	/**
	 * Property gets the DCI Conversation ID 1 transport (asynchronous Data I/O).  Use this
	 * transport to send/receive data with the ASR-2300.  This conversation should be used
	 * in a way that supports multiple interactions simultaneously.  Recommend you do not block
	 * (wait) for receives on this transport unless you are sure you are the only user.
	 */
	TransportDci& Dci1Transport();

	std::string IdentifyDevice();
	std::string FirmwareVersion( int idWhich);
	uint16 FpgaId();
	uint16 FpgaVersion();

	/**
	 * Synchronizes the internal RF Lime interfaces to eliminate interchannel bias
	 * resulting in filters and various counters not be reset at the same time.  This
	 * is only needed when channels must be synchronized.
	 */
	void SynchRfState();

	/**
	 * Attach ASR-2300 device by finding the device specified by address, if not defined,
	 * the first device found is selected.  Override to use specialized VID, PID values.
	 */
	int Attach( int addr = FindFirstDevice, int vid = A2300_VENDOR_ID, int pid = A2300_PRODUCT_ID);

	/**
	 * Attach to existing ASR-2300 device.   Device assumed to be initialized and started
	 * prior to attach.
	 */
	void Attach( UsbDevice* pdevice);

	/**
	 * Detachs configuration object from ASR-2300 does appropriate cleanup.  If profived
	 * external UsbDevice object, stop and termination operations are left for the calling software.
	 */
	void Detach();

	/**
	 * Initializes device to the current configuration state.
	 */
	void Synchronize();

	/**
	 * Resets the device to its default state.
	 */
	void Reset();

	ConfigComponentList& Components() { return m_listComponents;}

protected:
	struct DciCtrl
	{
		BulkDataPort	 port;
		Dci_Conversation conversation;
		TransportDci  	 transport;

		DciCtrl( byte epidIn, byte epidOut)
		: port( epidIn, epidOut) {}

		bool IsInitialized() { return transport.Port() != NULL;}
		void Init( UsbDevice& device, int idc, double timeout);
		void Term();
	};
private:
	bool 				m_bCreated;
	UsbDevice*			m_pDevice;
	DciCtrl	 			m_dci0;
	DciCtrl				m_dci1;
	double				m_timeoutDefault;

	ConfigRf			m_rf0;
	ConfigRf			m_rf1;
	ConfigComponentList m_listComponents;
};

} /* namespace A2300 */

#endif /* CONFIGDEVICE_H_ */
