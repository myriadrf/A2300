/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING 
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT 
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR 
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE 
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software 
* and to discontinue the availability of this software.  By using this software, 
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2012 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_intc.h
* Version      : CodeGenerator for RL78/G14 V1.01.03.06 [10 Sep 2012]
* Device(s)    : R5F104LE
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for INTC module.
* Creation Date: 7/29/2013
***********************************************************************************************************************/

#ifndef INTC_H
#define INTC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/*
    External Interrupt Rising Edge Enable Register 0 (EGP0)
*/
/* INTPn pin valid edge selection (EGPn) */
#define _01_INTP0_EDGE_RISING_SEL       (0x01U)   /* rising edge selected for INTP0 pin */
#define _00_INTP0_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP0 pin */
#define _02_INTP1_EDGE_RISING_SEL       (0x02U)   /* rising edge selected for INTP1 pin */
#define _00_INTP1_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP1 pin*/
#define _04_INTP2_EDGE_RISING_SEL       (0x04U)   /* rising edge selected for INTP2 pin */
#define _00_INTP2_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP2 pin */
#define _08_INTP3_EDGE_RISING_SEL       (0x08U)   /* rising edge selected for INTP3 pin */
#define _00_INTP3_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP3 pin */
#define _10_INTP4_EDGE_RISING_SEL       (0x10U)   /* rising edge selected for INTP4 pin */
#define _00_INTP4_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP4 pin */
#define _20_INTP5_EDGE_RISING_SEL       (0x20U)   /* rising edge selected for INTP5 pin */
#define _00_INTP5_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP5 pin */
#define _40_INTP6_EDGE_RISING_SEL       (0x40U)   /* rising edge selected for INTP6 pin */
#define _00_INTP6_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP6 pin */
#define _80_INTP7_EDGE_RISING_SEL       (0x80U)   /* rising edge selected for INTP7 pin */
#define _00_INTP7_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP7 pin */

/*
    External Interrupt Falling Edge Enable Register 0 (EGN0)
*/
/* INTPn pin valid edge selection (EGNn) */
#define _01_INTP0_EDGE_FALLING_SEL      (0x01U)   /* falling edge selected for INTP0 pin */
#define _00_INTP0_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP0 pin */
#define _02_INTP1_EDGE_FALLING_SEL      (0x02U)   /* falling edge selected for INTP1 pin */
#define _00_INTP1_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP1 pin */
#define _04_INTP2_EDGE_FALLING_SEL      (0x04U)   /* falling edge selected for INTP2 pin */
#define _00_INTP2_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP2 pin */
#define _08_INTP3_EDGE_FALLING_SEL      (0x08U)   /* falling edge selected for INTP3 pin */
#define _00_INTP3_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP3 pin */
#define _10_INTP4_EDGE_FALLING_SEL      (0x10U)   /* falling edge selected for INTP4 pin */
#define _00_INTP4_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP4 pin */
#define _20_INTP5_EDGE_FALLING_SEL      (0x20U)   /* falling edge selected for INTP5 pin */
#define _00_INTP5_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP5 pin */
#define _40_INTP6_EDGE_FALLING_SEL      (0x40U)   /* falling edge selected for INTP6 pin */
#define _00_INTP6_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP6 pin */
#define _80_INTP7_EDGE_FALLING_SEL      (0x80U)   /* falling edge selected for INTP7 pin */
#define _00_INTP7_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP7 pin */

/*
    External Interrupt Rising Edge Enable Register 1 (EGP1)
*/
/* INTPn pin valid edge selection (EGPn) */
#define _01_INTP8_EDGE_RISING_SEL       (0x01U)   /* rising edge selected for INTP8 pin */
#define _00_INTP8_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP8 pin */
#define _02_INTP9_EDGE_RISING_SEL       (0x02U)   /* rising edge selected for INTP9 pin */
#define _00_INTP9_EDGE_RISING_UNSEL     (0x00U)   /* rising edge not selected for INTP9 pin*/
#define _04_INTP10_EDGE_RISING_SEL      (0x04U)   /* rising edge selected for INTP10 pin */
#define _00_INTP10_EDGE_RISING_UNSEL    (0x00U)   /* rising edge not selected for INTP10 pin */
#define _08_INTP11_EDGE_RISING_SEL      (0x08U)   /* rising edge selected for INTP11 pin */
#define _00_INTP11_EDGE_RISING_UNSEL    (0x00U)   /* rising edge not selected for INTP11 pin*/

