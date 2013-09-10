// Name:  lime.c
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
#include "Comm/Dci/DciMsg.h"
#include "Comm/Dci/StandardMsgs.h"

#include "SPI.h"
#include "dciproc.h"

#include "lime.h"
#include "LimeDefs.h"
#include "../globals.h"
#include "Timers.h"
#include <string.h>
#include <stdio.h>

/****************************************************************************
*  Defines & Typedefs
****************************************************************************/
#define MAX_REG_COUNT		(128)	/* 128 Individual Registers. */

/*==== This is the calibration structure stored in non-volatile memory ====*/
typedef struct
{
   long	FreqHighest;			// Highest frequency in range
   BYTE	FREQSEL;				// Value to use for FREQSEL
} LMFREQSEL_STRUCT;



/****************************************************************************
*  Local Prototypes
****************************************************************************/
static void OnLimeWriteRegisters( Dci_Context* pctxt);
static void OnLimeReadRegisters( Dci_Context* pctxt);

/****************************************************************************
*  Local Declarations
****************************************************************************/

const LMFREQSEL_STRUCT	LMFreq[16] = {
	 285625, 0x27,		// 100111
	 336875, 0x2f,		// 101111
	 405000, 0x37,		// 110111
	 465000, 0x3f,		// 111111
	 571250, 0x26,		// 100110
	 673750, 0x2e,		// 101110
	 810000, 0x36,		// 110110
	 930000, 0x3e,		// 111110
	1142500, 0x25,		// 100101
	1347500, 0x2d,		// 101101
	1620000, 0x35,		// 110101
	1860000, 0x3d,		// 111101
	2285000, 0x24,		// 100100
	2695000, 0x2c,		// 101100
	3240000, 0x34,		// 110100
	3720000,	0x3c		// 111100
};


/*---- static variable declarations ----*/
int	SPI_wlen;
BYTE*	SPI_pData;

/*---- global variable declarations ----*/
TUNE_STRUCT	vcocap;

/**
* Define DCI Message map for Lime components
*/
DCI_BEGIN_MSGMAP( s_limemap)
	//Low Level Interface for setting and getting lime registers.
	DCI_TYPEDDATA_HANDLER		( 0x11,&OnLimeWriteRegisters)
	DCI_TYPEDDATA_HANDLER		( 0x21,&OnLimeWriteRegisters)
	DCI_TYPEDDATAQUERY_HANDLER	( 0x11,&OnLimeReadRegisters)
	DCI_TYPEDDATAQUERY_HANDLER	( 0x21,&OnLimeReadRegisters)
DCI_END_MSGMAP


/****************************************************************************
*  Implementation
****************************************************************************/
 
/** 
 *  initialize LimeParams struct
 */
void Lime_Init()
{
	//Bind DCI Message handlers to message map processor.
	main_AddDciMsgMap( s_limemap);
}

/**
 * Processes on going updates
 */
void Lime_OnUpdate( )
{
//    ProcessSerialPort();
}

/****************************************************************************
*  DCI Message Handlers 
****************************************************************************/

/**
* Low-level write directly to LIME chip registers.
*/
void OnLimeWriteRegisters( Dci_Context* pctxt)
{
    byte addrStart, addrEnd;
	byte  wdata[2], addr, idxLime, idComponent;
	byte  *pMsgData;

	//Cast to TypedDataRecord.
    Dci_TypedDataRecord *ptd = (Dci_TypedDataRecord*) pctxt->pMsg;

	//Get the component id based on typeid since this is not a 
	//WCA based message. idtypes = 0x11 (WCACOMP_RF0), 0x21 (WCACOMP_RF1)
	idComponent = (byte)(( ptd->idtype == 0x11) ? WCACOMP_RF0 : WCACOMP_RF1); 
	idxLime = idComponent - WCACOMP_RF0;

	//Get the data and address range.
    pMsgData  = Dci_TypedDataRecord_GetData(ptd);
    addrStart = (byte)(ptd->flags & 0xFF);
    addrEnd   = (byte)(ptd->flags >> 8) & 0xFF;

    // Write Register data to the Lime Chip.
    if( (addrStart <= addrEnd) && (addrEnd < MAX_REG_COUNT) )
    {
		for( addr=addrStart ; addr<=addrEnd ; addr++ ) {
			wdata[0] = (BYTE)addr;
			wdata[1] = *pMsgData++;
			Lime_Write( (int)idxLime, 2, wdata );
		}
    }
	pctxt->bHandled = true;
}

