/*
 *
 *     Module Name: naftpapp.c
 *         Version: 1.00
 *   Original Date: 01/23/01
 *          Author: Paul Jacobson
 *        Language: Ansi C
 * Compile Options:
 * Compile defines:
 *       Libraries:
 *    Link Options:
 *
 *    Entry Points:
 *
 *  Copyrighted (c) by NETsilicon, Inc.  All Rights Reserved.
 *
 * Description.
 * =======================================================================
 *  This is the RAM based FTP Server
 *
 *
 *
 *  Edit Date/Ver   Edit Description
 *  =============   =======================================================
 *  04/16/02  DXL   Inform user to check flash if NAFlashSectorSizes returns error
 *  05/01/02  JW    Removed argument from NAFlashSectors()
 *  05/02/02  JW    Changed sectorSizes to (unsigned long*) from (unsigned int*)
 *  05/22/02  JW    Increased buffer size to 128K for optimal 16 or 32 bit mode
 *                  performance
 *  06/17/02  JW    Added error handling for images larger than flash memory
 *  06/18/02  JW    Use #define constant for buffer size
 *  07/01/02  JW    Added more error handling for images larger than flash memory
 *  07/03/02  JW    Modified algorithm in ftp_flash_stor() to support any positive
 *                  non-zero BUFFERSIZE value.
 *  07/08/02  JW    Added error handling to allow re-downloading images should
 *                  NAFlashWrite() fail.
 *                  
 */

#include <stdio.h>
#include <stdlib.h>
#include "tx_api.h"
#include "naflash.h"
#include <Flash.h>
#include "sockapi.h"
#include "fservapi.h"
#include "sysAccess.h"
#include "bsp_api.h"
#include "reg_def.h"
#include "narmled.h"
#ifdef BSP_ARM9
#include "cs.h"
#include "gpiomux_def.h"
#endif


/*
 * Definitions and Declarations. 
 */
#define BUFFERSIZE    128 * 1024
#define DELAY_BEFORE_RESET      (10*NABspTicksPerSecond)
#define NOT_USED        0


static char sectorDatap[BUFFERSIZE];
static int  successful = 0;
static char errorlog[40];
static int  currentLength = 0;
static unsigned int  currentSector = 0;
static unsigned int  writeIndex = 0;
static int sectors = 0;
static unsigned long *sectorSizes;
static unsigned int  romsize;
static int flashFull = 0;

TX_TIMER resetTimer;

/*
 *
 *  Function: void reset(ULONG notUsed)
 *
 *  Description:
 *
 *      This function is used to reset the system after new firmware has been
 *      written to flash.  It is schedule to run from a timer after the
 *      flash download completes.  It runs off of a timer to give the FTP
 *      server time to shut down connections gracefully.
 *
 *  Parameters:
 *
 *      notUsed     parameter passed by ThreadX which isn't used
 *
 *  Return Values:
 *
 *      does not return
 *
 */

static void reset(ULONG notUsed)
{
    NA_UNUSED_PARAM(notUsed);
    customizeReset();   /* reset*/
}



/*
 * ftp_flash_stor
 *
 * This routine is the handler for the FTP server STOR command.
 * This routine collects 32K worth of image in g_flashbuf and then
 * writes flash memory at the current sector.
 *
 * INPUT
 *
 *    bufferp    Contains data read by the FTP server.
 *    buflen     Specifies length of bufferp data.
 *    argp       Specifies file name being read.
 *    handle     Handle to FTP server client connection object. 
 */ 
