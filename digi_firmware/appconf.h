/*
 *            Copyright (c) 1998-2003 by NETsilicon Inc.
 *
 *  This software is copyrighted by and is the sole property of 
 *  NETsilicon.  All rights, title, ownership, or other interests
 *  in the software remain the property of NETsilicon.  This
 *  software may only be used in accordance with the corresponding
 *  license agreement.  Any unauthorized use, duplication, transmission,  
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice may not be removed or modified without prior
 *  written consent of NETsilicon.
 *
 *  NETsilicon, reserves the right to modify this software
 *  without notice.
 *
 *  NETsilicon
 *  411 Waverley Oaks Road                  USA 781.647.1234
 *  Suite 304                               http://www.netsilicon.com
 *  Waltham, MA 02452                       AmericaSales@netsilicon.com
 *
 *************************************************************************
 *
 *  Description
 *  ====================================================================
 *  This file is one of two files, bsp.h and appconf.h, that contain configuration
 *  settings.  The file bsp.h contains configuration settings that are usually
 *  platform specific.  This file contains configuration settings that may be
 *  different from application to application, even if the applications run on
 *  the same platform.
 *
 */

#ifndef appconf_h

#define appconf_h

#include <stdio.h>
#include <stdlib.h>
#include "bsp.h"

/*ad*
 * Set to run POST at start up.
 *
 * Set APP_POST to TRUE if you want the BSP to run the Power On Self Test
 * at startup.  Set APP_POST to FALSE to skip the POST.
 *
 * @external
 * @category ApplicationConfiguration
 * @since 1.0
*ad*/
extern const int APP_POST;


/*ad*
 * Set to initialize the C++ runtime environment at start up.
 *
 * APP_CPP determines whether or not the application is a C++ based application.  If the
 * application is C++ based, set APP_CPP to TRUE, otherwise set to FALSE or leave undefined.
 *
 * @external
 * @category ApplicationConfiguration
 * @since 1.0
*ad*/
#define APP_CPP                 FALSE


/*ad*
 * APP_ERROR_HANDLER defines the user error handler for fatal errors.  It should either be set
 * to NULL, or set to the name of a routine that should be called if a fatal error occurs.  The
 * routine should be of the form:
 *
 *      void error_handler (char *errorDescription, int errorCode, int errorSubcode)
 *
 * If APP_ERROR_HANDLER is NULL, then the default error handler will be used.  This error handler
 * blinks the LEDs on the development board in an infinite loop.  The LEDs are blinked in a pattern
 * that indicates the errorType.  
 *
 * @external
 * @category ApplicationConfiguration
 * @since 1.0
 * @deprecated  6.0
*ad*/
#define APP_ERROR_HANDLER           NULL


/*ad*
 * APP_ROOT_STACK_STACK sets the size of the root thread's stack.  The root thread is the
 * thread that calls calls applicationStart.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_ROOT_STACK_SIZE         8192

/*ad*
 * APP_ROOT_PRIORITY sets the priority level for the root thread.  The root thread is the
 * thread that calls calls applicationStart.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_ROOT_PRIORITY           16


/*ad*
 * APP_DEFAULT_API_PRIORITY determines the priority level threads internal to the
 * NET+Works APIs will be use.  The global variable bsp_default_api_priority is
 * loaded with this value.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_DEFAULT_API_PRIORITY    (NATranslatePriorityLevel(naDefaultPriority))


/*ad*
 * APP_FILE_SYSTEM_SIZE determines the numbers of file the Advanced Web server(AWS)
 * will allocated for file system. The default value is 9, the number of connections 
 * The AWS alocated. Increasing APP_FILE_SYSTEM_SIZE will reduce memory available. 
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
#define APP_FILE_SYSTEM_SIZE          9


/*ad*
 * APP_DIALOG_PORT determines whether or not the initialization code will prompt for configuration
 * parameters.  It should be set to a string that contains the serial port to prompt with.  If it
 * is not set, or if it contains an invalid port, then the initialization code will not prompt for
 * configuration parameters.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
//#define APP_DIALOG_PORT             "/com/0" 
/*ad*
 * APP_DIALOG_APP_NAME determines the name of the current application program.  This name
 * is displayed at the top of the configuration dialog.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_DIALOG_APP_NAME         "Digi Firmware $Revision: 1.8 $"


/*ad*
 * APP_ROOT_PASSWORD determines the root account password.  This account has all privileges and
 * can be used for any service requiring a login (i.e. FTP, telnet).  The password set here is
 * a default value that is used if the rootPassword field in NVRAM is empty.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
#define APP_ROOT_PASSWORD           "Netsilicon"


/*ad*
 * APP_ADMINISTRATORS_ACCOUNT is a username used for a secure gateway account.  This account has 
 * privileges only for a gateway (so it can not be used for insecure applications like FTP or
 * Telnet).  This initial password is also APP_ADMINISTRATORS_ACCOUNT, but can be changed on
 * the dialog console, as is persistent via NVRAM.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
#define APP_ADMINISTRATORS_ACCOUNT  "admin"

#if defined(HAVE_SECURITY_API) 

/*ad*
 * APP_FIXED_GATEWAY_KEY is the fixed key used for the secure gateway.  This Key can be used  
 * at the remote side to when using Fixed Key distribution.
 *
 * This feature cannot be used unless you have purchased the security API.  You must leave this
 * value commented out to avoid build errors unless you have installed the security API.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
#define APP_FIXED_GATEWAY_KEY       "TESTKEY"


#endif /* defined(HAVE_SECURITY_API) */

