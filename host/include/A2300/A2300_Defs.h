/************************************************************************
 * Name:  A2300_Defs.h
 * 
 * This module is the proprietary property of Loctronix Corporation
 * Copyright (C) 2013 Loctronix Corporation
 * All Rights Reserved
 * 
 ***************************************************************************/
#ifndef A2300_DEFS_H_
#define A2300_DEFS_H_

// Loctronix System Defaults
#define A2300_DEFAULT_FREQ      (1.946e9)   	// gHz -> mHz -> kHz
#define A2300_DEFAULT_GAIN		(0.0)
#define A2300_DEFAULT_TICK_RATE (32e6)		// 32-MHz.

#define A2300_MIN_FREQ          (0.3e9)         // 0.3 GHz.
#define A2300_MAX_FREQ 			(3.8e9)		// 3.8 GHz.

#define A2300_MIN_BW			(0.75e6 * 2.0)	// 0.75 MHz.
#define A2300_MAX_BW 			(14.0e6 * 2.0)	// 14.0 MHz.


#define A2300_CLOCK_HZ     		(32.0e6)			// 32-MHz.
#define A2300_MIN_CLOCK   		(A2300_CLOCK_HZ/4096.0)
#define A2300_MAX_CLOCK   		(A2300_CLOCK_HZ/4.0)

#endif /* A2300_DEFS_H_ */
