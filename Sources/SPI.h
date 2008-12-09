#ifndef __SPI__
#define __SPI__

/* Includes */

#include "FslTypes.h"
#include "derivative.h"


/* definitions */

#define SPI_SS    PTBD_PTBD3      /* Slave Select */
#define _SPI_SS   PTBDD_PTBDD3    

#define ENABLE    0
#define DISABLE   1

/* Global Variables */

/* Prototypes */
void SPI_Init(void);
void SPI_Send_byte(UINT8 u8Data);
UINT8 SPI_Receive_byte(void);
void SPI_High_rate(void);


#endif /* __SPI__ */