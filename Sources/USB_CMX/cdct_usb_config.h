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
#ifndef _CDC_CONFIG_H_
#define _CDC_CONFIG_H_
#include "hcc_types.h"

#define CDC_RX_EP_NO  3u
#define CDC_TX_EP_NO  2u
/*****************************************************************************/
/* Configuration values for the CDC demo. */
#define USB_NO_OF_STRING_DESC  7u
#define CMD_IFC_INDEX          0u
#define DATA_IFC_INDEX         1u

#define EP0_PACKET_SIZE      8u
#define EP1_PACKET_SIZE     32u
#define EP2_PACKET_SIZE     32u
#define EP3_PACKET_SIZE     32u


extern void usb_cfg_init(void);
/***************************************************************************
 * Functions exported to the USB driver.
 **************************************************************************/
extern void* get_device_descriptor(void);
extern hcc_u8 is_cfgd_index(hcc_u16 cndx);
extern void *get_cfg_descriptor(hcc_u8 cndx);
extern hcc_u8 is_str_index(hcc_u8 sndx);
extern void *get_str_descriptor(hcc_u8 sndx);
extern hcc_u8 is_ifc_ndx(hcc_u8 cndx, hcc_u8 indx, hcc_u8 iset);
extern hcc_u8 is_ep_ndx(hcc_u8 cndx, hcc_u8 indx, hcc_u8 iset, hcc_u8 endx);
extern void *get_ep_descriptor(hcc_u8 cndx, hcc_u8 indx, hcc_u8 iset, hcc_u8 endx);
extern void *get_ep_rx_buffer(hcc_u8 ep, hcc_u8 buf);

#endif

/****************************** END OF FILE **********************************/
