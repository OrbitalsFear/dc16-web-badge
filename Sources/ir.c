/******************************************************************************
*
* DEFCON 16 BADGE
*
* Filename:		ir.c
* Author:		  Joe Grand (Kingpin)
* Revision:		1.0
*
* Description:DEFCON 16 Badge Infrared stuff
*
* Immense thanks to Luis Puebla at Freescale who wrote the original IR RX
* demo code for the 9S08LC60 Electricity Meter application note. He was 
* instrumental in helping me port the code to the JM60 and get it actually
* working! -kp
*******************************************************************************/
  
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "usb.h"
#include "target.h"
#include "usb_cdc.h"
#include "FslTypes.h"
#include "ir.h"
#include "DC16.h"

// globals from main.c
extern UINT8 usb_enabled_flag;
extern UINT8 ir_rx_byte;
  
  
/* ir definitions */
unsigned char ir_command;
unsigned char ir_address;
//unsigned char ir_timer;

unsigned char ir_temp_data = 0xFF; // receive unmodulated bits
unsigned int ir_received_data;    // bits demodulated
unsigned char ir_state;


void ir_init(unsigned int period) 
{
  while(1) // wait here until TPM2MOD has been written to the register
  {
     TPM2MOD = period;
     if (TPM2MOD == period) break;
  }  
   
  TPM2SC_TOIE = 1; 
  ir_as_falling_edge(); // TPM2CH1 as falling edge detect
  TPM2SC_CLKSA = 1;     // BUSCLOCK as TPM clock  
}


/**
 * \brief  This function works even when a change from Input capture to Output compare is done
 *         and works with timer with module
 * \author B01252  
 * \param  period on ticks units
 * \return void
 * \todo
 * \warning
 */ 

// timer overflow //
void ir_interrupt_period(unsigned int period) 
{
   unsigned int temp_toc;
   
   TPM2C1SC_CH1F = 0; // clear any pending flag
   
   temp_toc = TPM2C1V + period;
   if (temp_toc > IR_RC_PERIOD) temp_toc+= ~IR_RC_PERIOD; // update interrupt period to keep within range
     
   TPM2C1SC = TPM2C1SC_MS1A_MASK;   // change to timer output compare
   
   TPM2C1V = temp_toc;
   TPM2C1SC_CH1IE = 1;
}


void ir_as_falling_edge(void) 
{
   unsigned char tmp;

   tmp = TPM2C1SC_CH1F; // read flag bit first (per MC9S08JM60 data book 16.6.2)
   TPM2C1SC_CH1F = 0;   // clear any pending flag
   TPM2C1SC = TPM2C1SC_ELS1B_MASK | TPM2C1SC_CH1IE_MASK;  
}

/**
 * \brief  TPM1 Timer overflow service:  
 * \n time_oc_pulse service
 * \n ir_timer  service   /clear ir_command and ir_address if no data is being received by IR_RC

 * \author  Luis Puebla
 * \param   
 * \return  
 * \todo    
 * \warning
 */   
interrupt VectorNumber_Vtpm2ovf void isr_TPM2_TO(void) //CH1
{  
  TPM2SC_TOF = 0;

  // use PTC4 as debug output to show the IR_RC_PERIOD
  PTCDD_PTCDD4 = 1;
  PTCD_PTCD4 = !PTCD_PTCD4;

  /*if (ir_timer) ir_timer--;
  else
  {
    ir_command = 0;        
    ir_address = 0;
  }*/
}

 
/**
 * \brief  data is repeated every 40 msec for the IR remote control
 * \n  if no data presented during 80 milliseconds then
 * \n  ir command = 0 
 * \n  ir address = 0, if data is being received timer is updated
 * \author B01252  
 * \param  void
 * \return void
 * \todo
 * \warning   
*/
  
// to receive data, we wait for the first falling edge on the IR_IN line, which
// signifies the start of data transmission. we want to sample each bit at the
// middle of the bit period to ensure we're reading the proper value.
// upon receiving the first falling edge, we switch to timer output compare mode and
// set the interrupt time for TBIT/2, which will put us halfway into the bit.
// from then on, the interrupt is set to TBIT, which will keep us halfway into each 
// subsequent bit in the data stream.    

void interrupt VectorNumber_Vtpm2ch1 ir_timer_interrupt(void) 
{
   TPM2C1SC_CH1F = 0; // clear any pending flag
   
   // use PTC4 as debug output to show the interrupt period
   //PTCDD_PTCDD4 = 1;
   //PTCD_PTCD4 = !PTCD_PTCD4;
  
   if (ir_edge_mode) 
   {
     ir_interrupt_period(TBIT_2); // set up period for TBIT/2 and enter timer output compare mode  
      
     ir_temp_data = (ir_temp_data<<1) | 0x01;  //set a default value to avoid missunderstanding of start reading
     return;
   } 
   else   //if (ir_toc_mode) 
   {
     ir_interrupt_period(TBIT); // set period to TBIT to ensure we stay halfway into the bit
          
     //if (infrared_sony_protocol()) ir_as_falling_edge(); // use to receive standard sony remote controls 
     if (infrared_dc16_protocol()) ir_as_falling_edge();   // use to receive data transfer from DEFCON 16 badge
   }
}
   

 /* This Function must be called in the middle of the data bit.

  returns
     0 receiving data (toc mode)
     1 requires sincronization (detect falling edge)
   */

