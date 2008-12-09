/******************************************************************************
*
* DEFCON 16 BADGE
*
* Filename:		main.c
* Author:		  Joe Grand (Kingpin)
* Revision:		1.0
* Last Updated: June 30, 2008
*
* Description:	Main File for the DEFCON 16 Badge (Freescale MC9S08JM60)
* Notes:
*
* See DC16.h for more inf0z...		
*******************************************************************************/

#include <hidef.h>          /* for EnableInterrupts macro */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "usb.h"
#include "target.h"
#include "usb_cdc.h"
#include "utils.h"
#include "FslTypes.h"
#include "Fat.h"
#include "SD.h"
#include "ir.h"
#include "crc16.h"
#include "DC16.h"
#include "timeout.h"

/****************************************************************************
 ************************** Global variables ********************************
 ***************************************************************************/
 
UINT8 gu8IntFlags;
UINT8 gau8Minicom[MINICOM_BUFFER_SIZE]; // buffer for SD card incoming/outgoing user data
state_type state = RX; // set power-on state
led_state_type led_state = KNIGHT_RIDER;
UINT8 state_change_flag = 0; // if set, we need to change the state/mode
UINT8 power_on_flag = 1;  // if set, we have just powered up and changing state for the first time
UINT8 usb_enabled_flag = 0; // if not set, usb needs to be initialized
UINT8 ir_rx_byte = FALSE; // if set, we are receiving a byte via IR
UINT8 counter = 0; // counter for RTC
UINT8 n_str[3]; // temp. for num2asc and asc2num conversion functions

	//LED inputs
UINT8 gu8Led; // this byte is displayed on the LEDs if the led_state = BYTE
UINT8 guRxLed = LED_RX;
UINT8 guTxLed = LED_TX;

	//Load setup
LoadStep guLoadStep = LOAD_COMPLETE;
LoadStep guLoadCLK;
LoadStep guLoadBaud;
LoadStep guLoadSCI;

	//Input/Output Data buffers
char Input_Buf[INPUT_BUF + 1];
char Filename[INPUT_BUF + 1];
char scratch[MINICOM_BUFFER_SIZE];

//Log file
char dc16_log[DC16_LOG_SIZE];

// from tv_b_gone.c
char num_codes = 0;

// from ir.c
extern unsigned char ir_command;
extern unsigned char ir_address;
extern unsigned char ir_state;

/****************************************************************************
 ************************** Function definitions ****************************
 ***************************************************************************/
