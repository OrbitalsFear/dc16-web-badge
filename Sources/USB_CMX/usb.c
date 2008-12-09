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
#include "target.h"

/*****************************************************************************
 * Local types.
 *****************************************************************************/
typedef struct {
  volatile hcc_u32 tlength;
  volatile hcc_u32 maxlength;
  void * volatile address;
  volatile usb_callback_t data_func;
  hcc_u16 psize;
  volatile hcc_u8 state;
  volatile hcc_u8 flags;
  volatile hcc_u8 error;
  volatile hcc_u8 data0;      /* Data toggle bit for endpoint. */
} ep_info_t;


/*****************************************************************************
 * Macro definitions.
 *****************************************************************************/
#define DIR_TX             BIT7
#define MIN(a,b)     ((a) < (b) ? (a) : (b))

/* Packet identifier values. */
#define SOF_PID_VALUE		    0xd
#define DATA_PID_VALUE      0x3
#define DATA0_PID_VALUE     0x3
#define DATA1_PID_VALUE     0xb

/* Control endpoint state machine state values. */
#define EPST_IDLE             0x0
#define EPST_DATA_TX          0x1
#define EPST_DATA_TX_LAST     0x2
#define EPST_DATA_RX          0x3
#define EPST_STATUS_TX        0x4
#define EPST_STATUS_RX        0x5
#define EPST_TX_STOP          0x6
#define EPST_ABORT_TX         0x7
#define EPST_DATA_TX_WAIT_DB  0x8
#define EPST_DATA_TX_EMPTY_DB 0x9

/* Standard USB feature selector values. */
#define FEAT_ENDPOINT_HALT        0u
#define FEAT_DEVICE_REMOTE_WAKEUP 1u

/* Endpoint flag bits. */
#define EPFL_ERROR    (hcc_u8)BIT0  /* There was an error during the ongoing
                               transfer. */
#define EPFL_ZPACKET  (hcc_u8)BIT1  /* After the last data packet an additional zero
                               length packet needs to be transmitted to close
                               the transfer. */

/* number of endpoint module supports */
#define NO_OF_HW_EP     6

/* Base address of the USB RAM area. */
#define BDT_BASE          0x1860u
#define BDT_CTRL(bndx)    (*(hcc_u8*)(BDT_BASE+(3*(bndx))+0))
#define BDT_LENGTH(bndx)  (*(hcc_u8*)(BDT_BASE+(3*(bndx))+1))
#define BDT_ADDR(bndx)    (*(hcc_u8*)(BDT_BASE+(3*(bndx))+2))

#define BDT_CTRL_OWN      (1u<<7)
#define BDT_CTRL_DATA01   (1u<<6)
#define BDT_CTRL_KEEP     (1u<<5)
#define BDT_CTRL_NINC     (1u<<4)
#define BDT_CTRL_DTS      (1u<<3)
#define BDT_CTRL_BDTSTALL (1u<<2)
#define BDT_CTRL_BDTKPID  (0xfu<<2)

#define BDT_END   0x20
#define EP_RAM_SIZE  256
/* This will give us array like access to EPCTL registers. */
#define EPCTL (&EPCTL0)

/* USB ram buffers must be 4 byte aligned. */
#define ALIGN_RAM_BUF(adr)  ((adr) & 0xf ? ((adr) & 0xf0)+0x10 : (adr))

/*****************************************************************************
 * Global variables.
 *****************************************************************************/
/* N/A */

/*****************************************************************************
 * Module variables.
 *****************************************************************************/
static volatile hcc_u8 usb_current_config;
static volatile hcc_u8 usb_state;
static volatile hcc_u8 new_address;
static ep_info_t ep_info[NO_OF_HW_EP];

/*****************************************************************************
 * Function predefinitions.
 *****************************************************************************/
static void enter_default_state(void);
static void disable_ep(hcc_u8);
static void disable_ep(hcc_u8);
static void ready_ep_rx(hcc_u8, hcc_u8);
static void ready_ep_tx(hcc_u8, hcc_u8);
static void send_zero_packet(hcc_u8 ep);
static void _usb_send(hcc_u8 ep);
static void _usb_receive(hcc_u8 ep);
static callback_state_t cb_set_address(void);
static callback_state_t usb_stm_ctrl0(void);
static hcc_u8 usb_setup_ep(hcc_u8 addr, hcc_u8 type, hcc_u8 ep, hcc_u16 psize);

extern void enable_usb_pull_up();

/*****************************************************************************
 * Name:
 *   usb_get_done
 * In:
 *   ep: number of endpoint.
 * Out:
 *   The number of bytes the endpoint tansferred.
 *
 * Description:
 *   Returns te number of bytes that were transferred.
 *****************************************************************************/
hcc_u32 usb_get_done(hcc_u8 ep)
{
  return(ep_info[ep].maxlength-ep_info[ep].tlength);
}

/*****************************************************************************
 * Name:
 *    usb_ep_is_busy
 * In:
 *   ep: number of endpoint.
 * Out:
 *   nonzero if endpoint is buys (a transfer is ongoing).
 *
 * Description:
 *
 *****************************************************************************/
hcc_u8 usb_ep_is_busy(hcc_u8 ep)
{
  return(ep_info[ep].state != EPST_IDLE ? (hcc_u8)1 : (hcc_u8)0);
}

/*****************************************************************************
 * Name:
 *    usb_get_state
 * In:
 *   N/A
 * Out:
 *   Current USB state. See USBST_xxx in usb.h
 *
 * Description:
 *
 *****************************************************************************/
