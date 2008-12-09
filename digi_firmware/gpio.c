/*  Def con 16 badge firmware for passthrough and FTP reflash ability

  Orbital_sFear
  
  This software is GPL'ed, do with it as you will.

  This software is only used in the case the user wants a completely standalone
  web solution for their new defcon 16 badge webserver. *pat on back*
  This code requires NetOS 6.3 to update on your own. Typically it's easier
  for a user to buy a digi Connectme-C and flash this firmware onto it.
*/


#include "narmled.h"

	//Reset button pushed
int gpioCheckResetButton()
{
	unsigned int reset;
	int reset_list[] = SETTINGS_ZERO_LIST;
	unsigned long time;
	unsigned long t_time;

		//Check GPIO pin 0 for the reset button push
	if ( NAgetGPIOpin( 0, &reset ) != SUCCESS )
		return 0;

		//Check if the reset button is pushed
	if ( !reset )
	{
			//Shoot out some debug data
		debugPrint( "Reset switch is being pushed" );

			//Tell the user visually that this happened
		time = tx_time_get();
		do { 
			NALedOn( 0xFFFFFFFF );
			t_time = tx_time_get();

		} while ( t_time - time <= BSP_TICKS_PER_SECOND && 
			  t_time - time >= 0 ); 

			//Reset the settings in the flash memory
		settingsZeroList();

			//Do anything else
	}

	return 1;
}
