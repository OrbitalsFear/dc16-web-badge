#include "timeout.h"

volatile UINT16 TimeMcOut = 0;

	//Reset the timeout
void timeoutReset(void)
{
	TimeMcOut = 0;
}

	//Increase the timeout
void timeoutTouch(void)
{
	TimeMcOut++;
}

	//Return true if the user has timed out
UINT8 timeout( UINT16 time_out )
{
	return (time_out > 0 && (UINT16)TimeMcOut >= time_out)? 1: 0;
}
