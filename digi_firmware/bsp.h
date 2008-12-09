/*
 *
 *     Module Name: bsp.h
 *	       Version: 1.00
 *   Original Date: 08/18/99	
 *	        Author: Contact NetSilicon technical support if you have questions on this file.
 *	      Language: Ansi C
 * Compile Options:
 * Compile defines:
 *	     Libraries:
 *    Link Options:
 *
 *    Entry Points:
 *
 *  Copyrighted (c) by NETsilicon, Inc.  All Rights Reserved.
 *
 * Description.
 * =======================================================================
 *  This file defines BSP wide constants.
 *
 *
 *
 * Edit Date/Ver   Edit Description
 * ==============  =======================================================
 *
 */

#ifndef BSP_H
#define BSP_H

#include "Npttypes.h"
#include "bsp_api.h"
#include "tx_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*ad*
 * Set this manifest constant to TRUE if the Ethernet driver should use
 * the LED indicated by LED_ETHERNET to indicate when it is receiving 
 * and transmitting packets.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_WANT_ETHERNET_ACTIVITY_LED      TRUE

 
/*ad*
 * Constant that determines the format configuration settings in NVRAM 
 * for NET+OS development boards     
 *
 * NET+OS 5.0 and 5.1 users can   
 * set BSP_SERIAL_NUMBER_FORMAT to BSP_SERIAL_NUMBER_FORMAT_500
 * to use the serial number format used in 
 * NET+OS 5.0 and 5.1. 
 * The serial number is stored as eight digits.  The Ethernet
 * MAC address is calculated based on the last six digits.
 *
 *
 * New NET+OS 6.0 users should  
 * set BSP_SERIAL_NUMBER_FORMAT to BSP_SERIAL_NUMBER_FORMAT_600 to use the NET+OS 6.0
 * serial number format.  This format stores a serial number with
 * eight digits and one alpha character.  The Ethernet MAC address
 * is stored in a seperate field and is completely indpendent
 * of the serial number
 *
 * @category BSP_Configuration
 * @since 6.0
 * @name SerialNumberFormats    "Serial Number Formats"
 * @external
 * @see @link BSP_SERIAL_NUMBER_FORMAT
 *
 * @param BSP_SERIAL_NUMBER_FORMAT_500  Use NET+OS 5.0/5.1 format serial numbers and generate
 *                                      Ethernet MAC address from last 6 digits of serial number.
 * @param BSP_SERIAL_NUMBER_FORMAT_600  Use NET+OS 6.0 format serial numbers.  EThernet 
 *                                      MAC address will be stored seperately in NVRAM.
*ad*/
#define BSP_SERIAL_NUMBER_FORMAT_500        0
#define BSP_SERIAL_NUMBER_FORMAT_600        1


/*ad*
 * Constant that determines the format configuration settings in NVRAM
 * for NET+OS development boards    Set this value to 
 * either BSP_NVRAM_FORMAT_500 or BSP_NVRAM_FORMAT_600.
 *
 * @category BSP_Configuration
 * @since 6.0
 *
 * @external
 * @see @link SerialNumberFormats
*ad*/
#define BSP_SERIAL_NUMBER_FORMAT            BSP_SERIAL_NUMBER_FORMAT_600



/*ad*
 * BSP_TICKS_PER_SECOND determines the system clock rate.
 *
 * @since 1.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_TICKS_PER_SECOND        100                 /* 100 Hz clock*/



/*ad*
 * Set BSP_SERIAL_PORT_1 and BSP_SERIAL_PORT_2 to one of these values to
 * indicate what type of serial driver should be built for the port.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @name SerialPortConfigurations   "Serial Port Configurations"
 * @see @link BSP_SERIAL_PORT_1
 * @see @link BSP_SERIAL_PORT_2
 *
 * @param BSP_SERIAL_NO_DRIVER      Serial port will not be used
 * @param BSP_SERIAL_UART_DRIVER    Serial port will be used in standard UART mode
 * @param BSP_SERIAL_SPI_DRIVER     Serial port will be used in SPI mode
 * @param BSP_SERIAL_HDLC_DRIVER    Serial port will be used in HDLC mode
*ad*/
#define BSP_SERIAL_NO_DRIVER        0
#define BSP_SERIAL_UART_DRIVER      1
#define BSP_SERIAL_SPI_DRIVER       2
#define BSP_SERIAL_HDLC_DRIVER      3


/*ad*
 * Set BSP_SERIAL_PORT_API to one of these values to indicate which api will 
 * be used when BSP_SERIAL_UART_DRIVER is the driver selected for a port.  The
 * termios API is the newest, more featureful API and is recommended for use.
 * The legacy interface is deprecated and will eventually not be supported.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @name SerialPortApis             "Serial Port APIs"
 * @see @link BSP_SERIAL_PORT_API
 *
 * @param BSP_SERIAL_API_TERMIOS	The driver uses the termios interface
 * @param BSP_SERIAL_API_LEGACY		The driver uses the legacy NetOS interface
*ad*/
#define BSP_SERIAL_API_TERMIOS	0
#define BSP_SERIAL_API_LEGACY	1


/*ad*
 * This constant determines the serial API used in the system when 
 * BSP_SERIAL_UART_DRIVER is the driver selected for a port.  
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @index BSP_SERIAL_PORT_API
 * @see @link SerialPortApis
*ad*/
#define BSP_SERIAL_PORT_API	BSP_SERIAL_API_TERMIOS

/*ad*
 * This constant determines whether the serial driver uses the internal fast
 * interrupt and TIMER2 to provide better servicing characteristics of the 
 * FIFO at high baud rates.  It is recommended that this be set to TRUE if
 * the port will be run at 115200 baud or higher.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @index BSP_SERIAL_FAST_INTERRUPT
*ad*/
#define BSP_SERIAL_FAST_INTERRUPT FALSE	

