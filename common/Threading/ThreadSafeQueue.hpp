/* Name: ThreadSafeQueue.h
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

#ifndef THREADSAFEQUEUE_H_
#define THREADSAFEQUEUE_H_
#include <queue>
#include "Mutex.hpp"

namespace A2300 
{
	namespace Threading 
	{

		template< class T>
		class ThreadSafeQueue {
		private:
			std::queue<T>  m_queue;
			A2300::Threading::Mutex m_mtx;

		public:
			ThreadSafeQueue() {
				// TODO Auto-generated constructor stub

			}

			~ThreadSafeQueue() {
				// TODO Auto-generated destructor stub
			}

			size_t size()
			{
				size_t size;
				m_mtx.Lock();
				size = m_queue.size();
				m_mtx.Unlock();
				return size;
			}

			void Push( T val)
			{
				m_mtx.Lock();
				m_queue.push(val);
				m_mtx.Signal();
				m_mtx.Unlock();

			}

			bool Pop(T* pval, unsigned long timeout)
			{
				bool bHasVal = false;

				m_mtx.Lock();
				if( m_queue.empty())
				{
					m_mtx.WaitTimeout(timeout);

				}

				//Test to see if any waiting.
				if( !m_queue.empty())
				{
					*pval = m_queue.front();
					m_queue.pop();
					bHasVal = true;
				}
				m_mtx.Unlock();

				return bHasVal;
			}

			};
	}
}
#endif /* THREADSAFEQUEUE_H_ */