/**
* Low-level read directly from LIME chip registers.
*/
void OnLimeReadRegisters( Dci_Context* pctxt)
{
    byte buff[MAX_MSG_SIZE];
	byte *pdat;
    uint16 addrStart,addrEnd, addr;
	byte idComponent, idxLime;
	int lenMsg;

	//Cast to Query.
	Dci_TypedDataRecordQuery *ptdq = (Dci_TypedDataRecordQuery*) pctxt->pMsg;

	//Get the component id based on typeid since this is not a 
	//WCA based message. idtypes = 0x11 (WCACOMP_RF0), 0x21 (WCACOMP_RF1)
	idComponent = (byte)(( ptdq->idtype == 0x11) ? WCACOMP_RF0 : WCACOMP_RF1); 
	idxLime = idComponent - WCACOMP_RF0;

	//Get the requested address range.
    addrStart = (byte)(ptdq->flags & 0xFF);
    addrEnd   = (byte)(ptdq->flags >> 8) & 0xFF;

    // Verify address range to prevent buffer overflow.
    if( (addrEnd < MAX_REG_COUNT) && (addrStart <= addrEnd) )
    {
		//Construct a TypeDataRecord without data yet.
		lenMsg = Dci_TypedDataRecord_Init( buff, MAX_MSG_SIZE,
					0, 0, 0, ptdq->idtype, ptdq->flags, addrEnd - addrStart + 1, 
					NULL);

		//Get a pointer to data so we can populate with register values from 
		//the lime.
		pdat = Dci_TypedDataRecord_GetData(( Dci_TypedDataRecord*)buff);
		
		//Populate data by reading register values from the Lime chip.
		for( addr=addrStart ; addr<=addrEnd ; addr++, pdat++ ) 
		{
			Lime_Read( (int)idxLime, addr, pdat );
		}

		//Send back requested values.		
        Dciproc_SendMessage1( buff, lenMsg, false, pctxt);
	}
    pctxt->bHandled = true;
}


/****************************************************************************
*  Mid Level LIME Configuration
****************************************************************************/

/****************************************************************

   CALL:	Lime_SetRxFrequency()
                             
   INTRO:   This routine sets the frequency of the appropriate
			Lime Micro device.

   INPUT:   freq			- Frequency (in kHz)
   OUTPUT:  nothing

****************************************************************/
void	Lime_SetRxFrequency( int device, long freq )
{
int	   i;
BYTE   freqsel;
BYTE   freqsel_init;
BYTE   wdata[2];
int	   x;
double ftmp;
DWORD  ltmp;

	/*==== determine FREQSEL value (based on frequency) ====*/
	freqsel = LMFreq[15].FREQSEL;			// default to highest range (in case it's higher than max
	for( i=0 ; i<16 ; i++ ) {
		if( freq < LMFreq[i].FreqHighest ) {
			freqsel = LMFreq[i].FREQSEL;
			break;
		}
	}
	/*---- read register to determine which LNA is being used ----*/
	Lime_Read( device, LMREG_RXFREQSEL, &freqsel_init );
	wdata[0] = LMREG_RXFREQSEL;							// write FREQSEL register
	wdata[1] = (freqsel<<2) + (freqsel_init & 0x3);	// use new FREQSEL, but keep current LNA setting
	Lime_Write( device, 2, wdata );

	/*==== set synth fractional N values ====*/
	x = 1<<((freqsel & 0x7) - 3);
//	tmp = (MLS32)(((MLS64)(x * freq)*8388608L)/REFERENCE_FREQUENCY);
//  tmp = freq * x * 8388608 / 32000
	ftmp = (double)(x * freq) * 8388608.0 / 32000.;
	ltmp = (DWORD)ftmp;
	/*---- write divider values to Lime Micro ----*/
	wdata[0] = LMREG_RXPLLDIVIDER_1;			// write RX PLL divider registers
	wdata[1] = (BYTE)(ltmp>>24);
	Lime_Write( device, 2, wdata );	// bits 24-31
	wdata[0] = LMREG_RXPLLDIVIDER_2;
	wdata[1] = (BYTE)(ltmp>>16);
	Lime_Write( device, 2, wdata );	// bits 16-23
	wdata[0] = LMREG_RXPLLDIVIDER_3;
	wdata[1] = (BYTE)(ltmp>>8);
	Lime_Write( device, 2, wdata );	// bits 8-15
	wdata[0] = LMREG_RXPLLDIVIDER_4;
	wdata[1] = (BYTE)ltmp;
	Lime_Write( device, 2, wdata );	// bits 0-7

	/*---- retune the VCO capacitor for new frequency ----*/
	Lime_TuneRxCap( device );

}