void main(void)
{
  int cdc_in;
  int uart_in;

		//Variables for reading and sending files
	int i;
	int response;
	int fat_find_stop;
	UINT8 valid;
	UINT8 send_file;
	UINT16 len;
  volatile UINT16 u16Error;
  UINT16 u16BufferSize;
	UINT32 file_size;
	UINT8 format;
	ButtonPress btn;
  
		//Init My Badge
  DC16_Init();
  delay_ms(100);
  //DC16_TX_Test();
  cdc_in=uart_in=0xff+1; // USB CMX init
  cdc_init();

		//Enable my Interrupt for LEDS, timeoutes, etc
 	RTCSC_RTCPS = 0xA;

	/*************/
	/* CLK Speed */
	/*************/

		//Go into step one, Have the user pick the clock speed
	guLoadStep = LOAD_CLK_SPEED;
	guLoadCLK = LOAD_CLK_SPEED;	//This is junk, so we will use DEFAULT first!
	btn = BUTTON_HOLD;	//Notice this cause us to transition to 24MHZ right away
	do {
		if ( btn == BUTTON_HOLD )
		{
				//State machine
			if ( guLoadCLK == LOAD_VAL_24MHZ )
					guLoadCLK = LOAD_VAL_12MHZ;
			else
				guLoadCLK = LOAD_VAL_24MHZ;

				//Transition to a new speed
			switch ( guLoadCLK )
			{
					//This is what we were so we are going to 24mhz
				default:
				case LOAD_VAL_24MHZ:
		      hw_init(); // setup usb & 48MHz clock using 12MHz external crystal
    		  //usb_cfg_init();     
		      SPI2BR = 0x11; // SPI baud rate, 3MHz @ 24MHz bus clock
    		  TPM2SC_PS = 0b010;    // TPM2 timer prescaler = /4
					guLoadCLK = LOAD_VAL_24MHZ;
		      usb_enabled_flag = 0;
					break;

					//This means we were 24MHZ... or no one knows, either way goto 12MHZ
				case LOAD_VAL_12MHZ:
      		usb_stop();
		      set_clock_12MHz();
					guLoadCLK = LOAD_VAL_12MHZ;
    		  usb_enabled_flag = 0;
					break;
			}
		}

			//Get the user's input, if button isn't held down
		btn = BUTTON_NONE;
		if (SW_MODE)
			btn = DC16_Button( BUTTON_BLOCK );
	} while ( btn != BUTTON_CLICK );

	/**************/
	/* SCI Select */
	/**************/

		//Pick which SCI we want to use, any hackers want to make it use both?
		//totally possible to do!  My GF is already annoyed with how much time
		// I put into this so we are just going to get a choice of SCI1 or SCI2 
	guLoadStep = LOAD_SCI_CHAN;
	guLoadSCI = LOAD_SCI_CHAN;	//This is junk, so we will use DEFAULT first!
	btn = BUTTON_HOLD;	//Notice this will cause use to transtion to SCI2 first
	do {
		if ( btn == BUTTON_HOLD )
		{
				//State machine
			if ( guLoadSCI == LOAD_VAL_SCI2 )
					guLoadSCI = LOAD_VAL_SCI1;
			else
				guLoadSCI = LOAD_VAL_SCI2;
		}

			//Get the user's input, if button isn't held down
		btn = BUTTON_NONE;
		if (SW_MODE)
			btn = DC16_Button( BUTTON_BLOCK );
	} while ( btn != BUTTON_CLICK );

	/*************/
	/* BAUD Rate */
	/*************/
	
		//Now in step two we are going to pick the baud rate
	guLoadStep = LOAD_BAUD_RATE;
	guLoadBaud = LOAD_BAUD_RATE;	//This is junk, so we will use DEFAULT first!
	btn = BUTTON_HOLD;	//Notice that we are going to transition on the first time
	do {
		if ( btn == BUTTON_HOLD )
		{
				//Set the upper baud rate to zero, as always
			if ( guLoadSCI == LOAD_VAL_SCI1 )
				SCI1BDH	= 0;
			else
				SCI2BDH	= 0;

				//State machine to move around
			if 			( guLoadBaud == LOAD_VAL_9600 )
				guLoadBaud = LOAD_VAL_38400;
			else if	( guLoadBaud == LOAD_VAL_38400 || guLoadBaud == LOAD_BAUD_RATE )
			{
					//If we are running slow, go back to 9600, else, go for 1152
				if ( guLoadCLK == LOAD_VAL_12MHZ )
					guLoadBaud = LOAD_VAL_9600;
				else
					guLoadBaud = LOAD_VAL_115200;
			}
			else
				guLoadBaud = LOAD_VAL_9600;

				//Transition to a new speed
			switch ( guLoadBaud )
			{
					//Set the baud rate to 9600, valid for all clk speeds
				default:
				case LOAD_VAL_9600:
					if ( guLoadCLK == LOAD_VAL_12MHZ )
					{
						if ( guLoadSCI == LOAD_VAL_SCI1 )
							SCI1BDL = 0x27;	// 6000000/(9600*16)
						else
							SCI2BDL = 0x27;	// 6000000/(9600*16)
					}
					else
					{
						if ( guLoadSCI == LOAD_VAL_SCI1 )
							SCI1BDL = 0x9C;	//9600 baud		24000000/(16*9600)
						else
							SCI2BDL = 0x9C;	//9600 baud		24000000/(16*9600)
					}
					guLoadBaud = LOAD_VAL_9600;
					break;

					//Set the chip to 38400, valid for all clk speeds, just bearly
				case LOAD_VAL_38400:
					if ( guLoadCLK == LOAD_VAL_12MHZ )
					{
						if ( guLoadSCI == LOAD_VAL_SCI1 )
							SCI1BDL = 0x0a;		// 6000000/(38400*16)	(3% error)
						else
							SCI2BDL = 0x0a;		// 6000000/(38400*16)	(3% error)
					}
					else
					{
						if ( guLoadSCI == LOAD_VAL_SCI1 )
							SCI1BDL = 0x27;		//38400 baud	24000000/(16*38400)
						else
							SCI2BDL = 0x27;		//38400 baud	24000000/(16*38400)
					}
					guLoadBaud = LOAD_VAL_38400;
					break;

					//Set the chip to 115200, only valid for 24MHZ speed
				case LOAD_VAL_115200:
					if ( guLoadCLK == LOAD_VAL_12MHZ )
					{
						if ( guLoadSCI == LOAD_VAL_SCI1 )
							SCI1BDL = 0x27;		// 6000000/(115200*16)	(not possible), 9600
						else
							SCI2BDL = 0x27;		// 6000000/(115200*16)	(not possible), 9600
						guLoadBaud = LOAD_VAL_9600;
					}
					else
					{
						if ( guLoadSCI == LOAD_VAL_SCI1 )
							SCI1BDL = 0x0D;		//115200 baud	24000000/(16*115200)
						else
							SCI2BDL = 0x0D;		//115200 baud	24000000/(16*115200)
						guLoadBaud = LOAD_VAL_115200;
					}
					break;
			}

				//Set the default values that are always the same here
			if ( guLoadSCI == LOAD_VAL_SCI1 )
			{
				SCI1C1 = 0x00;	//Enable PE
				SCI1C2 = 0x0C;

					//Invert my reads and writes
				SCI1S2 = 0x10;
				SCI1C3 = 0x10;
			}
			else
			{
				SCI2C1 = 0x00;	//Enable PE
				SCI2C2 = 0x0C;

					//Invert my reads and writes
				SCI2S2 = 0x10;
				SCI2C3 = 0x10;
			}
		}

			//Get user input, if button isn't pushed
		btn = BUTTON_NONE;
		if (SW_MODE)
			btn = DC16_Button( BUTTON_BLOCK );
	} while ( btn != BUTTON_CLICK );


		//Tell the user hello
	//DC16_Log( logInfo, 0, "Welcome to the debug terminal...\n\r" );
	power_on_flag = 0;

		//Loop forever servicing requests
	(void)DC16_Com_Send( "Program Started\r\n", -1, 5000 );
	for(;;)
	{
			//Get my variables ready for a new request chatter
		response = 200;		//Request OK!
		send_file = 1;
		len = 0;
		valid = 0;

			//Loop reading headers until we find a get file request
		do {
				//Read a line of data
			len = DC16_Com_Recv( Input_Buf, INPUT_BUF, 10000 );

				//Debug info (comment out)
//			(void)sprintf( scratch, "User sent: %s", Input_Buf );
//			DC16_Log( logDebug, 0, scratch );

			//Exit once we have a valid filename, and we have read the blank \n line
		} while ( !DC16_Get_Request( Filename, Input_Buf, len ) );

			//Read data until we timeout
		//(void)DC16_Com_Runoff( 10 );

			//Debug info (comment out)
//		(void)sprintf( scratch, "User requested file: %s", Filename );
//		DC16_Log( logDebug, 0, scratch );

			//Clear the ubffer, I want to remove this seems wasteful!
		(void)memset(gau8Minicom, 0x00, MINICOM_BUFFER_SIZE); // clear buffer

			//Init the SD card, I hope I can figure out how to not do this everytime
		u16Error = SD_Init(); // SD card init
		if (u16Error == OK) 
		{
			//DC16_Log( logDebug, 0, "SD_Init Success, Reading Master Block" );
			FAT_Read_Master_Block(); // FAT driver init
		}
		else
		{
			send_file = 0;
			response = 500;	//Server Error!
			//DC16_Log(errSD, u16Error, "SD Init Failed");
		}

		//DC16_Log( logDebug, 0, "Read Complete, looking for file" );
  
			//Check that I do have the file the user wants
		if ( send_file )
		{
			fat_find_stop = 0;
			while ( !fat_find_stop )
			{
					//Check if we can send the file
				if ( !FAT_Find( Filename ) )
				{
						//If we don't even have a 404 file, then stop trying
					if ( response == 404 )
					{
						send_file = 0;
						fat_find_stop = 1;
					}
					response = 404;	//Not Found
//					(void)sprintf(scratch, "File not found: %s", Filename);
//					DC16_Log( warnNotFound, u16Error, scratch);

						//Attempt to send the 404 file
					(void)strcpy( Filename, "ERR404.HTM" );
				}
				else
					fat_find_stop = 1;
			}
		}

			//Open my file
		if ( send_file )
		{
			u16Error=FAT_FileOpen( Filename, READ);  
			if (u16Error != FILE_FOUND) 
			{ 
				send_file = 0;
				response = 500;
//				(void)sprintf(scratch, "Error during FileOpen(%s,READ);", Filename);
//				DC16_Log(errServer, u16Error, scratch);
			}

				//Read the file size of what we are sending
			file_size = FAT_Open_File_Size(READ);

//			(void)sprintf(scratch, "File size = %d", (int)file_size);
//			DC16_Log(logDebug, u16Error, scratch);
		}

			//If we aren't sending antyhing, set my filesize
		if ( !send_file )
			file_size = 0;

			//Get the file format
		format = DC16_File_Type( Filename, -1 );

			//Send out the server header
		u16BufferSize = (UINT16)DC16_Webserver( response, format, file_size );

			//once again read any data that might be jamming us up
		//(void)DC16_Com_Runoff( 10 );

			//Loop while we have data
		if ( send_file )
		{
				//Loop while we are reading data
			u16BufferSize = FAT_FileRead(gau8Minicom);
	 		while ( u16BufferSize > 0 )
			{
					//Send the data we just read and then read more
				(void)DC16_Com_Send( gau8Minicom, u16BufferSize, 5000 );
				u16BufferSize = FAT_FileRead(gau8Minicom);
			}
		}

			//Close down the file and get ready for the next request
//		FAT_FileClose();
	}
}

