/** Name: ConfigSensorBase.cpp
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

#ifdef WIN32
	#define NOMINMAX
#endif

#include <A2300/ConfigSensor.h>
#include <A2300/ConfigDevice.h>
#include <algorithm>

namespace A2300 {

ConfigSensorBase::ConfigSensorBase( byte idAction, byte idComponent, const std::string& sname, 
					   ConfigDevice* pDevice)
:  m_idAction( idAction), m_idComponent( idComponent), m_sName ( sname), m_pDevice(pDevice)
{
}

ConfigSensorBase::~ConfigSensorBase() 
{
	// TODO Auto-generated destructor stub
}

//************************************************************
// virtual overrides.
//************************************************************
std::string ConfigSensorBase::name()
{
	return m_sName;
}
byte ConfigSensorBase::componentId()
{
	return m_idComponent;
}
void ConfigSensorBase::Reset()
{
}
void ConfigSensorBase::Synch()
{
	//TODO
}

//************************************************************
// Sensor configuration methods.
//************************************************************

/**
* Method sends action message to the component, to turn on/off reporting.
*/
void ConfigSensorBase::Reporting( SensorReportEnum mode, uint16 frequency, uint16 ctSamplesPerReport, uint16 flags )
{
	//Note we are using Dci1 transport so we don't have asynchronous problems.
	byte buff[DCI_MAX_MSGSIZE];
	TransportDci& td = m_pDevice->Dci1Transport();

	int nLen = Dci_ExecuteAction_Init( buff, DCI_MAX_MSGSIZE, this->m_idComponent,
		this->m_idAction, sizeof( ActionData), NULL);
	
	ActionData* ad =  (ActionData*)( Dci_ExecuteAction_GetData( (Dci_ExecuteAction*) buff));

	//Initialize structure with caller provided initialization.
	ad->ctrlFlags = (uint16) mode;
	ad->dataFreq  = frequency;
	ad->ctSamples = ctSamplesPerReport;
	ad->dataFlags = flags;

	//Send the message 
	td.SendMsg( buff, nLen, false);
}



int ConfigSensorBase::ReadTypedDataMsg( void* pmsg, size_t lenMax, double timeout)
{
	TransportDci& td = m_pDevice->Dci1Transport();
	return td.ReceiveMsg( (byte*) pmsg, lenMax, timeout);

}

} /* namespace A2300 */