/****************************************************************

   CALL:		Lime_SetRxNco()
                             
   INTRO:   This routine sets the NCO value directly.

   INPUT:   NCO			- Lime PLL registers 0x20-0x23
   OUTPUT:  nothing

****************************************************************/
void    Lime_SetRxNco( int device, uint32 val)
{
BYTE  wdata[2];
BYTE* bptr;
BYTE  i;

	/*---- write RX PLL settings to Lime Micro ----*/
	bptr = (BYTE*)val;
	for( i=0 ; i<4 ; i++ ) {
		wdata[0] = LMREG_RXPLLDIVIDER_1 + i;
		wdata[1] = *bptr++;
		Lime_Write( device, 2, wdata );
	}
}


/****************************************************************

   CALL:		Lime_SetRxGain()
                             
   INTRO:   This routine sets the gain of the receive channel
   			on the appropriate Lime Micro device.

   INPUT:   gain			- gain in dB*3 (e.g. 3 = 9dB)
   OUTPUT:  nothing

****************************************************************/
void	Lime_SetRxGain( int device, BYTE gain )
{
BYTE  wdata[2];

	/*---- write RX VGA Gain setting to Lime Micro ----*/
	wdata[0] = LMREG_RXVGA2GAIN;
	wdata[1] = gain;
	Lime_Write( device, 2, wdata );
}

/****************************************************************

   CALL:		Lime_SetRxBandwidth()
                             
   INTRO:   This routine sets the RF bandwidth of the receive 
   			channel on the appropriate Lime Micro device.
				 Value	Bandwidth
					0 		14 (default)
					1 		10
					2 		7
					3 		6
					4 		5
					5 		4.375
					6 		3.5
					7 		3
					8 		2.75
					9 		2.5
					10		1.92
					11		1.5
					12		1.375
					13		1.25
					14		0.875
					15		0.75
   INPUT:   bandwidth		- 
   OUTPUT:  nothing

****************************************************************/
void	Lime_SetRxBandwidth( int device, BYTE bandwidth )
{
BYTE  wdata[2];

	/*---- write RX VGA Gain setting to Lime Micro ----*/
	wdata[0] = LMREG_RXLPF_BANDWIDTH;
	wdata[1] = ((bandwidth & 0x0f)<<2) | 0x2;
	Lime_Write( device, 2, wdata );
}