/*ad*
 * This constant controls which serial driver is built in
 * the BSP for serial port 1.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @index BSP_SERIAL_PORT_1
 * @see @link SerialPortConfigurations
*ad*/
#define BSP_SERIAL_PORT_1       BSP_SERIAL_UART_DRIVER

/*ad*
 * This constant controls which serial driver is built in
 * the BSP for serial port 2.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @see @link SerialPortConfigurations
*ad*/
#define BSP_SERIAL_PORT_2       BSP_SERIAL_NO_DRIVER

/*ad*
 * Set BSP_SPI_PORT_API to one of these values to indicate which api will 
 * be used when BSP_SERIAL_SPI_DRIVER is the driver selected for a port.  The
 * simple SPI API is the newest, more flexible API and is recommended for use.
 * The legacy interface is deprecated and will eventually not be supported.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
 * @name SpiPortApis             "SPI Port APIs"
 * @see @link BSP_SPI_PORT_API
 *
 * @param BSP_SPI_API_SIMPLE		The driver uses the simplified interface
 * @param BSP_SPI_API_LEGACY		The driver uses the legacy NetOS interface
*ad*/
#define BSP_SPI_API_SIMPLE	0
#define BSP_SPI_API_LEGACY	1


/*ad*
 * This constant determines the SPI API used in the system when 
 * BSP_SERIAL_SPI_DRIVER is the driver selected for a port.  
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
 * @see @link SpiPortApis
*ad*/
#define BSP_SPI_PORT_API	BSP_SPI_API_LEGACY


/*ad*
 * Set this constant to TRUE to build the IEEE-1284 parallel port driver.  This
 * driver can only be used on the NET+50 revision D development board.
 *
 * @since 1.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_INCLUDE_PARALLEL_DRIVER     FALSE

/*ad*
 * Set this constant to TRUE to include the NUL driver.
 *
 * @since 1.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_INCLUDE_NUL_DRIVER          TRUE

/*ad*
 * Set this constant to TRUE to include the loopback driver.
 *
 * @since 1.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_INCLUDE_LOOPBACK_DRIVER     TRUE

/*ad*
 * Set this constant to TRUE to include the UPD debug driver.
 *
 * @since 1.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_INCLUDE_UDP_DEBUG_DRIVER    TRUE


/*ad*
 * Sets this constant to TRUE to include the I2C_EXPANDER APIs in
 * the functional name based GPIO API functions. Not supported 
 * for ARM9 based processors.
 *
 * @since 6.3
 * @external
 * @category BSP:BSP_Configuration
*ad*/

#define BSP_INCLUDE_I2C_EXPANDER            FALSE





/*ad*
 * Set this constant to TRUE to add the PPP devices into the TCP/IP
 * stack's table of devices.  Set it to FALSE if you do not intend
 * to use PPP.
 *
 * @category BSP_Configuration
 * @since 6.0
 *
 * @external
 * @see @link PPP
*ad*/
#define BSP_WANT_PPP                        TRUE



/*ad*
 * These manifest constants determine the ThreadX priority levels for
 * various server processes created by the API libraries.  ThreadX
 * priority levels range from 0 to 31 with 0 being the highest.  Threads
 * with greater priority can preempt threads
 * with lower priority.
 *
 * @since 1.0
 * @external
 * @category BSP_Configuration
 * @name ThreadPriorityLevels           "Thread Priority Levels"
 *
 * @param BSP_HIGH_PRIORITY             priority level for threads that need to be able to preempt 
 *                                      most other threads
 * @param BSP_MEDIUM_PRIORITY           normal priority level for threads
 * @param BSP_LOW_PRIORITY              priority level for threads that should run at a low 
 *                                      priority level (and be preemptable by most other threads)
 * @param BSP_DEFAULT_PRIORITY          default priority level for threads
 * @param BSP_FTP_SERVER_PRIORITY       priority level for FTP server thread
 * @param BSP_SNMP_AGENT_PRIORITY       priority level for SNMP agent thread
 * @param BSP_TELNET_SERVER_PRIORITY    priority level for Telnet server thread
 * @param BSP_SNTP_THREAD_PRIORITY      priority level for SNTP client thread
*ad*/
#define BSP_HIGH_PRIORITY               8
#define BSP_MEDIUM_PRIORITY             16
#define BSP_LOW_PRIORITY                24
#define BSP_DEFAULT_PRIORITY            BSP_MEDIUM_PRIORITY
#define BSP_FTP_SERVER_PRIORITY         BSP_MEDIUM_PRIORITY
#define BSP_SNMP_AGENT_PRIORITY         BSP_MEDIUM_PRIORITY
#define BSP_TELNET_SERVER_PRIORITY      BSP_MEDIUM_PRIORITY
#define BSP_SNTP_THREAD_PRIORITY        BSP_MEDIUM_PRIORITY