hcc_u8 usb_get_state(void)
{
  return(usb_state);
}

/*****************************************************************************
 * Name:
 *    usb_ep_error
 * In:
 *   ep: number fo endpoint
 * Out:
 *   Endpoint specific error code. (See USBEPERR_xx macro definitions in usb.h)
 *
 * Description:
 *
 *****************************************************************************/
hcc_u8 usb_ep_error(hcc_u8 ep)
{
  hcc_u8 tmp=ep_info[ep].error;
  ep_info[ep].error=USBEPERR_NONE;
  return(tmp);
}


/*****************************************************************************
 * Name:
 *   ep2bdtndx
 * In:
 *   ep: number of endpoint.
 * Out:
 *   Pointer to the packet buffer containing data of the last received packet.
 *
 * Description:
 *   Returns te number of bytes that are left of the transfer. If
 *   usb_ep_is_busy returns false, then the transfer was aborted either
 *   by the host or by the application.
 *****************************************************************************/
static hcc_u8 ep2bdtndx(hcc_u8 ep, hcc_u8 dirodd) 
{
  hcc_u8 ndx;
  
  switch(ep) 
  {
    case 0:
      if (dirodd)  /* dirodd holds endpoint direction now */
      {
        ndx=1;     /* EP0 rx */
      }
      else 
      {
        ndx=0;     /* EP0 tx */
      }
      break;
    case 1:        /* value of dirodd is not used */
    case 2:
    case 3:
    case 4:
      ndx=ep+1;
      break;
    default:
      CMX_ASSERT(0);
  }
  return(ndx);
}

/*****************************************************************************
 * Name:
 *   usb_get_rx_pptr
 * In:
 *   ep: number of endpoint.
 * Out:
 *   Pointer to the packet buffer containing data of the last received packet.
 *
 * Description:
 *   Returns te number of bytes that are left of the transfer. If
 *   usb_ep_is_busy returns false, then the transfer was aborted either
 *   by the host or by the application.
 *****************************************************************************/
hcc_u8* usb_get_rx_pptr(hcc_u8 ep)
{
  hcc_u8 dirodd=0;

  if (ep==0)
  {
    dirodd=1;
  }
  
  return((hcc_u8 *)(BDT_BASE+(BDT_ADDR(ep2bdtndx(ep, dirodd))<<2)));
}


/*****************************************************************************
 * Name:
 *    send_zero_packet
 * In:
 *    N/A
 * Out:
 *    N/A
 *
 * Description:
 *    Will send a zero length data packet.
 *
 * Assumptions:
 *    ep is the index of a TX endpoint.
 *****************************************************************************/
static void send_zero_packet(hcc_u8 ep)
{ 
  if (ep < 7)
  {
    BDT_LENGTH(ep)  = 0;    
  }
  else
  {
    BDT_LENGTH(ep2bdtndx(ep, STAT_ODD ? 1 : 0)) = 0;
  }
  ready_ep_tx(ep, 0);  
}

/*****************************************************************************
 * Name:
 *    usb_init
 * In:
 *   ip - interrupt level for the USB interrupt
 * Out:
 *   0  - if all ok
 *   !0 - if failed
 *
 * Description:
 *   Initialises the usb driver. Will set the interrupt level.
 *   Note: clock sources includin the USB module clock shall be configured and
 *        stable prior the call to this function.
 *****************************************************************************/
hcc_u8 usb_init(void)
{
  /* Check module ID */
  if (PERID != 0x04)
  {
    /* Unknown USB module. */
    return(1);
  }

  if (PERID != (hcc_u8) ~IDCOMP)
  {
    return(1);
  }
  
  /* Reset USB module. Effect of reset is: 
        -Interrupts masked.
        -USB clock enabled
        -USB voltage regulator enabled
        -USB transceiver enabled
        -USBDP pullup disabled
        -Endpoints disabled except for EP0
        -USB address register set to zero. */
  USBCTL0_USBRESET = 1;
  while (USBCTL0_USBRESET)
    ;

  /* Enable phy and 3.3v voltage regulator. */
  USBCTL0 = USBCTL0_USBPHYEN_MASK | USBCTL0_USBVREN_MASK;


  /* Clear all pending interrupts except reset. */
  INTSTAT = ~INTSTAT_USBRSTF_MASK;
  
  /* Disable error interrupts. */   
  ERRENB=0;
  /* enable usb reset interrupt */
  INTENB_USBRST=1;
  
  /* Enable USB module. */
  CTL_USBEN=1;
  
  /* Enable pull-up resistor. */
  USBCTL0_USBPU=1;  
  return(0);
}

/*****************************************************************************
 * Name:
 *    usb_stop
 * In:
 *   n/a
 * Out:
 *   n/a
 *
 * Description:
 *   Stops USB driver.
 *****************************************************************************/
void usb_stop(void)
{
  /* Disabel pull-up resistor. */
  USBCTL0_USBPU=0;

  /* Reset USB module. */
  USBCTL0_USBRESET = 1;
  while (USBCTL0_USBRESET)
    ;
    
  /* Disable module. */  
  CTL_USBEN=0;

  /* Disable USBPHY */
  USBCTL0_USBPHYEN=0;

  /* Disable 3.3V ragulator. */
  USBCTL0_USBVREN=0;

  enter_default_state();
  (void)usb_setup_ep(0, EP_TYPE_DISABLE, 0, 0);
  usb_state=USBST_DISABLED;
}

