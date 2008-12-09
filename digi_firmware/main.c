/*	Def con 16 badge firmware for passthrough and FTP reflash ability

	Orbital_sFear
	
	This software is GPL'ed, do with it as you will.

	This software is only used in the case the user wants a completely standalone
	web solution for their new defcon 16 badge webserver. *pat on back*
	This code requires NetOS 6.3 to update on your own.	Typically it's easier
	for a user to buy a digi Connectme-C and flash this firmware onto it.

	A user with a serial port can compile this to run on your desktop by 
	commenting out the DIGI_FIRMWARE define.  I only tested it on ubuntu ibex,
	if anyone else wants to make it go please feel free.
*/


/************/
/* Controls */
/************/
	//Comment this out to compile in ubuntu
	//Uncomment it to compile for NetOS 6.3 using a digi connet me-c
#define DIGI

	//Uncomment this to enable the auto reboot function (digi only)
//#define AUTO_REBOOT

#include <stdio.h>
#include <stdlib.h>

		//Digi includes
#ifdef DIGI
	#include <tx_api.h>
	#include <bsp_api.h>
	#include <armutils.h>
	#include <sockapi.h>
	#include <fsock.h>
	#include "termios.h"
	#include "cs.h"
	#include "appconf.h"
	#include "gpio.h"

		//Trust me!
	#define O_RDWR	0x2	
	#define COM_NAME "/com/0"

		//Digi threading
	#define THREAD_STACK_SIZE 				8192	
	#define TCP_PASSTHROUGH_SLICE			10
	#define TCP_PASSTHROUGH_PRIORITY	naMediumPriority

	#define THREAD_PARAM	unsigned long
	#define THREAD_RETURN void

		//Define sockets
	#define SOCK 						SOCKET
	#define SOCK_LEN				int	
	#define SOCKADDR 				struct in_sockaddr
	#define SOCKP						struct in_sockaddr*
	#define CLOSE_SOCKET(x) 	closesocket(x)

		//Sleep commands
	#define SLEEP(x)	tx_thread_sleep(x)
	#define TICKS 		BSP_TICKS_PER_SECOND

		//Remote port
	#define TCP_PORT									80
#else
	#include <linux/types.h>
	#include <linux/socket.h>
	#include <linux/types.h>
	#include <linux/stat.h>
	#include <pthread.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <unistd.h>
	#include <string.h>
	#include <arpa/inet.h>

		//Define the name of my comport
	#define COM_NAME "/dev/ttyUSB0"

	#define THREAD_PARAM	void*
	#define THREAD_RETURN	void*

		//Define sockets
	#define SOCK 				int
	#define SOCK_LEN		socklen_t
	#define SOCKADDR 		sockaddr_in
	#define SOCKP				sockaddr*
	#define CLOSE_SOCKET(x) 	close(x)

		//Sleep commands
	#define SLEEP(x)	usleep(x)
	#define TICKS 		1000000

		//Remote port
	#define TCP_PORT									3000
#endif

	//Local defines
#define BAUD_RATE									9600
//#define BAUD_RATE									115200
#define BUFFER										4096
#define STORE_BUF									512	
#define MAX_INIT_ATTEMPT 					8
#define TCP_READ_COUNT						32		//Max number of connections at one time

#define CPS												20		//Checks Per Second
#define MAX_TOUCHES								3			//Seconds of stale connection time
#define FORCE_REBOOT 							3900	//Seconds until a forceful reboot
#define REBOOT_SECONDS						3600	//Seconds until a reboot request

	//Thread variables
#ifdef DIGI
	TX_THREAD TCP_Con_Thread;
	TX_THREAD TCP_Thread[TCP_READ_COUNT];
	TX_THREAD Buffer_Thread;
	TX_THREAD Serial_Thread;
#else
	pthread_t TCP_Con_Thread;
	pthread_t TCP_Thread[TCP_READ_COUNT];
	pthread_t Buffer_Thread;
	pthread_t Serial_Thread;
#endif

	//Serial and rebooting threads
int Reboot = 0;
int Com_Handle = -1;

	//Handles all my connection info
SOCK Main_Sock;
SOCK Connection[TCP_READ_COUNT];
char Connection_Buf[TCP_READ_COUNT][BUFFER];
int Connection_Len[TCP_READ_COUNT];
int Connection_Touch[TCP_READ_COUNT];
int Connection_Idx;
int Connection_Last;

