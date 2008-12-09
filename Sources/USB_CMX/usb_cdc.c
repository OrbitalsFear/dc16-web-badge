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
#include "usb.h"
#include "usb_config.h"
#include "usb_cdc.h"
#include "dc16.h"

/****************************************************************************
 ************************** Macro definitions *******************************
 ***************************************************************************/
/* Mandatory class specific requests. */
#define CDCRQ_SEND_ENCAPSULATED_COMMAND 0x0
#define CDCRQ_GET_ENCAPSULATED_RESPONSE 0x1

/* Optional class specific requests. Windows usbser.sys depends on these. */
#define CDCRQ_SET_LINE_CODING           0x20
#define CDCRQ_GET_LINE_CODING           0x21
#define CDCRQ_SET_CONTROL_LINE_STATE    0x22
#define CDCRQ_GET_SERIAL_STATE          0x20

/* Optional not implemented class specific requests.
#define CDCRQ_SET_COMM_FEATURE          0x2
#define CDCRQ_GET_COMM_FEATURE          0x3
#define CDCRQ_CLEAR_COMM_FEATURE        0x4
#define CDCRQ_SEND_BREAK                0x23
*/

/* This macro will evaluate to an array inicializer list. It will set the
   content of a line coding structure to the defined values. */
#define FILL_LINE_CODING(bps, stop, parity, data_bits) \
  (bps) & 0xff, ((bps)>>8) & 0xff, ((bps)>>16) & 0xff, ((bps)>>24) & 0xff\
  , (hcc_u8)(stop), (hcc_u8)(parity), (hcc_u8)(data_bits)
/****************************************************************************
 ************************** Type definitions ********************************
 ***************************************************************************/

/****************************************************************************
 ************************** Function predefinitions. ************************
 ***************************************************************************/
callback_state_t got_line_coding(void);

/****************************************************************************
 ************************** Global variables ********************************
 ***************************************************************************/
/* none */

/****************************************************************************
 ************************** Module variables ********************************
 ***************************************************************************/
/* Buffer long enouh to hold one USB frame. (Length equals to rx packet
   size. */
static hcc_u32 rx_buffer[32/4];
static hcc_u8 rx_length;
static hcc_u8 rx_ndx;
/* Buffer long enouh to hold one USB frame. (Length equals to tx packet
   size. */
static hcc_u8 *cur_tx_buffer;
static hcc_u32 tx_buffer1[32/4];
static hcc_u32 tx_buffer2[32/4];
static hcc_u8 tx_ndx;

static hcc_u8 new_line_coding;
static hcc_u8 line_coding[7] = {
  FILL_LINE_CODING(115200, 0, 0, 8) /* Default is 115200 BPS and 8N1 format. */
};

static hcc_u8 ctrl_line_state;
static hcc_u16 uart_state;
/****************************************************************************
 ************************** Function definitions ****************************
 ***************************************************************************/

/*****************************************************************************
 * USB callback function. Is called by the USB driver if an USB error event
 * occurs.
 ****************************************************************************/
void usb_bus_error_event(void)
{
  /* empty */
}

/*****************************************************************************
 * USB callback function. Is called by the USB driver if an USB suspend event
 * occurs.
 ****************************************************************************/
void usb_suspend_event(void)
{
  /* empty */
}

/*****************************************************************************
 * USB callback function. Is called by the USB driver if an USB wakeup event
 * occurs.
 ****************************************************************************/
void usb_wakeup_event(void)
{
  /* empty */
}

/*****************************************************************************
 * USB callback function. Is called by the USB driver if an USB reset event
 * occurs.
 ****************************************************************************/
void usb_reset_event(void)
{
  /* empty */
}

/*****************************************************************************
 * USB callback function. Is called by the USB driver when the line coding
 * has been changed by the host.
 ****************************************************************************/
callback_state_t got_line_coding(void)
{
  new_line_coding=1;
  return(clbst_ok);
}