/*****************************************************************************
 * Name:
 *    usb_setup_ep
 * In:
 *    addr  - endpoint address
 *    type  - endpoint type (control, bulk, interrupt, iso). This is the value
 *            of the endpoint type filed of the endpoint descriptor.
 *    ep    - number of endpoint
 *    psize - maximum packet size allowed for this endpoint.
 *    db    - nonzer of endpoint shall be double buffered. Note: only iso and
 *            bulk endpoints can be double buffered (this is hardware
 *            specific).
 * Out:
 *    0  - all ok
 *    !0 - initialisation failed
 *
 * Description:
 *    Configures the spcified endpoint.
 *****************************************************************************/
static hcc_u8 usb_setup_ep(hcc_u8 addr, hcc_u8 type, hcc_u8 ep, hcc_u16 psize)
{ 
  /* If one of these fails, the right endpoint descriptor need to be modified. */ 
  CMX_ASSERT(ep < NO_OF_HW_EP);
  CMX_ASSERT((addr & (hcc_u8)~DIR_TX) < 0x10);
  CMX_ASSERT(psize <= 0xff);
   
   /* Disable endpoint. */
  disable_ep(ep);

  if (ep_info[ep].state!=EPST_IDLE)
  {
    ep_info[ep].error=USBEPERR_HOST_ABORT;
  }
  else
  {
    ep_info[ep].error=USBEPERR_NONE;
  }

  ep_info[ep].state=EPST_IDLE;
  ep_info[ep].flags=0;
  ep_info[ep].data_func=(void*)0;
  ep_info[ep].address=0;
  ep_info[ep].tlength=0;
  ep_info[ep].maxlength=0;
  ep_info[ep].psize=psize;
  if (type == EP_TYPE_DISABLE)
  {
    return(0);
  }

  /* For ep 0 two BDT entries must be configured. */
  if (ep==0) 
  {
    /* Configure TX (in) BDT. */
    BDT_ADDR(0)=(hcc_u8)(BDT_END>>2);
    BDT_LENGTH(0)=(hcc_u8)psize;    
    BDT_CTRL(0) = 0;
    /* Configure RX (out) BDT. */
    BDT_ADDR(1)=(hcc_u8)(ALIGN_RAM_BUF(BDT_END+psize)>>2);
    BDT_LENGTH(1)=(hcc_u8)psize;
    BDT_CTRL(1) = BDT_CTRL_OWN | BDT_CTRL_DTS;
    
    EPCTL[0]=EPCTL0_EPRXEN_MASK | EPCTL0_EPTXEN_MASK | EPCTL0_EPHSHK_MASK;
    /* Cleat transaction suspend */
    CTL_TSUSPEND=0;
    
  } 
  else 
  { /* single buffered endpoint */
    hcc_u8 ram_ofs;
    
    /* Get next free USB ram address. Read BDT values of previous endpoint. */
    ram_ofs=(hcc_u8)(ALIGN_RAM_BUF((BDT_ADDR(ep)<<2)+BDT_LENGTH(ep)));
    
    /* This is a configuration error. Use smaller packet size, or
       remove some endpoints. */
    CMX_ASSERT(ram_ofs+psize < EP_RAM_SIZE);
  
    BDT_ADDR(ep+1)=(hcc_u8)(ram_ofs>>2);
    BDT_LENGTH(ep+1)=(hcc_u8)psize;
    BDT_CTRL(ep+1)=0;
    if (addr & DIR_TX) 
    {
      EPCTL[ep]=EPCTL0_EPTXEN_MASK | EPCTL0_EPHSHK_MASK;
    }
    else 
    {
      EPCTL[ep]=EPCTL0_EPRXEN_MASK | EPCTL0_EPHSHK_MASK;
    }
  }
  return(0);
}

/*****************************************************************************
 * Name:
 *    set_config
 * In:
 *    cfg_ndx - index of the configuration to be activated. The value shall
 *              shall equal to one defined in a configuration descriptor.
 * Out:
 *    N/A
 *
 * Description:
 *    Configures the USB module according to the specifyed configuration.
 * Assumptions:
 *    the spefified configuration exists.
 *    the first interface descriptor is for the default alternate setting.
 *    interfaces must be numbered from 0 increasing continously (0,1,2,3...)
 *    configurations must be numbered from 0 increasing continously
 *****************************************************************************/
static void set_config(hcc_u8 cfg_ndx)
{
  hcc_u8 cfg_ep=1;

  /* All endpoint shall use buffer 0 now. */
  CTL_ODDRST=1;

  usb_current_config=cfg_ndx;

  CTL_ODDRST=0;
    
  if (cfg_ndx != 0)
  {
    /* For all interfaces in this configuration. */
    hcc_u8 ifc=0;
    while(is_ifc_ndx(cfg_ndx, ifc, 0))
    { /* Emdpoint descriptor index in configuration descriptor. */
      hcc_u8 ifc_ep=0;
      while(is_ep_ndx(cfg_ndx, ifc, 0, ifc_ep))
      {
        const hcc_u8 *epd=get_ep_descriptor(cfg_ndx, ifc, 0, ifc_ep);
        /* Endpoint index and address is the same. */
        (void)usb_setup_ep(epd[2], epd[3], (hcc_u8)(epd[2] & 0x7fu), RD_LE16(&epd[4]));
        ifc_ep++;
        cfg_ep++;
      }
      ifc++;
    }
    usb_state = USBST_CONFIGURED;
  }
  else
  {
    usb_state=USBST_ADDRESSED;
    /* No endpoints to configure. The loop below will disable all except 0. */
  }

  while(cfg_ep < NO_OF_HW_EP)
  {
    (void)usb_setup_ep(0, EP_TYPE_DISABLE, cfg_ep++, 0);
  }
}

