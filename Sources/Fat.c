/******************************************************************************
*                                                  
*  (c) copyright Freescale Semiconductor 2008
*  ALL RIGHTS RESERVED
*
*  File Name:   Fat.c
*                                                                          
*  Description: Fat16 lite driver 
*                                                                                     
*  Assembler:   Codewarrior for HC(S)08 V6.1
*                                            
*  Version:     1.1                                                         
*                                                                                                                                                         
*  Author:      Jose Ruiz (SSE Americas)
*                                                                                       
*  Location:    Guadalajara,Mexico                                              
*                                                                                                                  
*                                                  
* UPDATED HISTORY:
*
* REV   YYYY.MM.DD  AUTHOR        DESCRIPTION OF CHANGE
* ---   ----------  ------        --------------------- 
* 1.0   2008.02.18  Jose Ruiz     Initial version
* 1.1   2008.05.02  Jose Ruiz     Initial version
* 
******************************************************************************/                                                                        
/* Freescale  is  not  obligated  to  provide  any  support, upgrades or new */
/* releases  of  the Software. Freescale may make changes to the Software at */
/* any time, without any obligation to notify or provide updated versions of */
/* the  Software  to you. Freescale expressly disclaims any warranty for the */
/* Software.  The  Software is provided as is, without warranty of any kind, */
/* either  express  or  implied,  including, without limitation, the implied */
/* warranties  of  merchantability,  fitness  for  a  particular purpose, or */
/* non-infringement.  You  assume  the entire risk arising out of the use or */
/* performance of the Software, or any systems you design using the software */
/* (if  any).  Nothing  may  be construed as a warranty or representation by */
/* Freescale  that  the  Software  or  any derivative work developed with or */
/* incorporating  the  Software  will  be  free  from  infringement  of  the */
/* intellectual property rights of third parties. In no event will Freescale */
/* be  liable,  whether in contract, tort, or otherwise, for any incidental, */
/* special,  indirect, consequential or punitive damages, including, but not */
/* limited  to,  damages  for  any loss of use, loss of time, inconvenience, */
/* commercial loss, or lost profits, savings, or revenues to the full extent */
/* such  may be disclaimed by law. The Software is not fault tolerant and is */
/* not  designed,  manufactured  or  intended by Freescale for incorporation */
/* into  products intended for use or resale in on-line control equipment in */
/* hazardous, dangerous to life or potentially life-threatening environments */
/* requiring  fail-safe  performance,  such  as  in the operation of nuclear */
/* facilities,  aircraft  navigation  or  communication systems, air traffic */
/* control,  direct  life  support machines or weapons systems, in which the */
/* failure  of  products  could  lead  directly to death, personal injury or */
/* severe  physical  or  environmental  damage  (High  Risk Activities). You */
/* specifically  represent and warrant that you will not use the Software or */
/* any  derivative  work of the Software for High Risk Activities.           */
/* Freescale  and the Freescale logos are registered trademarks of Freescale */
/* Semiconductor Inc.                                                        */ 
/*****************************************************************************/

/****************************************************************************/
/* FAT_LS function modified by J. Grand (Kingpin) for the DEFCON 16 Badge   */
/****************************************************************************/

/****************************************************************************/
/* FAT_LS function changed to FAT_Find by Orbital_sFear											*/
/****************************************************************************/



/* Includes */
#include <string.h>
#include "DC16.h"
#include "Fat.h"

//* File Handlers */
WriteRHandler WHandler;
ReadRHandler RHandler;

/* File Buffers */
UINT8 ag8FATReadBuffer[512];
UINT8 ag8FATWriteBuffer[512];

/* Global Variables */
UINT16 u16FAT_Sector_Size;
UINT16 u16FAT_Cluster_Size;
UINT16 u16FAT_FAT_BASE;
UINT16 u16FAT_Root_BASE;
UINT16 u16FAT_Data_BASE;
UINT16 u16Main_Offset=0;

// global from main.c
extern UINT8 gau8Minicom[MINICOM_BUFFER_SIZE];


