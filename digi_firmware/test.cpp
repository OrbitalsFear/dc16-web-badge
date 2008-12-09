#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>


int main()
{
	int handle;
	char out[] = "GET /how.htm Something\n";
	char in[8192];
	int len;

		//Open the handle
	handle = open( "/dev/ttyUSB0", O_RDWR );

		//Write out some data
	write( handle, out, sizeof( out ) );
	printf( "From DC\n");
	while ( (len = read( handle, in, sizeof(in) )) )
	{
		in[len] = 0;
		printf( "%s", in );
		fflush(stdout);
	}

	return 1;
}