/*****************************************************************************
 * Name:
 *    enter_default_state
 * In:
 *    N/A
 * Out:
 *    N/A
 *
 * Description:
 *    Configure USB driver to reflect the default state. In this state only
 *    standard requests on the default pipe are answered, all other endpoints
 *    are disabled.
 *****************************************************************************/
static void enter_default_state(void)
{
  /* In this state the USB module responds to the default address.
     Only EP0 is configured. */

  /* Configure ep 0. */
  (void)usb_setup_ep(0, EP_TYPE_CONTROL, 0, EP0_PACKET_SIZE);
  /* Disable all other endpoints. */
  set_config(0);

  /* Set address to default address. */
  ADDR = 0;

  while(INTSTAT_TOKDNEF )         /* Flush any pending transactions because 4 status buffer */
        INTSTAT = 0xff;  
  usb_state=USBST_DEFAULT;
  new_address=0;
  
  /* Enable some interrupt sources. */
  INTENB = INTENB_USBRST_MASK| INTENB_TOKDNE_MASK | INTENB_SLEEP_MASK;  
}


/*****************************************************************************
 * Name:
 *    usb_stop_ep_tx
 * In:
 *    ep - endpoint number
 * Out:
 *    N/A
 * Description:
 *    Will stall a tx endpoint (endpoint will not transmit any more packages,
 *    all IN request from the host will be denyed with error handsake).
 *****************************************************************************/
void usb_stop_ep_tx(hcc_u8 ep)
{
  /* This cal needs to be protected againt USB interrupts, to
     make EPCTL access atomic. */
  hcc_imask im=_irq_disable();

  if (EPCTL[ep] & EPCTL1_EPTXEN_MASK)
  {    
    EPCTL[ep] |= EPCTL1_EPSTALL;
  }

  _irq_restore(im);
}

/*****************************************************************************
 * Name:
 *    usb_stop_ep_rx
 * In:
 *    ep - endpoint number
 * Out:
 *    N/A
 * Description:
 *    Will stall a rx endpoint (endpoint will not treceive any more packages,
 *    all OUT request from the host will be denyed with error handsake).
 *****************************************************************************/
void usb_stop_ep_rx(hcc_u8 ep)
{
  /* This cal needs to be protected againt USB interrupts, to
     make EPCTL access atomic. */
  hcc_imask im=_irq_disable();

  if (EPCTL[ep] & EPCTL1_EPRXEN_MASK)
  {    
    EPCTL[ep] |= EPCTL1_EPSTALL;
  }

  _irq_restore(im);
}

/*****************************************************************************
 * Name:
 *    ready_ep_tx
 * In:
 *    ep - endpoint number
 * Out:
 *    N/A
 * Description:
 *    Make tx endpoint ready for transmission.
 *****************************************************************************/
static void ready_ep_tx(hcc_u8 ep, hcc_u8 buf)
{ /* Simply give buffer to USB. */
  hcc_u32 ndx=ep2bdtndx(ep, buf);
  
  /* Give buffer to USB, set correct data0/1 flag, configure if data toggle
     synchronisation shall be used or not. */
  BDT_CTRL(ndx) = BDT_CTRL_OWN | ep_info[ep].data0 | BDT_CTRL_DTS;
}

/*****************************************************************************
 * Name:
 *    ready_ep_rx
 * In:
 *    ep - endpoint number
 * Out:
 *    N/A
 * Description:
 *    Make rx endpoint ready for reception.
 *****************************************************************************/
static void ready_ep_rx(hcc_u8 ep, hcc_u8 buf)
{ /* Simply give buffer to USB. */
  hcc_u32 ndx=ep2bdtndx(ep, buf);

  /* Give buffer to USB, set correct data0/1 flag, configure if data toggle
     synchronisation shall be used or not. */
  BDT_LENGTH(ndx)=(hcc_u8)ep_info[ep].psize;
  BDT_CTRL(ndx)= BDT_CTRL_OWN | ep_info[ep].data0 | BDT_CTRL_DTS;
}


/*****************************************************************************
 * Name:
 *    disable_ep
 * In:
 *    ep - endpoint number
 * Out:
 *    N/A
 * Description:
 *    Disable RX endpoint. Endpoint behaves as it would not exist (it will not
 *    affect the USB and will not generate any events).
 *****************************************************************************/
static void disable_ep(hcc_u8 ep)
{
  EPCTL[ep] = 0;
}

/*****************************************************************************
 * Name:
 *    _usb_send
 * In:
 *    ep - endpoint number
 * Out:
 *    N/A
 * Description:
 *    This fucntion inmplements the basic state machine for transmit (IN)
 *    endpoints. It will
 *        - call user callback functions if neccessary,
 *        - set endpoint specific error codes
 *        - split data to be sent to packet sized pieces
 *    Note: it is called from the interrupt handler routine and from "user
 *          space" too. The function needs to be reentrant!
 *****************************************************************************/