/********************************************************/
	//This method is used to detect the user's button pushes gracefully
ButtonPress DC16_Button( ButtonPress mode )
{
	ButtonPress result = BUTTON_NONE; 
	int tick = 0;
	
		//Block if we are in block mode and aren't clicking anything
	while ( mode == BUTTON_BLOCK && SW_MODE );

		//While the button is held down, and we don't have a hold, block
	while (!SW_MODE && tick < BUTTON_HOLD_COUNT )
	{
		tick++;
		delay_ms(50); // poor man's debounce
	}

		//If the user was pusshing the button, check for how long
	if 			( tick >= BUTTON_HOLD_COUNT )
		result = BUTTON_HOLD;
	else if ( tick >= 2 )
		result = BUTTON_CLICK;

	return result;
}

/********************************************************/
UINT8 DC16_File_Type( char* filename, INT16 len )
{
	INT16 i;

		//Get my length
	if ( len < 0 )
		len = strlen( filename );

		//Find the dot
	for ( i = 0; i < len && filename[i] != '.'; i++ );
	i++;	//Go past the dot

		//Return the default if i is too big
	if ( i >= len )
		return FORMAT_DEFAULT;

		//Figure out what we are
	if			( strncmp( filename + i, "HTM", 3 ) == 0 ) return FORMAT_HTML;
	else if	( strncmp( filename + i, "PNG", 3 ) == 0 ) return FORMAT_PNG;
	else if	( strncmp( filename + i, "JPG", 3 ) == 0 ) return FORMAT_JPG;
	else if	( strncmp( filename + i, "GIF", 3 ) == 0 ) return FORMAT_GIF;
	else if	( strncmp( filename + i, "BMP", 3 ) == 0 ) return FORMAT_BMP;
	else if	( strncmp( filename + i, "PDF", 3 ) == 0 ) return FORMAT_PDF;

	return FORMAT_DEFAULT;
}


/********************************************************/
	//This function sends out the web server's header, pretty static response