char Recv_Buf[BUFFER];
int Recv_Len = 0;
int Recv_Match = 0;

//Load a ftp server to reflash this thing
void ftp_init_flash_download (void);

	//Filter to clean out junk data found often when using speeds over 9600
char* filter( char* buf, int* plen )
{
	int len;
	int i;
	int g;
	int m;
	int found;
  char c;
  char find[] = "TP/1.1 \0";	//I look for less to give a 2 byte error tolerance

    //Loop through the whole string looking for hits
	len = (*plen);
  for ( found = m = g = -1, i = 0; found < 0 && i < len; i++ )
  {
      //Store the char we are looking at
    c = buf[i];

      //Log all the find's I find
    if ( g < 0 && c == find[0] ) g = i;

      //If g >= 0 and match isn't we are starting a match sequence
    if ( g >= 0 && m < 0 )
    {
      g = -1; //Reset g to find the next header
      m = 0;
    }

      //If we are in a match sequence then make sure we are getting matchs
    if ( m >= 0 )
    {
        //Check that we are still matching as we go
      if ( find[m] == c )
      {
          //End of the header
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
          m = 1;  //1 because we know we already found the first letter
        }
        else
          m = -1;
      }
    }
  }

    //Quit if we didn't find a filename
  if ( found < 9 )	//9 because if you count 'HTTP/1.1 ' its 9 chars
    return buf;

		//Clean the header and resend it
	buf[--found] = ' ';
	buf[--found] = '1';
	buf[--found] = '.';
	buf[--found] = '1';
	buf[--found] = '/';
	buf[--found] = 'P';
	buf[--found] = 'T';
	buf[--found] = 'T';
	buf[--found] = 'H';

		//Tell the user how long this thing really is now
	(*plen) -= found;

	return buf + found;
}

	//Init my comport
int initComPort()
{
	int com_handle;
	int count;
	unsigned char bit_mask;
	struct termios term_ios;
																																								
#ifdef DIGI
		//Open my comport for reading and writing
	if ( (com_handle = open( COM_NAME, O_RDWR )) < 0 )
		return -1;

		//Set my baudrate
	tcgetattr( com_handle, &term_ios );
	cfsetospeed( &term_ios, BAUD_RATE );
	cfsetispeed( &term_ios, BAUD_RATE );
																																								
		//Set myself to 8bits, no parity, 1 stop - 8N1
	term_ios.c_cflag &= ~PARENB;
	term_ios.c_cflag &= ~CSTOPB;
	term_ios.c_cflag &= ~CSIZE;
	term_ios.c_cflag |= CS8;

	term_ios.c_iflag = IGNPAR;
	term_ios.c_oflag = 0;
	term_ios.c_lflag = 0;

		//Save these options
	if ( tcsetattr( com_handle, TCSADRAIN, &term_ios ) < 0 )
		return -1;

		//Pull the DTR and TRS high and low
		//This is for debugging only
	bit_mask = 0;
	bit_mask |= MS_DTR;
	bit_mask |= MS_RTS;
	tcsetsignals( com_handle, bit_mask );
#else	//Computer

		//Open my comport for reading and writing
	com_handle = open( COM_NAME, O_RDWR );
																																								
#endif
																																								
		//Our Com-Port is open and ready
	return com_handle;
}

	//Load my tcp socket