unsigned char infrared_dc16_protocol(void) 
{
  unsigned char sync_flag = 0; 
  //unsigned char ir_byte_local;
  
  // use PTC2 as debug output to show when IR signal is read
  PTCDD_PTCDD2 = 1;
  PTCD_PTCD2 = !PTCD_PTCD2;

  ir_temp_data  = (ir_temp_data<<1) | IR_DATA_PIN;

  // look for start signal
  if ((ir_temp_data & 0x0F) == 0x00)  // is start data 4 '1's signifying header?
  {
    //ir_rx_byte = TRUE;

    ir_state = IR_STATE_RECEIVING_COMMAND;
    ir_received_data = 0x100;  // we want to capture 8 data bits
    return 1; // wait for the next falling edge
  }
  
    // look for transmission end 
  if ((ir_temp_data & 0x07) == 0x07) // is transmission end ?
  {
    ir_state = IR_STATE_COMPLETING_DATABIT; // wait for next falling edge to signify start
    return 1; // wait for sync
  }
   
  // wait for more data in order to determine if a start or end is received
  if (ir_state == IR_STATE_COMPLETING_DATABIT)  return 0 ;  // wait for more data to validate start or end of transmision

  // if module is already receiving data...
  // check if we've received a valid "1" or "0" bit 
  
  if ((ir_temp_data & 0x07) == 0x04) // check if a "1" is received (comprising 3 bits of modulated data, "100")
  {
    ir_received_data= (ir_received_data>>1) | 0x100; 
    sync_flag = 1; // wait for sync
  }
     
  if ((ir_temp_data & 0x07) == 0x05) // check if a "0" is received (comprising 3 bits of modulated data, "101")
  {
    ir_received_data = ir_received_data>>1; 
    sync_flag = 1; // wait for sync
  }
    
  // if all 8 bits are received, then transfer ir_received_data to ir_command    
  if (ir_state == IR_STATE_RECEIVING_COMMAND && (ir_received_data & 0x01))
  {
    //ir_timer = IR_DELAY_TIME;
    ir_state = IR_STATE_COMPLETING_DATABIT; // wait for end and then for start signal
  
    ir_command = (ir_received_data>>1) & 0xFF;
    sync_flag = 0;  
    
    ir_rx_byte = TRUE;    
  }
 
  return sync_flag; // function is waiting for more input data
}


/*unsigned char infrared_sony_protocol(void) 
{
  unsigned char sync_flag = 0; 
  //static unsigned char ir_command_fil1;
  //static unsigned char ir_command_fil2;

  // use PTC2 as debug output to show when IR signal is read
  PTCDD_PTCDD2 = 1;
  PTCD_PTCD2 = !PTCD_PTCD2;

  ir_temp_data  = (ir_temp_data<<1) | IR_DATA_PIN;

  // look for start signal
  if ((ir_temp_data & 0x0F) == 0x00)  // is start data 4 '1's signifying header?
  {
    ir_state = IR_STATE_RECEIVING_COMMAND;
    ir_received_data = 0x80;  //to wait for 7 data bits on command
    return 1;                 // wait for the next falling edge
  }
    
  // look for transmission end 
  if ((ir_temp_data & 0x07) == 0x07) // is transmission end ?
  {
    ir_state = IR_STATE_COMPLETING_DATABIT; // wait for start bit
    return 1;      // wait for sincronization falling edge
  }
   
  // wait for more data in order to determine if a start or end is received
  if (ir_state == IR_STATE_COMPLETING_DATABIT)  return 0 ;  // wait for more data to validate start or end of transmision

  // if module is already receiving data...
  
  // check if one has been received (is really "100")
  if ((ir_temp_data & 0x07) == 0x04)
  {
    ir_received_data= (ir_received_data>>1) | 0x80;  // one is received
    sync_flag = 1;                         // wait for sincronization falling edge
  }
     
  // check if a zero has been received (is really "101"   
  if  ((ir_temp_data & 0x07) == 0x05) 
  {
    ir_received_data = ir_received_data>>1;          //zero is received
    sync_flag = 1;                         // wait for sincronization falling edge
  }
    
  // if all 7 bits of command are received, then transfer ir_received_data to ir_command    
  if (ir_state == IR_STATE_RECEIVING_COMMAND && (ir_received_data & 0x01))
  {
    ir_timer = IR_DELAY_TIME;
        
    // command must be received 3 times in order to confirm the data is correct
    //if (ir_command_fil1 == ir_command_fil2 && ir_command_fil1 == ir_received_data>>1 )  ir_command =  ir_command_fil1;
    //ir_command_fil2 = ir_command_fil1;
    //ir_command_fil1 = ir_received_data>>1;
        
    ir_command = ir_received_data>>1;
      
    ir_received_data = 0x20; // wait for address command (5 bits)
    ir_state = IR_STATE_RECEIVING_ADDRESS;
  }

  // if all 5 bits of address are received, then transfer ir_received_data to ir_address
  if (ir_state == IR_STATE_RECEIVING_ADDRESS && (ir_received_data & 0x01))
  {
    ir_address = ir_received_data>>3;
    ir_timer = 80;
    ir_state = IR_STATE_COMPLETING_DATABIT; // wait for end and then for start signal
    sync_flag = 1;
  }
 
  return sync_flag; // function is waiting for more input data
}*/

  
/********************************************************
*** infrared data transmission routines
*** depending on the remote control/IR protocol used,
*** the pulse widths of the "on" and "off" pulses will
*** vary.
*** 
*** for example, sony uses a common pulse-coded signal 
*** in which the length of the pulse is varied, but the 
*** length of space is constant. we're using that.
***
*** by changing these timings and protocol and TPM2MOD
*** carrier frequency, you can support just about any 
*** IR encoding scheme you want...
***
*** see the tv-b-gone example in main.c
*** maybe some hotel tv system 0wning, anyone?
*********************************************************/