static int ftp_flash_stor (char *bufferp, int buflen, char *argp, unsigned long handle)
{
    int templen, k;
    int result;
    
    /*  our protocol is to verify they are trying to download rom.bin   */
    if ((strcmp (argp, "rom.bin") == 0) || (strcmp (argp, "spi_rom.bin") == 0) 
       || (strcmp (argp, "romzip.bin") == 0) || (strcmp (argp, "image.bin") 
       == 0))
    {
        if ((strcmp (argp, "image.bin") == 0) && (currentSector == 0))
        {
           writeIndex = NAAppOffsetInFlash;
           for (k = currentSector; k < sectors; k++)
           {
              if (sectorSizes[k] <= writeIndex)
              {
                 writeIndex -= sectorSizes[k];
                 if (k == (sectors - 1))
                 {
                     /* No more flash memory to write, so set flag */
                     flashFull = 1;
                 }
              }
              else
              {
                 currentSector = k;
                 break;
              }
           }
           if (writeIndex != 0)
           {
                printf ("_NAAppOffsetInFlash in the linker customizatino file is set incorrectly.  \n");
                printf ("The application image must start on a sector boundary.\n");
                successful = 0;
                return (-2);
           }
        }

        if (flashFull)
        {
          /* If all flash memory has been written,
             clear flag and return -2 */
          printf("Image size exceeds flash memory size, and flash memory has been overwritten.\n");
          successful = 0;
          return (-2);
        }
        if ((currentLength + buflen) > romsize)
        {
          /* If data received exceeds available flash memory,
             clear flag and return -2 */
          printf("Image size exceeds flash memory size, but flash memory has not been overwritten.\n");
          successful = 0;
          return (-2);
        }
        /*  copy this file into our buffer  */
        if (currentLength + buflen > BUFFERSIZE)
        {
            templen = BUFFERSIZE - currentLength;
        } 
        else
        {
            templen = buflen;
        }

        memcpy ((char *)&sectorDatap[currentLength], bufferp, templen);

        currentLength += templen;
	
        /* Write the data to Flash in 128K chunks */
        /* Smaller data chunks decrease performance but require a smaller buffer */
        while (currentLength == BUFFERSIZE)
        {
            /* Write to a valid Flash sector */
            if (currentSector < sectors)
            {
               result = NAFlashWrite(currentSector, writeIndex, BUFFERSIZE, sectorDatap, ERASE_AS_NEEDED);
               if (result)
               {
                   printf ("ftp_flash_stor NAFlashWrite() error %d.\n", result);
                   sprintf (errorlog, "FLASH WRITE ERR:%2d\n", result);
                   successful = 0;
                   return result;
               }

	       /*  save the extra data not written to flash -- for the next NAFlashWrite() */
               currentLength = 0;
               if (buflen > templen)
               {
                  if ((buflen - templen) >= BUFFERSIZE)
                  {
                     memcpy ((char *)&sectorDatap[currentLength], (char *)&bufferp[templen], BUFFERSIZE);
                     templen += BUFFERSIZE;
                     currentLength = BUFFERSIZE;
                  }
                  else
                  {
                     memcpy ((char *)&sectorDatap[currentLength], (char *)&bufferp[templen], buflen - templen);
                     currentLength = buflen - templen;
                  }
               }		
               writeIndex += BUFFERSIZE;

               /* Compute the sector and sector offset to start writing next */
               for (k = currentSector; k < sectors; k++)
               {
                  if (sectorSizes[k] <= writeIndex)
                  {
                     writeIndex -= sectorSizes[k];
                     if (k == (sectors - 1))
                     {
                         /* No more flash memory to write, so set flag */
                         flashFull = 1;
                     }
                  }
                  else
                  {
                     currentSector = k;
                     break;
                  }
               }
            }
        }
        successful = 1;
    }
    return 0;
}


/* 
 * ftp_flash_retr
 *
 * This routine is for client to get the ID of the ftp server.
 *
 * INPUTS
 *
 *    argp       Points to string containing file to be created at client.
 *    handle     Used to access FTP server client connection object. 
 *
 */
static int ftp_flash_retr (char *argp, unsigned long handle)
{
    char buffer[64];

    /*  initialize the flag */
    successful = 0;

    if (strcmp (argp, "id") == 0)
    {
        errorlog[0]='\0';

        if (romsize == 0x100000)
        {
           sprintf(buffer, "NETARM-FTPDL-V100");
        }
        else if (romsize == 0x200000)
        {
           sprintf(buffer, "NETARM-FTPDL-V200");
        }
        else
        {
           sprintf(buffer, "NETARM-FTPDL-V400");
        }
    }
    else if (strcmp (argp, "errmsg") == 0)
    {
        sprintf (buffer, errorlog);
    }

    send (FSHandleToSocket(handle), buffer, strlen(buffer), 0);

    return 0;
}