/*ad*
 * This constant sets the UDP port on the UDP debug device.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_UDP_DEBUG_PORT              12345


/*ad*
 * BSP_NVRAM_DRIVER must be set to one of these constants to determine
 * which driver should be built as part of the BSP.  The constants 
 * select between different types of physical memory used for NVRAM.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @see @link BSP_NVRAM_DRIVER
 * @name NvramDriverConfigurations      "NVRAM Driver Configurations"
 *
 * @param BSP_NVRAM_NONE                do not build an NVRAM driver
 * @param BSP_NVRAM_EEPROM              NVRAM is implemented in an EEPROM attached to CS3
 * @param BSP_NVRAM_LAST_FLASH_SECTOR   the last sector of flash is used for NVRAM
 * @param BSP_NVRAM_SEEPROM             NVRAM is implemented using a serial EEPROM
 * @param BSP_NVRAM_SEEPROM_WITH_SEMAPHORES NVRAM is implemented using a serial EEPROM and access
 *                                      to it should be protected with semaphores
 * @param BSP_NVRAM_CUSTOM              platform specific versions of the functions
 *                                      declared in narmnvrm.h will be used
*ad*/
#define BSP_NVRAM_NONE                      0
#define BSP_NVRAM_EEPROM                    1
#define BSP_NVRAM_LAST_FLASH_SECTOR         2
#define BSP_NVRAM_SEEPROM                   3
#define BSP_NVRAM_SEEPROM_WITH_SEMAPHORES   4
#define BSP_NVRAM_CUSTOM                    255


/*ad*
 * This constant determines which NVRAM driver will be built.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @index BSP_NVRAM_DRIVER
 * @see @link NvramDriverConfigurations
 *
 * @note    Applications that run directly out of flash memory cannot use the
 *          flash for NVRAM.  BSP_NVRAM_DRIVER must not be set to 
 *          BSP_NVRAM_LAST_FLASH_SECTOR for these applications.
*ad*/
#define BSP_NVRAM_DRIVER    BSP_NVRAM_LAST_FLASH_SECTOR



/*ad*
 * If an EEPROM exists, the BSP expects it to be connected to CS3.  
 * At power up, the BSP will configure CS3 to support an EEPROM
 * if BSP_NVRAM_DRIVER is set to BSP_NVRAM_EEPROM.  This constant
 * determines where the part should be mapped to in memory.
 *
 * @since 3.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_EEPROM_BASE     0x3000000

/*ad*
 * If an EEPROM exists, the BSP expects it to be connected to CS3.  
 * At power up, the BSP will configure CS3 to support an EEPROM
 * if BSP_NVRAM_DRIVER is set to BSP_NVRAM_EEPROM.  This constant
 * determines the size of the part.
 *
 * @since 3.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_EEPROM_SIZE     0x2000


/*ad*
 * BSP_RAM_BASE sets address of RAM.  
 *
 * @since 3.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_RAM_BASE 0x00000000

/*ad*
 * BSP_ROM_BASE sets base address of ROM.
 *
 * @since 1.0
 * @external
 * @category BSP_Configuration
*ad*/

#define BSP_ROM_BASE 0x02000000

/*ad*
 * BSP_FLASH_BANKS sets the number of flash memory banks.  The number of
 * flash banks is the number of flash chips located in memory where one
 * chip follows another in the address map.
 * For example, 2 16-bit chips wired up as 16-bit flash would count as 2 
 * Flash banks.  2 16-bit chips wired up as 32-bit flash (in parallel) 
 * would count as 1 bank.  4 16-bit chips wired up as 32-bit flash would
 * count as 2 banks (2 pairs of chips where the pairs follow each other in
 * the address map).
 *
 * @since 5.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_FLASH_BANKS  1


/*ad*
 *  BSP_STARTUP_DELAY defines number of additional seconds the system 
 *  will delay before starting the TCP/IP stack and the application.
 *  The delay is used to receive network traffic.  The Ethernet statistics
 *  are used to generate a random seed for the TCP/IP stack.  The stack
 *  uses the random seed to generate unique sequence numbers for 
 *  connections.  This is necessary to ensure that new connections will 
 *  not collide with old connections that may have
 *  been open when the unit was reset.
 *
 * @note    You must specify a non-zero delay if your platform uses the
 *          NS7520 since it does not have a random number generator.
 *
 * @since 5.1
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_STARTUP_DELAY       2


/*ad*
 * This mask must be anded with the address mask written to CSOR registers to
 * setup the chip select to support standard NET+OS caching.  Doing this causes
 * the memory attached to the CS to appear in 4 different places in the 
 * address space.  The cache controller is then configured to set different
 * cache options for each of the regions.
 *
 * @since 3.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_CACHE_MASK      0xf3ffffff

/*ad*
 * If BSP_AUTOMATICALLY_ENABLE_CACHE is set, then the BSP startup code will
 * enable cache if it is present on the processor.  This constant is useful
 * if you will (or may in the future) run your firmware on different chips
 * that have or don't have cache.  Setting it TRUE will cause the BSP to 
 * turn cache on if it detects it in the chip.  Setting it FALSE will cause
 * it leave cache off whether the chip supports it or not.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_AUTOMATICALLY_ENABLE_CACHE  TRUE


/*ad*
 * If BSP_BUILD_CACHE_DRIVER is set, then the cache driver will be built
 * as part of the BSP library.  Otherwise only stub routines will be built.
 * Use this option to reduce the BSP code size if you are sure you do not
 * want to use cache.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_BUILD_CACHE_DRIVER          TRUE


/*ad*
 * Set this constant to TRUE to include the Real Time Clock driver.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_INCLUDE_RTC_DRIVER          0

/*ad*
 * Set this to TRUE to run the system POST tests during startup.
 *
 * @since 7.0
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_POST_TEST          FALSE

/*ad*
 * Set this to TRUE to run the dialog code during startup.
 *
 * @since 7.0
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_ENABLE_DIALOG          TRUE

/*ad*
 * Set this constant to TRUE to have the BSP initialize the file system
 * according to configuration values in startfilesystem.c, and enable
 * the C Library file I/O functions as well as the file system's native
 * API.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_INCLUDE_FILESYSTEM_FOR_CLIBRARY          FALSE

/*ad*
 * When the BSP creates a native file system volume, this constant specifies the
 * percentage of the maximum number of inode blocks that can be allocated to
 * store inodes for a volume.  This constant allows specifying the upper limit of
 * the number of blocks reserved to store inodes.  Valid values are from 1 to 100.
 * Refer to NAFSinit_volume_cb for more information.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
 * @see @link NAFSinit_volume_cb
*ad*/
#define BSP_NATIVE_FS_INODE_BLOCK_LIMIT              100

