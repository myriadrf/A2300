// 4ChTxRxRegisterDefs.vh


//*******************************************************************
//*******************************************************************
//DEFAULT WAVEFORM ASR-2300 Registers (Component ID = 0)		 
//*******************************************************************
//*******************************************************************

//*******************************************************************
// RF I/O Selection and Configuration Register
//*******************************************************************
// These are included in WcaHalRegisterDefs.vh

// WCAHAL_IDWAVEFORM ( WcaReadWordRegister)
//`define WCAHAL_IDWAVEFORM	8'h1

// WCAHAL_VERSION ( WcaReadWordRegister)
//`define WCAHAL_VERSION		8'h2

// WCAHAL_PORTCAPS ( WcaReadWordRegister)
//`define WCAHAL_PORTCAPS		8'h3
//`define WCAHAL_RFCONFIG        8'h4
//`define WCAHAL_LED		8'h5

//*******************************************************************
// 4CH TX/RX Register definitions.
//*******************************************************************

//The Logic defines four DSP channels identified by a WCA Component ID
//as follows:
// 
`define WCACOMP_DSP_DUC0  8'h10 // Digital Up Converter
`define WCACOMP_DSP_DDC0  8'h20 // Digital Down Converter

`define WCACOMP_DSP_DUC1  8'h30 // Digital Up Converter
`define WCACOMP_DSP_DDC1  8'h40 // Digital Down Converter.

//Each of these DSP channels define the following registers.  The specific
//register address is the sum of the DSP component ID and the register index below.

// DSP_DDUC_CTRL (WcaWriteByteRegister)
// DDC / DUC component Control register.  Bit flags 2 through 5 are
// only availble if the waveform was compiled with dynamic configuration
// enabling bypass of internal functions.  To achieve maximum 32 MHz sampling
// rate bypass CIC decimation, CIC strobe, and half band filters (0x39, 57 dec.).  To achieve 
//	16 MHz sampling rate,  bypass CIC functions  (0x19, 25 dec.).  To directly sample front-end
// with decimation sampling rate, bypass decimation, but use sampling strobe (0x29, 45 dec. )
//  
// bit#  |  Description
// -------|----------------------------------------------------------
//  0	      (1)Enable / (0)Disable Component USB port streaming.
//  1	      Reset FIFOS. 
//  2	   	Bypass Cordic rotation (DYNAMIC_CONFIG only)
//  3	 	   Bypass CIC Decimation/Interpolation  (DYNAMIC_CONFIG only)
//  4			Bypass CIC Decimation/Interpolation strobe (DYNAMIC_CONFIG only)
//				When enabled, the decimation/interpolation strobe is bypassed, and the original 
//			   sampling strobe is used to drive down/up stream.  
//  5       Bypass Half band filter (DYNAMIC_CONFIG only).  


//
// functions
// No values currently defined.  
`define DSP_DDUC_CTRL	  	8'h00

// DSP_DDUC_PHASERATE(WcaWriteDwordRegister)
// Frequency Translation rate.  This is the phase rotation to apply
// at the 32 MHz sample rate to change the center frequency of the RF 
// baseband input/output. These values are a 32 bit counter value, where
// the top 12 bits are used to drive cordic rotations.
`define DSP_DDUC_PHASERATE 8'h01

// DSP_DDUC_SAMPRATE(WcaWriteWordRegister)
// Decimation / Integration sampling rate conversion .  This is the 
// rate for down/up sampling sampling the 32 MHz sampling rate.  Valid
// range is 2 to 4096 equivalent to 8 MHz to  3906.25 KHz.  
`define DSP_DDUC_SAMPRATE  8'h02 