#if defined(AUTODOC_VERSION)
/*ad*
 * APP_USE_NETWORK_TIME_PROTOCOL uses the NVRAM configuration to store Network Time Protocol
 * Servers used for the NET+OS SNTP Client on a NAstartSntp() call.  If this constant is
 * set, then the BSP will start the SNTP client.  The C Library time() functions can be
 * used to read the time.
 *
 * @note    The BSP will call customizeErrorHandler if APP_USE_NETWORK_TIME_PROTOCOL is
 *          defined, but no SNTP server address has been set.
 *
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
//#define APP_USE_NETWORK_TIME_PROTOCOL
#endif
//#define APP_USE_NETWORK_TIME_PROTOCOL



/*ad*
 * APP_STDIO_PORT determines which serial port stdin, stdout, and stderr should be redirected to.
 * It should be set to the device name of the port or NULL.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
/*#if BSP_INCLUDE_UDP_DEBUG_DRIVER == TRUE*/
//#define APP_STDIO_PORT              "/com/0"

/*#endif*/

/*ad*
 * If APP_USE_NVRAM is set to a nonzero value, then the initialization code will attempt
 * to read the configuration values from NVRAM.  If the read is successful, then these
 * values will be used.  If not, then the values defined in this file will be used.  If
 * the configuration dialog is enabled, then the user will be prompted to change these
 * values.  If he does, then the new configuration values will be saved to NVRAM.
 *
 * When APP_USE_NVRAM is set to zero, the values, defined in this file, will be used, but
 * be written to NVRAM.
 *
 * See <root>/h/narmsrln.h for configuration structure devBoardParamsType.  If this structure
 * needs extensions, applications should add fields to the end of this data structure.  To 
 * protect legacy structures, fields should not be deleted.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_USE_NVRAM               1

/*ad*
 * APP_NET_HEAP_SIZE sets the TCP/IP stack heap size for dynamic allocations.  The TCP/IP 
 * allocates all packet buffers from this piece of memory.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_NET_HEAP_SIZE	        0x80000

/*ad*
 * PPP is enabled on COM1 when APP_ENABLE_PPP1 is nonzero.
 * Note when this is defined APP_DIALOG_PORT and APP_STDIO_PORT must not use
 * COM1 ("/com/0")
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
#define APP_ENABLE_PPP1           0

/*ad*
 * PPP is enabled on COM2 when APP_ENABLE_PPP2 is nonzero.
 * Note when this is defined APP_DIALOG_PORT and APP_STDIO_PORT must not use
 * COM2 ("/com/1")
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
#define APP_ENABLE_PPP2          0


/*ad*
 * APP_IP_ADDRESS will be used as the default IP address if APP_USE_NVRAM is set to 0, or if
 * NVRAM has not been initialized.
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
//#define APP_IP_ADDRESS              "192.168.0.145"


/*ad*
 * APP_IP_SUBNET_MASK will be used as the default subnet mask if APP_USE_NVRAM is set to 0, or if
 * NVRAM has not been initialized.
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
//#define APP_IP_SUBNET_MASK          "255.255.255.0"

/*ad*
 * APP_IP_GATEWAY will be used as the default gateway if APP_USE_NVRAM is set to 0, or if
 * NVRAM has not been initialized.  If there is no gateway, then set APP_IP_GATEWAY to "0.0.0.0".
 *
 * All values should be specified in dotted notation as strings.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 3.0
*ad*/
//#define APP_IP_GATEWAY              "192.168.0.1"