/*ad*
 * When the BSP creates a native file system volume, this constant specifies the
 * maximum number of open directories that the file system will track.  A directory
 * is considered open if there are open files in the directory.  Valid values are
 * from 1 to 64.  Refer to NAFSinit_volume_cb for more information.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
 * @see @link NAFSinit_volume_cb
*ad*/
#define BSP_NATIVE_FS_MAX_OPEN_DIRS                  20

/*ad*
 * When the BSP creates a native file system volume, this constant specifies the
 * the maximum number of open files per directory that the file system will track.  
 * Valid values are from 1 to 64.  Refer to NAFSinit_volume_cb for more information.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
 * @see @link NAFSinit_volume_cb
*ad*/
#define BSP_NATIVE_FS_MAX_OPEN_FILES_PER_DIR         20

/*ad*
 * When the BSP creates a native file system volume, this constant specifies the
 * block size used for the volume.  Valid values are:
 *
 * <list type = "unordered">
 *      <item> NAFS_BLOCK_SIZE_512 </item>
 *      <item> NAFS_BLOCK_SIZE_1K </item>
 *      <item> NAFS_BLOCK_SIZE_2K </item>
 *      <item> NAFS_BLOCK_SIZE_4K </item>
 * </list>
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_NATIVE_FS_BLOCK_SIZE                     NAFS_BLOCK_SIZE_512

/*ad*
 * When the BSP creates the native file system RAM volume, this constant specifies the
 * size of the RAM volume in bytes.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_NATIVE_FS_RAM0_VOLUME_SIZE               2 * 1024 * 1024

/*ad*
 * When the BSP creates the native file system Flash volume, this constant specifies the
 * advanced options to use.  Valid values are the bitwise ORing of the following:
 *
 * <list type = "unordered">
 *      <item> NAFS_MOST_DIRTY_SECTOR </item>
 *      <item> NAFS_RANDOM_DIRTY_SECTOR </item>
 *      <item> NAFS_TRACK_SECTOR_ERASES </item>
 *      <item> NAFS_BACKGROUND_COMPACTING </item>
 * </list>
 *
 * Refer to NAFSinit_volume_cb for more information.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
 * @see @link NAFSinit_volume_cb
*ad*/
#define BSP_NATIVE_FS_FLASH0_OPTIONS                 NAFS_MOST_DIRTY_SECTOR | NAFS_TRACK_SECTOR_ERASES

/*ad*
 * If the BSP_NATIVE_FS_FLASH0_OPTIONS constant includes NAFS_BACKGROUND_COMPACTING, then this
 * constant specifies the percentage of erased blocks in a flash sector to gain in order
 * to trigger the sector compacting process.  Valid values are from 1 to 100.  Refer to
 * NAFSinit_volume_cb for more information.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
 * @see @link NAFSinit_volume_cb
*ad*/
#define BSP_NATIVE_FS_FLASH0_COMPACTING_THRESHOLD    100
    
/*ad*
 * BSP_WATCHDOG_TYPE must be set to one of these constants.  
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @see @link BSP_WATCHDOG_TYPE
 * @name WatchdogDriverConfigurations   "Watchdog Driver Configurations"
 *
 * @param BSP_WATCHDOG_EXTERNAL Set BSP_WATCHDOG_TYPE to this value to build the watchdog driver
 *                              to support an external watchdog timer chip
 * @param BSP_WATCHDOG_INTERNAL Set BSP_WATCHDOG_TYPE to this value to build the watchdog driver
 *                              to support the internal watchdog timer circuit on NETARM chip
*ad*/
#define BSP_WATCHDOG_EXTERNAL   0
#define BSP_WATCHDOG_INTERNAL   1

/*ad*
 * This constant determines whether the watchdog driver will support the
 * internal watchdog circuit, or an external watchdog timer.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @see @link WatchdogDriverConfigurations
*ad*/
#define BSP_WATCHDOG_TYPE          BSP_WATCHDOG_INTERNAL

/*ad*
 * Set BSP_HANDLE_FATAL_ERROR to this value to trigger a reset when fatal
 * errors are encountered.
 *
 * Set BSP_HANDLE_UNEXPECTED_EXCEPTION to this value to trigger a reset
 * when an unexpected exception occurs.
 *
 * @since 6.0
 * @external
 * @category BSP:BSP_Configuration
 * @see @link BSP_HANDLE_FATAL_ERROR
 * @see @link BSP_HANDLE_UNEXPECTED_EXCEPTION
*ad*/
#define BSP_ERROR_RESET_UNIT        0

/*ad*
 * Set BSP_HANDLE_UNEXPECTED_EXCEPTION to this value to cause the error
 * handler to report the error.
 *
 * @since 6.0
 * @external
 * @category BSP:BSP_Configuration
 * @see @link BSP_HANDLE_FATAL_ERROR
 * @see @link BSP_HANDLE_UNEXPECTED_EXCEPTION
*ad*/
#define BSP_ERROR_REPORT_ERROR      1

/*ad*
 * Set BSP_HANDLE_UNEXPECTED_EXCEPTION to this value to cause the exception
 * handler to attempt to continue execution when an unexpected exception
 * is encountered.  This is not recommended.
 *
 * @since 6.0
 * @external
 * @category BSP:BSP_Configuration
 * @see @link BSP_HANDLE_FATAL_ERROR
 * @see @link BSP_HANDLE_UNEXPECTED_EXCEPTION
*ad*/
#define BSP_ERROR_CONTINUE          2

