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
#ifndef _TARGET_H_
#define _TARGET_H_

#include "hcc_types.h"
#include "derivative.h"

extern void _irq_restore (hcc_imask ip);
extern hcc_imask _irq_disable (void);
extern void hw_init(void);

#define SW1_ACTIVE()  (PTGD_PTGD0 == 0)
#define SW2_ACTIVE()  (PTGD_PTGD1 == 0)

#define LED1_ON       (PTED_PTED2=0)
#define LED2_ON       (PTED_PTED3=0)
#define LED3_ON       (PTFD_PTFD0=0)
#define LED4_ON       (PTFD_PTFD1=0)

#define LED1_OFF      (PTED_PTED2=1)
#define LED2_OFF      (PTED_PTED3=1)
#define LED3_OFF      (PTFD_PTFD0=1)
#define LED4_OFF      (PTFD_PTFD1=1)

#define LED1_TGL      (PTED_PTED2^=1)
#define LED2_TGL      (PTED_PTED3^=1)
#define LED3_TGL      (PTFD_PTFD0^=1)
#define LED4_TGL      (PTFD_PTFD1^=1)
#endif
/****************************** END OF FILE **********************************/