UINT32 DC16_Webserver( int response, UINT8 format, UINT32 file_size )
{
	char buf[16];	

		//Based on my response send out the first part of the header
	switch ( response )
	{
		case 200: (void)DC16_Com_Send( "HTTP/1.1 200 OK\r\n", -1, 5000 ); break;
		case 404: (void)DC16_Com_Send( "HTTP/1.1 404 Not Found\r\n", -1, 5000 ); break;
		case 500: (void)DC16_Com_Send( "HTTP/1.1 500 Server Error\r\n", -1, 5000 ); break;

		default: break;
	}

		//Tell them who I am
	(void)DC16_Com_Send( "Server: DC16 Web Badge 1.0\r\n", -1, 5000 );
	(void)DC16_Com_Send( "Accept-Ranges: bytes\r\n", -1, 5000 );

		//Send the content length
		//I realize the ultoa thing is... odd, but I had so many issues with
		// %ld, %u, %d, %%%%%% AAAAHAHHHH... %s, oo okay, there it goes
	(void)sprintf( scratch, "Content-Length: %s\r\n", ultoa( file_size, buf ) );
	(void)DC16_Com_Send( scratch, -1, 5000 );

		//Output the correct format
	switch ( format )
	{
		case FORMAT_HTML:
			(void)DC16_Com_Send( "Content-Type: text/html\r\n\r\n", -1, 5000 );
			break;

		case FORMAT_PNG:
			(void)DC16_Com_Send( "Content-Type: image/png\r\n\r\n", -1, 5000 );
			break;

		case FORMAT_JPG:
			(void)DC16_Com_Send( "Content-Type: image/jpg\r\n\r\n", -1, 5000 );
			break;

		case FORMAT_GIF:
			(void)DC16_Com_Send( "Content-Type: image/gif\r\n\r\n", -1, 5000 );
			break;

		case FORMAT_BMP:
			(void)DC16_Com_Send( "Content-Type: image/bmp\r\n\r\n", -1, 5000 );
			break;

		case FORMAT_PDF:
			(void)DC16_Com_Send( "Content-Type: Content-Type: text/html; charset=iso-8859-1\r\n\r\n", -1, 5000 );
			break;

		case FORMAT_DEFAULT:
		default:
			(void)DC16_Com_Send( "Content-Type: Content-Type: application/binary\r\n\r\n", -1, 5000 );
			break;
	}
	
/*
HTTP/1.1 200 OK
Date: Sun, 19 Oct 2008 23:39:08 GMT
Server: Apache/2.2.8 (Ubuntu) PHP/5.2.4-2ubuntu5.3 with Suhosin-Patch Phusion_Passenger/2.0.1
Last-Modified: Sun, 19 Oct 2008 23:36:09 GMT
ETag: "6cb3-31-459a3a9bb2c40"
Accept-Ranges: bytes
Content-Length: 49
Content-Type: text/html
*/

	return file_size;
}


/********************************************************/
void DC16_Log(unsigned char errType, UINT16 u16Error, char* msg)
{
		//If our usb port is plugged in, output some info
  if (usb_enabled_flag && USB_DETECT)
  {
    switch (errType)
    { 
      case errSD: 			Terminal_Send_String("(Error) SD Card: "); break;
      case errFAT: 			Terminal_Send_String("(Error) FAT Error: "); break;
      case errTX: 			Terminal_Send_String("(Error) TX Error: "); break;
      case errRX: 			Terminal_Send_String("(Error) RX Error: "); break;
			case errServer:		Terminal_Send_String("(Error) Server Error: "); break;
			case warnNotFound:Terminal_Send_String("(Warn) File Not Found: "); break;
			case logInfo:			Terminal_Send_String("(Info): "); break;
			case logDebug:		Terminal_Send_String("(Debug): "); break;
			default:					Terminal_Send_String("Unkown? "); break;
    }
		(void)sprintf( dc16_log, "{%d} %s\n\r", (unsigned int)u16Error, msg );
    Terminal_Send_String( dc16_log );
  }   
}


