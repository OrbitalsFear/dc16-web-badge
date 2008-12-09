/***************************************************************************
 *
 *            Copyright (c) 2007 by CMX Systems, Inc.
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
#ifndef _UTILS_H_
#define _UTILS_H_

#include "hcc_types.h"

extern void itoa(unsigned long number, char* buf, int length);
extern void itoah(int number, char* buf, int length);
extern hcc_u32 strtoi (char *str);
extern void *_memcpy(void *dst, const void *src, int n);
extern void *_memset(void *s, int c, int n);
#endif
/****************************** END OF FILE **********************************/