/***************************************************************************************/
UINT32 LWordSwap(UINT32 u32DataSwap)
{
    UINT32 u32Temp;
    u32Temp= (u32DataSwap & 0xFF000000) >> 24;
    u32Temp+=(u32DataSwap & 0xFF0000)   >> 8;
    u32Temp+=(u32DataSwap & 0xFF00)     << 8;
    u32Temp+=(u32DataSwap & 0xFF)       << 24;
    return(u32Temp);    
}


/***************************************************************************************/
UINT32 FAT_Open_File_Size(UINT8 func) 
{ 
	if ( func == READ )
		return RHandler.File_Size; 
	else
		return WHandler.File_Size; 
}

/***************************************************************************************/
void FAT_Read_Master_Block(void)
{
    MasterBoot_Entries *pMasterBoot;

    while(ag8FATReadBuffer[0]!= 0xEB && ag8FATReadBuffer[1]!=0x3C && ag8FATReadBuffer[2]!=0x90) // look for Master Boot Record header
    {
        GetPhysicalBlock(u16Main_Offset++,&ag8FATReadBuffer[0]);
    }
    u16Main_Offset--;

    pMasterBoot=(MasterBoot_Entries*)ag8FATReadBuffer;
    u16FAT_Cluster_Size=pMasterBoot->SectorsPerCluster;
    u16FAT_Sector_Size=ByteSwap(pMasterBoot->BytesPerSector);
    u16FAT_FAT_BASE=  u16Main_Offset+ByteSwap(pMasterBoot->ReservedSectors);
    u16FAT_Root_BASE= (ByteSwap(pMasterBoot->SectorsPerFat)<<1)+u16FAT_FAT_BASE;
    u16FAT_Data_BASE= (ByteSwap(pMasterBoot->RootDirectoryEntries) >>4)+u16FAT_Root_BASE;
}
/***************************************************************************************/

/**********/
/*	
		Update my Orbital_sFear.  This function takes a filename and returns 1 if
		that file exists inside my fat table, otherwise it returns 0.  All alpha's
		must be capital.
*/
/**********/
UINT8 FAT_Find(char* filename)
{
	UINT8 u8Counter, len, valid;
	root_Entries *sFileStructure;                                   

		//Read in the fat table and create an array to play with
	GetPhysicalBlock(u16FAT_Root_BASE,ag8FATReadBuffer);
	sFileStructure = (root_Entries*)&ag8FATReadBuffer[RootEntrySize];
	while(sFileStructure->FileName[0]!=FILE_Clear)
	{
			//Got a real file, is it what we want? Lets find out...
		if(sFileStructure->FileName[0]!=FILE_Erased)
		{
				//Copy the filename, while ensuring it matches the filename
				//Note, filename must be >= 11 bytes plus NULL or bad things...
			len = 0;
			valid = 1;
			for ( u8Counter = 0; valid && u8Counter < 8; u8Counter++ )
				if ( sFileStructure->FileName[u8Counter] != ' ' )
				{
					gau8Minicom[len] = sFileStructure->FileName[u8Counter];

						//Ensure Capital letters
					if ( gau8Minicom[len] >= 'a' && gau8Minicom[len] >= 'z' )
						gau8Minicom[len] = gau8Minicom[len] - 'a' + 'A';

						//Make sure this letter matches the requested filename
					if ( filename[len] != gau8Minicom[len] ) valid = 0;
					++len;
				}
             
				//Make sure there is a dot.  How bad ass is that?
			gau8Minicom[len] = '.';
			if ( filename[len] != gau8Minicom[len] )
				valid = 0;
			len++;
          
				//Make sure the extension is the same to
			for ( u8Counter = 0; valid && u8Counter < 3; u8Counter++ )
				if( sFileStructure->Extension[u8Counter] != ' ' )
				{
					gau8Minicom[len] = sFileStructure->Extension[u8Counter];

						//Ensure Capital letters
					if ( gau8Minicom[len] >= 'a' && gau8Minicom[len] >= 'z' )
						gau8Minicom[len] = gau8Minicom[len] - 'a' + 'A';

						//Make sure this letter matches the requested filename
					if ( filename[len] != gau8Minicom[len] ) valid = 0;
					++len;
				}

				//Cap the string with null
			gau8Minicom[len] = 0;

				//If we have had matches all the way, then we have this file quit
			if ( valid )
				return (1);   
		}

			//Move to the next file
		sFileStructure++;    
	}

		//Must have not found the file I wanted
	return 0;
}


