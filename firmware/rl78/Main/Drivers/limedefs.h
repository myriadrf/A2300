// Name:  limedefs.h
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

#ifndef	_LIMEDEFS_
#define	_LIMEDEFS_

enum LimeMicroRegisters
{
	/*---- Top Level Config Registers ----*/
	LMREG_CAL_TOPLEVEL = 0,
	LMREG_REG01,
	LMREG_REG02,
	LMREG_REG03,
	LMREG_REG04,
	LMREG_REG05,
	LMREG_REG06,
	LMREG_REG07,
	LMREG_REG08,
	LMREG_CLOCKENABLES,
	LMREG_REG0A,
	LMREG_REG0B,
	LMREG_REG0C,
	LMREG_REG0D,
	LMREG_REG0E,
	LMREG_REG0F,
	/*---- TX PLL configuration registers ----*/
	LMREG_TXPLLDIVIDER_1,
	LMREG_TXPLLDIVIDER_2,
	LMREG_TXPLLDIVIDER_3,
	LMREG_TXPLLDIVIDER_4,
	LMREG_REG14,
	LMREG_TXFREQSEL,
	LMREG_REG16,
	LMREG_REG17,
	LMREG_REG18,
	LMREG_TXVCOCAP,
	LMREG_TXVTUNE,
	LMREG_REG1B,
	LMREG_REG1C,
	LMREG_REG1D,
	LMREG_REG1E,
	LMREG_REG1F,
	/*---- RX PLL configuration registers ----*/
	LMREG_RXPLLDIVIDER_1,
	LMREG_RXPLLDIVIDER_2,
	LMREG_RXPLLDIVIDER_3,
	LMREG_RXPLLDIVIDER_4,
	LMREG_REG24,
	LMREG_RXFREQSEL,
	LMREG_REG26,
	LMREG_REG27,
	LMREG_REG28,
	LMREG_RXVCOCAP,
	LMREG_RXVTUNE,
	LMREG_REG2B,
	LMREG_REG2C,
	LMREG_REG2D,
	LMREG_REG2E,
	LMREG_REG2F,
	/*---- TX LPF modules configuration registers ----*/
	LMREG_REG30,
	LMREG_REG31,
	LMREG_REG32,
	LMREG_REG33,
	LMREG_TXLPF_BANDWIDTH,
	LMREG_REG35,
	LMREG_REG36,
	LMREG_REG37,
	LMREG_REG38,
	LMREG_REG39,
	LMREG_REG3A,
	LMREG_REG3B,
	LMREG_REG3C,
	LMREG_REG3D,
	LMREG_REG3E,
	LMREG_REG3F,
	/*---- TX RF modules configuration registers ----*/
	LMREG_REG40,
	LMREG_VGA1GAIN,
	LMREG_REG42,
	LMREG_REG43,
	LMREG_REG44,
	LMREG_REG45,
	LMREG_REG46,
	LMREG_REG47,
	LMREG_REG48,
	LMREG_REG49,
	LMREG_REG4A,
	LMREG_REG4B,
	LMREG_REG4C,
	LMREG_REG4D,
	LMREG_REG4E,
	LMREG_REG4F,
	/*---- RX LPF, DAC/ADC modules configuration registers ----*/
	LMREG_CAL_RXLPF,
	LMREG_REG51,
	LMREG_REG52,
	LMREG_REG53,
	LMREG_RXLPF_BANDWIDTH,
	LMREG_REG55,
	LMREG_REG56,
	LMREG_REG57,
	LMREG_REG58,
	LMREG_REG59,
	LMREG_REG5A,
	LMREG_REG5B,
	LMREG_REG5C,
	LMREG_REG5D,
	LMREG_REG5E,
	LMREG_REG5F,
	/*---- RX VGA2 configuration registers ----*/
	LMREG_CAL_RXVGA,
	LMREG_REG61,
	LMREG_REG62,
	LMREG_REG63,
	LMREG_REG64,
	LMREG_RXVGA2GAIN,
	LMREG_REG66,
	LMREG_REG67,
	LMREG_REG68,
	LMREG_REG69,
	LMREG_REG6A,
	LMREG_REG6B,
	LMREG_REG6C,
	LMREG_REG6D,
	LMREG_REG6E,
	LMREG_REG6F,
	/*---- RX FE modules configuration registers ----*/
	LMREG_REG70,
	LMREG_DCOFFSET_I,
	LMREG_DCOFFSET_Q,
	LMREG_REG73,
	LMREG_REG74,
	LMREG_REG75,
	LMREG_REG76,
	LMREG_REG77,
	LMREG_REG78,
	LMREG_REG79,
	LMREG_REG7A,
	LMREG_REG7B,
	LMREG_REG7C,
	LMREG_REG7D,
	LMREG_REG7E,
	LMREG_REG7F,
	LMREG_NUMOFREGS
};


#endif