static void _usb_send(hcc_u8 ep)
{
  hcc_u32 l;
  hcc_u8 x;
  hcc_u8 bndx;

  switch (ep_info[ep].state)
  {
  case EPST_DATA_TX:
    l=MIN(ep_info[ep].tlength, ep_info[ep].psize);
    CMX_ASSERT(l == ep_info[ep].psize || l==ep_info[ep].tlength);      

    /* Select next ep buffer. */
    bndx=ep2bdtndx(ep, 0);
    
    /* Copy data to packet buffer. */
    for(x=0; x< l; x++)
    {
      ((hcc_u8*)(BDT_BASE+(BDT_ADDR(bndx)<<2)))[x]=*(hcc_u8*)ep_info[ep].address;
      ep_info[ep].address=((hcc_u8*)ep_info[ep].address)+1;      
    }
    
    /* Set tx packet length. */
    BDT_LENGTH(bndx)=(hcc_u8)l;

    /* Make buffer ready for transmission. */
    ready_ep_tx(ep, 0);
    
    /* Invert the data toggle bit. */
    ep_info[ep].data0 ^= BDT_CTRL_DATA01;

    /* Calculate transfer status. */
    ep_info[ep].tlength -= l;

    /* Is the just created packet the last one? */
    if (ep_info[ep].tlength == 0)
    { /* Do we need to send a zero length packet to terminate the transmission? */
      if ((l == ep_info[ep].psize) && (ep_info[ep].flags & EPFL_ZPACKET))
      {
        ep_info[ep].state=EPST_TX_STOP;
      }
      else
      {
        ep_info[ep].state=EPST_DATA_TX_LAST;
      }
      /* Warning: when getting there, an additional interrupt is needed to advance the
         state succesfully. */
      break;
    }
    break;
  case EPST_TX_STOP:
    /* Send a sort packet. */
    ep_info[ep].state=EPST_DATA_TX_LAST;
    send_zero_packet(ep);
    break;
# if 0    
TODO: double buffering?
  case EPST_DATA_TX_WAIT_DB:
    /* Let second buffer get empty while waiting for a new buffer. */
    ep_info[ep].state=EPST_DATA_TX_EMPTY_DB;
    break;    
#endif     
  case EPST_DATA_TX_LAST:
    /* Enter IDLE state. */
    ep_info[ep].state=EPST_IDLE;
    break;
  default:
    CMX_ASSERT(0);

  }
}

/*****************************************************************************
 * Name:
 *    usb_send
 * In:
 *    ep          - endpoint number
 *    f           - pointer to user callback function. A callback will bemade
 *                  if:  - the buffer is empty and more data needs to be sent
 *                       - all transmission is finished
 *                       - in case of an error
 *    data        - pointer to data buffer
 *    buffer_size - size of data buffer
 *    tr_length   - number of bytes to be transferred.
 *    req_length  - the number of bytes the host wants to receive.
 *
 *    Note: since all packes transmission on USB are started by the host, it
 *          needs to know how many bytes shall be transferred during a
 *          transfer. Because of this the host will always tell the device
 *          how many bytes it can receive (req_length). On the other hand, the
 *          the device may have less data ready (tr_length).
 * Out:
 *    N/A
 * Description:
 *    Using this function an TX (IN) transfer can be enabled.
 *****************************************************************************/
void usb_send(hcc_u8 ep, usb_callback_t f, hcc_u8* data, hcc_u32 tr_length, hcc_u32 req_length)
{
  hcc_imask itst;

  ep_info[ep].tlength=tr_length <= req_length ? tr_length : req_length;
  ep_info[ep].maxlength=ep_info[ep].tlength;
  ep_info[ep].data_func=f;
  ep_info[ep].address=data;
  ep_info[ep].flags=req_length > tr_length ?   (hcc_u8)EPFL_ZPACKET : (hcc_u8)0;
  ep_info[ep].error = USBEPERR_NONE;
  ep_info[ep].state=EPST_DATA_TX;

  itst=_irq_disable();
  _usb_send(ep);
  _irq_restore(itst);
}

/*****************************************************************************
 * Name:
 *    _usb_receive
 * In:
 *    ep - endpoint number
 * Out:
 *    N/A
 * Description:
 *    This fucntion inmplements the basic state machine for receive (OUT)
 *    endpoints. It will
 *        - call user callback functions if neccessary,
 *        - set endpoint specific error codes
 *        - reassemble packets into the specified buffer
 *    Note: it is called from the interrupt handler routine and from "user
 *          space" too. The function needs to be reentrant!
 *****************************************************************************/
void _usb_receive(hcc_u8 ep)
{
  hcc_u8 loops=0;
  int x;

  /* Copy data to USER space. */
  do
  {
    hcc_u8 odd=0;
    hcc_u8 bndx;
    hcc_u16 plength;

    if (ep==0)
    {
      odd=1;
    }
    
    bndx=ep2bdtndx(ep, odd);
    plength=BDT_LENGTH(bndx);

    /* Check if amount of received data is ok. */
    if (ep_info[ep].tlength < plength)
    { /* Host sent too many data! This is a protocol error. */
      usb_stop_ep_rx(ep);
      ep_info[ep].error = USBEPERR_TO_MANY_DATA;
      return;
    }

    /* Copy data to the user buffer. */
    for(x=0; x < plength; x++)
    {
      ((hcc_u8*)(ep_info[ep].address))[x]=((hcc_u8*)(BDT_BASE+(BDT_ADDR(bndx)<<2)))[x];
    }

    ep_info[ep].tlength -= plength;
    ep_info[ep].address = ((hcc_u8 *)ep_info[ep].address) + plength;

    /* Was this the last data packet? */
    if ((ep_info[ep].tlength == 0) || (ep_info[ep].psize != plength))
    {
      ep_info[ep].state=EPST_IDLE;
      /* Control endpoints will execute the callback after the status stage. */
      /* Check if control endpoint. (Note: both ENDPT registers will have
         the same content, so we check reg0. )*/
      if (ep)
      {
        /* Tell application transfer ended. */
        if (ep_info[ep].data_func != (void *) 0)
        {
          /* We don't care about the return valus since the transfer is already
             finished and we can do nothing in case of an error. */
          (void)(*ep_info[ep].data_func)();
    
          /* Disable further callbacks. */
          ep_info[ep].data_func = (void *)0;      
        }
      }
    }
  }while(loops);
}