/*	Kingpin Update
UINT8 FAT_LS(void)
{
    UINT8 u8Counter, i;
    root_Entries *sFileStructure;                                   

    GetPhysicalBlock(u16FAT_Root_BASE,ag8FATReadBuffer);
    sFileStructure = (root_Entries*)&ag8FATReadBuffer[RootEntrySize];
    
    // look at each file in the root directory to find the first one with only the Read Only attribute set
    while(sFileStructure->FileName[0]!=FILE_Clear)
    {
      if (sFileStructure->FileName[0]!=FILE_Erased) // if the file isn't erased (since FAT doesn't really erase the file, only flag it as erased...)
      {
        i = NibbleSwap(sFileStructure->Attributes);
        if ((i & AT_READONLY) && !(i & (AT_VOLUME | AT_DIRECTORY | AT_HIDDEN | AT_SYSTEM | AT_ARCHIVE))) // the attribute is set
        {
          // copy the filename into the gau8Minicom buffer 
          i = 0; 
          for(u8Counter=0;u8Counter<8;u8Counter++)
            if(sFileStructure->FileName[u8Counter]!=' ')
            {
              gau8Minicom[u8Counter] = sFileStructure->FileName[u8Counter];
              ++i;
            }
             
          gau8Minicom[i] = '.';
          ++i;
          
          for(u8Counter=0;u8Counter<3;u8Counter++)
            if(sFileStructure->Extension[u8Counter]!=' ')
              gau8Minicom[u8Counter + i] = sFileStructure->Extension[u8Counter];
                
          return (1);   
        }
      }
      
      sFileStructure++;
    }
      
    return(0);
}
*/

/*void FAT_LS(void)
{
    UINT8 u8Counter;
    root_Entries *sFileStructure;                                   

    GetPhysicalBlock(u16FAT_Root_BASE,ag8FATReadBuffer);
    sFileStructure = (root_Entries*)&ag8FATReadBuffer[RootEntrySize];
    while(sFileStructure->FileName[0]!=FILE_Clear)
    {
        if(sFileStructure->FileName[0]!=FILE_Erased)
        {
            Terminal_Send_String((UINT8*)"\r\n");
            for(u8Counter=0;u8Counter<8;u8Counter++)
                if(sFileStructure->FileName[u8Counter]!=' ')
                    Terminal_Send_Byte(sFileStructure->FileName[u8Counter]);
            Terminal_Send_Byte('.');
            for(u8Counter=0;u8Counter<3;u8Counter++)
                if(sFileStructure->Extension[u8Counter]!=' ')
                    Terminal_Send_Byte(sFileStructure->Extension[u8Counter]);
        }
        sFileStructure++;    
    }
}*/


