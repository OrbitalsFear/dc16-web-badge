#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char filename[128];
	FILE* handle;
	int bc = 0;
	int len;

	while ( !feof( stdin ) )
	{
		fgets( filename, sizeof(filename), stdin );
		len = strlen( filename );
		while ( filename[len] < 'a' || filename[len] > 'z' ) filename[len--] = 0;
		if ( (handle = fopen( filename, "r" )) != NULL )
			for ( ; !feof( handle ); fgetc(handle) ) bc++;
	}

		//Print out the bytes
	printf( "Byte Count: %d\n", bc );

	return 1;
}
