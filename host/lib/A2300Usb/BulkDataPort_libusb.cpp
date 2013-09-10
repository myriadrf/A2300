// Name: BulkDataPort_libusb.cpp
//
// Copyright(c) 2013 Loctronix Corporation
// http://www.loctronix.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include <System/DataTypes.h>
#include <A2300/BulkDataPort.h>
#include <stdexcept>

//*************************************************************************
//*************************************************************************
//* BulkDataPort Implementation.
//*************************************************************************
//*************************************************************************
A2300::BulkDataPort::BulkDataPort( byte epidIn, byte epidOut)
: PortBase( epidIn, epidOut),
  m_timeout(10)	, m_pDevHandle(NULL)
{
}

A2300::BulkDataPort::~BulkDataPort()
{
}

int A2300::BulkDataPort::OnInit()
{
	return(0);
}

/**
 *
 */
void A2300::BulkDataPort::Close( )
{
#if defined(WIN32)
	//Tear down endpoint input
	m_pEndPointIn = NULL; //Is delete needed?

	//Tear down endpoint output
	m_pEndPointOut = NULL; //Is delete needed?
#endif
}

/**
 *
 */
void A2300::BulkDataPort::Open( )
{
	if( !IsInitialized() )
		throw std::runtime_error("Port must be initialized before Open().");

	m_pDevHandle = DeviceHandle();

	if (   (epidIn() > 0  && !EndPointAvailable(epidIn()))
		|| (epidOut() > 0 && !EndPointAvailable(epidOut())))
	{
		throw std::runtime_error("Required USB end point could not be found.");
	}

}

/**
 * Read the specified number of bytes from the port.
 */
int A2300::BulkDataPort::Read( byte * pdata, int ctBytes, int msecTimeout )
{
	if (DeviceHandle() == NULL || epidIn() == 0)
		return -1;

	int ctRead;
	int retval = libusb_bulk_transfer(DeviceHandle(), (epidIn() | LIBUSB_ENDPOINT_IN), pdata, ctBytes, &ctRead, msecTimeout);

	return (retval == 0) ? ctRead: retval;
}

/**
 * Writes the specified number bytes to the port.
 */
int A2300::BulkDataPort::Write(byte * pdata, int ctBytes, int msecTimeout )
{
	if (DeviceHandle() == NULL || epidOut() == 0)
		return -1;

	int ctWritten;
	int retval = libusb_bulk_transfer(DeviceHandle(), (epidOut() | LIBUSB_ENDPOINT_OUT), pdata, ctBytes, &ctWritten, msecTimeout);

	return (retval == 0) ? ctWritten: retval;
}
 

/**
 *  Checks if the given end point is available
 */
bool A2300::BulkDataPort::EndPointAvailable(byte epid)
{
	libusb_device *pDev = libusb_get_device(DeviceHandle());

    libusb_device_descriptor desc;

    int retval = libusb_get_device_descriptor(pDev, &desc);

    if (retval < 0)
    {
        return false;
    }

    libusb_config_descriptor *config;

    libusb_get_config_descriptor(pDev, 0, &config);

    const libusb_interface *inter;
    const libusb_interface_descriptor *interdesc;
    const libusb_endpoint_descriptor *epdesc;

    for(int i=0; i<(int)config->bNumInterfaces; i++)
    {
    	inter = &config->interface[i];
    	for(int j=0; j<inter->num_altsetting; j++)
    	{
    		interdesc = &inter->altsetting[j];

    		for(int k=0; k<(int)interdesc->bNumEndpoints; k++)
    		{
    			epdesc = &interdesc->endpoint[k];
    			if (epdesc->bEndpointAddress == epid)
				{
    				libusb_free_config_descriptor(config);
    				return true;
				}
    		}
    	}
    }

    libusb_free_config_descriptor(config);

    return false;
}


