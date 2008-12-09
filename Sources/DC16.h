/******************************************************************************
*
* DEFCON 16 BADGE
*
* Filename:		DC16.h
* Author:		  Joe Grand (Kingpin)
* Revision:		1.0
*
* Description:	Header File for the DEFCON 16 Badge (Freescale MC9S08JM60)
* Notes:
*
* Can you believe it's been a year already?
*
* SecureDigital cards MUST be formatted in FAT16. If you use FAT12 or FAT32, 
* you will end up with a corrupted FAT table. Windows automatically formats
* cards less than 64MB as FAT12, so use a bigger card to ensure that they are 
* formatted in FAT16. This will save you days of frustration. 		
*******************************************************************************/

#ifndef __DC16__
#define __DC16__

//#define _DC16_dev // uncomment this line for the DC16 development board pin-out

#include "FslTypes.h"

/**************************************************************************
************************** Definitions ************************************
***************************************************************************/

#define LOW   0
#define HIGH  1

#define ON    0
#define OFF   1

#define MINICOM_BUFFER_SIZE 512
#define FAT16_MAX_FILE_SIZE 2147483648  // 2GB maximum FAT16 file size for 32kB clusters
 
 
#ifdef _DC16_dev  // development board      
  // inputs
  #define SW_MODE     PTGD_PTGD0
  #define USB_DETECT  PTGD_PTGD1

  // LEDs
  #define LED1  PTED_PTED0
  #define LED2  PTED_PTED2
//  #define LED3  PTED_PTED3
//  #define LED4  PTFD_PTFD0
  #define LED5  PTFD_PTFD1 
  #define LED6  PTFD_PTFD2
  #define LED7  PTFD_PTFD3
  #define LED8  PTED_PTED1

  #define LED1_DD  PTEDD_PTEDD0
  #define LED2_DD  PTEDD_PTEDD2
//  #define LED3_DD  PTEDD_PTEDD3
//  #define LED4_DD  PTFDD_PTFDD0
  #define LED5_DD  PTFDD_PTFDD1 
  #define LED6_DD  PTFDD_PTFDD2
  #define LED7_DD  PTFDD_PTFDD3
  #define LED8_DD  PTEDD_PTEDD1

  #define LED1_DS  PTEDS_PTEDS0
  #define LED2_DS  PTEDS_PTEDS2
//  #define LED3_DS  PTEDS_PTEDS3
//  #define LED4_DS  PTFDS_PTFDS0
  #define LED5_DS  PTFDS_PTFDS1 
  #define LED6_DS  PTFDS_PTFDS2
  #define LED7_DS  PTFDS_PTFDS3
  #define LED8_DS  PTEDS_PTEDS1
#else  // production badge
  // inputs
  #define SW_MODE     PTGD_PTGD1
  #define USB_DETECT  PTGD_PTGD0

  // LEDs
  #define LED1  PTED_PTED3
  #define LED2  PTED_PTED2
//  #define LED3  PTED_PTED1
//  #define LED4  PTED_PTED0
  #define LED5  PTFD_PTFD3 
  #define LED6  PTFD_PTFD2
  #define LED7  PTFD_PTFD1
  #define LED8  PTFD_PTFD0

  #define LED1_DD  PTEDD_PTEDD3
  #define LED2_DD  PTEDD_PTEDD2
//  #define LED3_DD  PTEDD_PTEDD1
//  #define LED4_DD  PTEDD_PTEDD0
  #define LED5_DD  PTFDD_PTFDD3 
  #define LED6_DD  PTFDD_PTFDD2
  #define LED7_DD  PTFDD_PTFDD1
  #define LED8_DD  PTFDD_PTFDD0

  #define LED1_DS  PTEDS_PTEDS3
  #define LED2_DS  PTEDS_PTEDS2
//  #define LED3_DS  PTEDS_PTEDS1
//  #define LED4_DS  PTEDS_PTEDS0
  #define LED5_DS  PTFDS_PTFDS3 
  #define LED6_DS  PTFDS_PTFDS2
  #define LED7_DS  PTFDS_PTFDS1
  #define LED8_DS  PTFDS_PTFDS0
#endif


/**************************************************************************
************************** Macros *****************************************
***************************************************************************/