/*ad*
 * APP_SNTP_PRIMARY determine the unit's default primary 
 * SNTP servers.  All values should be specified in dotted notation as strings.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
//#define APP_SNTP_PRIMARY            "0.0.0.0"


/*ad*
 * APP_SNTP_SECONDARY determine the unit's default secondary
 * SNTP servers.  All values should be specified in dotted notation as strings.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 5.0
*ad*/
//#define APP_SNTP_SECONDARY          "0.0.0.0"







/*ad*
 * APP_BSP_SERIAL_NUM determines the default serial number to use if APP_USE_NVRAM is 0,
 * or if NVRAM is uninitialized.  The serial number should consist of 1 alpha character
 * followed by 8 digits.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_BSP_SERIAL_NUM	        "N99999999"


/*ad*
 * APP_BSP_MAC_ADDRESS determines the default Ethernet MAC address to use if 
 * APP_USE_NVRAM is 0, or if NVRAM is uninitialized.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 6.0
*ad*/
//#define APP_BSP_MAC_ADDRESS         "00:40:9d:28:8c:64"


/*ad*
 * APP_BSP_DELAY determines the amount of time to wait the configuration dialog will
 * wait for user input before continuing the BSP startup.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_BSP_DELAY		        5


/*ad*
 * APP_BSP_BAUD_RATE determines the baud rate that will be used for the configuration
 * dialog if APP_USE_NVRAM is 0, or if NVRAM is uninitialized.
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 1.0
*ad*/
#define APP_BSP_BAUD_RATE	        9600



/*ad*
 * APP_USE_STATIC_IP is used to create a default configuration for ACE.  This value
 * only has effect if there is no valid configuration stored in NVRAM, or if APP_USE_NVRAM
 * is not set.  It determines whether or not the IP stack should use the static IP
 * parameters specified in APP_IP_ADDRESS, APP_IP_SUBNET_MASK, and APP_IP_GATEWAY, or
 * get an IP address from the network using DHCP, BOOTP, RARP, and Ping-ARP.  
 *
 * @external
 * @category ApplicationConfiguration 
 * @since 6.0
*ad*/
#define APP_USE_STATIC_IP            FALSE


/*ad*
 * APP_DEFAULT_ETHERNET_DUPLEX determines the default Ethernet duplex setting.  Normally,
 * the PHY will negotiate the duplex setting with the hub or switch.  However, some hubs
 * do not support autonegotiation, or allow the end user to disable it.  In this case,
 * there is no way for the PHY to determine what duplex setting should be used.  It will
 * use the value stored in NVRAM.  The value of APP_DEFAULT_ETHERNET_DUPLEX will be
 * written to the NVRAM field when it is first initialized.
 *
 * Legal values are bpHalfDuplex, bpFullDuplex, and bpDefaultDuplex.  The recommended 
 * setting is bpDefaultDuplex.  This uses the default duplex setting supported by the
 * Ethernet PHY.  Most PHYs will default to half duplex.
 *
 * @external
 * @category ApplicationConfiguration
 * @since 6.0
 * @see @link bpDuplexLevelType
*ad*/
#define APP_DEFAULT_ETHERNET_DUPLEX  bpDefaultDuplex


/*ad*
 * This constant determines whether Auto-IP will be enabled by default.  The constant is used
 * when NVRAM is first initialized, and when APP_USE_NVRAM is set to 0.  If APP_ENABLE_AUTO_IP
 * is TRUE and then the default ACE configuration will be to setup the secondary network 
 * interface "eth0:0" to acquire an IP address using Auto-IP.  Note that this may cause problems
 * with some applications.  This is because Auto-IP addresses are not routable, and because
 * Auto-IP can usually assign an address very quickly, before other methods like DHCP can.  So,
 * if APP_ENABLE_AUTO_IP is set to TRUE, it is very likely that applicationStart() will be
 * called before TCP/IP has a routable TCP/IP address (DHCP et. al. will not have had time
 * to get an IP address yet).  This may cause unexpected errors with applications that 
 * immediately try to communicate over IP.  If APP_ENABLE_AUTO_IP is set to FALSE, then the
 * default ACE configuration will leave Auto-IP disabled.  This constant only effects
 * the default configuration.  It will not have any effect if a valid ACE configuration is
 * already stored in NVRAM.
 *  
 * @external
 * @category ApplicationConfiguration
 * @since 6.0
*ad*/
#define APP_ENABLE_AUTO_IP  TRUE
#endif