/********************************************************/
void DC16_Init (void)
{
  // _Entry() contains initial, immediately required configuration
  
  DisableInterrupts;

  set_clock_12MHz(); 
    
  // After reset, all pins are configured as high-impedance general-purpose
  // inputs with internal pullup devices disabled
  
    /*** LEDs ***/
  LED1_DD = LED2_DD = LED5_DD = LED6_DD = LED7_DD = LED8_DD = _OUT; // set LED pins to outputs
  LED1_DS = LED2_DS = LED5_DS = LED6_DS = LED7_DS = LED8_DS = _OUT; // set LED pins to high output drive strength

  // Set unused/unconnected pins to outputs so the pins don't float
  // and cause extra current drain
  // data register default = LOW
  PTADD_PTADD0 = PTADD_PTADD1= PTADD_PTADD2 = PTADD_PTADD3 = PTADD_PTADD4 = PTADD_PTADD5 = _OUT;
  PTBDD_PTBDD6 = PTBDD_PTBDD7 = _OUT;
  PTCDD_PTCDD0 = PTCDD_PTCDD1 = PTCDD_PTCDD4 = PTCDD_PTCDD6 = _OUT;
  PTDDD_PTDDD0 = PTDDD_PTDDD1 = PTDDD_PTDDD2 = PTDDD_PTDDD3 = PTDDD_PTDDD4 = PTDDD_PTDDD5 = PTDDD_PTDDD6 = PTDDD_PTDDD7 = _OUT;
  PTEDD_PTEDD4 = PTEDD_PTEDD5 = PTEDD_PTEDD6 = PTEDD_PTEDD7 = _OUT;
  PTFDD_PTFDD6 = PTFDD_PTFDD7 = _OUT;
  PTGDD_PTGDD2 = PTGDD_PTGDD2 = _OUT;
 
  /*** STOP MODE ***/
  // STOPE enabled in _Entry()
  SPMSC1_LVDE = 0; // disable low-voltage detection
  SPMSC2_PPDC = 0; // select stop3 mode for sleep
  
  /*** KBI ***/
  KBISC_KBIE = 1; // enable keyboard interrupt module
  
  // mode select switch
#ifdef _DC16_dev  // development board    
  KBIES_KBEDG0 = 0; // set polarity to pullup
  PTGPE_PTGPE0 = 1; // enable internal pullup/pulldown
  KBIPE_KBIPE0 = 1; // set pin as KBI
#else // production badge
  KBIES_KBEDG1 = 0; // set polarity to pullup
  PTGPE_PTGPE1 = 1; // enable internal pullup/pulldown
  KBIPE_KBIPE1 = 1; // set pin as KBI
#endif
 
  // usb detect
#ifdef _DC16_dev  // development board      
  KBIES_KBEDG1 = 1; // set polarity to rising edge/high level
  PTGPE_PTGPE1 = 0; // disable internal pullup/pulldown
  KBIPE_KBIPE1 = 1; // set pin as KBI
#else // productionbadge
  KBIES_KBEDG0 = 1; // set polarity to rising edge/high level
  PTGPE_PTGPE0 = 0; // disable internal pullup/pulldown
  KBIPE_KBIPE0 = 1; // set pin as KBI
#endif
  
  KBISC_KBACK = 1; // clear any false interrupts
  
  /*** SPI & SD CARD ***/
  SPI_Init();   // SPI module init (will be used by SD card)
  
  _SD_PRESENT = _IN;  // SD Card Detect
  PTBPE_PTBPE4 = 1;   // Set internal pullup

  _WP_ENABLED = _IN; // SD Card Write Protect Switch
  PTBPE_PTBPE5 = 1;   // Set internal pullup
  
  /*** RTC ***/
  RTCMOD = 0;
  RTCSC_RTIE = 1;     // enable interrupt module
  RTCSC_RTCLKS = 0;   // clock source = internal 1kHz low-power oscillator
      
  /*** Timer/PWM Module ***/
  TPM2SC_TOIE = 0;      // disable timer overflow interrupt
  TPM2SC_CPWMS = 0;     // each channel function set by MSnB:MSnA control bits
  TPM2SC_CLKSx = 0b01;  // clock select = bus
  TPM2SC_PS = 0b000;    // prescaler = /1
  
  // IR output (transmit) channel
  PTFDD_PTFDD4 = _OUT;
  PTFD_PTFD4 = 0;
  PTFDS_PTFDS4 = 1;   // high output drive strength
  TPM2C0SC_CH0IE = 0; // disable channel 0 interrupt
  TPM2C0SC_MS0B = 1;  // edge-aligned PWM
  IR_TX_Off;          // disable PWM output until we're ready to use it
  
  // IR input (receive) channel
  PTFDD_PTFDD5 = _IN;
  PTFPE_PTFPE5 = 0;   // disable pull-up
  TPM2C1SC_CH1F = 0;  // clear any pending flag
  TPM2C1SC_CH1IE = 1; // enable channel 1 interrupt
  TPM2C1SC_MS1x = 0;  // input capture mode
     
  EnableInterrupts;
}
 
 
/********************************************************/
void set_clock_12MHz(void)
{
  /*** CLOCK ***/
  // PLL Engaged External (PEE) mode
  // Set to 12MHz for default (non-USB) operation   
  MCGC2 = 0x36; // MCG clock initialization: BDIV = /1, RANGE = High freq., HGO = 1, LP = 0, External reference = oscillator, MCGERCLK active, disabled for stop mode
  while(!(MCGSC & 0x02));		      //wait for the OSC stable
  MCGC1 = 0x98; // External reference clock (12MHz), reference divider = /8, internal reference clock disabled
  while((MCGSC & 0x1C ) != 0x08); // external clock is selected
	MCGC3 = 0x42; // // PLL selected, no Loss of Lock Interrupt, VCO Divider * 8 = 12MHz
	while ((MCGSC & 0x48) != 0x48);	//wait for the PLL to lock
	MCGC1 = 0x18; // Switch to PLL reference clock
  while((MCGSC & 0x6C) != 0x6C); 
  
  /*** TIMER ***/
  TPM2SC_PS = 0b000;    // TPM2 prescaler = /1

  /*** SPI BAUD RATE ***/
  SPI2BR = 0x00; // 3 MHz @ 6MHz bus clock    

		//Setup my comport
/*
	SCI2BDH	= 0;
	//SCI2BDL = 0x27;	// 6000000/(9600*16)
	SCI2BDL = 0x0a;		// 6000000/(38400*16)	(3% error)
//	SCI2BDL = 0x01;		// 6000000/(115200*16)	(not possible)
	SCI2C1 = 0x00;	//Enable PE
	SCI2C2 = 0x0C;

		//Invert my reads and writes
	SCI2S2 = 0x10;
	SCI2C3 = 0x10;
*/
}


