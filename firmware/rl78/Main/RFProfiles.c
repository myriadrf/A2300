// Name:  RFProfiles.c
//
// Copyright(c) 2013 Loctronix Corporation
// http://www.loctronix.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_wdt.h"
#include "r_cg_userdefine.h"
#include "main.h"
#include "RfComponent.h"
#include "RFProfiles.h"


//****************************************************************************
// Include RF PATH PROFILE Data.
//****************************************************************************
#include "RFProfiles.dat"

//****************************************************************************
// Path Profiles Implementation
//****************************************************************************

/**
* Function returns the specified path Definition given the identifier.
*/
const RF_PathDef* RF_GetPathProfile( byte id)
{
	int i;
	
	//Cast to the path definition.
	RF_PathDef* pdefs = (RF_PathDef*)s_defsStandard;	
	
	for( i = 0; i < COUNT_STDPATHS; i++)
	{
		RF_PathDef* pdef = pdefs + i;
		if( pdef->id == id)
			return pdef;
	}

	//Not found
	return NULL;
}

/**
* Function Copies the array of available PIds available. Returns
* the number of PIDs copied.
*/
int  RF_EnumerateProfiles( byte *pIds, int maxlen)
{
	int i;
	int ct = ( maxlen < COUNT_STDPATHS) ? maxlen : COUNT_STDPATHS;

	//Cast to the path definition.
	RF_PathDef* pdefs = (RF_PathDef*)s_defsStandard;	
	
	for( i = 0; i < ct; i++)
	{

		*pIds++ = (pdefs+i)->id;
	}
	return ct;
}

/**
* Returns a pointer to the specified lime register data given
* the RF_PathDef id and the 
*/
const RF_LimeRegData* RF_GetLimeRegData( const RF_PathDef* pdef)
{
	//If it is a standard definition look up 
	if( (pdef->id & 0x10) == 0)
		return (RF_LimeRegData*)(s_limeregStandard + pdef->idxRegData);
	else // user def (non-volatile)
		return NULL; // Not supported yet.
}


