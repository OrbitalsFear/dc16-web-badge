#ifndef TIMEOUT
#define TIMEOUT

	//Required to use all my cool typedefs
#include "FslTypes.h"

	//Basic timeout functions
void timeoutReset(void);
void timeoutTouch(void);
UINT8 timeout( UINT16 time_out );

#endif

