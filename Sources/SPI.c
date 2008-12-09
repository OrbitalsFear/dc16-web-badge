#include "SPI.h"
#include "timeout.h"

/************************************************/
void SPI_Init(void)
{
  
  //SOPT2 = SOPT2_SPI1PS_MASK;  // Drive PTE as SPI port
  
  SPI_SS = 1;
  _SPI_SS= 1;
  
  SPI2BR = 0x24; // 250 Khz     		
  SPI2C2 = 0x00;     
  SPI2C1 = SPI2C1_SPE_MASK | SPI2C1_MSTR_MASK;
}

/************************************************/
void SPI_Send_byte(UINT8 u8Data)
{
	(void)SPI2S;
	SPI2DL=u8Data;
	while(!SPI2S_SPTEF);
}

/************************************************/
UINT8 SPI_Receive_byte(void)
{
	(void)SPI2DL;
	SPI2DL=0xFF;
	timeoutReset();
	while(!SPI2S_SPRF && !timeout(5000)); // we could hang here forever...
	return(SPI2DL);
}

/************************************************/
void SPI_High_rate(void)
{
  SPI2C1 = 0x00;
  SPI2BR = 0x11; // 3 MHz     		
  SPI2C1 = SPI2C1_SPE_MASK | SPI2C1_MSTR_MASK;
}
