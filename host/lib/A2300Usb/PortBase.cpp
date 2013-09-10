// Name: PortBase.cpp
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

#include "System/DataTypes.h"
#include <A2300/UsbDevice.h>

namespace A2300
{
	PortBase::PortBase() :
		m_pUsbDevice(NULL), m_epidIn( 0), m_epidOut( 0)
	{
	}
	PortBase::PortBase( byte epidIn, byte epidOut) :
		m_pUsbDevice(NULL), m_epidIn( epidIn), m_epidOut( epidOut)
	{
	}

	int PortBase::Init( UsbDevice *pDevice)
	{
		m_pUsbDevice = pDevice;
		return OnInit();
	}
}
