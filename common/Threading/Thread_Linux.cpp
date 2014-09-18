/* Name: Thread_Linux.cpp
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
#include <unistd.h>
#include "Mutex.hpp"
#include <string>
#include <stdio.h>

//Declare static storage for key and bInitStatics.
using namespace Ltx::Threading;

pthread_key_t Thread::m_keyCurrentThread;
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
	pthread_key_create(&m_keyCurrentThread, NULL);

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
	return (Thread*) pthread_getspecific(m_keyCurrentThread); 
}

std::string Thread::Name()
{
	char buff[64];
#ifdef NDK
	buff[0] = '\0';
#else
	::pthread_getname_np(*m_pthread, buff, sizeof(buff));
#endif
	return std::string(buff);
}
void 	Thread::Name( Ltx::System::pcstr pszName)
{
	::pthread_setname_np(*m_pthread, pszName);
}

Thread::Thread(void* (*pfncThreadMain)(void*)) 
:  m_pthread(NULL), m_pmtxSuspend(NULL), m_pfncThreadMain(pfncThreadMain),
   m_bDestroyOnExit(false), m_exitvalue(NULL)
{
	if( s_bInitStatics)
		InitStatics();
}

Thread::~Thread()
{
	if( m_pmtxSuspend != NULL)
		delete m_pmtxSuspend;
}

/**
* Method actually does the work of creating the Thread in the OS.
*/
int Thread::Start( void* pArgs)
{
	try
	{
		m_pthread = new pthread_t;
	}
	catch(std::exception &e)
	{
		std::string err("Thread::Start - Error. Instantiating pthread object\n\t");
		err += e.what();
		throw std::runtime_error(err);
	}
	catch(...)
	{
		throw std::runtime_error("Thread::Start - Error. Instantiating pthread object");
	}

	if(m_pthread == NULL)
		throw std::runtime_error("Thread::Start - Error. Instantiating pthread object");

	//Create the thread and set startup objects.
	m_pArgs = pArgs;

	int retval = pthread_create( m_pthread, NULL, Thread::__ThreadStart, this);

	return retval;
}

///<summary>
///Internal method starts all threads and ensures thread is properly initialized.
///</summary>
void* Thread::__ThreadStart( void* pThreadArgs)
{
	Thread* pThread = (Thread*) pThreadArgs;

	//Store the thread object in thread local storage to retrieve as CurrentThread.
	pthread_setspecific(m_keyCurrentThread, pThread);

#ifdef DEBUG	
	Thread* pthd = (Thread*) pthread_getspecific( m_keyCurrentThread);
	if( pthd != pthargs->pThread)
		printf("Could not store thread object in thread storage\n");
#endif	

	//Now call user thread function and let them have it!
	try
	{
		pThread->m_pfncThreadMain( pThread->m_pArgs);
	}
	catch(std::exception& e)
	{
		printf("Thread: Unhandled exception - %s\n", e.what());
		return NULL;
	}
	catch(...)
	{
		printf("Thread: Unhandled exception occurred\n");
		return NULL;
	}
	pThread->m_pArgs = NULL;

	//No longer need the thread.
	delete pThread;

	return NULL;
}

void Thread::Join( void ** value_ptr )
{
	pthread_join( *m_pthread, value_ptr);
}
void Thread::Exit( void* value)
{
	m_exitvalue = value;
	pthread_exit(&value);
}


void Thread::Resume( )
{
	if( m_pthread == NULL 
		|| m_pmtxSuspend == NULL)
		return; //nothing todo.
	else
	{
		m_pmtxSuspend->SignalLock();
	}
}

void Thread::Suspend( )
{
	if( m_pthread == 0)
		return;

	if( m_pmtxSuspend == NULL)
	{
		try
		{
			m_pmtxSuspend = new Mutex();
		}
		catch(std::exception &e)
		{
			std::string err("Thread::Suspend - Error. Instantiating mutex object\n\t");
			err += e.what();
			throw std::runtime_error(err);
		}
		catch(...)
		{
			throw std::runtime_error("Thread::Suspend - Error. Instantiating mutex object");
		}
		if(m_pmtxSuspend == NULL)
			throw std::runtime_error("Thread::Suspend - Error. Instantiating mutex object");
	}

	//Wait until signalled.
	m_pmtxSuspend->WaitLock();
}



void Thread::Sleep( int nMilliseconds )
{
	timespec spec;
	spec.tv_sec  = nMilliseconds/1000;
	spec.tv_nsec = (nMilliseconds -(spec.tv_sec*1000))*1000000;
	if( spec.tv_sec > 0 && spec.tv_nsec == 0)
		sleep( spec.tv_sec);
	else
		nanosleep(&spec, NULL);
	//	sleep( nMilliseconds / 1000);
}