/****************************************************************

   CALL:		Lime_SetTxFrequency()
                             
   INTRO:   This routine sets the frequency of the appropriate
				Lime Micro device.

   INPUT:   freq			- Frequency (in kHz)
   OUTPUT:  nothing

****************************************************************/
void	Lime_SetTxFrequency( int device, long freq )
{
int	   i;
BYTE   freqsel;
BYTE   freqsel_init;
BYTE   wdata[2];
int	   x;
double ftmp;
DWORD  ltmp;

	/*==== determine FREQSEL value (based on frequency) ====*/
	freqsel = LMFreq[15].FREQSEL;			// default to highest range (in case it's higher than max
	for( i=0 ; i<16 ; i++ ) {
		if( freq < LMFreq[i].FreqHighest ) {
			freqsel = LMFreq[i].FREQSEL;
			break;
		}
	}
	/*---- read register to determine which LNA is being used ----*/
	Lime_Read( device, LMREG_TXFREQSEL, &freqsel_init );
	wdata[0] = LMREG_TXFREQSEL;							// write FREQSEL register
	wdata[1] = (freqsel<<2) + (freqsel_init & 0x3);	// use new FREQSEL, but keep current LNA setting
	Lime_Write( device, 2, wdata );

	/*==== set synth fractional N values ====*/
	x = 1<<((freqsel & 0x7) - 3);
//	tmp = (MLS32)(((MLS64)(x * freq)*8388608L)/REFERENCE_FREQUENCY);
//  tmp = freq * x * 8388608 / 32000
	ftmp = (double)(x * freq) * 8388608.0 / 32000.;
	ltmp = (DWORD)ftmp;
	/*---- write divider values to Lime Micro ----*/
	wdata[0] = LMREG_TXPLLDIVIDER_1;			// write RX PLL divider registers
	wdata[1] = (BYTE)(ltmp>>24);
	Lime_Write( device, 2, wdata );	// bits 24-31
	wdata[0] = LMREG_TXPLLDIVIDER_2;
	wdata[1] = (BYTE)(ltmp>>16);
	Lime_Write( device, 2, wdata );	// bits 16-23
	wdata[0] = LMREG_TXPLLDIVIDER_3;
	wdata[1] = (BYTE)(ltmp>>8);
	Lime_Write( device, 2, wdata );	// bits 8-15
	wdata[0] = LMREG_TXPLLDIVIDER_4;
	wdata[1] = (BYTE)ltmp;
	Lime_Write( device, 2, wdata );	// bits 0-7

	/*---- retune the VCO capacitor for new frequency ----*/
	Lime_TuneTxCap( device );
}

/****************************************************************

   CALL:	Lime_SetTxNco()
                             
   INTRO:   This routine sets the NCO value directly.

   INPUT:   NCO			- Lime PLL registers 0x10-0x13
   OUTPUT:  nothing

****************************************************************/
void    Lime_SetTxNco( int device, uint32 val)
{
BYTE  wdata[2];
BYTE* bptr;
BYTE  i;

	/*---- write TX PLL settings to Lime Micro ----*/
	bptr = (BYTE*)val;
	for( i=0 ; i<4 ; i++ ) {
		wdata[0] = LMREG_TXPLLDIVIDER_1 + i;
		wdata[1] = *bptr++;
		Lime_Write( device, 2, wdata );
	}

}

/****************************************************************

   CALL:		Lime_SetTxGain()
                             
   INTRO:   This routine sets the gain of the transmit channel
   			on the appropriate Lime Micro device.

   INPUT:   gain			- gain in dB (0 to 31)
   OUTPUT:  nothing

****************************************************************/
void	Lime_SetTxGain( int device, BYTE gain )
{
BYTE  wdata[2];

	/*---- write TX VGA1 Gain setting to Lime Micro ----*/
	wdata[0] = LMREG_VGA1GAIN;
	if( gain > 31 ) {		// don't allow value more than 31
		wdata[1] = 31;
	} else {
		wdata[1] = gain;
	}
	Lime_Write( device, 2, wdata );
}

