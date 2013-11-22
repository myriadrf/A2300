// Name:  TransportDci.h
//
// Copyright 2013 Loctronix Corporation
// http://www.loctronix.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TransportDci_H_
#define TransportDci_H_

#include <Dci/DciMsg.h>
#include <Dci/DciConversation.h>
#include <Dci/InfrastructureMsgs.h>
#include <Dci/WcaMsgs.h>
#include <Dci/StandardMsgs.h>

#include <A2300/BulkDataPort.h>

#include <stdexcept>

namespace A2300
{

	/**
	 * Class implements functionality for sending and receiving DCI messages with the A2300.
	 */
	class TransportDci
	{
	public:
		static const double DEFAULT_TIMEOUT_VALUE;
		static const int USE_DEFAULT_TIMEOUT;
	public:
		TransportDci();

		virtual ~TransportDci();

		int Init(A2300::BulkDataPort* pPort, double timeout, Dci_Conversation* pconv);
		void Term();

		/**
		 * Send a DCI Message
		 */
		int SendMsg( byte* pmsg, size_t lenMsg, bool bRequestAck, double timeout = USE_DEFAULT_TIMEOUT);

		/**
		 * Receive DCI Message.
		 */
		int ReceiveMsg( byte* pmsg, size_t lenMax, double timeout = USE_DEFAULT_TIMEOUT);

		template< typename T> int GetProperty( int idComponent,  int idProp, T& value)
		{
			throw std::runtime_error("DCI Property type not supported");
		}

		template< typename T> int SetProperty( int idComponent,  int idProp, T value)
		{
			throw std::runtime_error("DCI Property type not supported");
		}

		int GetProperty( int idComponent, Dci_Property& prop, double timeout);
		int SetProperty( int idComponent, Dci_Property& prop, double timeout);

		Dci_Conversation* Conversation() { return m_pConv;}
		BulkDataPort*    Port() { return m_pPort;}
		double			 DefaultTimeout() const { return m_timeout;}
	private:
		//TODO implement asynchronous messaging.
		//void OnMessageReceived( BulkDataPort::TransferContext* pctxt);

	private:
		A2300::BulkDataPort*  m_pPort;
		double			  	 m_timeout;
		Dci_Conversation* 	 m_pConv;
		//byte*				 m_pbuff;
	};

	template <> int TransportDci::GetProperty( int idComponent,  int idProp, byte& value);
	template <> int TransportDci::GetProperty( int idComponent,  int idProp, uint16& value);
	template <> int TransportDci::GetProperty( int idComponent,  int idProp, int16& value);
	template <> int TransportDci::GetProperty( int idComponent,  int idProp, uint32& value);
	template <> int TransportDci::GetProperty( int idComponent,  int idProp, int32& value);
	template <> int TransportDci::GetProperty( int idComponent,  int idProp, float& value);
	template <> int TransportDci::GetProperty( int idComponent,  int idProp, double& value);
	/*
	template <> int TransportDci::GetProperty( int idComponent,  int idProp, uint64& value);
	template <> int TransportDci::GetProperty( int idComponent,  int idProp, int64& value);
	*/

	template <> int TransportDci::SetProperty( int idComponent,  int idProp, byte value);
	template <> int TransportDci::SetProperty( int idComponent,  int idProp, uint16 value);
	template <> int TransportDci::SetProperty( int idComponent,  int idProp, int16 value);
	template <> int TransportDci::SetProperty( int idComponent,  int idProp, uint32 value);
	template <> int TransportDci::SetProperty( int idComponent,  int idProp, int32 value);
	template <> int TransportDci::SetProperty( int idComponent,  int idProp, float value);
	template <> int TransportDci::SetProperty( int idComponent,  int idProp, double value);
	/*
	template <> int TransportDci::SetProperty( int idComponent,  int idProp, uint64 value);
	template <> int TransportDci::SetProperty( int idComponent,  int idProp, int64 value);
	*/
}

#endif /* TransportDci_H_ */
