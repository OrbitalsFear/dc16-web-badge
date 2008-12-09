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
  \file     	MCU.c
  \brief    	MCU initialization functions
  \author   	Freescale Semiconductor
  \author     Jose Ruiz
  \author   	Guadalajara Applications Laboratory RTAC Americas
  \version    1.0
  \date     	17/October/2007

  * History:
  
  30/July/2007      Start of application coding                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
*/

/* Include Files */
#include "MCU.h"            /* Include MCU Header */

/*****************************************************************************/
void Mcu_Init()
{
    SOPT1= SOPT1_STOPE_MASK;
    SPMSC1=SPMSC1_LVWACK_MASK;
}

/*****************************************************************************/

#ifdef CRYSTAL_12MHZ    

  void MCG_Init()
  {
	    /* the MCG is default set to FEI mode, it should be change to FBE mode*/
	    MCGC2=MCGC2_RANGE_MASK|MCGC2_HGO_MASK|MCGC2_EREFS_MASK|MCGC2_ERCLKEN_MASK;
                         
	    while(!MCGSC_OSCINIT);

      MCGC1= MCGC1_CLKS1_MASK | MCGC1_RDIV1_MASK | MCGC1_RDIV0_MASK | 
            MCGC1_IRCLKEN_MASK | MCGC1_IREFSTEN_MASK;

	    while((MCGSC & 0x1C ) != 0x08);		// check the external reference clock is selected or not

	    /* Switch to PBE mode from FBE */

      MCGC3=MCGC3_PLLS_MASK | MCGC3_VDIV3_MASK;

	    while ((MCGSC & 0x48) != 0x48);		//wait for the PLL is locked & 

	    /* Switch to PEE mode from PBE mode */
	    MCGC1&= MCGC1_RDIV2_MASK | MCGC1_RDIV1_MASK | MCGC1_RDIV0_MASK | 
	            MCGC1_IREFS_MASK | MCGC1_IRCLKEN_MASK | MCGC1_IREFSTEN_MASK;
	        
	    while((MCGSC & 0x6C) != 0x6C);
  }
#endif