/*****************************************************************************
 * Name:
 *    usb_receive
 * In:
 *    ep          - endpoint number
 *    f           - pointer to user callback function. A callback will bemade
 *                  if:  - the buffer is empty and more data needs to be sent
 *                       - all transmission is finished
 *                       - in case of an error
 *    data        - pointer to data buffer
 *    buffer_size - size of data buffer
 *    tr_length   - number of bytes to be transferred. (This shal be the same
 *                  amount that the host wants to send).
 *
 * Out:
 *    N/A
 * Description:
 *    Using this function an RX (OUT) transfer can be enabled.
 *****************************************************************************/
void usb_receive(hcc_u8 ep, usb_callback_t f, hcc_u8* data, hcc_u32 tr_length)
{

  ep_info[ep].tlength=tr_length;
  ep_info[ep].maxlength=ep_info[ep].tlength;
  ep_info[ep].data_func=f;
  ep_info[ep].address=data;
  ep_info[ep].state=EPST_DATA_RX;
  ep_info[ep].error=USBEPERR_NONE;
  ep_info[ep].flags=0;

  /* Enable next rx buffer. */
  if (ep)
  {
    ready_ep_rx(ep, 0);
  }
  else
  {
    ready_ep_rx(ep, 1);    
  }

  ep_info[ep].data0 ^= BDT_CTRL_DATA01;
}

/*****************************************************************************
 * Name:
 *    usb_abort_ep
 * In:
 *    ep          - endpoint number
 *
 * Out:
 *    N/A
 * Description:
 *    A call to this function will stop the ongoing transfer on the selected
 *    endpoint (early stop). Early stop is only possible if there is some
 *    penging data left in the buffer. If not, then transfer willstop anyway.
 *****************************************************************************/
void usb_abort_ep(hcc_u8 ep)
{
  /* Get endpoint into a fixed state. */
  hcc_imask itst=_irq_disable();
  /* If the endpoint is busy. */
  switch(ep_info[ep].state)
  {
  case EPST_IDLE:
    /* If endpoint is idle, we can not stop it. */
    break;
    /* In these cases is is too late for an early stop since the transfer
       is already going to end. */
  case EPST_DATA_TX_LAST:
  case EPST_STATUS_TX:
  case EPST_STATUS_RX:
    break;

  case EPST_DATA_RX:
    ep_info[ep].error=USBEPERR_USER_ABORT;
    ep_info[ep].flags |= EPFL_ERROR;

    /* Disable following calbacks in case of an error. */
    ep_info[ep].data_func=(void *) 0;
    /* Stall endpoint. */
    usb_stop_ep_rx(ep);
    break;
  default:
    /* UPS! invalid or unknown state... */
    CMX_ASSERT(0);
  }
  _irq_restore(itst);
}

/*****************************************************************************
 * Name:
 *    cb_set_address
 * In:
 *    N/A
 *
 * Out:
 *    N/A
 *
 * Description:
 *    This callback is used by the state machine that handles the standard
 *    requests on the default pipe to set the device address after the
 *    status stage of the "set address" request.
 *****************************************************************************/
static callback_state_t cb_set_address()
{
  if (new_address != 0)
  {
    usb_state=USBST_ADDRESSED;
    ADDR = new_address;
    new_address = 0;    
  }
  else
  {
    enter_default_state();
  }
  return(clbst_ok);
}

/*****************************************************************************
 * Name:
 *    usb_stm_ctrl0
 * In:
 *    N/A
 * Out:
 *    status of callback execution
 *
 * Description:
 *    Will decode and handle setup packets on the default endpoint.
 * Assumptions:
 *    Is only called if a setup packet is received.
 *****************************************************************************/
