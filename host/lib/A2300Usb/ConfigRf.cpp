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

#include <A2300/ConfigDevice.h>
#include <A2300/ConfigRf.h>
#include <algorithm>

namespace A2300 {

// Look-up table to map bandwidth (MHz) to A2300 entry.
static const double s_rfbandwidths[] =
	{28.0, 20.0, 14.0, 12.0, 10.0, 8.75, 7.0, 6.0, 5.5, 5.0, 3.84, 3.0, 2.75, 2.5, 1.75, 1.5};
	
#define  CT_BANDWIDTHS 16

ConfigRf::ConfigRf(byte idComponent, const std::string& sname, ConfigDevice* pDevice )
: m_idComponent( idComponent), m_sName ( sname), m_pDevice(pDevice)
{
	// TODO Auto-generated constructor stub

}

ConfigRf::~ConfigRf()
{
	// TODO Auto-generated destructor stub
}

RfBandwidthValuesEnum ConfigRf::BandwidthFromMHz( double bandwidthMhz)
{
	// Verify range.
	if( bandwidthMhz > s_rfbandwidths[0] ||
			bandwidthMhz < s_rfbandwidths[CT_BANDWIDTHS-1] )
	{
		return RFBW_5MHZ;
	}

	// Find best match (below).
	int iFound = 0;
	for( int nn = 0; nn < CT_BANDWIDTHS; ++nn )
	{
		if( s_rfbandwidths[nn] >=  bandwidthMhz)
			iFound = nn;
		else
			break;
	}
	return (RfBandwidthValuesEnum) (iFound);
}


double  ConfigRf::BandwidthToMHz( RfBandwidthValuesEnum bw)
{
	return s_rfbandwidths[bw];
}

uint32 ConfigRf::RxFrequency( uint32 freqkHz)
{
	TransportDci& td = m_pDevice->Dci0Transport();
	m_rxfreq = std::max<uint32>( 300000, std::min<uint32>( 3800000, freqkHz));
	td.SetProperty<uint32>(m_idComponent, RFPROP_RXFREQ, m_rxfreq);
	return m_rxfreq;
}
uint32 ConfigRf::RxFrequency( ) const
{
	return m_rxfreq;
}

byte ConfigRf::RxGain( byte gainDb)
{
	TransportDci& td = m_pDevice->Dci0Transport();
	m_rxgain = std::max<byte>( 0, std::min<byte>( 60, gainDb));
	td.SetProperty<byte>(m_idComponent, RFPROP_RXGAIN, m_rxgain/3);
	return m_rxgain;
}

byte ConfigRf::RxGain( ) const
{
	return m_rxgain;
}

byte ConfigRf::RxPath( byte idPath)
{
	TransportDci& td = m_pDevice->Dci0Transport();
	m_rxpath = idPath;
	td.SetProperty<byte>(m_idComponent,   RFPROP_RXPATH, m_rxpath);
	return m_rxpath;
}
byte ConfigRf::RxPath( ) const
{
	return m_rxpath;
}

RfBandwidthValuesEnum ConfigRf::RxBandwidth( RfBandwidthValuesEnum bw)
{
	TransportDci& td = m_pDevice->Dci0Transport();
	m_rxbw = bw;
	td.SetProperty<byte>(m_idComponent, RFPROP_RXBANDWIDTH, (byte) m_rxbw);
	return m_rxbw;
}

RfBandwidthValuesEnum ConfigRf::RxBandwidth( ) const
{
	return m_rxbw;
}

uint32 ConfigRf::TxFrequency( uint32 freqkHz)
{
	TransportDci& td = m_pDevice->Dci0Transport();
	m_txfreq = std::min<uint32>( 300000, std::max<uint32>( 3800000, freqkHz));
	td.SetProperty<uint32>(m_idComponent, RFPROP_TXFREQ, m_txfreq);
	return m_txfreq;
}
uint32 ConfigRf::TxFrequency( ) const
{
	return m_txfreq;
}

byte ConfigRf::TxGain( byte gainDb)
{
	TransportDci& td = m_pDevice->Dci0Transport();
	m_txgain = std::min<byte>( 0, std::max<byte>( 60, gainDb));
	td.SetProperty<byte>(m_idComponent, RFPROP_TXGAIN, m_txgain);
	return m_txgain;
}

byte ConfigRf::TxGain( ) const
{
	return m_txgain;
}

byte ConfigRf::TxPath( byte idPath)
{
	TransportDci& td = m_pDevice->Dci0Transport();
	m_txpath = idPath;
	td.SetProperty<byte>(m_idComponent,   RFPROP_TXPATH, m_txpath);
	return m_txpath;
}
byte ConfigRf::TxPath( ) const
{
	return m_txpath;
}

RfBandwidthValuesEnum ConfigRf::TxBandwidth( RfBandwidthValuesEnum bw)
{
	TransportDci& td = m_pDevice->Dci0Transport();
	m_txbw = bw;
	td.SetProperty<byte>(m_idComponent, RFPROP_TXBANDWIDTH, (byte) m_txbw);
	return m_txbw;
}

RfBandwidthValuesEnum ConfigRf::TxBandwidth( ) const
{
	return m_txbw;
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
