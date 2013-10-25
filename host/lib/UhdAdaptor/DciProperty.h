// Name:  DciProperty.h
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

#ifndef DCIPROPERTY_H_
#define DCIPROPERTY_H_


#include "Dci/DciMsg.h"
#include "Dci/DciConversation.h"
#include "Dci/InfrastructureMsgs.h"
#include "Dci/WcaMsgs.h"
#include "Dci/StandardMsgs.h"

#include "a2300_iface.hpp"

/**
 * Helper class takes the pain out of setting and getting component properties.
 */
class DciProperty
{
public:
	DciProperty(byte idComponent, A2300_iface::sptr piface, double timeout)
	:	 m_idComponent( idComponent), m_piface(piface ), m_timeout( timeout)
	{
	}
	~DciProperty();

	template< typename T, byte _idType> int GetProperty( byte idProp, T& value)
	{
		Dci_Property prop;
		prop.idtype = _idType;
		prop.idprop = idProp;

		int retval = GetProperty( prop, m_timeout);
		if( retval == 0)
		{
			switch( _idType)
			{
			case PT_BYTE: 		value = prop.value.vByte; break;
			case PT_UINT16: 	value = prop.value.vUint16; break;
			case PT_INT16: 		value = prop.value.vInt16; break;
			case PT_UINT32: 	value = prop.value.vUint32; break;
			case PT_INT32: 		value = prop.value.vInt32; break;
			case PT_FLOAT: 		value = prop.value.vFloat; break;
			case PT_DOUBLE: 	value = prop.value.vDouble; break;
			case PT_UINT64: 	memcpy(&value, prop.value.data,  8); break;
			case PT_INT64: 		memcpy(&value, prop.value.data, 8); break;
			}

		}

		return retval;

	}
	template< typename T, byte _idType> int SetProperty( byte idProp, T& value)
	{
		Dci_Property prop;
		prop.idtype = _idType;
		prop.idprop = idProp;
		switch( _idType)
		{
		case PT_BYTE: 		prop.value.vByte 	= value; break;
		case PT_UINT16: 	prop.value.vUint16 	= value; break;
		case PT_INT16: 		prop.value.vInt16 	= value; break;
		case PT_UINT32: 	prop.value.vUint32 	= value; break;
		case PT_INT32: 		prop.value.vInt32 	= value; break;
		case PT_FLOAT: 		prop.value.vFloat 	= value; break;
		case PT_DOUBLE: 	prop.value.vDouble 	= value; break;
		case PT_UINT64: 	memcpy(prop.value.data, &value, 8); break;
		case PT_INT64: 		memcpy(prop.value.data, &value, 8); break;
		}
		return SetProperty( prop, m_timeout );
	}


	int GetProperty( Dci_Property& prop, double timeout);
	int SetProperty( Dci_Property& prop, double timeout);





private:
	byte			  m_idComponent;
	A2300_iface::sptr m_piface;
	double			  m_timeout;
};




#endif /* DCIPROPERTY_H_ */
