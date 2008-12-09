/****************************************************************************
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

#ifndef _USB_CDC_H_
#define _USB_CDC_H_

#include"hcc_types.h"

typedef struct {
  hcc_u32 bps;
  hcc_u8 ndata;
  hcc_u8 nstp;
  hcc_u8 parity;
} line_coding_t;

extern int cdc_putch(char c);
extern int cdc_kbhit(void);
extern int cdc_getch(void);
extern void cdc_init(void);
extern int cdc_line_coding_changed(void);
extern void cdc_get_line_coding(line_coding_t *l);
extern void CDC_Send_String(unsigned char *Array_to_send);
extern void CDC_Send_Buffer(unsigned char *Array_to_send,unsigned int u16Size);
extern void cdc_process(void);
#endif

/****************************** END OF FILE **********************************/
