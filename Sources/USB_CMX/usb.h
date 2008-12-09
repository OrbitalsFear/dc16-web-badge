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
#ifndef _USB_H_
#define _USB_H_
#include "hcc_types.h"

#include "usb_config.h"
#include "derivative.h"
#define USB_DRIVER_VERSION  0x0004
/******************************************************************************
 ************************ Type definitions ************************************
 *****************************************************************************/
 /* Return values for callback functions. */
typedef enum {
  clbst_ok,       /* Callback executed ok. */
  clbst_in,       /* Start IN transfer. */
  clbst_out,      /* Start out transfer. */
  clbst_error,    /* Error encountered, stop endpoint. */
  clbst_not_ready /* No buffer avaiable, pause endpoint. */
} callback_state_t;

typedef callback_state_t (*usb_callback_t)(void);
/******************************************************************************
 ************************ Exported functions **********************************
 *****************************************************************************/
extern hcc_u8 usb_init(void);
void usb_stop(void);
extern void usb_it_handler(void);

extern hcc_u8 usb_ep_is_busy(hcc_u8 ep);
extern hcc_u8 usb_ep_error(hcc_u8 ep);
extern hcc_u32 usb_get_done(hcc_u8 ep);
extern void usb_abort_ep(hcc_u8 ep);
extern void usb_send(hcc_u8 ep, usb_callback_t f, hcc_u8* data, hcc_u32 tr_length, hcc_u32 req_length);
extern void usb_receive(hcc_u8 ep, usb_callback_t f, hcc_u8* data, hcc_u32 tr_length);
extern hcc_u8 usb_get_state(void);

extern hcc_u16 usb_get_rx_plength(hcc_u8 ep);
extern hcc_u8 *usb_get_rx_pptr(hcc_u8 ep);
extern hcc_u16 usb_get_rx_plength(hcc_u8 ep);
extern void usb_stop_ep_rx(hcc_u8 ep);
extern void usb_stop_ep_tx(hcc_u8 ep);

/******************************************************************************
 ************************ Imported functions **********************************
 *****************************************************************************/
/* Event callbacks. */
void usb_bus_error_event(void);
void usb_wakeup_event(void);
void usb_suspend_event(void);
void usb_reset_event(void);
callback_state_t usb_ep0_callback(void);

/* Configuration callbacks. */
void* get_device_descriptor(void);
hcc_u8 is_cfgd_index(hcc_u16 cndx);
void *get_cfg_descriptor(hcc_u8 cndx);
hcc_u8 is_str_index(hcc_u8 sndx);
void *get_str_descriptor(hcc_u8 sndx);
hcc_u8 is_ifc_ndx(hcc_u8 cndx, hcc_u8 indx, hcc_u8 iset);
hcc_u8 is_ep_ndx(hcc_u8 cndx, hcc_u8 indx, hcc_u8 iset, hcc_u8 endx);
void *get_ep_descriptor(hcc_u8 cndx, hcc_u8 indx, hcc_u8 iset, hcc_u8 endx);
void *get_ep_rx_buffer(hcc_u8 ep, hcc_u8 buf);

/******************************************************************************
 ************************ Macro definitions ***********************************
 *****************************************************************************/
/* This 16 bit counter is increased each mS while the USB is connected and not
   in sleep state. */
#define usb_get_frame_ctr() ((hcc_u16)((FRMNUMH<<8) | FRMNUML))

/* These macros access fileds of a setup packet. */
#define STP_REQU_TYPE(a)	  (((hcc_u8*)(a))[0])
#define STP_REQUEST(a)      (((hcc_u8*)(a))[1])
#define STP_VALUE(a)        RD_LE16(((hcc_u8*)(a))+2)
#define STP_VALUE_LO(a)     (((hcc_u8*)(a))[2])
#define STP_VALUE_HI(a)     (((hcc_u8*)(a))[3])
#define STP_INDEX(a)        RD_LE16(((hcc_u8*)(a))+4)
#define STP_INDEX_LO(a)     (((hcc_u8*)(a))[4])
#define STP_INDEX_HI(a)     (((hcc_u8*)(a))[5])
#define STP_LENGTH(a)       RD_LE16(((hcc_u8*)(a))+6)
#define STP_LENGTH_LO(a)    (((hcc_u8*)(a))[6])
#define STP_LENGTH_HI(a)    (((hcc_u8*)(a))[7])



#define EP0_PACKET_SIZE 8u


/* Handled standard USB request. (bmRequest values). */
#define USBRQ_GET_STATUS         0u
#define USBRQ_CLEAR_FEATURE      1u
#define USBRQ_SET_FEATURE        3u
#define USBRQ_SET_ADDRESS        5u
#define USBRQ_GET_DESCRIPTOR     6u
#define USBRQ_SET_DESCRIPTOR     7u
#define USBRQ_GET_CONFIGURATION  8u
#define USBRQ_SET_CONFIGURATION  9u
#define USBRQ_GET_INTERFACE      10u
#define USBRQ_SET_INTERFACE      11u
#define USBRQ_SYNCH_FRAME        12u

/* Standard USB descriptor type vaules. */
#define STDD_DEVICE         1u
#define STDD_CONFIG         2u
#define STDD_STRING         3u
#define STDD_INTERFACE      4u
#define STDD_ENDPOINT       5u

/* Endpoint type vaues. */
#define EP_TYPE_CONTROL 0u
#define EP_TYPE_ISO     1u
#define EP_TYPE_BULK    2u
#define EP_TYPE_IT      3u
#define EP_TYPE_DISABLE 4u