/*ad*
 * This constant determines how fatal errors are handled.  The possible values for
 * BSP_HANDLE_FATAL_ERROR are BSP_ERROR_RESET_UNIT, and BSP_ERROR_REPORT_ERROR.
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @see @link ErrorHandlerConfigurations
*ad*/
#define BSP_HANDLE_FATAL_ERROR              BSP_ERROR_REPORT_ERROR

/*ad*
 * Determines how unexpected exceptions are handled  The possible
 * values for this directive are BSP_ERROR_RESET_UNIT,
 * BSP_ERROR_REPORT_ERROR, and BSP_ERROR_CONTINUE.
 *
 *
 * @since 6.0
 * @external
 * @category BSP_Configuration
 * @see @link ErrorHandlerConfigurations
*ad*/
#define BSP_HANDLE_UNEXPECTED_EXCEPTION     BSP_ERROR_REPORT_ERROR

    

/*ad*
 *      Determines the clock source to be used  This directive is  
 *      the address input to the SYSCLK signal multiplexer.  The SYSCLK 
 *      has two possible sources: one from the TTL clock input applied 
 *      to the XTAL1 pin and the other from the crystal oscillator and 
 *      phase lock loop (PLL) circuit.
 *
 *      When BSP_CLOCK_SOURCE = 0 = SELECT_THE_XTAL1_INPUT
 *          SYSCLK uses the XTAL1 its source.
 *
 *      When BSP_CLOCK_SOURCE = 1 = SELECT_THE_CRYSTAL_OSCILLATOR_INPUT
 *          SYSCLK uses the crystal ocsillator and PLL as its source.
 *
 *
 * @since 5.1
 * @external
 * @category BSP_Configuration
*ad*/
#define BSP_CLOCK_SOURCE     SELECT_THE_CRYSTAL_OSCILLATOR_INPUT  

/*ad*
 * Set this constant TRUE to inform NETOS startup process to setup SNTP for retreiving time. 
 * 
 * @since 7.0
 * @external
 * @category BSP:BSP_Configuration
 *
 * @note This constant must be TRUE and APP_USE_NETWORK_TIME_PROTOCOL must be defined in the application to enable SNTP.
*ad*/
#define BSP_INCLUDE_SNTP    FALSE

/*ad*
 * This is used to define a callback function for SNTP. BSP initializes
 * SNTP with this callback definition.
 *
 * @since 6.0
 * @external
 * @category SNTP
 *
 * @note BSP_INCLUDE_SNTP must be TRUE and APP_USE_NETWORK_TIME_PROTOCOL must be defined in the application to enable SNTP.
*ad*/
#define BSP_SNTP_CALLBACK    NULL

/*ad*
 * This constant determines whether the BSP waits for the stack to be configured before
 * starting the application by calling the applicationStart() function.  Previous versions
 * of NET+OS always waited for the stack to be configured.
 *
 * Your application should not use any network resources until the stack has been
 * configured by setting an IP address on at least one interface.  You can use the
 * customizeAceGetInterfaceAddrInfo() function to determine if an IP address has
 * been assigned to an interface.
 *
 * Set BSP_WAIT_FOR_IP_CONFIG to TRUE cause the BSP to wait for an IP address to be
 * configured on at least one interface before calling applicationStart.
 *
 * Set BSP_WAIT_FOR_IP_CONFIG to FALSE to call applicationStart without waiting for
 * an IP address to be assigned.
 *
 * @since 6.1
 * @external
 * @category BSP:BSP_Configuration
 * @see @link customizeAceGetInterfaceAddrInfo
*ad*/
#define BSP_WAIT_FOR_IP_CONFIG  TRUE

/*ad*
 * This constant determines how the Ethernet link is monitored. 
 * Set BSP_USE_PHY_INTERRUPT to TRUE to use PHY interupt to minitor the Ethernet link.
 * Set BSP_USE_PHY_INTERRUPT to FALSE to use 500ms ThreadX timer to minitor the link.
 *
 * BSP_USE_PHY_INTERRUPT should be set to TRUE only on platforms that support PHY interupt.
 * ns7520_a does not support PHY interrupt in NET+OS 6.0.
 *
 * @since 6.0
 * @external
 * @category BSP:BSP_Configuration:EthernetDriver
*ad*/
#define BSP_USE_PHY_INTERRUPT   FALSE

/*ad*
 * This constant determines how the TCP/IP stack implements its critical section. 
 * Set BSP_LOW_INTERRUPT_LATENCY to TRUE to use a semaphome to for the TCP/IP critical section.
 * Set BSP_LOW_INTERRUPT_LATENCY to FALSE to disable processor interrupts to implement 
 * the TCP/IP critical section.
 *
 * @since 6.1
 * @external
 * @category general_purpose
*ad*/
#define BSP_LOW_INTERRUPT_LATENCY   FALSE

#if (BSP_LOW_INTERRUPT_LATENCY == FALSE)
/*ad*
 * This constant enables Fast IP. 
 * Set BSP_ENABLE_FAST_IP to TRUE to enable Fast IP.
 * Set BSP_ENABLE_FAST_IP to FALSE to disable Fast IP.
 * Fast IP is not supported for Low Interrupt Latency.
 *
 * @since 6.1
 * @external
 * @category general_purpose
 *
 * @see @link fastIP
*ad*/
#define BSP_ENABLE_FAST_IP  TRUE    
#else
#define BSP_ENABLE_FAST_IP  FALSE
#endif   