/****************************************************************

   CALL:		Lime_SetTxBandwidth()
                             
   INTRO:   This routine sets the RF bandwidth of the transmit 
   			channel on the appropriate Lime Micro device.
				 Value	Bandwidth
					0 		14 (default)
					1 		10
					2 		7
					3 		6
					4 		5
					5 		4.375
					6 		3.5
					7 		3
					8 		2.75
					9 		2.5
					10		1.92
					11		1.5
					12		1.375
					13		1.25
					14		0.875
					15		0.75
   INPUT:   bandwidth		- 
   OUTPUT:  nothing

****************************************************************/
void	Lime_SetTxBandwidth( int device, BYTE bandwidth )
{
BYTE  wdata[2];

	/*---- write TX VGA Gain setting to Lime Micro ----*/
	wdata[0] = LMREG_TXLPF_BANDWIDTH;
	wdata[1] = ((bandwidth & 0x0f)<<2) | 0x2;
	Lime_Write( device, 2, wdata );
}

/****************************************************************************
*  Low Level LIME I/O
****************************************************************************/


/****************************************************************

   CALL:		Lime_TuneRxCap()
                             
   INTRO:   This routine tunes the Rx VCO cap on the selected
   			Lime Micro device.

   INPUT:   device			- Lime Micro 0 or 1
   OUTPUT:  nothing

****************************************************************/
void	Lime_TuneRxCap( int device )
{
BYTE  wdata[2];
BYTE  vtune;
BYTE  vtune_init;
BYTE  i;

	wdata[0] = LMREG_RXVCOCAP;		// write VCO cap register
	vcocap.tune_low = 0xff;

	for( i=0 ; i<64 ; i++ ) {
		/*---- write next value for VCAP ----*/
		wdata[1] = 0x80 + i;
		Lime_Write( device, 2, wdata );
		Timer_Delay( 100 );

		/*---- see if properly tuned ----*/
		Lime_Read( device, LMREG_RXVTUNE, &vtune );

		if( vcocap.tune_low==0xff ) {
			if( (vtune & 0xc0) == 0 ) {
				vcocap.tune_low = i;
				vcocap.tune_high = i;
			}
		} else {
			if( (vtune & 0xc0) == 0 ) {
				vcocap.tune_high = i;
			} else {
				/*==== WE'RE DONE CHECKING ====*/
				break;
			}
		}
	}

	/*---- set value for VCAP exactly between low and high values ----*/
	vcocap.tune_value = (vcocap.tune_high - vcocap.tune_low) / 2 + vcocap.tune_low;
	wdata[1] = 0x80 + vcocap.tune_value;
	Lime_Write( device, 2, wdata );

}

/****************************************************************

   CALL:		Lime_TuneTxCap()
                             
   INTRO:   This routine tunes the Tx VCO cap on the selected
   			Lime Micro device.

   INPUT:   device			- Lime Micro 0 or 1
   OUTPUT:  nothing

****************************************************************/
void	Lime_TuneTxCap( int device )
{
BYTE  wdata[2];
BYTE  vtune;
BYTE  vtune_init;
BYTE  i;

	wdata[0] = LMREG_TXVCOCAP;		// write VCO cap register
	vcocap.tune_low = 0xff;

	for( i=0 ; i<64 ; i++ ) {
		/*---- write next value for VCAP ----*/
		wdata[1] = 0x80 + i;
		Lime_Write( device, 2, wdata );
		Timer_Delay( 100 );

		/*---- see if properly tuned ----*/
		Lime_Read( device, LMREG_TXVTUNE, &vtune );

		if( vcocap.tune_low==0xff ) {
			if( (vtune & 0xc0) == 0 ) {
				vcocap.tune_low = i;
				vcocap.tune_high = i;
			}
		} else {
			if( (vtune & 0xc0) == 0 ) {
				vcocap.tune_high = i;
			} else {
				/*==== WE'RE DONE CHECKING ====*/
				break;
			}
		}
	}

	/*---- set value for VCAP exactly between low and high values ----*/
	vcocap.tune_value = (vcocap.tune_high - vcocap.tune_low) / 2 + vcocap.tune_low;
	wdata[1] = 0x80 + vcocap.tune_value;
	Lime_Write( device, 2, wdata );

}