/*****************************************************************************
 * USB callback function. Is called by the USB driver if a non standard
 * request is received from the HOST. This callback extends the known
 * requests by masstorage related ones.
 ****************************************************************************/
callback_state_t usb_ep0_callback(void)
{
  hcc_u8 *pdata=usb_get_rx_pptr(0);

  callback_state_t r=clbst_error;

  /* A request to the command interface. */
  if (STP_INDEX(pdata) == CMD_IFC_INDEX)
  {
    switch(STP_REQU_TYPE(pdata))
    {
    /* Class specific in request. */
    case ((1<<7) | (1<<5) | 1):
      /* Host wants to get a descriptor */
      switch (STP_REQUEST(pdata))
      {
      case CDCRQ_GET_SERIAL_STATE:
        usb_send(0, 0, (void*)&uart_state, 2, STP_LENGTH(pdata));
        r=clbst_in;  
        break;      
      case CDCRQ_GET_LINE_CODING:
        usb_send(0, (void *) 0, (void *)&line_coding, 7, STP_LENGTH(pdata));
        r=clbst_in;
        break;
      case CDCRQ_GET_ENCAPSULATED_RESPONSE:
      default:
        break;
      }
      break;
    /* Class specific out request. */
    case ((0<<7) | (1<<5) | 1):
      switch (STP_REQUEST(pdata))
      {
      case CDCRQ_SET_LINE_CODING:
        usb_receive(0, got_line_coding, (void *)&line_coding, 7);
        r=clbst_out;
        break;
      case CDCRQ_SET_CONTROL_LINE_STATE:
        ctrl_line_state=(hcc_u8)STP_VALUE_LO(pdata);
        r=clbst_out;
        break;
      case CDCRQ_SEND_ENCAPSULATED_COMMAND:
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  return(r);
}

/*****************************************************************************
* Name:
*    cdc_getch
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Get the next character from rx_buffer.
*
* Assumptions:
*    
*****************************************************************************/
int cdc_getch(void)
{ 
  if (rx_ndx >= rx_length)
  {
    return(0);
  }
 
  /* Return the next character, and advance read index. */
  return((int)((hcc_u8*)rx_buffer)[rx_ndx++]);
}

/*****************************************************************************
* Name:
*    cdc_kbhit
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Will return one, if the rx_buffer contains any unread characters.
*
* Assumptions:
*    This function is called periodicaly. (Otherwise usb_receive call will
*    not bee made, and no data will ge received over the USB.)
*****************************************************************************/
int cdc_kbhit(void)
{
  cdc_process();
  /* If the receive buffer is not empty retun 1. */
  return(rx_length > rx_ndx);
}

/*****************************************************************************
* Name:
*    cdc_putch
* In:
*    c: character to be sent
* Out:
*    0: output is busy, character dropped
*    1: character buffered and will be sent later.
*
* Description:
*    Put one character into tx_buffer.
*
* Assumptions:
*    --
*****************************************************************************/
int cdc_putch(char c)
{
  unsigned int r=(unsigned char)c;
  /* Store character into current buffer. */
  if (tx_ndx < sizeof(tx_buffer1))
  {
    cur_tx_buffer[tx_ndx++]=(hcc_u8)c;
  }
  else
  {
    cdc_process();
    r++;
  }
  return((int)r);
}

/*****************************************************************************
 *
 ****************************************************************************/
int cdc_line_coding_changed(void)
{
  if(new_line_coding)
  {
    new_line_coding=0;
    return(1);
  }
  return(0);
}

/*****************************************************************************
 *
 ****************************************************************************/
void cdc_get_line_coding(line_coding_t *l)
{
  l->bps=RD_LE32(line_coding); 
  l->nstp=line_coding[4];
  l->parity=line_coding[5];
  l->ndata=line_coding[6];
}

/*****************************************************************************
 *
 ****************************************************************************/
void cdc_init(void)
{
  new_line_coding=0;
  uart_state=0;
  cur_tx_buffer=(hcc_u8*)tx_buffer1;
  tx_ndx=0;
  rx_ndx=rx_length=1;
}

/*****************************************************************************
 *    CUSTOM CDC FUNCTIONS
 ****************************************************************************/
 
void CDC_Send_String(unsigned char *Array_to_send)
{
  while(*Array_to_send!='\0')
	{
	  // if called while no usb is hooked up, usb buffer will get full and cause
	  // this to loop indefinitely
	  while(*Array_to_send != (char)cdc_putch(*Array_to_send));
		Array_to_send++;
		cdc_process();
	}
	delay_ms(25); // KP
}

void CDC_Send_Buffer(unsigned char *Array_to_send,unsigned int u16Size)
{
  unsigned int u16Counter=0;

	while(u16Size--)
	{
		while(*Array_to_send != (char)cdc_putch(*Array_to_send));
		Array_to_send++;
		cdc_process();
	}
}

/*****************************************************************************
 *
 ****************************************************************************/
void cdc_process(void)
{
  /* See if USB is in usable state. */    
  if (usb_get_state() == USBST_CONFIGURED)
  {    
    /*  If endpoint is not busy, and receive buffer is empty */
    if (!usb_ep_is_busy(CDC_RX_EP_NO) && (rx_length <= rx_ndx))
    {
      /* Read out error status of endpoint. The reception may be aborted due
         to a status change on the USB (disconnect, sleep, etc..) or due to
         an error (CRC, bit stuffing, etc...). In both case wee need to restart
         reception if possible.  */
      switch(usb_ep_error(CDC_RX_EP_NO))
      {
      case USBEPERR_NONE: /* Reception finished with no error. */
        /* If received bytes are not yet handled. */
        if (rx_length == 0)
        {          
          /* Read out number of received bytes. This will make us to return
             received characters for the next call. Note: the transfer may
             contain 0 characters, so we return the first character only when
             the next call is made. */
          rx_length=(hcc_u8)usb_get_done(CDC_RX_EP_NO);
          rx_ndx=0;
          /* If we did not received any data, then we need to start a new receive. */
          if (rx_length)
          {
            break;          
          }
        }
      case USBEPERR_PROTOCOL_ERROR:
        /* restart the reception */
        rx_length=0;
        usb_receive(CDC_RX_EP_NO, (void *) 0, (void *) rx_buffer, sizeof(rx_buffer));
        break;      
      case USBEPERR_HOST_ABORT:        
        /* This error can only be detected if error happens after execution of thefirst if
           in this function and before the switch. This is unlikely to happen. On the other
           hand this error meand usb has been disconnected or put to low power mode and thus
           we may safely ignore it. */
        break;
      case USBEPERR_USER_ABORT:
      case USBEPERR_TO_MANY_DATA:
      default:
        /* Upps! unexpected error. Stop here. */
        CMX_ASSERT(0);
      }
    }

    /* If tx buffer is not empty, start transmission. */
    if (!usb_ep_is_busy(CDC_TX_EP_NO) && (tx_ndx != 0))
    {
      /* Check the status of the next transfer. */
      switch (usb_ep_error(CDC_TX_EP_NO))
      {
      case USBEPERR_PROTOCOL_ERROR:   /* Ignore error, send next chunk. */
      case USBEPERR_HOST_ABORT:
      case USBEPERR_NONE: /* Finished with no error. */
        /* Start sending next chunk. */
        usb_send(CDC_TX_EP_NO, (void *) 0, (void *) cur_tx_buffer, tx_ndx, tx_ndx);
        /* Switch buffer. */
        tx_ndx=0;
        cur_tx_buffer = (hcc_u8*)((cur_tx_buffer == (hcc_u8*)tx_buffer1) ? tx_buffer2 : tx_buffer1);
        break;
      case USBEPERR_USER_ABORT:
      case USBEPERR_TO_MANY_DATA:
      default:
        /* Upps! unexpected error. Stop here. */
        CMX_ASSERT(0);
      }
    }
  }
}

/****************************** END OF FILE **********************************/
