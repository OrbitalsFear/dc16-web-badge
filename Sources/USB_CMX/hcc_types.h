/****************************************************************************
 *
 *            Copyright (c) 2006-2007 by CMX Systems, Inc.
 *
 * This software is copyrighted by and is the sole property of
 * CMX.  All rights, title, ownership, or other interests0
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
#ifndef _CMX_TYPES_H_
#define _CMX_TYPES_H_

/* Type definitions */
typedef unsigned char hcc_u8;
typedef unsigned int hcc_u16;
typedef unsigned long hcc_u32;

typedef volatile hcc_u8 hcc_reg8;
typedef volatile hcc_u16 hcc_reg16;
typedef volatile hcc_u32 hcc_reg32;

typedef hcc_u8 hcc_imask;

#ifdef NDEBUG
#define CMX_ASSERT(c) (void)0
#else
#define CMX_ASSERT(c)\
do {\
  if(!(c))\
  {\
    int a=1;\
    while(a)\
      ;\
  }\
}while(0)
#endif

#define BREW32(v)   ((hcc_u32)(((hcc_u32)(((hcc_u32)(v)) << 24)) \
                     | ((hcc_u32)(((hcc_u32)(v)) >> 24)) \
                     | (hcc_u32)((hcc_u32)((hcc_u32)(v) & (hcc_u32)0xff00ul) << 8)\
                     | (hcc_u32)((hcc_u32)((hcc_u32)(v) & (hcc_u32)0xff0000ul) >> 8)))
                     
#define BREW16(v)   ((hcc_u16)(((hcc_u16)(v)) << 8) | (hcc_u16)(((hcc_u16)(v)) >> 8))

#define WR_LE32(a, v) ((*(hcc_u32*)(a))=BREW32(v))
#define WR_LE16(a, v) ((*(hcc_u16*)(a))=BREW16(v))
#define RD_LE32(a)    (BREW32(*((hcc_u32*)(a))))
#define RD_LE16(a)    (BREW16(*((hcc_u16*)(a))))

/* Read 16 bit big endian value from address. */
#define RD_BE16(a) (*(hcc_u16*)(a))
/* Write 16bit value in v to address a in big endian order. */
#define WR_BE16(a, v) (*(hcc_u16*)(a) = (hcc_u16)(v))
/* Read 32 bit little endian value from address. */
#define RD_BE32(a) (*(hcc_u32*)(a))
/* Write 32bit value in v to address a in big endian order. */
#define WR_BE32(a, v) (*(hcc_u32*)(a) = (hcc_u32)(v))


#define BIT0 (1u<<0)
#define BIT1 (1u<<1)
#define BIT2 (1u<<2)
#define BIT3 (1u<<3)
#define BIT4 (1u<<4)
#define BIT5 (1u<<5)
#define BIT6 (1u<<6)
#define BIT7 (1u<<7)



#endif /*_CMX_TYPES_H_*/

/****************************** END OF FILE **********************************/
