/*************************************************************************
 * DISCLAIMER                                                            *
 * Services performed by FREESCALE in this matter are performed          *
 * AS IS and without any warranty. CUSTOMER retains the final decision   *
 * relative to the total design and functionality of the end product.    *
 * FREESCALE neither guarantees nor will be held liable by CUSTOMER      *
 * for the success of this project. FREESCALE disclaims all warranties,  *
 * express, implied or statutory including, but not limited to,          *
 * implied warranty of merchantability or fitness for a particular       *
 * purpose on any hardware, software ore advise supplied to the project  *
 * by FREESCALE, and or any product resulting from FREESCALE services.   *
 * In no event shall FREESCALE be liable for incidental or consequential *
 * damages arising out of this agreement. CUSTOMER agrees to hold        *
 * FREESCALE harmless against any and all claims demands or actions      *
 * by anyone on account of any damage, or injury, whether commercial,    *
 * contractual, or tortuous, rising directly or indirectly as a result   *
 * of the advise or assistance supplied CUSTOMER in connection with      *
 * product, services or goods supplied under this Agreement.             *
 *************************************************************************/
 /**
  Copyright (c) 2007 Freescale Semiconductor
  Freescale Confidential Proprietary
  \file     	MCU.h
  \brief    	MCU initialization functions
  \author   	Freescale Semiconductor
  \author     Jose Ruiz
  \author   	Guadalajara Applications Laboratory RTAC Americas
  \version    1.0
  \date     	17/October/2007

  * History:
  
  30/July/2007      Start of application coding                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
*/
#ifndef _Vendor_H
#define _Vendor_H

/* Includes */
#include "derivative.h"     /* include peripheral declarations */

/* Definitions */
#define CRYSTAL_12MHZ       /* 12 MHz external crystal */


/* Prototypes */
void Mcu_Init(void);
void MCG_Init(void);


#endif /* _MCU_H */
