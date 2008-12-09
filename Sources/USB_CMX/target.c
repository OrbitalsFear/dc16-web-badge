/***************************************************************************
 *
 *            Copyright (c) 2006-2007 by CMX Systems, Inc.
 *
 * This software is copyrighted by and is the sole property of
 * CMX.  All rights, title, ownership, or other interests
 * in the software remain the property of CMX.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of CMX.
 *
 * CMX reserves the right to modify this software without notice.
 *
 * CMX Systems, Inc.
 * 12276 San Jose Blvd. #511
 * Jacksonville, FL 32223
 * USA
 *
 * Tel:  (904) 880-1840
 * Fax:  (904) 880-1632
 * http: www.cmx.com
 * email: cmx@cmx.com
 *
 ***************************************************************************/
#include "target.h"
#include "hcc_types.h"
#include "derivative.h"

static void init_clock(void)
{
 /* Assume 12MHz external clock source connected. */
  
  /* In order to use the USB we need to enter PEE mode and MCGOUT set to 48 MHz. 
     Out of reset MCG is in FEI mode. */
     
  /**** Moving from FEI (FLL engaged internal) to PEE (PLL engaged external) mode. */ 
  /* switch from FEI to FBE (FLL bypassed external) */ 
    /* enable external clock source */
  MCGC2 = MCGC2_HGO_MASK       /* oscillator in high gain mode */
          | MCGC2_EREFS_MASK   /* because crystal is being used */
          | MCGC2_RANGE_MASK   /* 12 MHz is in high freq range */
          | MCGC2_ERCLKEN_MASK;     /* activate external reference clock */
  while (MCGSC_OSCINIT == 0)
    ;
    /* select clock mode */
  MCGC1 = (2<<6)         /* CLKS = 10 -> external reference clock. */
          | (3<<3);      /* RDIV = 3 -> 12MHz/8=1.5 MHz */
  
    /* wait for mode change to be done */
  while (MCGSC_IREFST != 0)
    ;
  while (MCGSC_CLKST != 2)
    ;
    
  /* switch from FBE to PBE (PLL bypassed internal) mode */
  MCGC3=MCGC3_PLLS_MASK
        | (8<<0);     /* VDIV=6 -> multiply by 32 -> 1.5MHz * 32 = 48MHz */
  while(MCGSC_PLLST != 1)
    ;
  while(MCGSC_LOCK != 1)
    ;
  /* finally switch from PBE to PEE (PLL enabled external mode) */
  MCGC1 = (0<<6)         /* CLKS = 0 -> PLL or FLL output clock. */
          | (3<<3);      /* RDIV = 3 -> 12MHz/8=1.5 MHz */
  while(MCGSC_CLKST!=3)
    ;
    
  /* Now MCGOUT=48MHz, BUS_CLOCK=24MHz */  
}


void init_board(void)
{
  /* Configure LED io pins to be outputs */
  PTEDD=(1<<3) | (1<<2);
  PTFDD=(1<<5) | (1<<1) | (1<<0);
  PTCDD=(1<<4) | (1<<2);
  PTDDD=(1<<2);
  /* Enable internal pull-ups on port E pins to get switches working. */
  PTGPE=0x0F;
  PTGDD=0x00;
  
  PTED_PTED2=0;
  PTEDD_PTEDD2=1;

    
}

void _irq_restore (hcc_imask ip)
{
  if(ip)
  {
    /* Disable interrupts */
    asm ("sei");
  }
  else
  { 
    /* Enable interrupts */
    asm("cli");
  }
}

hcc_imask _irq_disable (void)
{
  hcc_u8 r;
  asm ("tpa;");     /* transfer CCR to A */
  asm ("sei;");     /* disable interrupts */
  asm ("and #8;");  /* mask I bit of CCR */
  asm ("sta r;");
  return(r);
}



/********************************************************************/
void hw_init(void)
{
  /* Disable watchdog. */
  SOPT1_COPT=0;
  
  init_clock();
  //init_board();   // JG
  //_irq_restore(0);  // JG
}
/****************************** END OF FILE **********************************/
