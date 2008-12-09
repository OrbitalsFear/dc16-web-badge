/******************************************************************************
*                                                  
*  (c) copyright Freescale Semiconductor 2008
*  ALL RIGHTS RESERVED
*
*  File Name:   SD.c
*                                                                          
*  Description: SD Card using SPI Driver
*                                                                                     
*  Assembler:   Codewarrior for HC(S)08 V6.1
*                                            
*  Version:     1.0                                                         
*                                                                                                                                                         
*  Author:      Jose Ruiz (SSE Americas)
*                                                                                       
*  Location:    Guadalajara,Mexico                                              
*                                                                                                                  
*                                                  
* UPDATED HISTORY:
*
* REV   YYYY.MM.DD  AUTHOR        DESCRIPTION OF CHANGE
* ---   ----------  ------        --------------------- 
* 1.0   2008.02.18  Jose Ruiz     Initial version
* 
******************************************************************************/                                                                        
/* Freescale  is  not  obligated  to  provide  any  support, upgrades or new */
/* releases  of  the Software. Freescale may make changes to the Software at */
/* any time, without any obligation to notify or provide updated versions of */
/* the  Software  to you. Freescale expressly disclaims any warranty for the */
/* Software.  The  Software is provided as is, without warranty of any kind, */
/* either  express  or  implied,  including, without limitation, the implied */
/* warranties  of  merchantability,  fitness  for  a  particular purpose, or */
/* non-infringement.  You  assume  the entire risk arising out of the use or */
/* performance of the Software, or any systems you design using the software */
/* (if  any).  Nothing  may  be construed as a warranty or representation by */
/* Freescale  that  the  Software  or  any derivative work developed with or */
/* incorporating  the  Software  will  be  free  from  infringement  of  the */
/* intellectual property rights of third parties. In no event will Freescale */
/* be  liable,  whether in contract, tort, or otherwise, for any incidental, */
/* special,  indirect, consequential or punitive damages, including, but not */
/* limited  to,  damages  for  any loss of use, loss of time, inconvenience, */
/* commercial loss, or lost profits, savings, or revenues to the full extent */
/* such  may be disclaimed by law. The Software is not fault tolerant and is */
/* not  designed,  manufactured  or  intended by Freescale for incorporation */
/* into  products intended for use or resale in on-line control equipment in */
/* hazardous, dangerous to life or potentially life-threatening environments */
/* requiring  fail-safe  performance,  such  as  in the operation of nuclear */
/* facilities,  aircraft  navigation  or  communication systems, air traffic */
/* control,  direct  life  support machines or weapons systems, in which the */
/* failure  of  products  could  lead  directly to death, personal injury or */
/* severe  physical  or  environmental  damage  (High  Risk Activities). You */
/* specifically  represent and warrant that you will not use the Software or */
/* any  derivative  work of the Software for High Risk Activities.           */
/* Freescale  and the Freescale logos are registered trademarks of Freescale */
/* Semiconductor Inc.                                                        */ 
/*****************************************************************************/

/* Include Files */
#include "SD.h"

/* Global Variables */
T32_8 gu8SD_Argument;


/************************************************/
/************************************************/
UINT8 SD_Init(void) 
{
    _SD_PRESENT=_IN;
    
    /* Check for SD */
    if(SD_PRESENT)
        return(NO_SD_CARD);
    
    /* Initialize SPI Module */
    SPI_Init();

    
    /* Start SD card Init */
    SPI_SS=ENABLE;
    SD_CLKDelay(10);            // Send 80 clocks 
    SPI_SS=DISABLE;
  
    gu8SD_Argument.lword=0;
    SD_CLKDelay(8);  
    
    /* IDLE Command */
    
    SPI_SS=ENABLE;
    if(SD_SendCommand(SD_CMD0|0x40,SD_IDLE))
    {
        SPI_SS=DISABLE;
        return(INIT_FAILS);      
    }
    SPI_SS=DISABLE;
    
    (void)SPI_Receive_byte();  // Dummy SPI cycle
    
    /*  Initialize SD Command */
    SPI_SS=ENABLE;
    while(SD_SendCommand(SD_CMD1|0x40,SD_OK));
    SPI_SS=DISABLE;
    
    (void)SPI_Receive_byte();  // Dummy SPI cycle

    /*  Block Length */
    SPI_SS=ENABLE;
      
    gu8SD_Argument.lword=SD_BLOCK_SIZE;
    if(SD_SendCommand(SD_CMD16|0x40,SD_OK))
    {
        SPI_SS=DISABLE;
        return(INIT_FAILS);      
    }
    
    SPI_SS=DISABLE;
    
    SPI_High_rate();

    SPI_Send_byte(0x00);
    SPI_Send_byte(0x00);
    return(OK);
}

