/* Name: Mutex.cpp
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
#if defined(WIN32) || defined(WIN64)
#include <Windows.h>
#include <process.h>
#elif defined(LINUX)
#include "../System/std/TimeSpecUtil.h"
#endif

#include "Mutex.hpp"
#include <time.h>
#include <errno.h>
#include <stdexcept>
#include <string.h>

A2300::Threading::Mutex::Mutex()
    : m_counter(0), m_ctWait(0)
{
#ifdef LINUX
	memset(&m_mutex, 0, sizeof(pthread_mutex_t));
	memset(&m_cond, 0, sizeof(pthread_cond_t));

	if( pthread_mutex_init( &m_mutex, NULL) < 0)
		throw std::runtime_error("Could not initialize Mutex.");

	if( pthread_cond_init( &m_cond, NULL) < 0)
		throw std::runtime_error("Could not initialize Mutex signalling object.");
#elif defined(WIN32) || defined(WIN64)
	InitializeCriticalSection( &m_cs);
	InitializeConditionVariable( (PCONDITION_VARIABLE)&m_cond );
#endif
}

A2300::Threading::Mutex::~Mutex()
{
#ifdef LINUX
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_cond);
#elif defined(WIN32) || defined(WIN64)
		DeleteCriticalSection(& m_cs);
#endif
}

void A2300::Threading::Mutex::Wait()
{
	m_ctWait++;
#ifdef LINUX
	int retval =1;
	while( retval != 0)
	{
		retval = pthread_cond_wait( &m_cond, &m_mutex);
	}
#elif defined(WIN32) || defined(WIN64)
	// Windows requires critical section.
	SleepConditionVariableCS( (PCONDITION_VARIABLE)&m_cond, &m_cs, INFINITE );
#endif
	m_ctWait--;
}


void A2300::Threading::Mutex::WaitLock()
{
#ifdef LINUX
	int retval =1;
	while( retval != 0)
	{
		pthread_mutex_lock( &m_mutex);
		m_ctWait++; //TODO DON'T LIKE THIS AS IT THRASHES WAIT COUNT WHILE WAITING.
		retval = pthread_cond_wait( &m_cond, &m_mutex);
		m_ctWait--;
		pthread_mutex_unlock( &m_mutex);
	}
#elif defined(WIN32) || defined(WIN64)
	// Windows requires critical section.
	EnterCriticalSection(&m_cs);
	m_ctWait++;
	SleepConditionVariableCS( (PCONDITION_VARIABLE)&m_cond, &m_cs, INFINITE );
	m_ctWait--;
	LeaveCriticalSection(&m_cs);
#endif
}



int A2300::Threading::Mutex::WaitTimeoutLock( unsigned long msec)
{
	int retval =0;
#ifdef LINUX
	//Construct absolute time to wait.
	timespec timeout;
	clock_gettime( CLOCK_REALTIME, &timeout);

	TimeSpecUtil::AddMsec(&timeout, msec);

	//Wait on condition until true or timed out.
	retval =1;
	//while( !(retval == 0 || retval == ETIMEDOUT  || retval == EINVAL))
	//{
		pthread_mutex_lock( &m_mutex);
		m_ctWait++;
		retval = pthread_cond_timedwait( &m_cond, &m_mutex, &timeout);
		m_ctWait--;
		pthread_mutex_unlock( &m_mutex);
	//}
#elif defined(WIN32) || defined(WIN64)
	EnterCriticalSection(&m_cs);
	m_ctWait++;
	retval = (SleepConditionVariableCS((PCONDITION_VARIABLE)&m_cond, &m_cs, msec) == TRUE);
	m_ctWait--;
	LeaveCriticalSection(&m_cs);
#endif
	return retval;
}

int A2300::Threading::Mutex::WaitTimeout( unsigned long msec)
{
	int retval = 0;
	m_ctWait++;
#ifdef LINUX
	//Construct absolute time to wait.
	timespec timeout;
	clock_gettime( CLOCK_REALTIME, &timeout);

	TimeSpecUtil::AddMsec(&timeout, msec);

	//Wait on condition until true or timed out.
	retval =1;
	//while( retval != 0 || retval == ETIMEDOUT)
	//{
		retval = pthread_cond_timedwait( &m_cond, &m_mutex, &timeout);
	//}
#elif defined(WIN32) || defined(WIN64)
	retval = (SleepConditionVariableCS((PCONDITION_VARIABLE)&m_cond, &m_cs, msec) == TRUE);
#endif
	m_ctWait--;
	return retval;
}