/********************************************************/
void DC16_Led(void)
{
	UINT8 c;

		//These will be a progress bar
	/*
	LED8 = !(gu8Led & 0x01);
	LED7 = !(gu8Led & 0x02);
	LED6 = !(gu8Led & 0x04);
	LED5 = !(gu8Led & 0x08);
*/

		//If we are in the power on phase, role these lights
	if ( power_on_flag )
	{
			//Show the user what step they are on
		switch ( guLoadStep )
		{
			default:
			case LOAD_CLK_SPEED:
				LED5 = ON;
				LED6 = LED7 = OFF;

					//Tell the user what state they have selected
				if ( guLoadCLK == LOAD_VAL_12MHZ ) 	c = 1;
				else 																c = 2;
				break;

			case LOAD_SCI_CHAN:
				LED6 = ON;
				LED5 = LED7 = OFF;

					//Tell the user what SCI they have selected
				if ( guLoadSCI == LOAD_VAL_SCI1 )	c = 1;
				else															c = 2;
				break;

			case LOAD_BAUD_RATE:
				LED7 = ON;
				LED5 = LED6 = OFF;
				
					//Tell the user what baud rate they have selected
				if 			( guLoadBaud == LOAD_VAL_9600 )	c = 1;
				else if ( guLoadBaud == LOAD_VAL_38400)	c = 2;
				else																		c = 3;
				break;
		}

			//Convert my C value to LED's on and off
		LED1 = (c & 2)? ON: OFF;
		LED2 = (c & 1)? ON: OFF;
/*
		c = counter;
		LED5 = ( c < 32 || c >= 224 )? ON: OFF;
		LED6 = ( c >= 32 && c < 64 || c >= 192 && c < 224 )? ON: OFF;
		LED7 = ( c >= 64 && c < 96 || c >= 160 && c < 192 )? ON: OFF;
		LED8 = ( c >= 96 && c < 160 )? ON: OFF;
*/
	}
	else
		LED5 = LED6 = LED7 = OFF;
	
		//If there is no SD card blink, else stay on
  if (SD_PRESENT == HIGH)
	{
		LED8 = (counter >= 127)? ON: OFF;
	}
	else
		LED8 = ON;

		//Don't use this for anything
		//Mostly because my LED broke off in the car ride home! =)
		//See, theres a reason too the madness
	//LED3 = OFF;

		//My TX and RX Leds
	if ( !power_on_flag )
	{
		LED1 = ( guRxLed < LED_RX )? ON: OFF;
		LED2 = ( guTxLed < LED_TX )? ON: OFF;
	}
}


/************************************************************/
// Parse http headers sent by the client
/************************************************************/

	//Parse out the header
UINT8 DC16_Get_Request( char* filename, char* http, INT16 len )
{
	INT16 i;
	INT16 g;
	INT16 m;
	INT16 found;
	char c;
	char find[] = "GET /\0";

		//Loop through the whole string looking for hits
	for ( found = m = g = -1, i = 0; found < 0 && i < len; i++ )
	{
			//Store the char we are looking at and capitalize it
		c = http[i];
		if ( c >= 'a' && c <= 'z' ) c = c - 'a' + 'A';

			//Log all the G's I find, since you spell GET with a g!
		if ( g < 0 && c == find[0] ) g = i;

			//If g >= 0 and match isn't we are starting a match sequence
		if ( g >= 0 && m < 0 )
		{
			g = -1;	//Reset g to find the next G
			m = 0;
		}

			//If we are in a match sequence then make sure we are getting matchs
		if ( m >= 0 )
		{
				//Check that we are still matching as we go
			if ( find[m] == c )
			{
					//Start of the filename not including the '/'
				if ( find[++m] == 0 ) found = i + 1;
			}
				//We didn't match, reset my match variables
			else
			{
					//If we have already found another g, start again
				if ( g >= 0 )
				{
					i = g;
					g = -1;
					m = 1;	//1 because we know we already found the first letter
				}
				else
					m = -1;
			}
		}
	}

		//Quit if we didn't find a filename
	if ( found < 0 )
		return 0;

		//Well we must have found a filename, save it and quit
	for ( m = 0, i = found; http[i] != ' ' && i < len; i++ )
	{
			//Store the char we are looking at and capitalize it
		c = http[i];
		if ( c >= 'a' && c <= 'z' ) c = c - 'a' + 'A';

			//Only allow FAT friendly chars
		if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' )
			filename[m++] = c;
	}
	filename[m] = 0;

		//Check if the user just gave us the /, if they did, return INDEX.HTM
	if ( m == 0 )
		(void)strcpy( filename, "INDEX.HTM" );

	return 1;
}

/************************************************************/
// Send/Recv Seral communication code
/************************************************************/

	//This function will send any string through the comport
INT16 DC16_Com_Send( char* data, INT16 len, UINT16 time_out )
{
	INT16 i;

		//Calc the len if the user didn't, lazy user's
	if ( len < 0 )
		len = strlen( data );

		//Wait until this thing is ready to send
	timeoutReset();
	if ( guLoadSCI == LOAD_VAL_SCI1 )
		while(!SCI1S1_TC && !timeout(time_out));
	else
		while(!SCI2S1_TC && !timeout(time_out));

		//Loop through the array sending out data
	for ( i = 0; i < len; i++ )
	{
			//Wait tell the buffer is clear
		timeoutReset();
		if ( guLoadSCI == LOAD_VAL_SCI1 )
		{
			while(!SCI1S1_TDRE)
			{
				if ( timeout(time_out) )
				{
					data[i] = 0;
					return i;
				}
			}
			SCI1D = data[i];			//Fill the buffer
		}
		else
		{
			while(!SCI2S1_TDRE)
			{
				if ( timeout(time_out) )
				{
					data[i] = 0;
					return i;
				}
			}
			SCI2D = data[i];			//Fill the buffer
		}
		TOUCH_TX(0);					//Toggle the TX led
	}

	return i;
}

	//This function will read a string from the comport
INT16 DC16_Com_Recv( char* data, INT16 len, UINT16 time_out )
{
	INT16 idx;
	char input;

		//If the user gave us the full buf size, protect them
	len--;

		//Read input data
	idx = 0;
	input = 0;
	while ( input != '\n' && idx < len )
	{
			//Wait until we get data
		timeoutReset();
		if ( guLoadSCI == LOAD_VAL_SCI1 )
		{
			while(!SCI1S1_RDRF)
			{
				if ( timeout(time_out) )	//Quit if we time out
				{
					data[idx] = 0;
					return idx;
				}
			}
			input = SCI1D;	//Read one byte from the serial
		}
		else
		{
			while(!SCI2S1_RDRF)
			{
				if ( timeout(time_out) )	//Quit if we time out
				{
					data[idx] = 0;
					return idx;
				}
			}
			input = SCI2D;	//Read one byte from the serial
		}
		TOUCH_RX(0);

			//If this is a valid char, save it
		if ((input >= 32 && input <= 126) ||	//Full normal ascii chars
				input == '\n' )										//Other symbols
			data[idx++] = input;
	}
	data[idx] = 0;

	return idx;
}

	//This just reads data until the timeout is met
