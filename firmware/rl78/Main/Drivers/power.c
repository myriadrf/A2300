// Name:  power.c
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
#include "main.h"
#include "dciproc.h"
#include "power.h"
#include <Comm/Dci/WcaMsgs.h>
#include <Comm/Dci/DciMsg.h>
#include <RFConfig.h>


/****************************************************************************
*  Defines & Typedefs
****************************************************************************/


/****************************************************************************
*  Local Declarations
****************************************************************************/
static void OnWcaSetRfPower( Dci_Context* pctxt);

/**
* Define DCI Message map for Power management functionality.
*/
DCI_BEGIN_MSGMAP( s_pwrmap)
	DCI_WCAPROP_HANDLER( WCACOMP_BATTERY, 0x0, 0x0, &OnWcaSetRfPower)
DCI_END_MSGMAP




/****************************************************************************
*  Implementation
****************************************************************************/
 


/**
 * Initialization
 */
void Power_Init( )
{
	//Bind DCI Message handlers to message map processor.
	main_AddDciMsgMap( s_pwrmap);
}

/**
* Enable / Disables RF Power by setting the state of the RF voltage
* regulators.
*/
void OnWcaSetRfPower( Dci_Context* pctxt)
{
    byte bEnable;
	Dci_Property *pProps;
	pProps = Dci_TypedProperties_GetProperties( (Dci_TypedProperties*) pctxt->pMsg );
	bEnable = (pProps->value.vByte);
	// 1 = Enable / 0 = Disable.

	/*---- control the RF power regulator ----*/
	RF_SetPower( bEnable);

}
/**
 * Processes on going updates
 */
void Power_OnUpdate( )
{

}
