/* Name: Mutex.h
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
#ifndef Mutex_h_h
#define Mutex_h_h



#ifdef LINUX
	#include <pthread.h>
#elif defined(WIN32) || defined(WIN64)
	#include <Windows.h>
#endif

namespace A2300
{
	namespace Threading
	{
		/**
		* Wrapper on Mutex implementation for Critical Sections.  Makes it easy to wrap section of codes that must be thread safe.
		*/
		class Mutex
		{
		public:
			Mutex();
			~Mutex();

			/**
			 * Lock
			 * Acquire a lock on the specified mutex.
			 * If the mutex has already been acquired by another thread, the calling thread
			 * will be blocked until the lock is released.
			 */
			int Lock( )
			{
		#ifdef LINUX
				pthread_mutex_lock( &m_mutex);
		#elif defined(WIN32) || defined(WIN64)
				EnterCriticalSection(&m_cs);
		#endif
				m_counter++;
				return m_counter;
			}

			//Standard Unlock function
			void Unlock( )
			{
				if( m_counter <= 0)
					return;

				m_counter--;
		#ifdef LINUX
				pthread_mutex_unlock(&m_mutex);
		#elif defined(WIN32) || defined(WIN64)
				LeaveCriticalSection(&m_cs);
		#endif
			}


			/**
			 * Issue a Signal.  Must surround this with Luck/Unlock calls to ensure thread synchronization.
			 */
			int Signal()
			{
				int iretval=0;
		#ifdef LINUX
				iretval = pthread_cond_signal( &m_cond);
		#elif defined(WIN32) || defined(WIN64)
				WakeConditionVariable( &m_cond);
		#endif
				return iretval;
			}
			/**
			 * Implements same function as Signal but with Synchronous Lock/Unlock surrounding the signal condition.
			 * Don't use if handling lock/unlock externally; use Signal instead.
			 */
			int SignalLock()
			{
				int iretval=0;
		#ifdef LINUX
				pthread_mutex_lock( &m_mutex);
				iretval = pthread_cond_signal( &m_cond);
				pthread_mutex_unlock( &m_mutex);
		#elif defined(_WIN32)
				EnterCriticalSection(&m_cs);
				WakeConditionVariable( &m_cond);
				LeaveCriticalSection(&m_cs);
		#endif
				return iretval;
			}

			/**
			 * Issue a Signal for all waiting threads.  Must surround this with Luck/Unlock calls to ensure thread synchronization.
			 */
			int SignalAll()
			{
				int iretval=0;
		#ifdef LINUX
				iretval = pthread_cond_broadcast(&m_cond);
		#elif defined(WIN32) || defined(WIN64)
				WakeAllConditionVariable( &m_cond);
		#endif
				return iretval;				
			}


			/**
			 * Wait for a Signal condition indefinitely.  Must surround this with Luck/Unlock calls to ensure thread synchronization.
			 */
			void Wait();

			/**
			 * Wait for a Signal condition for the specified milliseconds.  Must surround this with Luck/Unlock calls to ensure thread synchronization.
			 */
			int WaitTimeout( unsigned long msec);
	
			/**
			 * Implements same function as Wait but with Synchronous Lock/Unlock surrounding the wait condition.
			 * Don't use if handling lock/unlock externally; use Wait instead.
			 */
			void WaitLock();

			/**
			 * Implements same function as WaitTimeout but with Synchronous Lock/Unlock surrounding the wait condition.
			 * Don't use if handling lock/unlock externally; use WaitTimeout instead.
			 */
			int WaitTimeoutLock( unsigned long msec);


			/**
			* Returns the count of thread currently waiting on this mutex.
			*/
			int WaitCount() const { return m_ctWait;}

			/**
			 * Declarations.
			 */
		private:
			int m_counter;
			int m_ctWait;

		#ifdef LINUX
			pthread_mutex_t m_mutex;
			pthread_cond_t  m_cond;
		#elif defined(WIN32) || defined(WIN64)
			CRITICAL_SECTION m_cs;
			CONDITION_VARIABLE m_cond;
		#endif

		};
	}
}
#endif
