/* Name: DataStreamLogger.h
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
#pragma once

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN 
#include<windows.h>
#define SLEEP_SEC(a)  Sleep((a)*1000)
#else
#include <unistd.h>
#define SLEEP_SEC(a) sleep((a))
#endif

#include <A2300/ConfigDevice.h>
#include <A2300/ConfigDduc.h>
#include <System/ArgParser.hpp>
#include <Threading/ThreadSafeQueue.hpp>

using namespace A2300;


class DataStreamLogger
{
private:
	std::string			m_sId;
	std::string			m_sFilename;
	int					m_idxRfPath;
	byte				m_byteGain;
	double				m_dFreqRf;
	double				m_dBandwidth;
	double				m_dSampRate;

	ConfigDevice*		m_pDevice;
	ConfigRf*			m_pRf;
	ConfigDduc*			m_pDduc;
	BulkDataPort*		m_pPort;
	BulkDataPort::TransferContextList m_listContext;
	A2300::Threading::ThreadSafeQueue< BulkDataPort::TransferContext*> m_queueFramesToWrite;

	//Runtime variables.
	size_t				m_BytesPerSample;
	size_t				m_sizeFrame;
	size_t				m_iMaxBuffs;
	size_t				m_ctFramesProcessed;
	size_t				m_totalFramesToProcess;
	size_t				m_framesPerSec;
	char				m_chTick;
	byte*				m_pbuff;
	FILE*				m_file;
	bool				m_bIsRunning;
	bool 				m_bSpeedTest;

public:
	DataStreamLogger(size_t bytesPerSample, size_t sizeFrame, size_t iMaxFrames, char chTick);
	~DataStreamLogger();
	void RegisterArgs( std::string sId, ArgParser& args);
	int Init( ArgParser& args, ConfigDevice* m_pDevice);
	void DisplayConfiguration();
	int Start(size_t msecDur, bool bIsMaster);
	int CheckStatus();
	void Stop();
	void Terminate();

private:
	static void OnFrameReady( void* pobj, BulkDataPort::TransferContext* pctxt);
	static void* WriteDataThreadFunc( void* arg);

};
