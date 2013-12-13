/** Name: DciProperty.h
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
	~DciProperty() {}

	template< typename T> int GetProperty( byte /* idProp */, T& /* value */)
	{
		throw std::runtime_error("DCI Property type not supported");
	}

	template< typename T> int SetProperty( byte /* idProp */, T /* value */)
	{
		throw std::runtime_error("DCI Property type not supported");
	}

	int GetProperty( Dci_Property& prop, double timeout);
	int SetProperty( Dci_Property& prop, double timeout);

private:
	byte			  m_idComponent;
	A2300_iface::sptr m_piface;
	double			  m_timeout;
};


template <> int DciProperty::GetProperty( byte idProp, byte& value);

template <> int DciProperty::GetProperty( byte idProp, uint16& value);

template <> int DciProperty::GetProperty( byte idProp, int16& value);

template <> int DciProperty::GetProperty( byte idProp, uint32& value);

template <> int DciProperty::GetProperty( byte idProp, int32& value);

template <> int DciProperty::GetProperty( byte idProp, float& value);

template <> int DciProperty::GetProperty( byte idProp, double& value);

/*
template <> int DciProperty::GetProperty( byte idProp, uint64& value);

template <> int DciProperty::GetProperty( byte idProp, int64& value);
*/



template <> int DciProperty::SetProperty( byte idProp, byte value);

template <> int DciProperty::SetProperty( byte idProp, uint16 value);

template <> int DciProperty::SetProperty( byte idProp, int16 value);

template <> int DciProperty::SetProperty( byte idProp, uint32 value);

template <> int DciProperty::SetProperty( byte idProp, int32 value);

template <> int DciProperty::SetProperty( byte idProp, float value);

template <> int DciProperty::SetProperty( byte idProp, double value);

/*
template <> int DciProperty::SetProperty( byte idProp, uint64 value);

template <> int DciProperty::SetProperty( byte idProp, int64 value);
*/


#endif /* DCIPROPERTY_H_ */