SOCK TCP_Load()
{
	SOCK sock;
	SOCKADDR fsin;
	SOCK_LEN fsin_len = sizeof( SOCKADDR );

		//Load the socket
	#ifdef DIGI
	if ( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
	#else
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	#endif
		return sock;
																																								
		//Create my local address to bind to
	memset((void *)&fsin, 0, sizeof(fsin));
	fsin.sin_family				= AF_INET;
	fsin.sin_addr.s_addr	= htonl(INADDR_ANY);
	fsin.sin_port					= htons( TCP_PORT );
																																								
		//Bind my port
	bind(sock, (SOCKP)&fsin, sizeof(fsin));
	listen( sock, TCP_READ_COUNT );

	return sock;
}

	//This handles my incoming connections
THREAD_RETURN TCP_Connections(THREAD_PARAM nothing)
{
	SOCK sock;
	int len;
	SOCKADDR fsin;
	SOCK_LEN fsin_len = sizeof( SOCKADDR );
	int i;
	int r;
	int e;
	int missed;

		//Clear out my remote info
	memset((void *)&fsin, 0, sizeof(fsin));
	fsin.sin_family				= AF_INET;
	fsin.sin_addr.s_addr	= htonl(INADDR_ANY);
	fsin.sin_port					= htons( TCP_PORT );

		//Stay in this loop for ever and ever
	while (1)
	{
			//Accept a Connection
		sock = accept( Main_Sock, (SOCKP)&fsin, &fsin_len );
		if ( sock >= 0 )
		{
				//Got through all possible connection slots start at the last one
				//Only allow the connection to be attached if we aren't in reboot mood
			missed = 1;
			if ( !Reboot )
				for ( i = 0; missed && i < TCP_READ_COUNT; i++ )
				{
						//Attempt to attach this to an open tcp slot
					r = (i + Connection_Last + 1) % TCP_READ_COUNT;
					if ( Connection[r] < 0 )
					{
							//Set that we found a connection hit and get the variables setup
						missed = 0;
						Connection[r] = sock;
						Connection_Len[r] = 0;
						Connection_Touch[r] = 0;

							//Tell the user what happened
						#ifndef DIGI
						printf("Attached new connection to id %d\n", r);
						#endif
	
							//Store the last connection, Check if we can find a new connection
						Connection_Last = r;
						for (r=0; r < TCP_READ_COUNT && Connection[Connection_Idx] < 0; r++)
						{
							e = (Connection_Idx + 1 + r) % TCP_READ_COUNT;
							if ( Connection[e] >= 0 )
								Connection_Idx = e;
						}
					}
				}

				//If we didn't find an open slot, close the connection
			if ( missed )
			{
				CLOSE_SOCKET( sock );
				#ifndef DIGI
				printf("No connection aviable\n");
				#endif
			}
		}
	}

	#ifndef DIGI
	return NULL;
	#endif
}


	//TCP pass through
THREAD_RETURN TCP_Passthrough(THREAD_PARAM id_ptr)
{
	#ifdef DIGI
	unsigned long id = id_ptr;
	#else
	unsigned long id = ((unsigned long)id_ptr);
	#endif
	int len;

		//Loop forever
	while (1)
	{
		if ( Connection[id] >= 0 && Connection_Len[id] == 0 )
		{
				//Read data from the connection
			if ((len = recv(Connection[id], Connection_Buf[id], BUFFER-1, 0)) <= 0)
			{
				CLOSE_SOCKET(Connection[id]);
				Connection[id] = -1;
				Connection_Len[id] = 0;
				#ifndef DIGI
				printf("Closed after recv\n");
				#endif
			}
			else
			{
					//Store the length of data that is in my buffer
				if ( len >= BUFFER )
					len = BUFFER - 1;
				Connection_Len[id] = len;

					//Check if I can just send this data right now
				if ( Connection_Idx == id )
				{
					write(Com_Handle, Connection_Buf[id], Connection_Len[id]);
					Connection_Len[id] = -1;
				}
			}
		}

			//Sleep one second then try to read data again
		SLEEP(TICKS);
	}

	#ifndef DIGI
	return NULL;
	#endif
}

	//Serial Passthrough
THREAD_RETURN Serial_Passthrough(THREAD_PARAM nothing)
{
	char store_buf[STORE_BUF];
	int len;
	int new_len;
	int last_idx = -1;
	char* new_buf;
	int q = 0;
	unsigned long touch_reset = MAX_TOUCHES * CPS - CPS / 10;	//10th of a sec TTL

		//Get any variables I need to user ready
	Recv_Len = 0;

		//Stay in this loop for ever and ever
	while (1)
	{
			//Read data from the DC16 webserver
		if ( (len = read(Com_Handle, store_buf, STORE_BUF - 1)) > 0 )
		{
				//If we are dealing with a new connection, then this is new data
			if ( last_idx != Connection_Idx )
				 Recv_Len = Recv_Match = 0;
			last_idx = Connection_Idx;

				//Copy the store buffer into the recv_buf
			memcpy( Recv_Buf + Recv_Len, store_buf, len );
			Recv_Len += len;
			
				//Output what is being passed around
			#ifndef DIGI
			store_buf[len] = 0;
			printf("%s", store_buf ); fflush(stdout);
			#endif

				//Ensure the there is a valid connection
			if ( Connection[Connection_Idx] >= 0 )
			{
					//Reset the connection watchdog
					//However, keep it very close to exiting since there is basically
					//no time between packets unless the badge is done
				Connection_Touch[Connection_Idx] = touch_reset;

					//I apply this filter since 38400 and 115200 both seem to cause
					// a few bytes of crap at the start of my http/1.1 header?
				if ( !Recv_Match )
				{
					new_len = Recv_Len;
					new_buf = filter( Recv_Buf, &new_len );
	
						//Make sure we filter before we send
					if ( new_len != Recv_Len) Recv_Match = 2;
					Recv_Match = 2;
				}
				else
				{
					new_len = Recv_Len;
					new_buf = Recv_Buf;
				}

						//Check if we should send out data?
				if ( Recv_Match == 2 || Recv_Match && new_len >= BUFFER - STORE_BUF)
				{
					Recv_Match = 1;
					send(Connection[Connection_Idx], new_buf, new_len, 0);
					Recv_Len = 0;
				}
			}
			else
				Recv_Len = 0;
		}
	}

	#ifndef DIGI
	return NULL;
	#endif
}

	//Handle multiple connections
THREAD_RETURN Buffer_Control(THREAD_PARAM nothing)
{
	unsigned long sleep = TICKS / CPS;
	unsigned long touch_max = MAX_TOUCHES * CPS;
	unsigned long reset_max = REBOOT_SECONDS * CPS;
	unsigned long reset_force = FORCE_REBOOT * CPS;
	unsigned long reset_count = 0;
	int connections;
	int i;
	int r;
	
		//Never quit out of this guy
	while (1)
	{
			//Make sure we are still on a valid connection
		connections = 0;
		for ( i = 0; i < TCP_READ_COUNT; i++ )
			if ( Connection[i] >= 0 )
				connections++;

			//If we have any connections to work with, make sure they are fresh
		if ( connections )
		{
				//ensure that the connection isn't stale, if it is, kill it and reset
			if ( Connection[Connection_Idx] >= 0 && 
						++Connection_Touch[Connection_Idx] > touch_max )
			{
					//Send any data left over to the user
				if ( Recv_Match )
				{
					send(Connection[Connection_Idx], Recv_Buf, Recv_Len, 0);
					Recv_Len = 0;
					Recv_Match = 0;
					//SLEEP(sleep);	//Not sure if this is needed but if feels right
				}

					//Close down the connection
				#ifndef DIGI
				printf("Touch maxed out %d for id %d\n",(int)touch_max, Connection_Idx);
				#endif
				CLOSE_SOCKET( Connection[Connection_Idx] );
				Connection[Connection_Idx] = -1;
				connections--;
			}

				//Check that the current connection we have is still valid
			if ( Connection[Connection_Idx] < 0 )
				for ( i = 0; i < TCP_READ_COUNT; i++ )
				{
						//Look for a new slot that is ready to go
					r = (i + Connection_Idx + 1) % TCP_READ_COUNT;
					if ( Connection[r] >= 0 )
					{
						Connection_Idx = r;	
						i = TCP_READ_COUNT;
					}
				}	
		}

			//Increase my reset count and check if its time to reset
		#ifdef AUTO_REBOOT
		reset_count++;
		if ( reset_count >= reset_force || Reboot && !connections )
			customizeReset();	
		if ( reset_count >= reset_max ) 
			Reboot = 1;
		#endif

			//Check if there is data that I should send out
		if ( Connection[Connection_Idx] >= 0 && Connection_Len[Connection_Idx] > 0 )
		{
				//Write out a stored buffer to the webserver
			write(Com_Handle, Connection_Buf[Connection_Idx], 
						Connection_Len[Connection_Idx]);

				//Set that I have sent out this data, now we wait
			Connection_Len[Connection_Idx] = -1;
		}

			//Sleep for one second then recheck for more data
		SLEEP(sleep);
	}

	#ifndef DIGI
	return NULL;
	#endif
}


	//Tcp is down
void applicationTcpDown (void)
{
		static int ticksPassed = 0;
		ticksPassed++;
}


	//Application started
#ifdef DIGI
void applicationStart (void)
#else
int main()
#endif
{
	int count;
	void* stack = NULL;
	int i;

		//Load an ftp server
	#ifdef DIGI
	ftp_init_flash_download();
	#endif

		//Setup all my variables
	Connection_Idx = 0;
	Connection_Last = 0;	//Kinda lame cause this makes me start at 1, not 0
	for ( i = 0; i < TCP_READ_COUNT; i++ )
	{
		Connection[i] = -1;
		Connection_Len[i] = 0;
		Connection_Touch[i] = 0;
	}

		//Load up the com-port first
	count = 0;
	do {
			//Make sure we started
		if ( (Com_Handle = initComPort()) < 0 )
			SLEEP( TICKS );
																																								
	} while ( count++ < MAX_INIT_ATTEMPT && Com_Handle < 0 );

		//Create my tcp socket
	Main_Sock = TCP_Load();

		//Create threads for all my buffers
	for ( i = 0; i < TCP_READ_COUNT; i++ )
	{
			//Create the TCP side pass through
	#ifdef DIGI
		stack = (void*)malloc( THREAD_STACK_SIZE );
		if ( stack != NULL && tx_thread_create(	TCP_Thread + i,
														"TCP Passthrough Thread",
														TCP_Passthrough,
														(unsigned long)i,
														stack,
														THREAD_STACK_SIZE,
														NATranslatePriorityLevel(TCP_PASSTHROUGH_PRIORITY),
														NATranslatePriorityLevel(TCP_PASSTHROUGH_PRIORITY),
														TCP_PASSTHROUGH_SLICE,
														TX_AUTO_START ) != TX_SUCCESS );
	#else
		pthread_create(TCP_Thread + i, NULL, TCP_Passthrough, (void*)i);
	#endif
	}

		//Create the serial side pass through
	#ifdef DIGI
	stack = (void*)malloc( THREAD_STACK_SIZE );
	if ( stack != NULL && tx_thread_create(	&TCP_Con_Thread,
													"TCP Connections Thread",
													TCP_Connections,
													(unsigned long)0,
													stack,
													THREAD_STACK_SIZE,
													NATranslatePriorityLevel(TCP_PASSTHROUGH_PRIORITY),
													NATranslatePriorityLevel(TCP_PASSTHROUGH_PRIORITY),
													TCP_PASSTHROUGH_SLICE,
													TX_AUTO_START ) != TX_SUCCESS );
	#else
		pthread_create(&TCP_Con_Thread, NULL, TCP_Connections, NULL);
	#endif

		//Create the serial side pass through
	if ( Com_Handle >= 0 )
	{
		#ifdef DIGI
		stack = (void*)malloc( THREAD_STACK_SIZE );
		if ( stack != NULL && tx_thread_create(	&Serial_Thread,
														"Serial Passthrough Thread",
														Serial_Passthrough,
														(unsigned long)0,
														stack,
														THREAD_STACK_SIZE,
														NATranslatePriorityLevel(TCP_PASSTHROUGH_PRIORITY),
														NATranslatePriorityLevel(TCP_PASSTHROUGH_PRIORITY),
														TCP_PASSTHROUGH_SLICE,
														TX_AUTO_START ) != TX_SUCCESS );
		#else
			pthread_create(&Serial_Thread, NULL, Serial_Passthrough, NULL);
		#endif
	}

		//Create a buffer monitor
	#ifdef DIGI
	stack = (void*)malloc( THREAD_STACK_SIZE );
	if ( stack != NULL && tx_thread_create(	&Buffer_Thread,
													"Buffer Control Thread",
													Buffer_Control,
													(unsigned long)0,
													stack,
													THREAD_STACK_SIZE,
													NATranslatePriorityLevel(TCP_PASSTHROUGH_PRIORITY),
													NATranslatePriorityLevel(TCP_PASSTHROUGH_PRIORITY),
													TCP_PASSTHROUGH_SLICE,
													TX_AUTO_START ) != TX_SUCCESS );
	#else
		pthread_create(&Buffer_Thread, NULL, Buffer_Control, NULL);
	#endif

	#ifndef DIGI
	printf( "Running\n");
	char bone[] = "GET / SOMETHING\n";
	while(fgetc(stdin) != 'q') 
		write( Com_Handle, bone, sizeof(bone) );
	return 0;
	#endif
}
