/*
 * DciProperty.cpp
 *
 *  Created on: Oct 21, 2013
 *      Author: mike
 */

#include "DciProperty.h"

using namespace uhd;
using namespace uhd::transport;


int DciProperty::GetProperty(  Dci_Property& prop, double timeout)
{
	// Get work buffer.
	managed_send_buffer::sptr txbuff = m_piface->GetSendDciMessageBuffer(timeout);

	//Query Identify Device.
	int len = Dci_TypedPropertiesQuery_Init(txbuff->cast<void*>(), DCI_MAX_MSGSIZE, m_idComponent, 1, &(prop.idprop), &(prop.idtype) );

	//Send the DCI command.
	m_piface->CommitDciMessageBuffer(txbuff, len, false);
	txbuff = NULL;

	//Process DCI Response.
	managed_recv_buffer::sptr rxbuff = m_piface->ReceiveDciMessage(timeout);
	if( rxbuff != NULL)
	{
		byte* buff = rxbuff->cast<byte*>();
		Dci_TypedProperties* ptp = (Dci_TypedProperties*) buff;

		//If we got back what we expected.
		if( Dci_Hdr_MatchesId( (Dci_Hdr *)buff, Dci_WcaCategoryId, Dci_TypedProperties_Id)
			&& ptp->idComponent == m_idComponent)
		{
			Dci_Property *pProp = Dci_TypedProperties_GetProperties(ptp);
			memcpy( prop.value.data, pProp->value.data, 8);
			return 0;
		}
	}
	return -1;
}

int DciProperty::SetProperty( Dci_Property& prop, double timeout)
{
	managed_send_buffer::sptr mbuff = m_piface->GetSendDciMessageBuffer(timeout);
	if( mbuff != NULL )
	{
		int len = Dci_TypedProperties_Init(mbuff->cast<void*>(), DCI_MAX_MSGSIZE, m_idComponent, 1, &prop );

		//Send the DCI command.
		m_piface->CommitDciMessageBuffer(mbuff, len, false);
		mbuff = NULL;  // Release Buffer.
		return 0;
	}
	else
		return -1;
}