/***************************************************************************************/
void FAT_FileClose(void)
{
    root_Entries *sFileStructure;
    UINT16 *pu16FATPointer;
    UINT8 u8Counter;
    UINT32 u32Sector;
    UINT16 u16Offset;
    
    /* Directory Entry*/
    u32Sector=WHandler.Dir_Entry/(u16FAT_Sector_Size>>5);
    u16Offset=WHandler.Dir_Entry%(u16FAT_Sector_Size>>5);
    
    GetPhysicalBlock(u16FAT_Root_BASE+u32Sector,ag8FATReadBuffer);
    sFileStructure=(root_Entries*)ag8FATReadBuffer;
    sFileStructure+=u16Offset;

    // FileName
    for(u8Counter=0;u8Counter<8;u8Counter++)
        sFileStructure->FileName[u8Counter]=WHandler.FileName[u8Counter];

    // Entension
    for(u8Counter=0;u8Counter<3;u8Counter++)
        sFileStructure->Extension[u8Counter]=WHandler.Extension[u8Counter];


    // Attributes
    sFileStructure->Attributes=0x20;
    sFileStructure->_Case=0x18;
    sFileStructure->MiliSeconds=0xC6;
    
    // Date & Time Information
    sFileStructure->CreationTime=0x2008;
    sFileStructure->CreationDate=0x2136;
    sFileStructure->AccessDate=0x2136;
    sFileStructure->ModificationTime=0x2008;
    sFileStructure->ModificationDate=0x2136;
    
    // Fat entry and file Size
    sFileStructure->ClusterNumber=ByteSwap(WHandler.BaseFatEntry);
    
    sFileStructure->SizeofFile=LWordSwap(WHandler.File_Size); 

    StorePhysicalBLock(u16FAT_Root_BASE+u32Sector,ag8FATReadBuffer)
    
    /* FAT Table */
    u32Sector=WHandler.CurrentFatEntry/(u16FAT_Sector_Size>>1);
    u16Offset=WHandler.CurrentFatEntry%(u16FAT_Sector_Size>>1);

    GetPhysicalBlock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer);
    
    pu16FATPointer=(UINT16*)ag8FATReadBuffer;
    pu16FATPointer+=u16Offset;
    *pu16FATPointer=0xFFFF;     // Write Final Cluster    

    StorePhysicalBLock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer)
}

/***************************************************************************************/
UINT16 FAT_SearchAvailableFAT(UINT16 u16CurrentFAT)
{
    UINT16 *pu16DataPointer;
    UINT16 u16FatEntry=0;
    UINT16 u16Sector=0;
    UINT16 u16byteSector;
    
    u16Sector=u16FAT_FAT_BASE;
    while(u16Sector < (((u16FAT_Root_BASE-u16FAT_FAT_BASE)>>1)+u16Main_Offset))
    {        GetPhysicalBlock(u16Sector++,ag8FATReadBuffer);
        pu16DataPointer=(UINT16*)ag8FATReadBuffer;
        u16byteSector=0;
        
        while(u16byteSector<u16FAT_Sector_Size)
        {
            if(*pu16DataPointer==0x0000)
                if(u16FatEntry!=u16CurrentFAT)
                    return(u16FatEntry);
            pu16DataPointer++;
            u16FatEntry++;
            u16byteSector++;
        }
    }
    return(0);  // Return 0 if no more FAT positions available
}

/***************************************************************************************/
UINT16 FAT_Entry(UINT16 u16FatEntry,UINT16 u16FatValue, UINT8 u8Function)
{
    UINT16 *pu16DataPointer;
    
    UINT16 u16Block;
    UINT8 u8Offset;
    
    u16Block = u16FatEntry / (u16FAT_Sector_Size>>1);
    u8Offset = (UINT8)(u16FatEntry % (u16FAT_Sector_Size >>1));

    GetPhysicalBlock(u16FAT_FAT_BASE+u16Block,ag8FATReadBuffer);
    pu16DataPointer=(UINT16*)ag8FATReadBuffer;
    pu16DataPointer+=u8Offset;

    if(u8Function==NEXT_ENTRY)
        return(ByteSwap(*pu16DataPointer));
    
    if(u8Function==WRITE_ENTRY)
    {
        *pu16DataPointer=ByteSwap(u16FatValue);
        StorePhysicalBLock(u16FAT_FAT_BASE+u16Block,ag8FATReadBuffer);
        return(0x00);
    }
                
}

