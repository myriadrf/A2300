/* Name: Thread.h
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

#ifndef Thread_h_h
#define Thread_h_h

#ifdef LINUX
	#include <pthread.h>
#elif defined(WIN32) || defined(WIN64)
	#include <Windows.h>
#endif

namespace A2300
{
	namespace Threading
	{
		class Mutex;

		/**
		* Class wraps threading for OS providing basic primitive for thread management.
		* A reference to the thread object is stored in the thread context. so we can retrieve it at any time.
		*/
		class Thread
		{
		private:
			static bool   s_bInitStatics;
			static void   InitStatics();

		#ifdef LINUX
			static pthread_key_t m_keyCurrentThread;
		#elif defined(WIN32) || defined(WIN64)
			static DWORD _tidCurrentThread;
		#endif

		private:	
		#ifdef LINUX
			pthread_t* m_pthread;
			Mutex*     m_pmtxSuspend;
		#elif defined(WIN32) || defined(WIN64)
			HANDLE _handle;	
			DWORD  _tid;
		#endif

		private:
			Thread(void*(*pfncThreadMain)(void*));
			~Thread();
			void*(*m_pfncThreadMain)(void*);

			bool m_bDestroyOnExit;

		public:

			/**
			* Returns the current thread object.
			*/
			static Thread* CurrentThread( );
			//MBM: Sleep is a thread oriented function and needs to be static.
			//static void    SleepCurrent( int nMilliseconds)
			//{ 
			//	Sleep( nMilliseconds);
			//}
			void Join( void ** value_ptr );
			void Exit( void* value = NULL);
			void Resume( );
			static void Sleep( int nMilliseconds );

		#ifndef _WIN32
			std::string Name();
			void 		Name( Ltx::System::pcstr pszName);
			void 		Name( std::string sName) { Name( sName.c_str());}
		#else
			// Workaround for Windows Thread Naming.
			std::string m_name;
			std::string Name() { return m_name;}
			void Name(const char* pszName) { m_name = *pszName; }
			void Name(const std::string& sName) { m_name = sName; }
		#endif

			/**
			* Method actually does the work of creating the Thread in the OS.
			*/
			int Start( void* pArgs);
			bool IsStarted() 
			{ 
		#ifdef LINUX
				return m_pthread != NULL;
		#elif defined(WIN32) || defined(WIN64)
				return _handle != NULL;
		#endif
			}
			void Suspend( );
			
			bool DestroyOnExit() const {return m_bDestroyOnExit;}
			void DestroyOnExit( bool bEnable) { m_bDestroyOnExit = bEnable;}
			void* ExitValue() { return m_exitvalue;}

			static Thread* ThreadCreate( bool bStart, void* (*pfncThreadMain)(void*), void* pArgs );
		private:
			void* m_pArgs;
			void* m_exitvalue;
			///<summary>
			///Internal method starts all threads and ensures thread is properly initialized.
			///</summary>
#ifdef LINUX
			static void* __ThreadStart( void* pThreadArgs);
#elif defined(WIN32) || defined(WIN64)
			static  DWORD WINAPI __ThreadStart( void* pThreadArgs);
#endif

		};
	}
}
#endif