/************************************************/
/************************************************/
UINT8 SD_Write_Block(UINT32 u16SD_Block,UINT8 *pu8DataPointer) 
{
    UINT16 u16Counter;

    SPI_SS=ENABLE;

    gu8SD_Argument.lword=u16SD_Block;
    gu8SD_Argument.lword=gu8SD_Argument.lword<< SD_BLOCK_SHIFT;

    if(SD_SendCommand(SD_CMD24|0x40,SD_OK))
    {
        SPI_SS=DISABLE;
        return(WRITE_COMMAND_FAILS);      
    }
    
    SPI_Send_byte(0xFE);
    
    for(u16Counter=0;u16Counter<BLOCK_SIZE;u16Counter++)
        SPI_Send_byte(*pu8DataPointer++);

    SPI_Send_byte(0xFF);    // checksum Bytes not needed
    SPI_Send_byte(0xFF);

    for(u16Counter=0;u16Counter<BLOCK_SIZE;u16Counter++);
    
    if((SPI_Receive_byte() & 0x0F) != 0x05)
    {
        SPI_SS=DISABLE;
        return(WRITE_DATA_FAILS);      
    }

    while(SPI_Receive_byte()==0x00);  // Dummy SPI cycle

    SPI_SS=DISABLE;
    return(OK);
}
/************************************************/
/************************************************/
UINT8 SD_Read_Block(UINT32 u16SD_Block,UINT8 *pu8DataPointer) 
{
    UINT8 u8Temp=0;
    UINT16 u16Counter;

    SPI_SS=ENABLE;

    gu8SD_Argument.lword=u16SD_Block;
    gu8SD_Argument.lword=gu8SD_Argument.lword<< SD_BLOCK_SHIFT;

    if(SD_SendCommand(SD_CMD17|0x40,SD_OK))
    {
        SPI_SS=DISABLE;
        return(READ_COMMAND_FAILS);      
    }
    
    while(u8Temp!=0xFE)
        u8Temp=SPI_Receive_byte();
    
    for(u16Counter=0;u16Counter<BLOCK_SIZE;u16Counter++)
        *pu8DataPointer++=SPI_Receive_byte();

    (void)SPI_Receive_byte();  // Dummy SPI cycle
    (void)SPI_Receive_byte();  

    SPI_SS=DISABLE;

    (void)SPI_Receive_byte();  // Dummy SPI cycle
    
    return(OK);
}

/************************************************/
/************************************************/
UINT8 SD_SendCommand(UINT8 u8SDCommand,UINT8 u8SDResponse) 
{
    UINT8 u8Counter;
    volatile UINT8 u8Temp=0;

    /* Send Start byte */
    SPI_Send_byte(u8SDCommand);

    /* Send Argument */
    for(u8Counter=0;u8Counter<4;u8Counter++) 
        SPI_Send_byte(gu8SD_Argument.bytes[u8Counter]);
  
    /* Send CRC */
    SPI_Send_byte(0x95);
  
    /* Response RHandler */
    u8Counter=SD_WAIT_CYCLES;
    do
    {
        u8Temp=SPI_Receive_byte();
        u8Counter--;
    }while((u8Temp != u8SDResponse) && u8Counter > 0);
    
    if(u8Counter)   return(OK);
    else            return(COMMAND_FAILS);
}

/************************************************/
/************************************************/
void SD_CLKDelay(UINT8 u8Frames) 
{
    while(u8Frames--)
        SPI_Send_byte(0xFF);
}