/***************************************************************************************/
void FAT_FileWrite(UINT8 *pu8DataPointer,UINT32 u32Size)
{
    UINT32 u32SectorToWrite;
    UINT8 *pu8ArrayPointer;
    UINT16 u16TempFat;
    UINT8  u8ChangeSector=1;

    while(u32Size)
    {
        if(u8ChangeSector)
        {
            u32SectorToWrite= u16FAT_Data_BASE + WHandler.ClusterIndex + (WHandler.CurrentFatEntry-2)*u16FAT_Cluster_Size;
            GetPhysicalBlock(u32SectorToWrite,ag8FATWriteBuffer); 
            pu8ArrayPointer=ag8FATWriteBuffer+WHandler.SectorIndex;
            u8ChangeSector=0;
        }
        
        while(WHandler.SectorIndex<u16FAT_Sector_Size  &&  u32Size)
        {
            u32Size--;    
            WHandler.SectorIndex++;
            WHandler.File_Size++;
            *pu8ArrayPointer++=*pu8DataPointer++;    
        }
        
        StorePhysicalBLock(u32SectorToWrite,ag8FATWriteBuffer);     // Write Buffer to Sector
    
        /* Check Sector Size */
        if(WHandler.SectorIndex == u16FAT_Sector_Size)
        {
            WHandler.SectorIndex=0;
            WHandler.ClusterIndex++;    
            u8ChangeSector=1;
        }
    
        /* Check Cluster Size */
        if(WHandler.ClusterIndex == u16FAT_Cluster_Size)
        {
            //_BGND;
            WHandler.ClusterIndex=0;
            u16TempFat=FAT_SearchAvailableFAT(WHandler.CurrentFatEntry);   
            (void)FAT_Entry(WHandler.CurrentFatEntry,u16TempFat,WRITE_ENTRY);
            WHandler.CurrentFatEntry=u16TempFat;
            u8ChangeSector=1;
        }
    }
}

/***************************************************************************************/
UINT16 FAT_FileRead(UINT8 *pu8UserBuffer)
{
    UINT32 u32SectorToRead; 
    UINT16 u16BufferSize;

    if(RHandler.File_Size==0)
        return(0);
    
    
    u32SectorToRead= u16FAT_Data_BASE + ((RHandler.FAT_Entry-2)*u16FAT_Cluster_Size)+RHandler.SectorOffset;
    GetPhysicalBlock(u32SectorToRead,pu8UserBuffer);
    

    if(RHandler.File_Size > u16FAT_Sector_Size)
    {
        RHandler.File_Size-=u16FAT_Sector_Size;
        u16BufferSize=512;
    }
    else
    {
        u16BufferSize=(UINT16)RHandler.File_Size;
        RHandler.File_Size=0;
    }
    
    if(RHandler.SectorOffset < (u16FAT_Cluster_Size)-1)
        RHandler.SectorOffset++;        
    else
    {
            RHandler.SectorOffset=0;
            RHandler.FAT_Entry = FAT_Entry(RHandler.FAT_Entry,0,NEXT_ENTRY); // Get Next FAT Entry
    }
    return(u16BufferSize);    
}



/***************************************************************************************/
void FAT_FileNameOrganizer(UINT8 *pu8FileName,UINT8 *pu8Destiny)
{
    UINT8 u8Counter=0;    
    
    while(u8Counter<12)
    {
        if(*pu8FileName != '.')
            *pu8Destiny++=*pu8FileName++;
        else
        {
            if(u8Counter<8)
                *pu8Destiny++=0x20;
            else
                pu8FileName++;    
        }
        u8Counter++;
    }
}