/*
    External Interrupt Falling Edge Enable Register 1 (EGN1)
*/
/* INTPn pin valid edge selection (EGNn) */
#define _01_INTP8_EDGE_FALLING_SEL      (0x01U)   /* falling edge selected for INTP8 pin */
#define _00_INTP8_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP8 pin */
#define _02_INTP9_EDGE_FALLING_SEL      (0x02U)   /* falling edge selected for INTP9 pin */
#define _00_INTP9_EDGE_FALLING_UNSEL    (0x00U)   /* falling edge not selected for INTP9 pin */
#define _04_INTP10_EDGE_FALLING_SEL     (0x04U)   /* falling edge selected for INTP10 pin */
#define _00_INTP10_EDGE_FALLING_UNSEL   (0x00U)   /* falling edge not selected for INTP10 pin */
#define _08_INTP11_EDGE_FALLING_SEL     (0x08U)   /* falling edge selected for INTP11 pin */
#define _00_INTP11_EDGE_FALLING_UNSEL   (0x00U)   /* falling edge not selected for INTP11 pin */

/*
    Key Return Mode Register (KRM)
*/
/* Key interrupt mode control (KRM0) */
#define _00_KR0_SIGNAL_DETECT_OFF       (0x00U)   /* not detect KR0 signal */
#define _01_KR0_SIGNAL_DETECT_ON        (0x01U)   /* detect KR0 signal */
/* Key interrupt mode control (KRM1) */
#define _00_KR1_SIGNAL_DETECT_OFF       (0x00U)   /* not detect KR1 signal */
#define _02_KR1_SIGNAL_DETECT_ON        (0x02U)   /* detect KR1 signal */        
/* Key interrupt mode control (KRM2) */
#define _00_KR2_SIGNAL_DETECT_OFF       (0x00U)   /* not detect KR2 signal */
#define _04_KR2_SIGNAL_DETECT_ON        (0x04U)   /* detect KR2 signal */        
/* Key interrupt mode control (KRM3) */
#define _00_KR3_SIGNAL_DETECT_OFF       (0x00U)   /* not detect KR3 signal */
#define _08_KR3_SIGNAL_DETECT_ON        (0x08U)   /* detect KR3 signal */        
/* Key interrupt mode control (KRM4) */
#define _00_KR4_SIGNAL_DETECT_OFF       (0x00U)   /* not detect KR4 signal */
#define _10_KR4_SIGNAL_DETECT_ON        (0x10U)   /* detect KR4 signal */        
/* Key interrupt mode control (KRM5) */
#define _00_KR5_SIGNAL_DETECT_OFF       (0x00U)   /* not detect KR5 signal */
#define _20_KR5_SIGNAL_DETECT_ON        (0x20U)   /* detect KR5 signal */        
/* Key interrupt mode control (KRM6) */
#define _00_KR6_SIGNAL_DETECT_OFF       (0x00U)   /* not detect KR6 signal */
#define _40_KR6_SIGNAL_DETECT_ON        (0x40U)   /* detect KR6 signal */        
/* Key interrupt mode control (KRM7) */
#define _00_KR7_SIGNAL_DETECT_OFF       (0x00U)  /* not detect KR7 signal */
#define _80_KR7_SIGNAL_DETECT_ON        (0x80U)   /* detect KR7 signal */        

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define KEY_WAITTIME                    (8U)      /* change the waiting time according to the system */ 

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_INTC_Create(void);
void R_INTC0_Start(void);
void R_INTC0_Stop(void);
void R_INTC6_Start(void);
void R_INTC6_Stop(void);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
