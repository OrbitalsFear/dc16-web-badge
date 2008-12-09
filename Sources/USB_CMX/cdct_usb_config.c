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
#include "cdct_usb_config.h"

// these defines changed by kingpin
#define USB_VENDOR_ID   0xbeefu
#define USB_PRODUCT_ID  0x1337u
#define USB_DEVICE_REL_NUM  0x666u


const hcc_u8 string_descriptor0[4] = {4, 0x3, 0x09, 0x04 };
const hcc_u8 str_manufacturer[36] = { 36, 0x3, 'G',0, 'r',0, 'a',0, 'n',0
                  , 'd',0, ' ',0, 'I',0, 'd',0, 'e',0, 'a',0, ' ',0, 's',0
                  , 't',0, 'u',0, 'd',0, 'i',0, 'o',0};
const hcc_u8 str_config[44] = { 44, 0x3, 'D',0, 'e',0, 'f',0, 'a',0, 'u',0
                  , 'l',0, 't',0, ' ',0, 'c',0, 'o',0, 'n',0, 'f',0, 'i',0
                  , 'g',0, 'u',0, 'r',0, 'a',0, 't',0, 'i',0, 'o',0, 'n',0};
const hcc_u8 str_interface1[8] = { 8, 0x3, 'C',0, 'D',0, 'C',0 };
const hcc_u8 str_interface2[18] = { 18, 0x3, 'C',0, 'D',0, 'C',0, '-',0, 'D',0
                  , 'a',0, 't',0, 'a',0, };

const hcc_u8 str_serail_number[10] = { 10, 0x3, 'V',0, '1',0, '.',0, '0',0};
const hcc_u8 str_product[32] = { 32, 0x3, 'D',0, 'E',0, 'F',0, 'C',0, 'O',0
                  , 'N',0, ' ',0, '1',0, '6',0, ' ',0, 'B',0, 'a',0, 'd',0
                  , 'g',0, 'e',0};

const hcc_u8 * const usb_string_descriptors[USB_NO_OF_STRING_DESC] = {
  string_descriptor0, str_manufacturer, str_product, str_serail_number
  , str_config, str_interface1, str_interface2
};

const hcc_u8 usb_device_descriptor[] = {
  USB_FILL_DEV_DESC(0x0101, 2, 0, 0, EP0_PACKET_SIZE, USB_VENDOR_ID, USB_PRODUCT_ID
      , USB_DEVICE_REL_NUM, 0, 0, 0, 1)
};

#define USB_FILL_HDR_FUNCT_DESCR(cdc_ver)\
  0x5, 0x24, 0x0, (hcc_u8)(cdc_ver), (hcc_u8)((cdc_ver)>>8)

#define USB_FILL_CALL_MGM_FUNCT_DESCR(capability, dataifc)\
  0x5, 0x24, 0x1, (hcc_u8)(capability), (hcc_u8)(dataifc)

#define USB_FILL_ACM_FUNCT_DESCR(capability)\
  0x4, 0x24, 0x2, (hcc_u8)(capability)

#define USB_FILL_UNION_FUNCT_DESCR(master_ifc, no_of_slaves)\
  0x4+(no_of_slaves), 0x24, 0x6, (hcc_u8)(master_ifc)


const hcc_u8 usb_config_descriptor[] = {
  USB_FILL_CFG_DESC(9+9+5+4+5+5+7+9+7+7, 2, 1, 4, CFGD_ATTR_SELF_PWR, 0),
  USB_FILL_IFC_DESC(0, 0, 1, 2, 2, 1, 5), /* Comm interface/ abstract contorl model/ 00 no class protocol*/
  USB_FILL_HDR_FUNCT_DESCR(0x0110),
  USB_FILL_CALL_MGM_FUNCT_DESCR(0, 1),
  USB_FILL_ACM_FUNCT_DESCR(0),
  USB_FILL_UNION_FUNCT_DESCR(0, 1),1,
  USB_FILL_EP_DESC(0x1, EPD_DIR_TX, EPD_ATTR_INT, EP1_PACKET_SIZE, 0x2),
  USB_FILL_IFC_DESC(1, 0, 2, 10, 0, 0x0, 6), /* Data interface/0/ no class specific protocol. */
  USB_FILL_EP_DESC(0x2, EPD_DIR_TX, EPD_ATTR_BULK, EP2_PACKET_SIZE, 0x0),
  USB_FILL_EP_DESC(0x3, EPD_DIR_RX, EPD_ATTR_BULK, EP3_PACKET_SIZE, 0x0),
};

void *get_device_descriptor(void)
{
  return((void*)usb_device_descriptor);
}

hcc_u8 is_cfgd_index(hcc_u16 cndx)
{
  return((hcc_u8)(cndx == 1 ? 1 : 0));
}

void *get_cfg_descriptor(hcc_u8 cndx)
{
  cndx++;
  return((void*)usb_config_descriptor);
}

hcc_u8 is_str_index(hcc_u8 sndx)
{
  return((hcc_u8)(sndx < (sizeof(usb_string_descriptors)/sizeof(usb_string_descriptors[0]) ? 1 : 0)));
}

void *get_str_descriptor(hcc_u8 sndx)
{
  return((void*)(usb_string_descriptors[sndx]));
}


hcc_u8 is_ifc_ndx(hcc_u8 c, hcc_u8 i, hcc_u8 is)
{
  if (c==1 &&  is == 0)
  {
    if (i==0 || i==1)
    {
      return(1);
    }
  }
  return(0);
}

hcc_u8 is_ep_ndx(hcc_u8 c, hcc_u8 i, hcc_u8 is, hcc_u8 ep)
{
  if (c==1 && is==0)
  {
    if (i==0 && ep < 1)
    {
      return(1);
    }
    if (i==1 && ep < 2)
    {
      return(1);
    }
  }
  return(0);
}

void * get_ep_descriptor(hcc_u8 c, hcc_u8 i, hcc_u8 is, hcc_u8 ep)
{
  c++;
  is++;
  if (i == 0)
  {
    return((void*)(&usb_config_descriptor[9+9+5+5+4+5]));
  }
  if (i == 1)
  {
    if (ep==0)
    {
      return((void*)(&usb_config_descriptor[9+9+5+5+4+5+7+9]));
    }
    else
    {
      return((void*)(&usb_config_descriptor[9+9+5+5+4+5+7+9+7]));
    }
  }
  return((void *)0);
}

void usb_cfg_init(void)
{
  (void)usb_init();
}
/****************************** END OF FILE **********************************/
