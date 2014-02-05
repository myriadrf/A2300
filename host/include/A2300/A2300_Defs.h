/** Name: A2300_Defs.h
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

#ifndef A2300_DEFS_H_
#define A2300_DEFS_H_

#include <System/DataTypes.h>

#include <string>
#include <list>

// Loctronix System Defaults
#define A2300_DEFAULT_FREQ      ((double)(1.946e9))   	// gHz -> mHz -> kHz
#define A2300_DEFAULT_GAIN		((double)(0.0))
#define A2300_DEFAULT_TICK_RATE ((double)(32e6))		// 32-MHz.

#define A2300_RFFREQ_MIN        ((double)(0.3e9))         // 0.3 GHz.
#define A2300_RFFREQ_MAX	    ((double)(3.8e9))		// 3.8 GHz.
#define A2300_RFFREQ_STEP		((double)(1000))

#define A2300_SAMPFREQ_MAX		((double)(32e6))
#define A2300_SAMPFREQ_MIN      ((double)(7812.5))

#define A2300_DDUC_FREQ_MAX     ((double)(14e6))
#define A2300_DDUC_FREQ_MIN		((double)(-14e6))
#define A2300_DDUC_FREQ_STEP	((double)(0.0149011611938))

#define A2300_MIN_BW			((double)(0.75e6 * 2.0))	// 0.75 MHz.
#define A2300_MAX_BW 			((double)(14.0e6 * 2.0))	// 14.0 MHz.

#define A2300_RXGAIN_MIN			((double)(0))
#define A2300_RXGAIN_MAX			((double)(30))
#define A2300_RXGAIN_STEP			((double)(3))

#define A2300_TXGAIN_MIN			((double)(0))
#define A2300_TXGAIN_MAX			((double)(56))
#define A2300_TXGAIN_STEP			((double)(1))

#define A2300_MAX_SAMPLING_FREQ ((double)(32.0e6)) // 32-MHz.
#define A2300_MIN_CLOCK   		(A2300_MAX_SAMPLING_FREQ/((double)4096.0))
#define A2300_MAX_CLOCK   		(A2300_MAX_SAMPLING_FREQ/((double)1.0))
#define A2300_SYSTEM_CLOCK		((double)(100e6))

#define A2300_WAIT_TIME 		((double)(0.1))

/**
 * Interface implemented components supporting device configuration
 * api.
 */
struct IConfigComponent
{
	virtual std::string name() = 0;
	virtual byte componentId() = 0;
	virtual void Reset()= 0;
	virtual void Synch() = 0;
};

#endif /* A2300_DEFS_H_ */
