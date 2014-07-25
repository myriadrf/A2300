/* Name: DataStreamWriter.h
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
#include <list>
using namespace A2300;


/**
* Structure provides a simple smart buffer implementation.
*/ 
struct TransferBuffer
{
	TransferBuffer( A2300::BulkDataPort *pPort, size_t sizeFrame /*, bool bReadTransfer*/)
	{
		pbuffer = new byte[sizeFrame];

		//if( bReadTransfer)
		//	pctxt = pPort->CreateReadTransferContext(  pbuffer, sizeFrame);
		//else
		pctxt = pPort->CreateWriteTransferContext( pbuffer, sizeFrame);
		pctxt->pObj = this; // Stash a pointer to ourself.
	}

	~TransferBuffer()
	{
		pctxt->Destroy();
		delete pbuffer;
	}

	/**
	* Submits the buffer for transmit
	*/
	void Submit(int size)
	{
		pctxt->Submit( size);
	}

	byte* pbuffer;
	BulkDataPort::TransferContext* pctxt;
};

typedef std::list< TransferBuffer*> TransferBufferList;


/*
* Class implements TX streaming functionality from a file.
*/
class DataStreamWriter
{
private:
	std::string			m_sId;
	std::string			m_sFilename;
	int					m_idxRfPath;
	byte				m_byteGain;
	double				m_dFreqRf;
	double				m_dFreqIf;
	double				m_dBandwidth;
	double				m_dSampRate;

	ConfigDevice*		m_pDevice;
	ConfigRf*			m_pRf;
	ConfigDduc*			m_pDduc;
	BulkDataPort*		m_pPort;
	TransferBufferList  m_listTxBuffers;

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

public:
	DataStreamWriter(size_t bytesPerSample, size_t sizeFrame, size_t iMaxFrames, char chTick);
	~DataStreamWriter();
	void RegisterArgs( std::string sId, ArgParser& args);
	int Init( ArgParser& args, ConfigDevice* m_pDevice);
	void DisplayConfiguration();
	int Start(size_t msecDur, bool bIsMaster);
	int CheckStatus();
	void Stop();
	void Terminate();
	BulkDataPort*  Port() { return m_pPort;}


private:
	bool   GetDataAndSubmit( TransferBuffer* pBuff);
	static void OnFrameSent( void* pobj, BulkDataPort::TransferContext* pctxt);

};