INT16 DC16_Com_Runoff( UINT16 time_out )
{
	INT16 read = 0;
	char input;

		//Loop forever just reading data
	while ( 1 )
	{
			//Wait until we get data
		timeoutReset();
		if ( guLoadSCI == LOAD_VAL_SCI1 )
		{
				//Try to timeout
			while(!SCI1S1_RDRF)
				if ( timeout(time_out) ) return read;
			read++;
			input = SCI1D;	//Read one byte from the serial
		}
		else
		{
			while(!SCI2S1_RDRF)
				if ( timeout(time_out) ) return read;
			read++;
			input = SCI2D;	//Read one byte from the serial
		}
		TOUCH_RX(0);
	}

	return read;
}



/************************************************************/
// Checksum code
/************************************************************/

  //Return the checksum
UINT32 Adler( unsigned char *data, int len)
{
  UINT32 a, b;
  int tlen;

     //Loop through the entire string
  a = b = 0;
  while (len > 0)
  {
           //Figure out how much we can loop through
        tlen = len > 5550 ? 5550 : len;
        len -= tlen;

           //Loop through the string checksumming
        do
        {
              a += *data++;
              b += a;
        } while (--tlen);

           //Modules these things to keep em small
        a %= MOD_ADLER;
        b %= MOD_ADLER;
  }

     //Return my 32bit positive number
  return (((b & 0xFFFF) << 16) | (a & 0xFFFF));
}

	//Convert a number to a string
unsigned char* ultoa( UINT32 num, unsigned char* str )
{
  int i;
  int len;
  char t;

     //Find the length of this number
  for ( len = 0; num > 0 || len == 0; len++, num /= 10 )
     str[len] = (unsigned char)((num % 10) + '0');
  str[len] = 0;

     //Loop through and flip the order of this number
  for ( i = 0; i < len / 2; i++ )
  {
     t = str[i];
     str[i] = str[len - i - 1];
     str[len - i - 1] = t;
  }

  return str;
}


  //Convet a string to an unsigned long
UINT32 atoul( char* str )
{
  UINT32 num;
  int i;

     //Convert this string to a number
  num = 0;
  for ( i = 0; str[i] >= '0' && str[i] <= '9'; i++)
     num = num * 10 + (UINT32)(str[i] - '0');

  return num;
}


/**************************************************************
*	Function: 	delay
*	Parameters: 16-bit delay value
*	Return:		none
*
*	Simple delay loops
* These are not very accurate because they could get interrupted
**************************************************************/
 
void delay_ms(unsigned int ms)  // delay the specified number of milliseconds
{
  unsigned int timer, count;

  // this number is dependent on the system clock frequency  
  if (MCGC3_VDIV == 2)
    count = 248; // 12MHz
  else
    count = 996; // 48MHz
    
  while (ms != 0) 
  {
    for (timer=0; timer <= count; timer++);
    ms--;
  }
}

void delay_10us(unsigned int us) // delay of (us * 10) 
{
  unsigned char timer;
  
    // this number is dependent on the system clock frequency  
  if (MCGC3_VDIV == 2)  // 12MHz
  {
    while (us != 0) 
    { 
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );
      asm ( nop; );      
      asm ( nop; );      
      us--;
    }
  }
  else  // 48MHz
  {
    while (us != 0) 
    {
      for (timer=0; timer <= 33; timer++);
      us--;
    }
  }
}

/********************************************************/
UINT8* num2asc(UINT8 n) 
{
  n_str[0] = (n>>0x04)+0x30;  // convert MSN 'n' to ascii
  if(n_str[0]>0x39)           // if MSN is $a or larger...
    n_str[0]+=0x07;           // ...add $7 to correct
  n_str[1] = (n&0x0f)+0x30;   // convert LSN 'n' to ascii
  if(n_str[1]>0x39)           // if LSN is $a or larger...
    n_str[1]+=0x07;           // ...add $7 to correct
  n_str[2] = 0x00;            // add null
  return n_str;
} //end num2asc

UINT8 asc2num(char n_asc) 
{
  UINT8 n;

  n = n_asc - 0x30;      //convert from ascii to int
  if(n > 0x09)           // if num is $a or larger...
    n -= 0x07;           // ...sub $7 to correct
  if(n > 0x0f)           // if lower case was used...
    n -= 0x20;           // ...sub $20 to correct
  if(n > 0x0f)           // if non-numeric character...
    n = 0x00;            // ...default to '0'
  return n;
} //end asc2num


/*******************************************************
********************************************************
** Bootloader function code & interrupt handlers      **
********************************************************
*******************************************************/

const byte NVOPT_INIT  @0x0000FFBF = 0x02;    // vector redirect, flash unsecure
const byte NVPROT_INIT @0x0000FFBD = 0xFA;    // 0xFC00-0xFFFF are protected 

extern void _Startup(void);
extern interrupt VectorNumber_Vusb void usb_it_handler(void);         // in /USB_CMX/usb.c
extern interrupt VectorNumber_Vtpm2ch1 void ir_timer_interrupt(void); // in ir.c 
extern interrupt VectorNumber_Vtpm2ovf void isr_TPM2_TO(void);        // in ir.c