/****************************************************************

   CALL:		Lime_CalBlock()
                             
   INTRO:   This routine performs a calibration on the block
   			starting at the specified address.

   INPUT:   device			- Lime Micro 0 or 1
   			block_addr		- start address of block to calibrate
			channel			- channel on the block being calibrated
   OUTPUT:  nothing

****************************************************************/
void	Lime_CalBlock( int device, BYTE block_addr, BYTE channel )
{
BYTE  wdata[2];
BYTE	caldata;
int	i, retry;

	/*---- try up to 10 times ----*/
	for( retry=0 ; retry<10 ; retry++ ) {

		/*---- start calibration on selected block ----*/
		wdata[0] = block_addr + 3;			// write to register that turns on/off calibration
		wdata[1] = 0x28 + channel;
		Lime_Write( device, 2, wdata );		// turn on calibration
		Timer_Delay( 10 );
		wdata[1] = 0x08 + channel;
		Lime_Write( device, 2, wdata );		// turn off calibration
		Timer_Delay( 10 );
		Lime_Write( device, 2, wdata );		// turn off again (that's what Lime Micro program does)

		/*---- wait until calibration is complete ----*/
		for( i=0 ; i<100 ; i++ ) {
			Timer_Delay( 10 );
			Lime_Read( device, block_addr+1, &caldata );
			if( (caldata & 0x2)==0 ) {
				/*---- calibration is complete, now don't quit until block is locked ----*/
				caldata = (caldata>>2) & 0x7;
				if( (caldata>=1) && (caldata<=6) ) {
					break;
				}
			}
		}

		/*---- if successful, then we're done ----*/
		if( i<500 ) {
			break;
		}
	}

	/*---- if error, then indicate in error status ----*/
	if( retry==10 ) {
		if( status.Error[ERROR_LIMECAL] < 255 ) {
			status.Error[ERROR_LIMECAL]++;
		}
	}

}

/****************************************************************

   CALL:		Lime_RemoveRxDCBias()
                             
   INTRO:   This routine reduces the DC offset of the I and Q
			bias by writing the appropriate registers on the
			receive side of the Lime Micro IC.

   INPUT:   device		- Lime device to work on
   OUTPUT:  nothing

****************************************************************/
void Lime_RemoveRxDCBias( int device )
{
BYTE  wdata[2];
BYTE	clocksave;

	/*---- clear out DC offset registers ----*/
	wdata[1] = 0x80;
	wdata[0] = LMREG_DCOFFSET_I;
	Lime_Write( device, 2, wdata );
	wdata[0] = LMREG_DCOFFSET_Q;
	Lime_Write( device, 2, wdata );

	/*---- save clock settings and turn all calibration clocks on ----*/
	Lime_Read( device, LMREG_CLOCKENABLES, &clocksave );
	wdata[0] = LMREG_CLOCKENABLES;
	wdata[1] = clocksave | 0x7f;
	Lime_Write( device, 2, wdata );							// turn on all clocks

	/*---- calibrate all modules ----*/
	Lime_CalBlock( device, LMREG_CAL_TOPLEVEL, 0 );		// Top level cal
	Lime_CalBlock( device, LMREG_CAL_RXLPF, 0 );			// Rx LPF, I channel
	Lime_CalBlock( device, LMREG_CAL_RXLPF, 1 );			// Rx LPF, Q channel
	Lime_CalBlock( device, LMREG_CAL_RXVGA, 0 );			// Rx VGA, DC reference
	Lime_CalBlock( device, LMREG_CAL_RXVGA, 1 );			// Rx VGA, 1st stage I channel
	Lime_CalBlock( device, LMREG_CAL_RXVGA, 2 );			// Rx VGA, 1st stage Q channel
	Lime_CalBlock( device, LMREG_CAL_RXVGA, 3 );			// Rx VGA, 2nd stage I channel
	Lime_CalBlock( device, LMREG_CAL_RXVGA, 4 );			// Rx VGA, 2nd stage Q channel

	/*---- restore clock settings ----*/
	wdata[1] = clocksave;
	Lime_Write( device, 2, wdata );							// restore clocks

	/*---- now startup DC offset state machine ----*/
//	DCBias_state = BIASSTATE_FIXINIT;

}