/* Driver states. */
#define USBST_DISABLED    (1u<<0)
#define USBST_DEFAULT     (1u<<1) /* Only standard requests are handled on
                                     then default pipe. */
#define USBST_ADDRESSED   (1u<<2) /* USB has a unique address, but no
                                     configuration is active. */
#define USBST_CONFIGURED  (1u<<3) /* Device is configured, and fully
                                     functional. */

/* Error flags returned by usb_ep_error(). */
#define USBEPERR_NONE             (0) /* No error. */
#define USBEPERR_TO_MANY_DATA     (BIT0) /* To many data received. */
#define USBEPERR_PROTOCOL_ERROR   (BIT1) /* Protocol error. */
#define USBEPERR_USER_ABORT       (BIT2) /* Transfer was aborted by the
                                             application. */
#define USBEPERR_HOST_ABORT       (BIT3) /* Host aborted the transfer. */

/* This macro will evaluate to an array inicializer list with values of a
   device descriptor. */
#define USB_FILL_DEV_DESC(usb_ver, dclass, dsubclass, dproto, psize, vid,\
                          pid, relno, mstr, pstr, sstr, ncfg) \
  (hcc_u8)0x12u, STDD_DEVICE, (hcc_u8)(usb_ver), (hcc_u8)((usb_ver) >> 8)\
  , (hcc_u8)(dclass), (hcc_u8)(dsubclass), (hcc_u8)(dproto), (hcc_u8)(psize)\
  , (hcc_u8)(vid), (hcc_u8)((vid) >> 8), (hcc_u8)(pid), (hcc_u8)((pid) >> 8)\
  , (hcc_u8)(relno), (hcc_u8)((relno) >> 8), (hcc_u8)(mstr), (hcc_u8)(pstr)\
  , (hcc_u8)(sstr), (hcc_u8)(ncfg)

/* This macro will evaluate to an array inicializer list with values of a
   configuration descriptor. */
#define USB_FILL_CFG_DESC(size, nifc, cfg_id, str_ndx, attrib, pow) \
  (hcc_u8)0x09u, STDD_CONFIG, (hcc_u8)(size), (hcc_u8)((size) >> 8)\
  , (hcc_u8)(nifc), (hcc_u8)(cfg_id), (hcc_u8)(str_ndx), (hcc_u8)(attrib), (hcc_u8)(pow)
/* Values for the attrib field of the configuration descriptor. */
/* Devide ic BUS powered. */
#define CFGD_ATTR_BUS_PWR  (1u<<7)
/* Device is self powered. */
#define CFGD_ATTR_SELF_PWR (1u<<6)
/* Device can wake up the BUS. */
#define CFGD_ATTR_RWAKEUP  (1u<<5)


/* This macro will evaluate to an array inicializer list with values of an
   interface association descriptor. */
#define FILL_IFC_ASSOC_DESC(first_ifc, nifc, fclass, fsubclass, fproto, strndx)\
  (hcc_u8)0x08u, (hcc_u8)0x0bu, (hcc_u8) first_ifc, (hcc_u8) nifc\
  , (hcc_u8)(fclass), (hcc_u8)(fsubclass), (hcc_u8)(fproto), (hcc_u8)(strndx)

/* This macro will evaluate to an array inicializer list with values of a
   interface descriptor. */
#define USB_FILL_IFC_DESC(ifc_id, alt_set, no_ep, iclass, isubclass, iproto, strndx) \
  (hcc_u8)0x09u, STDD_INTERFACE, (hcc_u8)(ifc_id), (hcc_u8)(alt_set), (hcc_u8)(no_ep)\
  , (hcc_u8)(iclass), (hcc_u8)(isubclass), (hcc_u8)(iproto), (hcc_u8)(strndx)

/* This macro will evaluate to an array inicializer list with values of a
   endpoint descriptor. */
#define USB_FILL_EP_DESC(addr, dir, attrib, psize, interval) \
  (hcc_u8)0x07u, STDD_ENDPOINT, (hcc_u8)((addr)&0x7f) | (((hcc_u8)(dir))<<0x7)\
  , (hcc_u8)(attrib), (hcc_u8)((psize) & 0xff), (hcc_u8)(((psize) >> 8) & 0xff)\
  , (interval)

/* IN endpoint (device to host) */
#define EPD_DIR_TX                1
/* Out endpoint (host todevice) */
#define EPD_DIR_RX                0

/* Control endpoint */
#define EPD_ATTR_CTRL             0
/* Isochronous endpoint. */
#define EPD_ATTR_ISO              1
/* Bulk endpoint. */
#define EPD_ATTR_BULK             2
/* Interrupt endpoint. */
#define EPD_ATTR_INT              3
/* Iso endpoint synchronisation type: none */
#define EPD_ATTR_ISO_SYNC_NONE    (0 << 2)
/* Iso endpoint synchronisation type: asynchronous */
#define EPD_ATTR_ISO_SYNC_ASYNC   (1 << 2)
/* Iso endpoint synchronisation type: adaptive */
#define EPD_ATTR_ISO_SYNC_ADAPT   (2 << 2)
/* Iso endpoint synchronisation type: synchronous */
#define EPD_ATTR_ISO_SYNC_SYNC    (3 << 2)
/* Iso endpoint usage type: data endpoint */
#define EPD_ATTR_ISO_USAGE_DATA   (0 << 4)
/* Iso endpoint usage type: feedback endpoint */
#define EPD_ATTR_ISO_USAGE_FEEDB  (1 << 4)
/* Iso endpoint usage type: explicite feedback endpoint */
#define EPD_ATTR_ISO_USAGE_EFEEDB (2 << 4)

#endif
/****************************** END OF FILE **********************************/
