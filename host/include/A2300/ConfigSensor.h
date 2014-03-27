/** Name: ConfigSensorBase.h
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

#ifndef CONFIGSENSOR_H_
#define CONFIGSENSOR_H_

#include <A2300/A2300_Defs.h>
#include <A2300/A2300InterfaceDefs.h>
#include <System/DataTypes.h>
#include "TransportDci.h"
#include <string>

namespace A2300 {

class ConfigDevice;

/**
* Configuration class for ASR-2300 sensors.
*/
class ConfigSensorBase  : public IConfigComponent{
public:

	enum SensorReportEnum
	{
		Disabled	= 0,
		SingleShot  = 1,
		Continuous  = 2
	};

// Declare byte packed wire DCI WCA payloads.
#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(push, 1)
#endif

	struct ActionData
	{
		uint16 ctrlFlags;
		uint16 dataFlags;
		uint16 dataFreq;
		uint16 ctSamples;
	};
#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(pop)
#endif


public:
	ConfigSensorBase(byte idAction, byte idComponent, const std::string& sname, ConfigDevice* pDevice);
	virtual ~ConfigSensorBase();

	/* Define IConfigComponent interface*/
	virtual std::string name();
	virtual byte componentId();
	virtual void Reset();
	virtual void Synch();

	/**
	* Method sends action message to the component, to turn on/off reporting.
	*/
	void Reporting( SensorReportEnum mode, uint16 frequency, uint16 ctSamplesPerReport, uint16 flags = 0);

	int ReadTypedDataMsg( void* pmsg, size_t lenMax, double timeout);


private:
	byte			m_idAction;
	byte			m_idComponent;
	std::string		m_sName;
	ConfigDevice*	m_pDevice;

};

template< int idAction, int idComponent,  typename Report>
class ConfigSensor : public ConfigSensorBase
{
public:
	ConfigSensor(const std::string& sname, ConfigDevice* pDevice)
		: ConfigSensorBase( idAction, idComponent, sname, pDevice)
	{
	}

	Report* ReceiveReport( void* buff, size_t lenMax, double timeout =  A2300::TransportDci::USE_DEFAULT_TIMEOUT)
	{
		int retval = ReadTypedDataMsg( buff, lenMax, timeout);

		Report* rpt = NULL;
		if( retval > sizeof( Report))
		{
			rpt = (Report*) Dci_TypedDataRecord_GetData( (Dci_TypedDataRecord*) buff);
		}
		return rpt;
	}
};

#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(push, 1)
#endif

	//Motion Sensor Types.
	struct Motion_RawSample
	{
		short gyro[3];
		short accel[3];
		long quat[3];
		unsigned long timecode;
		short flags;
	};

	struct Motion_DataSet
	{
		short ctRecs;
		Motion_RawSample Samples[6]; //Max 6 raw samples per set.
	};

#if defined( WIN32) || defined( WIN64) || defined(LINUX) || defined(APPLE)
	#pragma pack(pop)
#endif

	typedef ConfigSensor<0,0x83, Motion_DataSet> ConfigMotion;

} /* namespace A2300 */

#endif /* CONFIGDSP_H_ */