static callback_state_t usb_stm_ctrl0()
{
  hcc_u8 *pdata=(hcc_u8 *)usb_get_rx_pptr(0);

  /* The return value shall reflect the direction of the transfer. */
  callback_state_t r=(STP_REQU_TYPE(pdata) & 1u<<7) ? clbst_in: clbst_out;

  switch (STP_REQU_TYPE(pdata) & 0x7f)
  {
  case 0: /* Standard request for the device. */
    /* Determine what request this is. */
    switch (STP_REQUEST(pdata))
    {
    case USBRQ_SET_ADDRESS:
      new_address=(hcc_u8)(STP_VALUE_LO(pdata) & 0x7f);
      ep_info[0].data_func=cb_set_address;
      break;
    case USBRQ_GET_DESCRIPTOR:
      switch(STP_VALUE_HI(pdata))
      {
      case STDD_DEVICE:
        {
          hcc_u8 *p=(hcc_u8*)get_device_descriptor();
          usb_send(0, (void *) 0, p, p[0], STP_LENGTH(pdata));
        }
        break;
      case STDD_CONFIG:
        /* Do we have a CFG descriptor with the requested index? */
        {
          hcc_u8 cfg=STP_VALUE_LO(pdata);
          /* For index 0 we return the first config descriptor. */
          if (cfg == 0)
          {
            cfg++;
          }
          if (is_cfgd_index(cfg))
          {
            const hcc_u8 *cd;
            cd=get_cfg_descriptor(cfg);
            usb_send(0, (void *) 0, (void*)cd, RD_LE16(&cd[2])
                     , STP_LENGTH(pdata));
            break;
          }
        }
        /* No such descriptor, report error. */
        r=clbst_error;
        break;

      case STDD_STRING:
        /* See if te required descriptor exists. */
        if (is_str_index(STP_VALUE_LO(pdata)))
        {
          usb_send(0, (void *) 0, (void *)get_str_descriptor(STP_VALUE_LO(pdata))
                   , *(hcc_u8*)get_str_descriptor(STP_VALUE_LO(pdata))
                   , STP_LENGTH(pdata));
          break;
        }
        /* No such string descriptor. */
        r=clbst_error;
        break;

      default:
        /* Call user callback if avaiable. */
        goto call_usercb;
      }
      break;
    case USBRQ_GET_CONFIGURATION:
      usb_send(0, (void *) 0, (void *)&usb_current_config
                 , 1
                 , STP_LENGTH(pdata));
      break;
    case USBRQ_SET_CONFIGURATION:
      if (STP_VALUE_LO(pdata) == 0 || is_cfgd_index(STP_VALUE_LO(pdata)))
      {

        set_config(STP_VALUE_LO(pdata));
        break;
      }
      r=clbst_error;
      break;
    case USBRQ_SYNCH_FRAME:
    case USBRQ_SET_FEATURE:
    case USBRQ_CLEAR_FEATURE:
    case USBRQ_GET_STATUS:
    case USBRQ_SET_DESCRIPTOR:
    default: /* Unknown or not implemented request. */
      /* Call user callback if avaiable. */
      goto call_usercb;
    }
    break;
  case 1: /* Standard request for an interface. */
      /* Call user callback. */
      goto call_usercb;
  case 2: /* Standard request for an endpoint. */
    switch(STP_REQUEST(pdata))
    {
    case USBRQ_CLEAR_FEATURE:
      switch(STP_VALUE(pdata))
      {
      case FEAT_ENDPOINT_HALT:
        {
          /* Find the endpoint with the address specified in pdata->index.
          assumption: endpoints are allocated after each other. */
          hcc_u8 adr=STP_INDEX_LO(pdata) & 0x7f;
                   
          if (STP_INDEX_LO(pdata) & DIR_TX)
          {
            hcc_u8 bndx=ep2bdtndx(adr, 0);
            ep_info[adr].data0=0;
            BDT_CTRL(bndx) = 0;
          }
          else
          {
            hcc_u8 bndx;
            
            if(adr==0)
            {
              bndx=ep2bdtndx(adr, 1);
            }
            else
            {
              bndx=ep2bdtndx(adr, 0);              
            }
            ep_info[adr].data0=0;
            if (adr==0)
            {
              bndx++;
            }
            BDT_CTRL(bndx)=0;
          }
        }
        break;
      default:
        goto call_usercb;
      }
      break;
    default:
      /* Call user callback if avaiable. */
      goto call_usercb;
    }
    break;
  default:
  call_usercb:
    r=usb_ep0_callback();
  }

  return(r);
}

/*****************************************************************************
 * Name:
 *    usb_it_handler
 * In:
 *    N/A
 * Out:
 *    N/A
 *
 * Description:
 *    Low priority interrupt handler.
 *
 * Assumptions:
 *
 *****************************************************************************/