interrupt void RTC_ISR(void) // ~every 1ms
{
  RTCSC_RTIF = 1;
  counter++;
	timeoutTouch();
	if ( guRxLed < LED_RX )	guRxLed++;
	if ( guTxLed < LED_TX ) guTxLed++;
  DC16_Led();
  if (usb_enabled_flag && USB_DETECT) cdc_process(); // USB CMX CDC process 
}


interrupt void KBI_ISR(void) 
{
  KBISC_KBACK = 1;
}


interrupt void Dummy_ISR(void) {
   asm nop;
}


void (* volatile const _UserEntry[])()@0xFBBC={
  0x9DCC,             // asm NOP(9D), asm JMP(CC)
  _Startup
};
  
// redirect vector 0xFFC0-0xFFFD to 0xFBC0-0xFBFD
void (* volatile const _Usr_Vector[])()@0xFBC4= {
    RTC_ISR,          // Int.no.29 RTC                (at FFC4)
    Dummy_ISR,        // Int.no.28 IIC                (at FFC6)
    Dummy_ISR,        // Int.no.27 ACMP               (at FFC8)
    Dummy_ISR,        // Int.no.26 ADC                (at FFCA)
    KBI_ISR,          // Int.no.25 KBI                (at FFCC)
    Dummy_ISR,        // Int.no.24 SCI2 Transmit      (at FFCE)
    Dummy_ISR,        // Int.no.23 SCI2 Receive       (at FFD0)
    Dummy_ISR,        // Int.no.22 SCI2 Error         (at FFD2)
    Dummy_ISR,        // Int.no.21 SCI1 Transmit      (at FFD4)
    Dummy_ISR,        // Int.no.20 SCI1 Receive       (at FFD6)
    Dummy_ISR,        // Int.no.19 SCI1 error         (at FFD8)
    isr_TPM2_TO,      // Int.no.18 TPM2 Overflow      (at FFDA)
    ir_timer_interrupt,  // Int.no.17 TPM2 CH1           (at FFDC)
    Dummy_ISR,        // Int.no.16 TPM2 CH0           (at FFDE)
    Dummy_ISR,        // Int.no.15 TPM1 Overflow      (at FFE0)
    Dummy_ISR,        // Int.no.14 TPM1 CH5           (at FFE2)
    Dummy_ISR,        // Int.no.13 TPM1 CH4           (at FFE4)
    Dummy_ISR,        // Int.no.12 TPM1 CH3           (at FFE6)
    Dummy_ISR,        // Int.no.11 TPM1 CH2           (at FFE8)
    Dummy_ISR,        // Int.no.10 TPM1 CH1           (at FFEA)
    Dummy_ISR,        // Int.no.9  TPM1 CH0           (at FFEC)
    Dummy_ISR,        // Int.no.8  Reserved           (at FFEE)
    usb_it_handler,   // Int.no.7  USB Statue         (at FFF0)
    Dummy_ISR,        // Int.no.6  SPI2               (at FFF2)
    Dummy_ISR,        // Int.no.5  SPI1               (at FFF4)
    Dummy_ISR,        // Int.no.4  Loss of lock       (at FFF6)
    Dummy_ISR,        // Int.no.3  LVI                (at FFF8)
    Dummy_ISR,        // Int.no.2  IRQ                (at FFFA)
    Dummy_ISR,        // Int.no.1  SWI                (at FFFC) 
};


#pragma CODE_SEG Bootloader_ROM

void Bootloader_Main(void);

void _Entry(void) 
{      
  SOPT1 = 0x33;    // disable COP, enable STOP mode
  SOPT2 = 0x00;    // enable high rate SPI                   
  
  /*** MODE SELECT SWITCH ***/
#ifdef _DC16_dev  // development board      
  PTGDD_PTGDD0 = _IN;  // PTG0 is input
  PTGPE_PTGPE0 = 1;    // internal pullup for PTG0
#else // production badge
  PTGDD_PTGDD1 = _IN;  // PTG1 is input
  PTGPE_PTGPE1 = 1;    // internal pullup for PTG1
#endif
    
  // PLL Engaged External (PEE) mode
  // Setup @ 48MHz for USB Bootloader   
  MCGC2 = 0x36; // MCG clock initialization: BDIV = /1, RANGE = High freq., HGO = 1, LP = 0, External reference = oscillator, MCGERCLK active, disabled for stop mode
  while(!(MCGSC & 0x02));		      //wait for the OSC stable
  MCGC1 = 0x98; // External reference clock (12MHz), reference divider = /8, internal reference clock disabled
  while((MCGSC & 0x1C ) != 0x08); // external clock is selected
	MCGC3 = 0x48; // PLL selected, no Loss of Lock Interrupt, VCO Divider * 32 = 48MHz
	while ((MCGSC & 0x48) != 0x48);	//wait for the PLL to lock
	MCGC1 = 0x18; // Switch to PLL reference clock
  while((MCGSC & 0x6C) != 0x6C);
	   
  // If SW_MODE is not held down, enter normal program 
  if(SW_MODE) 
    asm JMP _UserEntry;           // Enter User Mode
  else // Enter Bootloader Mode
  {
    // set LEDs so it is evident that we've entered Bootloader mode
   LED1_DD = LED8_DD = _OUT; // set to output
   LED1_DS = LED8_DS = _OUT; // set to high output drive
   LED1 = LED8 = ON;
                   
   USBCTL0=0x44; // Enable DP pull-up resistor and USB voltage regulator             
    
   Bootloader_Main();            // Enter bootloader code
  }

} // _Entry end

#pragma CODE_SEG default
      
/****************************** END OF FILE **********************************/