#if (BSP_ENABLE_FAST_IP == TRUE) && (BSP_LOW_INTERRUPT_LATENCY == TRUE)
    #error  Fast IP is not supported in Low Interrupt Latancy mode.
#endif


/*ad*
 * This constant enables IP address conflict detection, during initial IP address configuration.
 * If BSP_ENABLE_ADDR_CONFLICT_DETECTION is defined to TRUE, ACE subsystem sends ARP probes 
 * to detect IP address conflict for BOOTP, RARP, Ping ARP, and static IP address protocols.
 *
 * IP address conflict detection must also be enabled on a network device. The device 
 * configuration for IP address conflict detection can be retrieved by NAGetAddrConflictData
 * function.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_ENABLE_ADDR_CONFLICT_DETECTION          TRUE


/*ad*
 * This constant specifies default number of ARP probes, sent to detect IP address 
 * conflict on Ethernet. Used for BOOTP, RARP, Ping ARP, and static IP address protocols.
 *
 * An ARP probe is an ARP request with source protocol address equal to 0.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_DEFAULT_ETH_NUM_ADDR_PROBES             4

/*ad*
 * This constant specifies an interval in milliseconds between ARP probes, sent to detect IP 
 * address conflict on Ethernet. Used for BOOTP, RARP, Ping ARP, and static IP address protocols.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_DEFAULT_ETH_PROBE_INTERVAL_MS           200

/*ad*
 * This constant specifies default number of ARP address announcements, sent on Ethenet, after 
 * the IP address has been configured. Used for BOOTP, RARP, Ping ARP, and static IP address 
 * protocols.
 *
 * An ARP announcement is an ARP request with both source and destination protocol address 
 * equal the interface IP address.
 * 
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_DEFAULT_ETH_NUM_ADDR_ANNOUNCEMENTS      2

/*ad*
 * This constant specifies an interval in milliseconds between ARP address announcements, sent 
 * after the IP address has been configured. Used for BOOTP, RARP, Ping ARP, and static IP address 
 * protocols.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_DEFAULT_ETH_ANNOUNCE_INTERVAL_MS        2000

/*ad*
 * This constant specifies maximum number of IP asddress conflicts during IP address 
 * configuration. Reaching BSP_MAX_ADDR_CONFLICTS for the virtual interface causes ACE 
 * to be stopped on this interface.
 *
 * @since 6.1
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_MAX_ADDR_CONFLICTS                      3


/*ad*
 * Set this constant to TRUE to enable support for other protocols besides
 * TCP/IP.
 *
 * If this constant is set to FALSE, then the Ethernet driver will pass up
 * all received packets to the TCP/IP stack.  This is the recommended setting
 * for all TCP/IP applications.
 *
 * Set this constant to TRUE if your application needs to support other 
 * protocols.  This will enable code in the Ethernet driver that examines the
 * protocol ID of each received packet to determine which protocol it is 
 * addressed to.  The Ethernet driver will send packets to the protocol stack
 * that has been registered to receive them with @link NAEthAddPacketType.
 * TCP/IP is automatically registered to receive IP, ARP, and RARP packets.
 *
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration:EthernetDriver
 * @see @link NAEthAddPacketType
 * @see @link BSP_WANT_2ND_ETHERNET_CHECKSUM
*ad*/
#define BSP_ENABLE_ETHERNET_PACKET_TYPES        FALSE

/*ad*
 * Set this constant to TRUE to force the Ethernet driver to perform a second
 * CRC checksum check of incoming packets.  
 *
 * Under very rare conditions, the NET50 and NS7520 chips will corrupt data in
 * Ethernet receive packets.  This error is not detected by the Ethernet H/W.
 * See the errata on these parts for more information.  Set BSP_WANT_2ND_ETHERNET_CHECKSUM
 * to TRUE to have the Ethernet driver check received packets by performing a second 
 * CRC check.  The driver will discard packets that fail the CRC check.
 *
 * Note that UDP and TCP both perform their own checksums on received data, so this
 * check is NOT necessary for TCP/IP based applications.
 *
 * This constant is only valid for NET50 and NS7520 platforms.  It is ignored on all
 * others.
 *
 * If @link BSP_ENABLE_ETHERNET_PACKET_TYPES and BSP_WANT_2ND_ETHERNET_CHECKSUM are 
 * both set, the Ethernet driver will determine whether it should checksum packets 
 * by determining which protocol the packet is addressed to and then examining the 
 * want2ndCrcCheck field of the @link ethPacketType structure passed to @link 
 * NAEthAddPacketType when the packet type and protocol were registered.  
 * If @link BSP_ENABLE_ETHERNET_PACKET_TYPES is not set, but BSP_WANT_2ND_ETHERNET_CHECKSUM
 * is, then the Ethernet driver will perform a CRC checksum on all received packets.
 *
 * @note    There is a performance penalty when this feature is enabled.
 *          You do not need to enable this feature if your application only uses
 *          TCP/IP since TCP and UDP both perform checksums of incoming packets.
 * 
 * @since 6.2
 * @external
 * @category BSP:BSP_Configuration:EthernetDriver
 * @see @link NAEthAddPacketType
 * @see @link BSP_ENABLE_ETHERNET_PACKET_TYPES
*ad*/
#define BSP_WANT_2ND_ETHERNET_CHECKSUM       FALSE


