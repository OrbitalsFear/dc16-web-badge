#ifndef __Definitions__
#define __Definitions__


/* Typedefs */
typedef unsigned char   UINT8;  		/*unsigned 8 bit definition */
typedef unsigned short  UINT16; 		/*unsigned 16 bit definition*/
typedef unsigned long   UINT32; 		/*unsigned 32 bit definition*/
typedef signed char     INT8;   		/*signed 8 bit definition */
typedef short      		INT16;  		/*signed 16 bit definition*/
typedef long int    	INT32;  		/*signed 32 bit definition*/

/* Definitions */
#define   _OUT       1
#define   _IN        0

enum ISR
{
    SCI_Flag,
    ADC_Flag
};

/* Macros */
#define FLAG_SET(BitNumber, Register)        (Register |=(1<<BitNumber))
#define FLAG_CLR(BitNumber, Register)        (Register &=~(1<<BitNumber))
#define FLAG_CHK(BitNumber, Register)        (Register & (1<<BitNumber))


#define    _BGND    asm(BGND)
#define    _NOP     asm(NOP)
#define    _WAIT    asm(WAIT)
#define    False    0x00
#define    True     0x01


#endif /* __Definitions__ */