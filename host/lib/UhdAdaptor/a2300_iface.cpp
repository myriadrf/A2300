/** Name: a2300_iface.cpp
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

#include "a2300_iface.hpp"
#include <uhd/utils/log.hpp>
#include <uhd/exception.hpp>
#include <uhd/utils/byteswap.hpp>
#include <boost/format.hpp>

#include <stdexcept>
#include <iomanip>

#include "Dci/DciMsg.h"
#include "Dci/DciConversation.h"
#include "Dci/InfrastructureMsgs.h"
#include "Dci/WcaMsgs.h"
#include "Dci/StandardMsgs.h"

using namespace uhd;
using namespace uhd::transport;

class A2300_iface_impl : public A2300_iface
{
public:
    /*******************************************************************
     * Constructors
     ******************************************************************/
	A2300_iface_impl(
			uhd::transport::usb_zero_copy::sptr transport,
			const int    idConversation)
	: _transport(transport)
   {
       // Initialize the DCI context...
       Dci_Conversation_Init( &m_conv, idConversation);
   }

	~A2300_iface_impl()
	{
		/* NOP */
	}


	/*
	 * Board Alive Test.
	 */
	void SendIdleMsg(double timeout)
	{
		// Get work buffer.
		managed_send_buffer::sptr txBuff = GetSendDciMessageBuffer(timeout);
		if( txBuff != NULL )
		{
			int len = Dci_IdleMsg_Init(txBuff->cast<void *>());
			CommitDciMessageBuffer(txBuff, len, true);
			txBuff = NULL;
		}
	}

	/*
	 * DCI Message Receive.
	 */
	managed_recv_buffer::sptr ReceiveDciMessage( double timeout)
    {
		managed_recv_buffer::sptr mbuff = _transport->get_recv_buff(timeout);
		if( mbuff.get() != NULL)
    	{
    		// Process the conversation data
    		Dci_Conversation_UpdateState( &m_conv, mbuff->cast<byte*>(), mbuff->size());
    	}
    	return mbuff;
    }

	/*
	 * DCI Send Message.
	 */
	managed_send_buffer::sptr GetSendDciMessageBuffer( double timeout)
	{
		return _transport->get_send_buff( timeout);
	}

	void CommitDciMessageBuffer( managed_send_buffer::sptr mbuff, size_t msgLen, bool bAck )
	{
		if( mbuff.get() != NULL)
		{
	    	Dci_Conversation_PrepareMessageHdr( &m_conv, mbuff->cast<Dci_Hdr*>( ), bAck);
	    	mbuff->commit(msgLen);
		}
	}

	/*
	 * Open Testing Routine.
	 */
	void Test()
	{
		volatile int iSendFrameSize = _transport->get_send_frame_size();
		volatile int iSendCount = _transport->get_num_send_frames();

		volatile int iReceiveFrameSize = _transport->get_recv_frame_size();
		volatile int iReceiveCount = _transport->get_num_recv_frames();

		printf("Send Frame/Count: %d/%d... Receive Frame/Count: %d/%d\n",
				iSendFrameSize, iSendCount, iReceiveFrameSize, iReceiveCount);
	}

private:
    //Create a DCI Conversation, Assumes little endian.
    Dci_Conversation m_conv;
	uhd::transport::usb_zero_copy::sptr _transport;
};

/***********************************************************************
 * Public Make Function
 **********************************************************************/

/*!
 * Make a implementation layer interface
 *
 * \param handle a device handle that uniquely identifying the device
 * \param interface an integer specifiying an interface number
 * \param recv_endpoint an integer specifiying an IN endpoint number
 * \param send_endpoint an integer specifiying an OUT endpoint number
 * \param hints optional parameters to pass to the underlying transport
 * \return a new zero copy usb object
 */
A2300_iface::sptr A2300_iface::make(
	uhd::transport::usb_zero_copy::sptr transport,
	const int    idConversation)
{
    return sptr(new A2300_iface_impl(transport, idConversation ) );
}