/*ad*
 * Set this constant to TRUE to queue received Ethernet TCP/IP packets and 
 * pass them up to the TCP/IP stack in a lower priority thread. The priority of
 * the receive queue thread is determined by the BSP_ETH_RECV_QUEUE_THREAD_PRIORITY.
 *
 * This mode is recommended when another protocol stack is running in parallel 
 * with TCP/IP and the bypass packets need higher priority over TCP/IP packets.
 *
 * @since 6.3
 * @external
 * @category BSP:BSP_Configuration:EthernetDriver:EthernetBypass
 * @see @link NAEthRegisterCustomPacketFn
 * @see @link NAEthAddPacketType
*ad*/
#define BSP_USE_ETHERNET_RECV_QUEUE  FALSE

/*ad*
 * This constant defines the priority of the Ethernet receive queue thread,
 * if BSP_USE_ETHERNET_RECV_QUEUE is set to TRUE. 
 *
 * @since 6.3
 * @external
 * @category BSP:BSP_Configuration:EthernetDriver:EthernetBypass
*ad*/
#define BSP_ETHERNET_RECV_QUEUE_THREAD_PRIORITY  9

/*ad*
 * Set this constant to TRUE to enable the Ethernet transmit bypass of the TCP/IP stack. 
 * Bypass packets are sent using the NAEthBypassTransmit routine. 
 *
 * To avoid dependancy on TCP/IP stack recources for bypass packets, different queues are used
 * for bypass and TCP/IP packets. Total number of transmit DMA buffers is split to be used by
 * the Ethernet bypass and the TCP/IP stack. 
 *
 * The BSP_ETHERNET_BYPASS_SEND_BUFFERS number of DMA buffers is used for bypass. 
 * The rest of the DMA buffers - 1 are used for TCP/IP. To calculate the actual number of packets 
 * in DMA used for bypass, or TCP/IP divide the number buffers by the maximum number of chained 
 * packets (see eth.h).
 *
 * All bypass packets, submitted to the driver, above the once that fit into BSP_ETHERNET_BYPASS_SEND_BUFFERS,
 * are queued.
 *
 * @since 6.3
 * @external
 * @category BSP:BSP_Configuration:EthernetDriver:EthernetBypass
*ad*/
#define BSP_ENABLE_ETHERNET_BYPASS            FALSE

/*ad*
 * The number of Ethernet transmit DMA buffer descriptors, allocated for bypass packets,
 * if BSP_ENABLE_ETHERNET_BYPASS is enabled.
 *
 * @since 6.3
 * @external
 * @category BSP:BSP_Configuration:EthernetDriver:EthernetBypass
 * @see @link NAEthBypassTransmit
*ad*/
#define BSP_ETHERNET_BYPASS_SEND_BUFFERS       54


/*ad*
 * This structure is used to hold configuration settings for NET+ARM chip
 * selects.  The table NANetarmInitData is an array of NAInitRegisterType structures.
 * The values stored in the table are used by the routines in ncc_init.c that
 * determine the memory type and size, and configure the chip selects to support
 * it. 
 *
 * @since 4.0
 * @external
 * @category BSP_Configuration
 * @see @link NANetarmInitData
 *
 * @param usesInternalOscillator    Flag that signifies if Internal Oscillator is used.
 *                                  When non-zero PLLTST_SELECT is set to  
 *                                  SELECT_THE_CRYSTAL_OSCILLATOR_INPUT otherwise, 
 *                                  PLLTST_SELECT is set to  SELECT_THE_XTAL1_INPUT 
 * @param cs0WaitStates             CS0 (Flash) wait states (legal range 0-f)
 * @param cs0BusCycles              CS0 (Flash) bcyc value (legal range 0-3)
 * @param cs0bsize                  CS0 (Flash) bsize value (legal range 0-3)
 * @param cs0UseRsync               CS0 (Flash) bsize value (legal range 0-3)
 *                                  For NS7520 this field is OE field of chip 
 *                                  select option register A. (legal range 0,1)
 * @param cs0AddRegBurst            CS0 (Flash) burst enable (legal range 0,1)
 * @param cs1EdoMode                CS1 (Fast Page) EDO Mode setting (legal range 0,1)
 * @param cs1IdleBit                CS1 Idle bit flag -- sets the CS1 idle bit (legal range 0,1)  
 * @param cs1EdoWaitStates          CS1 (EDORAM) wait states (legal range 0-f)
 * @param cs1EdoBusCycles           CS1 (EDORAM) bcyc value (legal range 0-3)
 * @param cs1Edobsize               CS1 (EDORAM) bsize value (legal range 0-3)
 * @param cs1SdWaitStates           CS1 (SDRAM) wait states (legal range 0-f)
 * @param cs1SdBusCycles            CS1 (SDRAM) bcyc value (legal range 0-3)
 * @param cs1Sdbsize                CS1 (SDRAM) bsize value (legal range 0-3)
 * @param cs3WaitStates             CS3 (NVRAM) wait states (legal range 0-f)
*ad*/
typedef struct
{
    const unsigned int usesInternalOscillator;
    const unsigned int cs0WaitStates;
    const unsigned int cs0BusCycles;
    const unsigned int cs0bsize;
    const unsigned int cs0UseRsync;
    const unsigned int cs0AddRegBurst;
    const unsigned int cs1EdoMode;
    const unsigned int cs1IdleBit;
    const unsigned int cs1EdoWaitStates;
    const unsigned int cs1EdoBusCycles;
    const unsigned int cs1Edobsize;
    const unsigned int cs1SdWaitStates;
    const unsigned int cs1SdBusCycles;
    const unsigned int cs1Sdbsize;
    const unsigned int cs3WaitStates;
} NAInitRegisterType;

extern const NAInitRegisterType NANetarmInitData[];

extern unsigned const NAInterruptPriority[];

extern const unsigned int NAOptRegWaitAndBurstMask;

extern const unsigned int NAOptRegRsyncMask;

extern unsigned int NAusesTheInternalOscillator(void);


