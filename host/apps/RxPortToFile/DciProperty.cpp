// Name:  DciProperty.cpp
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

#include "DciProperty.h"
#include <string.h>
int DciProperty::GetProperty(  Dci_Property& prop, double timeout)
{
	byte buff[DCI_MAX_MSGSIZE];

	//Query Identify Device.
	int len = Dci_TypedPropertiesQuery_Init(buff, DCI_MAX_MSGSIZE, m_idComponent, 1, &(prop.idprop), &(prop.idtype) );

	Dci_Conversation_PrepareMessageHdr( m_pConv, (Dci_Hdr*) buff, false);

	//Send the DCI command.
	m_pPort->Write(buff, len, timeout);

	//Process DCI Response.
	int ctRead = m_pPort->Read(buff, DCI_MAX_MSGSIZE, timeout);

	if( ctRead > 0)
	{
		Dci_Conversation_UpdateState( m_pConv, buff, ctRead);

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
	byte buff[DCI_MAX_MSGSIZE];
	int len = Dci_TypedProperties_Init(buff, DCI_MAX_MSGSIZE, m_idComponent, 1, &prop );

	Dci_Conversation_PrepareMessageHdr( m_pConv, (Dci_Hdr*) buff, true);

	//Send the DCI command.
	int wresult = m_pPort->Write(buff, len, timeout);

	//Wait for Ack
	//Process DCI Response.
	int ctRead = m_pPort->Read(buff, DCI_MAX_MSGSIZE, timeout);

	return (ctRead > 0 ) ? wresult : -1;

}


template <> int DciProperty::GetProperty( int idProp, byte& value)
{
	Dci_Property prop; 	prop.idtype = PT_BYTE;	prop.idprop = idProp;
	int retval = GetProperty( prop, m_timeout);
	value = prop.value.vByte;
	return retval;
}

template <> int DciProperty::GetProperty( int idProp, uint16& value)
{
	Dci_Property prop; 	prop.idtype = PT_UINT16;	prop.idprop = idProp;
	int retval = GetProperty( prop, m_timeout);
	value = prop.value.vUint16;
	return retval;
}

template <> int DciProperty::GetProperty( int idProp, int16& value)
{
	Dci_Property prop; 	prop.idtype = PT_INT16;	prop.idprop = idProp;
	int retval = GetProperty( prop, m_timeout);
	value = prop.value.vInt16;
	return retval;
}

template <> int DciProperty::GetProperty( int idProp, uint32& value)
{
	Dci_Property prop; 	prop.idtype = PT_UINT32;	prop.idprop = idProp;
	int retval = GetProperty( prop, m_timeout);
	value = prop.value.vUint32;
	return retval;
}

template <> int DciProperty::GetProperty( int idProp, int32& value)
{
	Dci_Property prop; 	prop.idtype = PT_INT32;	prop.idprop = idProp;
	int retval = GetProperty( prop, m_timeout);
	value = prop.value.vInt32;
	return retval;
}

template <> int DciProperty::GetProperty( int idProp, float& value)
{
	Dci_Property prop; 	prop.idtype = PT_FLOAT;	prop.idprop = idProp;
	int retval = GetProperty( prop, m_timeout);
	value = prop.value.vFloat;
	return retval;
}

template <> int DciProperty::GetProperty( int idProp, double& value)
{
	Dci_Property prop; 	prop.idtype = PT_DOUBLE;	prop.idprop = idProp;
	int retval = GetProperty( prop, m_timeout);
	value = prop.value.vDouble;
	return retval;
}
/*
template <> int DciProperty::GetProperty( int idProp, uint64& value)
{
	Dci_Property prop; 	prop.idtype = PT_UINT64;	prop.idprop = idProp;
	int retval = GetProperty( prop, m_timeout);
	value = prop.value.vUint64;
	return retval;
}

template <> int DciProperty::GetProperty( int idProp, int64& value)
{
	Dci_Property prop; 	prop.idtype = PT_INT64;	prop.idprop = idProp;
	int retval = GetProperty( prop, m_timeout);
	value = prop.value.vInt64;
	return retval;
}
*/


template <> int DciProperty::SetProperty( int idProp, byte value)
{
	Dci_Property prop;	prop.idtype = PT_BYTE;
	prop.idprop = idProp;
	prop.value.vByte = value;
	return SetProperty( prop, m_timeout );
}

template <> int DciProperty::SetProperty( int idProp, uint16 value)
{
	Dci_Property prop;	prop.idtype = PT_UINT16;
	prop.idprop = idProp;
	prop.value.vUint16 = value;
	return SetProperty( prop, m_timeout );
}


template <> int DciProperty::SetProperty( int idProp, int16 value)
{
	Dci_Property prop;	prop.idtype = PT_INT16;
	prop.idprop = idProp;
	prop.value.vInt16 = value;
	return SetProperty( prop, m_timeout );
}

template <> int DciProperty::SetProperty( int idProp, uint32 value)
{
	Dci_Property prop;	prop.idtype = PT_UINT32;
	prop.idprop = idProp;
	prop.value.vUint32 = value;
	return SetProperty( prop, m_timeout );
}


template <> int DciProperty::SetProperty( int idProp, int32 value)
{
	Dci_Property prop;	prop.idtype = PT_INT32;
	prop.idprop = idProp;
	prop.value.vInt32 = value;
	return SetProperty( prop, m_timeout );
}

template <> int DciProperty::SetProperty( int idProp, float value)
{
	Dci_Property prop;	prop.idtype = PT_FLOAT;
	prop.idprop = idProp;
	prop.value.vFloat = value;
	return SetProperty( prop, m_timeout );
}


template <> int DciProperty::SetProperty( int idProp, double value)
{
	Dci_Property prop;	prop.idtype = PT_DOUBLE;
	prop.idprop = idProp;
	prop.value.vDouble= value;
	return SetProperty( prop, m_timeout );
}
/*
template <> int DciProperty::SetProperty( int idProp, uint64 value)
{
	Dci_Property prop;	prop.idtype = PT_UINT64;
	prop.idprop = idProp;
	prop.value.vUint64 = value;
	return SetProperty( prop, m_timeout );
}

template <> int DciProperty::SetProperty( int idProp, int64 value)
{
	Dci_Property prop;	prop.idtype = PT_INT64;
	prop.idprop = idProp;
	prop.value.vInt64 = value;
	return SetProperty( prop, m_timeout );
}
*/

