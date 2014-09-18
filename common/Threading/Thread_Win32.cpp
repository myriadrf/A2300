/* Name: Thread_Win32.cpp
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
#include <stdexcept>
#include "Thread.hpp"
#include <time.h>
#include "Mutex.hpp"
#include <string>
#include <stdio.h>
#include <assert.h>

//Declare static storage for key and bInitStatics.
using namespace A2300::Threading;

DWORD		  Thread::_tidCurrentThread = 1;
bool   		  Thread::s_bInitStatics = true;

///<summary>
/// Thread Start arguments pass necessary information to initialize thread object.
///</summary>
struct ThreadArgs
{
	Thread* pThread;
	void* pArgs;
};

void   Thread::InitStatics()
{
	//Create Key for managing threads
	_tidCurrentThread = 1;//TlsAlloc();
	s_bInitStatics = false;
}

Thread* Thread::ThreadCreate( bool bStart, void* (*pfncThreadMain)(void*), void* pArgs )
{
	Thread* pthread;
	try
	{
		pthread = new Thread(pfncThreadMain);
	}
	catch(std::exception &e)
	{
		std::string err("Thread::Start - Error. Instantiating Thread object\n\t");
		err += e.what();
		throw std::runtime_error(err);
	}
	catch(...)
	{
		throw std::runtime_error("Thread::Start - Error. Instantiating Thread object");
	}
	if( bStart)
		pthread->Start(pArgs);
	return pthread;
}

/**
* Returns the current thread object.
*/
Thread* Thread::CurrentThread( )
{
	Thread* pthread = (Thread*) TlsGetValue( _tidCurrentThread);

	//If this goes off.  The thread was not created by this class.
	assert( pthread != NULL);
	return pthread;
}



Thread::Thread(void* (*pfncThreadMain)(void*)) 
:  _handle(NULL), _tid(0), m_pfncThreadMain(pfncThreadMain),
   m_bDestroyOnExit(false), m_exitvalue(NULL)
{
	if( s_bInitStatics)
		InitStatics();
}

Thread::~Thread()
{
}

/**
* Method actually does the work of creating the Thread in the OS.
*/
int Thread::Start( void* pArgs)
{
	m_pArgs = pArgs;

	//Run immediately.
    _handle = CreateThread (
        0, // Security attributes
        0, // Stack size
		Thread::__ThreadStart,
        this,
        0,
        &_tid);
	return _handle != NULL ? 0 : -1;
}

///<summary>
///Internal method starts all threads and ensures thread is properly initialized.
///</summary>
DWORD Thread::__ThreadStart( void* pThreadArgs)
{
	Thread * pThread = (Thread*) pThreadArgs;

	//Store the thread object in thread local storage to retrieve as CurrentThread.
	TlsSetValue( _tidCurrentThread, pThread);

	//Now call user thread function and let them have it!
	try
	{
		pThread->m_pfncThreadMain( pThread->m_pArgs);
	}
	catch(std::exception e)
	{
		printf("Thread: Unhandled exception - %s\n", e.what());
		return (DWORD)-1;
	}
	catch(...)
	{
		printf("Thread: Unhandled exception occurred\n");
		return (DWORD)-1;
	}
	pThread->m_pArgs = NULL;

	//No longer need the thread.
	delete pThread;

	return 0;
}

void Thread::Join( void ** /*value_ptr*/ )
{
	//NOT Supported.
	assert(FALSE);
}
void Thread::Exit( void* value)
{
	m_exitvalue = value;
	ExitThread( 0);
}


void Thread::Resume( )
{
	if( _handle == NULL)
		return;

	ResumeThread( _handle);
}

void Thread::Suspend( )
{
	if( _handle == NULL)
		return;

	SuspendThread(_handle);
}



void Thread::Sleep( int nMilliseconds )
{
	::Sleep( nMilliseconds);
}