void ir_tx_data_0(void) // used for data transfer
{
  IR_TX_On;
  delay_10us(Data0_Time_ON);
  
  IR_TX_Off;
  delay_10us(Data0_Time_OFF);
}

void ir_tx_data_1(void) // user for data transfer
{
  IR_TX_On;
  delay_10us(Data1_Time_ON);
  
  IR_TX_Off;
  delay_10us(Data1_Time_OFF);
}

void ir_tx_data_0_sony(void) // used to transmit our power-on test pulses
{
  IR_TX_On;
  delay_10us(Data0_Time_ON_Sony);
  
  IR_TX_Off;
  delay_10us(Data0_Time_OFF_Sony);
}

void ir_tx_data_1_sony(void) // used to transmit our power-on test pulses
{
  IR_TX_On;
  delay_10us(Data1_Time_ON_Sony);
  
  IR_TX_Off;
  delay_10us(Data1_Time_OFF_Sony);
}

void ir_sony_off(void)
{
  // brute-force method of just turning on and off the carrier
  // at the proper timings to transmit the command/data
  //
  // sony requires:
  // 600uS on/600uS off for bit 0
  // 1200uS on/600uS off for bit 1
  
  IR_TX_On;
  delay_10us(245); // header
  IR_TX_Off;
  delay_10us(60);
  
  ir_tx_data_1_sony(); // command (7 bits) = 21d
  ir_tx_data_0_sony();
  ir_tx_data_1_sony();
  ir_tx_data_0_sony();
  ir_tx_data_1_sony();
  ir_tx_data_0_sony();
  ir_tx_data_0_sony();
  ir_tx_data_1_sony(); // address (5 bits) = 1d
  ir_tx_data_0_sony();
  ir_tx_data_0_sony();
  ir_tx_data_0_sony();
  ir_tx_data_0_sony();
}
 

void ir_tx_byte (unsigned char val)
{
  unsigned char whichBit, temp;

  IR_TX_On;
  delay_10us(Data0_Time_ON << 2); // header
  IR_TX_Off;
  delay_10us(Data0_Time_OFF);
    
  whichBit = 0;
  while (whichBit < 8)
  {
    temp = 1 << whichBit;
    
    if ((val & temp) == 0)
      ir_tx_data_0();
    else 
      ir_tx_data_1();
    
    whichBit++;
  }
}


void ir_tx_string (unsigned char *Array_to_send)
{
  while(*Array_to_send != '\0')
	{
	  ir_tx_byte (*Array_to_send);
		Array_to_send++;
		delay_ms(3); // need to have a delay in between bytes to prevent IR receiver from locking up
	}	
}


/*unsigned char ir_rx_byte (void)
{
  unsigned char whichBit, temp, val;
 
 // wait for PTF5 to go low, signifying it's receiving IR 
  
 whichBit = 0;
 val = 0;
 
 while (whichBit < 8)
 {
    // wait 1.5 * T (the bit's time period) before we sample
    // we want to be in the middle of each bit
    
    if (!PTFD_PTFD5)
    {
      temp = 1 << whichBit;
      val += temp;
    }
    
    whichBit++;
 }
 
 return val; 
  
}*/