/***************************************************************************************/
UINT8 FAT_FileOpen(UINT8 *pu8FileName,UINT8 u8Function)
{
    
    UINT16 u16Temporal;
    UINT8  u8FileName[11];
    UINT8  u8Counter=0;
    UINT8  u8Flag=False;
    UINT16 u16Index;
    UINT16 u16Block;
    UINT16 u16BlockNum=u16FAT_Data_BASE-u16FAT_Root_BASE;
    UINT8  u8ErrorCode=ERROR_IDLE;
    UINT8  *pu8Pointer;
    root_Entries *sFileStructure;                                   
    
    FAT_FileNameOrganizer(pu8FileName,&u8FileName[0]);
    
    u16Block=0;
    
    while(u16Block < u16BlockNum && u8ErrorCode==ERROR_IDLE)
    {
    
        GetPhysicalBlock(u16FAT_Root_BASE+u16Block,ag8FATReadBuffer);
        sFileStructure = (root_Entries*)ag8FATReadBuffer;

        u16Index=0;
        while(u16Index<u16FAT_Sector_Size && u8ErrorCode==ERROR_IDLE)    
        {
            /* If Read or Modify Function */
            if(u8Function==READ || u8Function==MODIFY)
            {
                if(sFileStructure->FileName[0]==FILE_Clear) 
                    u8ErrorCode=FILE_NOT_FOUND;
        
                if(sFileStructure->FileName[0] == u8FileName[0])
                {
                    u8Flag=True;
                    u8Counter=0;
                    while(u8Flag==True && u8Counter < 10)
                    {
                        u8Counter++;
                        if(sFileStructure->FileName[u8Counter] != u8FileName[u8Counter])
                            u8Flag=False;    
                    }
                    if(u8Flag==True)
                    {
                        /* If Read Function */
                        if(u8Function==READ)
                        {
                            RHandler.Dir_Entry=(u16Block*RootEntrySize)+((u16Index)/RootEntrySize);
                            RHandler.File_Size=LWordSwap(sFileStructure->SizeofFile);
                            RHandler.FAT_Entry=ByteSwap(sFileStructure->ClusterNumber);
                            RHandler.SectorOffset=0;
                            u8ErrorCode=FILE_FOUND;
                        } 
                        /* If Modify Function */
                        else
                        {
                            pu8Pointer=WHandler.FileName;
                            for(u8Counter=0;u8Counter<11;u8Counter++)
                                *pu8Pointer++=u8FileName[u8Counter];
                            WHandler.Dir_Entry=(u16Block*RootEntrySize)+((u16Index)/RootEntrySize);
                            WHandler.File_Size=LWordSwap(sFileStructure->SizeofFile);
                            WHandler.BaseFatEntry=ByteSwap(sFileStructure->ClusterNumber);
                            
                            if(WHandler.BaseFatEntry != 0)
                            {
                                u16Temporal=WHandler.BaseFatEntry;
                                do
                                {
                                    WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                                    WHandler.BaseFatEntry=FAT_Entry(WHandler.CurrentFatEntry,0,NEXT_ENTRY);
                                }while(WHandler.BaseFatEntry!=0xFFFF);
                                WHandler.BaseFatEntry=u16Temporal;
                            } 
                            else
                            {
                                WHandler.BaseFatEntry=FAT_SearchAvailableFAT(0);
                                WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                            }
                            
                            u16Temporal=(UINT16)WHandler.File_Size % (u16FAT_Sector_Size<<4);
                            WHandler.ClusterIndex= u16Temporal/u16FAT_Sector_Size;
                            WHandler.SectorIndex=  u16Temporal%u16FAT_Sector_Size;
                            u8ErrorCode=FILE_FOUND;
                        }
                    }
                }
            }

            /* If Write function */
            if(u8Function==CREATE)
            {
                if(sFileStructure->FileName[0]==FILE_Clear || sFileStructure->FileName[0]==FILE_Erased) 
                {
                    
                    pu8Pointer=WHandler.FileName;
                    for(u8Counter=0;u8Counter<11;u8Counter++)
                        *pu8Pointer++=u8FileName[u8Counter];

                    WHandler.Dir_Entry=(u16Block*RootEntrySize)+((u16Index)/RootEntrySize);
                    WHandler.File_Size=0;
                    WHandler.BaseFatEntry=FAT_SearchAvailableFAT(0);
                    WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                    WHandler.ClusterIndex=0;
                    WHandler.SectorIndex=0;
        
                    if(WHandler.BaseFatEntry)
                        u8ErrorCode=FILE_CREATE_OK;
                    else
                        u8ErrorCode=NO_FAT_ENTRY_AVAIlABLE;
                }
            }
            sFileStructure++;
            u16Index+=RootEntrySize;
        }
        u16Block++;
    }
    if(u16BlockNum==u16Block)
        u8ErrorCode=NO_FILE_ENTRY_AVAILABLE;
    
    return(u8ErrorCode);

}