interrupt VectorNumber_Vusb void usb_it_handler(void)
{
  hcc_u8 istr;

    /* Save irq USB status. */
  istr=INTSTAT & INTENB;
  
  if(istr & ~INTSTAT_TOKDNEF_MASK)
  {
    if (istr & INTSTAT_USBRSTF_MASK)
    {
      /* Clear IT flag. */
      INTSTAT_USBRSTF=1;
      /* Enter default state. */
      enter_default_state();
      usb_reset_event();
      goto it_exit;
    }

    if (istr & INTSTAT_ERRORF_MASK)
    {
      INTSTAT_ERRORF=0;
      usb_bus_error_event();
    }
    
    if (istr & INTSTAT_RESUMEF_MASK)
    {
      INTENB_RESUME=0;
      
      INTSTAT_RESUMEF=1;
      INTENB_SLEEP=1;
      usb_wakeup_event();
    }
    
    if (istr & INTSTAT_SLEEPF_MASK)
    {
      INTENB_SLEEP=0;

      INTSTAT_SLEEPF=0;
      INTENB_RESUME=1;
      usb_suspend_event();
    }

    /* Stall has been sent. */
    if (istr & INTSTAT_STALLF_MASK)
    {
      INTSTAT_STALLF=1;
    }
  }

  if (istr & INTSTAT_TOKDNEF_MASK)
  {
    hcc_u8 ep=(hcc_u8)(STAT_ENDP);
    int is_rx;
    int is_stp=0;
  
    CMX_ASSERT(ep<7);

    if (STAT_IN)
    {
      is_rx=0;
    }
    else
    {/* This is an RX packet.*/
      is_rx=1;      
    }

    /* is this a control endpoint? */
    if (ep==0)
    {/* then use a special state machine. */
      is_stp=(BDT_CTRL(1) & BDT_CTRL_BDTKPID) == SOF_PID_VALUE<<2 ? 1 : 0;
      
      switch(ep_info[ep].state)
      {
      default:
        CMX_ASSERT(0);
        break;
      case EPST_IDLE:
      idle:
        if (is_stp)
        {
          /* Reset the endpoint state. */

          /* Since TOKBUSY is still set, this is a safe point to disable
             packet buffers (NAK in and out direction). */
          BDT_CTRL(0)=0;
          BDT_CTRL(1)=0;


          /* Dnager: if a setup frame will be received not in idle
             state, then the TOKBUSY will not be cleared,
             and the whole USB will not answer any more.*/

          /* This flag shall be cleared after each setup packet. Note:
             it seems it shall be cleared before trying to use endpoint
             buffers. */
          CTL_TSUSPEND=0;

          /* remove any callback */
          ep_info[ep].data_func = (void*)0;
          /* clear flags*/
          ep_info[ep].flags=0;
          /* clear user error indicator */
          ep_info[ep].error=USBEPERR_NONE;
          /* After a setup packet the next packet is always a data1. */
          ep_info[ep].data0 = BDT_CTRL_DATA01;

          switch(usb_stm_ctrl0())
          {
            case clbst_error:
              usb_stop_ep_tx(0);
              usb_stop_ep_rx(0);
              ep_info[0].state = EPST_IDLE;
              ep_info[0].error = USBEPERR_USER_ABORT;
              break;
            case clbst_not_ready:
              /* Nothing to do. */
              break;
            case clbst_in:
              ready_ep_rx(0, 1);
              break;
            case clbst_out:
              if (ep_info[0].state == EPST_IDLE)
              {
                goto send_status;
              }
              break;
            default:
              break;
          }
        }
        else
        {/* This is not a setup packet, and we encountered a protocol error.
            Possible reasons:
               -spurious package on the bus (not our fault)
               -we ended the transfer before the host.
                  - host and device transfer length was not the same
                  - driver error miscounted packages
          */
          /* Stall endpoint to make error visible to the host. */
          ep_info[0].error = USBEPERR_TO_MANY_DATA;

          usb_stop_ep_rx(0);
          usb_stop_ep_tx(0);
          CTL_TSUSPEND=0;
        }
        break;
      case EPST_DATA_TX:
      case EPST_DATA_TX_LAST:
      case EPST_TX_STOP:

        /* If there is an RX interrupt pending , stop transmission. */
        if (is_rx)
        {
          /* Inform application about transfer end. */
          ep_info[0].state=EPST_IDLE;
          if (ep_info[0].data_func != (void *)0)
          {
            (void)(*ep_info[0].data_func)();
            ep_info[0].data_func=(void *) 0;
          }
          /* Go to EPST_IDLE and handle the packet as it would be the start
             of a new transfer.  */
          goto idle;
        }
        
        if (ep_info[ep].state == EPST_DATA_TX_LAST)
        {
          break;          
        }
        
        _usb_send(0);

        /* If transfer is ended, then we shall receive a handshake from the host. */
        if (ep_info[0].state == EPST_IDLE)
        {
          ep_info[0].state=EPST_STATUS_TX;
        }
        break;
        
      case EPST_STATUS_TX:
        CMX_ASSERT(is_rx);
        /* In the IN transfer status stage we received a 0 byte long DATA 1 packet. */
        ep_info[0].state=EPST_IDLE;
        /* Tell application transfer ended. */
        if (ep_info[0].data_func != (void *) 0)
        {
          /* We don't care about the return value at this point, since we already
             sent the status, and the transfer is already ended. */
          (void)(*ep_info[0].data_func)();
          /* Disable callbacks. */
          ep_info[0].data_func = (void *)0;
        }
        ready_ep_rx(0, 1);
        break;
      case EPST_DATA_RX:
        _usb_receive(0);
        /* Was this the last packet to receive? */
        send_status:
        if(ep_info[0].state == EPST_IDLE)
        {
          /* If there was an error, stall the status stage. */
          if (ep_info[0].flags & EPFL_ERROR)
          {
            usb_stop_ep_tx(0);
          }
          else
          {
            /* Handshake is allways a data 1 packet. */
            ep_info[0].data0 = BDT_CTRL_DATA01;
            /* If all ok send handshake. */
            ep_info[0].state=EPST_STATUS_RX;
            /* Send a 0 byte long data1 packet as response.*/
            send_zero_packet(0);
          }
        }
        break;
      case EPST_STATUS_RX:
        /* In the OUT transfer status stage we sent a 0 byte long DATA 0 packet. */
        ep_info[0].state=EPST_IDLE;
        /* Tell application transfer ended. */
        if (ep_info[0].data_func != (void *) 0)
        {
          /* We don't care about the return value at this point, since we already
             sent the status, and the transfer is already ended. */
          (void)(*ep_info[0].data_func)();
          /* Disable callbacks. */
          ep_info[0].data_func = (void *)0;
        }
        ready_ep_rx(0, 1);
        break;
      }
    }
    else
    {
      /* Is this an OUT (rx) endpoint? */
      if(is_rx)
      {
        /* Handle reception. */
        _usb_receive(ep);
      }
      else
      { /* This is an IN endpoint (TX) */
        _usb_send(ep);
        if (ep_info[ep].state==EPST_IDLE)
        {
          /* Inform user application. */
          if (ep_info[ep].data_func != (void *)0)
          {
            (void)(*ep_info[ep].data_func)();
            /* Disable futher callbacks. */
            ep_info[ep].data_func = (void *)0;
          }
        }
      }
    }
	/* Clear interrupt flag. */
    INTSTAT_TOKDNEF=1;

  }
  it_exit:
  	;
}

/****************************** END OF FILE **********************************/


