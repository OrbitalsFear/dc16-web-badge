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
#include "utils.h"

void itoah(int number, char* buf, int length)
{ 
  int ndx=length-1;
  int neg;
  buf[ndx--]='\0';

  if (number < 0)
  {
    neg=1;
    number*=-1;
  }
  else
  {
    neg=0;
  }

  while((number >= 0) && (ndx > 0))
  {
    int digit=number&0xf;
    buf[ndx--]=(char)('0'+digit);
    number>>=4;
  }
  
  while(ndx > 0)
  {
    buf[ndx--]=' ';
  }
  
  buf[0]=(char)(neg ? '-' : ' ');
}

void itoa(unsigned long number, char* buf, int length)
{ 
  int ndx=length-1;
  int neg;
  buf[ndx--]='\0';

  if (number < 0)
  {
    neg=1;
    number*=-1;
  }
  else
  {
    neg=0;
  }

  while((number >= 0) && (ndx > 0))
  {
    int digit=number%10;
    buf[ndx--]=(char)('0'+digit);
    number/=10;
  }
  
  while(ndx > 0)
  {
    buf[ndx--]=' ';
  }
  
  buf[0]=(char)(neg ? '-' : ' ');
}


hcc_u32 strtoi (char *str)
{
	hcc_u32 rvalue;
  char *c;

	rvalue = 0;

	/* Check for invalid chars in str */
	for ( c = str; *c != '\0'; ++c)
	{
	  /* Convert char to num in 0..36 */
	  hcc_u8 val=(hcc_u8)(*c-'0');
      rvalue = (rvalue * 10) + val;
	}

  return rvalue;
}

void *_memcpy(void *dest, const void *src, int n)
{
  int x;
  for(x=0; x<n; x++)
    {
      ((char*)dest)[x]=((char*)src)[x];
    }
    return(dest);
}

void *_memset(void *s, int c, int n)
{
  int x;
  for(x=0;x<n;x++)
  {
    ((unsigned char*)s)[x]=(unsigned char)c;
  }
  return(s);
}

/****************************** END OF FILE **********************************/