/* 
 * ftp_flash_run
 *
 * This routine is used run the rom image after a successful download
 *
 * INPUTS
 *
 *    argp       Points to string containing file to be created at client.
 *    handle     Used to access FTP server client connection object. 
 *
 */
static int ftp_flash_run (char *argp, unsigned long handle)
{
    int result;

    if (successful)  /* the whole rom.bin is downloaded successfully */
    {
        if (currentLength)
        {
            /* Write to a valid Flash sector */
            if (currentSector < sectors)
            {
               result = NAFlashWrite(currentSector, writeIndex, currentLength, sectorDatap, ERASE_AS_NEEDED);
               if (result)
               {
                   printf ("ftp_flash_run NAFlashWrite() error %d.\n", result);
                   sprintf (errorlog, "FLASH WRITE ERR:%2d\n", result);

                   /* Reset variables to allow downloading again */
                   currentSector = 0;
                   writeIndex = 0;
                   currentLength = 0;
                   flashFull = 0;
                   successful = 0;
                   return result;
               }
            }
        }

        printf ("FTP: Flash download complete.\n" );
        if (tx_timer_create (&resetTimer, "Reset Timer", reset, NOT_USED, DELAY_BEFORE_RESET, NOT_USED, TX_AUTO_ACTIVATE) == TX_SUCCESS)
        {
            printf ("Resetting system in %d seconds.\n", (int)(DELAY_BEFORE_RESET/NABspTicksPerSecond));
        }
        else
        {
            printf ("Unable to automatically reset system.\n");
        }

    }
    /* Reset variables to allow downloading again */
    currentSector = 0;
    writeIndex = 0;
    currentLength = 0;
    flashFull = 0;
    successful = 0;
    return 0;
}

static int app_syst_value (char * fsParameters, char * fsUserData, unsigned long handle)
{
    NA_UNUSED_PARAM (handle);
    NA_UNUSED_PARAM (fsParameters);

    sprintf (fsUserData, "NAFTPAPP SYST response string for NET+OS");

    return 0;
}


/*
 *
 *  Function: void ftp_init_flash_download (void)
 *
 *  Description:
 *
 *      This routine setups up the RAM based FTP Server.
 *
 *  Parameters:
 *
 *      none
 *
 *  Return Values:
 *
 *      none
 *
 */
void ftp_init_flash_download (void)

{
    int result;

    /*
     * Initialize the FTP server internal data structures. The following
     * allocates control data structures for two concurrent users.
     */
    FSInitialize (2); 
    FSRegisterSTOR (ftp_flash_stor);
    FSRegisterRETR (ftp_flash_retr);
    FSRegisterControlClose (ftp_flash_run);

    result = FSRegisterSYST (app_syst_value);
    if (result != NAFTPS_SUCCESS)
    {
        printf ("FSRegisterSYST failed to register an override to default.\n");
    }

    /*  Add Username (none) with no password to the System Access Database   */
    NAsetSysAccess (NASYSACC_ADD, "(none)", "", NASYSACC_LEVEL_RW, NULL);

    /* determine rom size here, via Mask */
    /*romsize = physical_size ((*(unsigned long *)0xffc00014 & 0xfffff000));*/

#ifdef BSP_ARM9
	romsize = customizeGetCSSize ((WORD32 *)NA_SCM_CS1_BASE_ADDR, (WORD32 *)NA_SCM_CS1_BASE_ADDR);
#else
	romsize = customizeGetCSSize (NARM_CSAR0_ADDR, NARM_CSOR0_ADDR);
#endif

    /* determine the flash physical sectors and sector sizes */
    sectors = NAFlashSectors();
    sectorSizes = (unsigned long *)malloc(sizeof(unsigned long) * sectors);
    if (NAFlashSectorSizes(sectorSizes))
    {
       printf ("FTP Server not started\n");
       printf ("Unable to get Flash sector size\n");
       printf ("Check if flash is enabled\n");
       return;
    }

    
    /*  Actually start the FTP server.  */ 
    FSStartServer ();

    printf ("RAM based FTP Server ready.\n");

    NALedBlinkGreen (2, 1);
}