/****************************************************************

   CALL:		Lime_RemoveTxDCBias()
                             
   INTRO:   This routine reduces the DC offset of the I and Q
			bias by writing the appropriate registers on the
			transmit side of the Lime Micro IC.

   INPUT:   device		- Lime device to work on
   OUTPUT:  nothing

****************************************************************/
void Lime_RemoveTxDCBias( int device )
{
BYTE  wdata[2];
BYTE	clocksave;

	/*---- clear out DC offset registers ----*/
//	wdata[1] = 0x80;
//	wdata[0] = LMREG_DCOFFSET_I;
//	Lime_Write( device, 2, wdata );
//	wdata[0] = LMREG_DCOFFSET_Q;
//	Lime_Write( device, 2, wdata );

	/*---- save clock settings and turn all calibration clocks on ----*/
	Lime_Read( device, LMREG_CLOCKENABLES, &clocksave );
	wdata[0] = LMREG_CLOCKENABLES;
	wdata[1] = clocksave | 0x7f;
	Lime_Write( device, 2, wdata );							// turn on all clocks

	/*---- calibrate all modules ----*/
	Lime_CalBlock( device, LMREG_CAL_TOPLEVEL, 0 );			// Top level cal

	/*---- restore clock settings ----*/
	wdata[1] = clocksave;
	Lime_Write( device, 2, wdata );							// restore clocks

	/*---- now startup DC offset state machine ----*/
//	DCBias_state = BIASSTATE_FIXINIT;

}

/****************************************************************

   CALL:		Lime_AnalogOutput()
                             
   INTRO:   This routine turns on or off the analog outputs on
			the Lime Micro IC.  These signals can be seen using
			a scope on J8.

   INPUT:   device			- Lime Micro 0 or 1
   			onoff				- 0=off, nonzero=on
   OUTPUT:  nothing

****************************************************************/
void	Lime_AnalogOutput( int device, BYTE onoff )
{
BYTE  wdata[2];
BYTE	clocksave;
int	i, retry;

	Lime_Read( device, LMREG_CLOCKENABLES, &clocksave );
	wdata[0] = LMREG_CLOCKENABLES;
	if( onoff ) {
		wdata[1] = clocksave | 0x80;
	} else {
		wdata[1] = clocksave & 0x7f;
	}
	Lime_Write( device, 2, wdata );							// turn on all clocks

}


/****************************************************************

   CALL:	Lime_Write()
                             
   INTRO:   This routine writes the specified data to one of
   			the Lime Micro devices.  Writing to the LM must
			always be done in pairs where the first byte is the
			address and the second byte is the data.
				
			IMPORTANT: The calling program must take care of
			setting the MSB of each address byte to make sure
			a write is performed.

   INPUT:   device		- LIME_MICRO_1 or LIME_MICRO_2
			wlen		- number of bytes to write
			pData		- pointer to data to write
   OUTPUT:  error code	- 0 if no error

****************************************************************/
BYTE	Lime_Write( int device, int wlen, BYTE* pData )
{
int	 ec;

	*pData |= 0x80;		// make sure MSB=1 for a write
	ec = SPI_WriteReadData( (BYTE)(SPI_DEVICE_LIME1+device), (BYTE)wlen, pData, 0, 0, FALSE );

	return( ec );
}


/****************************************************************

   CALL:	Lime_Read()
                             
   INTRO:   This routine reads data from one of the Lime Micro
   			devices from the specified address.

   INPUT:   device		- 0=LIME1, 1=LIME2
			addr		- address to read
			pData		- pointer to result
   OUTPUT:  error code	- 0 if no error

****************************************************************/
BYTE	Lime_Read( int device, int addr, BYTE* pData )
{
BYTE cmd;
int	 ec;

	cmd = (BYTE)addr;
	ec = SPI_WriteReadData( (BYTE)(SPI_DEVICE_LIME1+device), 1, &cmd, 1, pData, FALSE );

	return( ec );

}

