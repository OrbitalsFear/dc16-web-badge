/******************************************************************************
*
* DEFCON 16 BADGE
*
* Filename:		ir.h
* Author:		  Joe Grand (Kingpin)
* Revision:		1.0
*
* Description:	Header File for the DEFCON 16 Badge Infrared stuff
*******************************************************************************/

#ifndef __IR__
#define __IR__


/**************************************************************************
************************** Macros *****************************************
***************************************************************************/

#define IR_TX_On   { TPM2C0SC_ELS0x = 0b10; TPM2C0SC_MS0B = 1; } // turn on 38kHz IR carrier (set polarity: high-true pulses, edge aligned PWM) 
#define IR_TX_Off  TPM2C0SC_ELS0x = 0b00; // turn off 38kHz IR carrier (disable TPM support on pin - revert to GPIO)

//#define IR_RX_On   TPM2C1SC_ELS1x = 0b10; // turn on input compare (capture on falling edge only)
#define IR_RX_Off  { TPM2C1SC_ELS1x = 0b00; TPM2SC_TOIE = 0; } // turn off input compare (disable TPM support on pin - revert to GPIO)
  

/******************************************************************
************************** Constants ******************************
*******************************************************************/

#define IR_DATA_PIN  PTFD_PTFD5 // TPM2, channel 1

#define  ir_edge_mode   !TPM2C1SC_MS1A    // input capture mode
#define  ir_toc_mode     TPM2C1SC_MS1A    // timer output compare mode

#define IR_STATE_COMPLETING_DATABIT  0
#define IR_STATE_RECEIVING_COMMAND   1
#define IR_STATE_RECEIVING_ADDRESS   2

// IR TX timing
// this gives us a transmit rate of ~771 bits/second. yeah! take that, acoustic coupler!
#define Data0_Time_ON   30    // carrier on time for data 0 (300uS)
#define Data0_Time_OFF  30    // carrier off time for data 0
#define Data1_Time_ON   60    // carrier on time for data 1 
#define Data1_Time_OFF  30    // carrier off time for data 1

// for standard Sony IR
#define Data0_Time_ON_Sony   60    // carrier on time for data 0 (600uS) 60 * 10uS
#define Data0_Time_OFF_Sony  60    // carrier off time for data 0
#define Data1_Time_ON_Sony   120   // carrier on time for data 1 
#define Data1_Time_OFF_Sony  60    // carrier off time for data 1


// IR RX timing
// Fbus = 6MHz (12MHz clock)
//#define TBIT  3600  // 600uS bit time
#define TBIT    1800  // 300uS bit time
#define TBIT_2  TBIT >> 1

//#define IR_RC_PERIOD   12000  // 2000uS period 
#define IR_RC_PERIOD  6000  // 1000uS period

//#define IR_DELAY_TIME  100  // time before clearing ir_command and ir_address if no more data received

// These codes are for a standard Sony TV remote control 
/*#define IRRC_1          0
#define IRRC_2          1
#define IRRC_3          2
#define IRRC_4          3
#define IRRC_5          4
#define IRRC_6          5
#define IRRC_7          6
#define IRRC_8          7
#define IRRC_9          8
#define IRRC_0          9
#define IRRC_CH_UP      16
#define IRRC_CH_DOWN    17
#define IRRC_VOL_UP     18
#define IRRC_VOL_DOWN   19
#define IRRC_UP         116
#define IRRC_DOWN       117
#define IRRC_RIGHT      51
#define IRRC_LEFT       52
#define IRRC_SELECT     101
#define IRRC_ENTER      101
*/


/***********************************************************************
 ************************** Function prototypes ************************
 ***********************************************************************/
void ir_interrupt_period(unsigned int period);
void ir_as_falling_edge(void);
void interrupt VectorNumber_Vtpm2ch1 ir_timer_interrupt(void);
//unsigned char infrared_sony_protocol(void);
unsigned char infrared_dc16_protocol(void);
void ir_init(unsigned int period);  // configure the TPM2 module

void ir_tx_data_0(void);
void ir_tx_data_1(void);
void ir_tx_data_0_sony(void);
void ir_tx_data_1_sony(void);
void ir_sony_off(void);
void ir_tx_byte (unsigned char val);
void ir_tx_string (unsigned char *Array_to_send);
//unsigned char ir_rx_byte (void);

/* */
extern unsigned char ir_command;
extern unsigned char ir_address;
//extern unsigned char ir_timer;


#endif /* __IR__ */
