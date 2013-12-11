/** Name: ConfigRf.cpp
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

#include <A2300/ConfigRf.h>

namespace A2300 {

ConfigRf::ConfigRf(int idComponent, const std::string& sname, ConfigDevice* pDevice )
: m_idComponent( idComponent), m_sName ( sname), m_pDevice(pDevice)
{
	// TODO Auto-generated constructor stub

}


ConfigRf::~ConfigRf()
{
	// TODO Auto-generated destructor stub
}

uint32 ConfigRf::RxFrequency( uint32 freqkHz)
{
	//TODO
}
uint32 ConfigRf::RxFrequency( ) const
{
	//TODO
}

byte ConfigRf::RxGain( byte gainDb)
{
	//TODO
}

byte ConfigRf::RxGain( ) const
{
	//TODO
}

byte ConfigRf::RxPath( byte idPath)
{
	//TODO
}
byte ConfigRf::RxPath( ) const
{
	//TODO
}

RfBandwidthValuesEnum ConfigRf::RxBandwidth( RfBandwidthValuesEnum bw)
{
	//TODO
}

RfBandwidthValuesEnum ConfigRf::RxBandwidth( ) const
{
	//TODO
}

uint32 ConfigRf::TxFrequency( uint32 freqkHz)
{
	//TODO
}
uint32 ConfigRf::TxFrequency( ) const
{
	//TODO
}

byte ConfigRf::TxGain( byte gainDb)
{
	//TODO
}

byte ConfigRf::TxGain( ) const
{
	//TODO
}

byte ConfigRf::TxPath( byte idPath)
{
	//TODO
}
byte ConfigRf::TxPath( ) const
{
	//TODO
}

RfBandwidthValuesEnum ConfigRf::TxBandwidth( RfBandwidthValuesEnum bw)
{
	//TODO
}

RfBandwidthValuesEnum ConfigRf::TxBandwidth( ) const
{
	//TODO
}



std::string ConfigRf::name()
{
	return m_sName;
}
int  ConfigRf::componentId()
{
	return m_idComponent;
}
void ConfigRf::Reset()
{
	//TODO
}
void ConfigRf::Synch()
{
	//TODO
}


} /* namespace A2300 */