#define F_CPU  6000000 // 6MHz bus speed (12MHz clock)
#define freq_to_timerval(x) ( F_CPU / x )

#define Sleep   _Stop;
#define Snooze  _Wait;

  //Size of my input and output buffers (ALERT YOU BUF OVERFLOOWERS!)
#define INPUT_BUF   512 
#define OUTPUT_BUF  32
#define DC16_LOG_SIZE 128

  //LED output params
#define LED_RX  16  
#define LED_TX  16

	//Checksum Number
#define MOD_ADLER         65521

	//Macros, I know this is lame but space is precess, my precess..
#define TOUCH_RX( f ) guRxLed = ( f || guRxLed >= LED_RX )? 0: guRxLed
#define TOUCH_TX( f ) guTxLed = ( f || guTxLed >= LED_TX )? 0: guTxLed

	//Button defines
#define BUTTON_HOLD_COUNT 30	//50ms * 20 = 1 second ish


/**************************************************************************
************************** Structs ****************************************
***************************************************************************/

enum // Log Codes
{
  errSD,
  errFAT,
  errTX,
  errRX,
	errServer,
	warnNotFound,
	logInfo,
	logDebug
}; 

typedef enum // define the modes for the state machine
{
   SLEEP,
   RX, 
   TX
} state_type;

typedef enum
{
   ALL_OFF,
   ALL_ON,
   ALL_BLINK,
   KNIGHT_RIDER,
   TRANSMIT,
   BYTE,
   IGNORE 
} led_state_type;

	//These are the load setups we go through
typedef enum
{
	LOAD_COMPLETE,
	LOAD_CLK_SPEED,
	LOAD_SCI_CHAN,
	LOAD_BAUD_RATE,

	LOAD_VAL_12MHZ,
	LOAD_VAL_24MHZ,

	LOAD_VAL_SCI2,
	LOAD_VAL_SCI1,

	LOAD_VAL_9600,
	LOAD_VAL_38400,
	LOAD_VAL_115200
} LoadStep;

typedef enum
{
	BUTTON_NONE,
	BUTTON_CLICK,
	BUTTON_HOLD,
	BUTTON_BLOCK,
	BUTTON_NON_BLOCK

} ButtonPress;

typedef enum
{
	FORMAT_DEFAULT,	//Not realy known?
	FORMAT_HTML,
	FORMAT_PNG,
	FORMAT_JPG,
	FORMAT_GIF,
	FORMAT_BMP,
	FORMAT_PDF
};

typedef struct _codeElement {
  unsigned int onTime;   // duration of "On" time
  unsigned int offTime;  // duration of "Off" time
} codeElement;

typedef struct _powercode {
  unsigned int timer_val; // not the actual frequency, but the timer value to generate the frequency (using TPM2MOD)
  codeElement codes[100];  // maximum number of on/off codes per entry - this is horribly inefficient, but had compilation problems with codes[]
} powercode;


/***********************************************************************
 ************************** Function prototypes ************************
 ***********************************************************************/

// main.c
ButtonPress DC16_Button( ButtonPress mode );
UINT8 DC16_File_Type( char* filename, INT16 len );
UINT32 DC16_Webserver( int response, UINT8 format, UINT32 file_size );
void DC16_Log(unsigned char errType, UINT16 u16Error, char* msg);
void DC16_Init(void);
void set_clock_12MHz(void);
void DC16_Led(void);

	//Pull the file name out of a get request
UINT8 DC16_Get_Request( char* filename, char* http, INT16 len );

	//Functions to read and write data
INT16 DC16_Com_Send( char* data, INT16 len, UINT16 timeout );
INT16 DC16_Com_Recv( char* data, INT16 len, UINT16 timeout );
INT16 DC16_Com_Runoff( UINT16 time_out );

	//Checksum code
UINT32 Adler( unsigned char *data, int len);
unsigned char* ultoa( UINT32 num, unsigned char* str );
UINT32 atoul( char* str );

void delay_ms(unsigned int ms);
void delay_10us(unsigned int us);
UINT8* num2asc(UINT8 n);
UINT8 asc2num(UINT8 n_asc);

#endif /* __DC16__ */