/*
 * Error check the serial port selections
 */
#if (BSP_SERIAL_PORT_2 != BSP_SERIAL_NO_DRIVER)
    #if (BSP_SERIAL_PORT_1 == BSP_SERIAL_NO_DRIVER)
        #error  Serial port 2 cannot be used without serial port 1
    #endif
#endif

/*ad*
 * This constant controls if the HDLC driver is to be enabled
 *
 * @since 7.0
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_ENABLE_HDLC  TRUE 

/*ad*
 * Set BSP_ENI_HOST_DRIVER this value as the Host
 * ENI driver. 
 *
 * @since 3.0
 * @external
 * @category ENI
*ad*/
#define BSP_ENI_HOST_DRIVER             1

/*ad*
 * Set BSP_ENI_CLIENT_DRIVER this value as the Client
 * ENI driver.
 *
 * @since 3.0
 * @external
 * @category ENI
*ad*/
#define BSP_ENI_CLIENT_DRIVER           2

/*ad*
 * Set BSP_ENI_PORT to this value to configure the ENI port to be 
 * a client or host. ENI port is defined as Client or Host driver
 *
 * @since 3.0
 * @external
 * @category ENI
*ad*/
#define BSP_ENI_PORT					BSP_ENI_HOST_DRIVER

/*ad*
 * Bootloader flash recovery using TFTP mode (default value)
 *
 * @since 6.3
 * @include 	<bsp.h>
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_BOOTLOADER_TFTP_RECOVERY	0x0001

/*ad*
 * Bootloader flash recovery using serial mode. The default serial recovery uses
 * XModem communication protocol
 *
 * @since 6.3
 * @include 	<bsp.h>
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_BOOTLOADER_SERIAL_RECOVERY 	0x0002

/*ad*
 * Try the all flash recovery modes in the following order
 *
 * <list type="unordered"> 
 *  <item> TFTP </item>
 *  <item> Serial </item>
 * </list>
 *
 * @since 6.3
 * @include 	<bsp.h>
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_BOOTLOADER_TRY_ALL   	(BSP_BOOTLOADER_TFTP_RECOVERY | BSP_BOOTLOADER_SERIAL_RECOVERY)

/*ad*
 * Bootloader flash recovery mode. The default is set to BSP_BOOTLOADER_TRY_ALL
 *
 * @since 6.3
 * @include 	<bsp.h>
 * @external
 * @category BSP:BSP_Configuration
*ad*/
#define BSP_BOOTLOADER_RECOVERY_METHOD  BSP_BOOTLOADER_TRY_ALL

/*ad*
 * ENABLE_ADDP_SERVER is used to activate the ADDP server.
 * 
 * Set ENABLE_ADDP_SERVER to FALSE to disable the ADDP server.
 *
 * @external
 * @category ADDPConfiguration
 * @since 6.2
 *ad*/

#define ENABLE_ADDP_SERVER TRUE

/*ad*
 * This structure defines the internals of the GUID data structure.
 *
 * @since 6.2
 * @external
 * @category ADDPConfiguration
 * @param Data1	Specifies the first 8 hexadecimal digits of the GUID. 
 * @param Data2	Specifies the first group of 4 hexadecimal digits of the GUID. 
 * @param Data3	Specifies the second group of 4 hexadecimal digits of the GUID. 
 * @param Data4	Array of eight elements. The first two elements contain the third group of 4 hexadecimal digits of the GUID. The remaining six elements contain the final 12 hexadecimal digits of the GUID. 
 *
*ad*/
typedef struct _GUID{
   unsigned long	Data1;
   unsigned short	Data2;
   unsigned short	Data3;
   unsigned char	Data4[8];
} GUID, UUID;

/*ad*
 * ADDP_OEM_GUID Is the unique GUID assigned to this user.  This value is used
 * by ADDP to communicate with devices made by this user.  Currently set to 
 * Netsilicon GUID.
 * 
 * This value should be modified by the user to reflect their own GUID.
 * The GUID value is broken up based on the GUID structure defined above.
 *
 * @external
 * @category ADDPConfiguration
 * @since 6.2
 *ad*/

#define ADDP_OEM_GUID  {0xbf6db409, 0xc83d, 0x44a3, {0xa3, 0x6d, 0x21, 0x79, 0x7d, 0x2f, 0x73, 0xf9}}

/*ad*
 * ADDP_COOKIE Is the magic cookie.  This value is used by ADDP to communicate
 * with devices made by this user.  Currently set to DVKT.
 * 
 * This value should match the value defined in the ADDP client library.
 *
 * @external
 * @category ADDPConfiguration
 * @since 6.2
 *ad*/
#define ADDP_COOKIE	0x44564B54

/*ad*
 * ADDP_HARDWARE_NAME Is the name for the hardware.  
 * 
 * This value should be modified by the user to reflect their hardware name.
 *
 * @external
 * @category ADDPConfiguration
 * @since 6.2
 *ad*/

#define ADDP_HARDWARE_NAME	"Unknown"

/*ad*
 * ADDP_FEP_REV Is the FEP revision value.
 * 
 * The value should be modified by the user to represent the FEP revision for 
 * this application.
 *
 * @external
 * @category ADDPConfiguration
 * @since 6.2
 *ad*/

#define ADDP_FEP_REV		"Unknown"

/*ad*
 * ADDP_HARDWARE_REV Is the hardware revision of the device.
 * 
 * The value should be modified by the user to represent the hardware revision
 * for this device.
 *
 * @external
 * @category ADDPConfiguration
 * @since 6.2
 *ad*/

#define ADDP_HARDWARE_REV	1

#ifdef __cplusplus
}
#endif

#endif


