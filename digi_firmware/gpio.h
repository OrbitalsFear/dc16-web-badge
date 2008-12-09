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
 *  Copyright (c) 2003 Digi International Inc., All Rights Reserved
 * 
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees
 *  to retain this software in confidence, to prevent disclosure to others,
 *  and to make no use of this software other than that for which it was
 *  delivered.  This is an unpublished copyrighted work of Digi International
 *  Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
 *  prohibited.
 * 
 *  Restricted Rights Legend
 *
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions set forth in sub-paragraph (c)(1)(ii) of The Rights in
 *  Technical Data and Computer Software clause at DFARS 252.227-7031 or
 *  subparagraphs (c)(1) and (2) of the Commercial Computer Software -
 *  Restricted Rights at 48 CFR 52.227-19, as applicable.
 *
 *  Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 *
 *****************************************************************************
 *
 *  Description.
 *
 *  This file contains the public API code that sets up the GPIO ports.  THE
 *  DEFINITIONS IN THIS FILE AND THE SOURCE CODE FOR THIS INTERFACE SHOULD BE 
 *  CUSTOMIZED FOR YOUR PARTICULAR HARDWARE.  The version of the file that is 
 *  shipped with the NET+OS BSP is written to support NET+OS development boards.  
 *  Your hardware will be different and the code in this file must be 
 *  customized to support it.
 *
 *
 * Edit Date/Ver   Edit Description
 * ==============  =======================================================
 *
 *
 */


#ifndef __GPIO_H_
#define __GPIO_H_

#include "gpiomux_def.h"
#include "bsp.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * The NS7520 has 16 pins that are multiplexed with various functions
 * including GPIO functionality.  These pins can be rapidly configured 
 * using the definitions in this file.  The funcions multiplexed include:
 * serial, DMA, Ethernet CAM, external IRQs, and GPIO.
 *
 * By selecting options other than @link BSP_GPIO_MUX_INTERNAL_USE_ONLY, groups 
 * of pins can be defined, setup and programmed at system startup.  An 
 * example of a multiplexer group is @link BSP_GPIO_MUX_SERIAL_A, which can be 
 * assigned the values: @link BSP_GPIO_MUX_INTERNAL_USE_ONLY or 
 * @link BSP_GPIO_MUX_SERIAL_8_WIRE_UART.
 *
 * When @link BSP_GPIO_MUX_SERIAL_A is set to @link BSP_GPIO_MUX_SERIAL_8_WIRE_UART, the 
 * pins needed to implement an 8 wire interface on serial port A will configured 
 * accordingly.
 *
 * Setting @link BSP_GPIO_MUX_SERIAL_A to @link BSP_GPIO_MUX_USE_PRIMARY_PATH makes
 * these pins available for other functions.
 *
 * Also included are GPIO access functions for setting and sensing the 
 * state of the pins.  See @link NAgetGPIOpin and @link NAsetGPIOpin for more
 * details.
 *
 *
 * @note Only one multiplexer group can claim the functionlity on a pin.  If
 * multiple groups claim use on a pin, a compile time error will occur.  This
 * should save time when configuring a BSP.
 *
 * When a multiplexer pin is left unspecified, it defaults to a GPIO
 * receiver.  That means all possible functions that can be routed out a 
 * multiplexer pin have been assigned the BSP_GPIO_MUX_INTERNAL_USE_ONLY 
 * setting.
 *
 * To change a multiplexer pin to a GPIO output, see the section on
 * "Altering the BSP_GPIO_INITIAL_STATE_PINx".  For convenience, all 16 
 * pins have been included.  However, only pins that specify a GPIO output
 * are necessary.
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO
 * @overview Signal Multiplexing and GPIO
 */







/**
 * The NS7520 has the ability to multiplex the 2 DMA channels.  These  
 * channels are used to transfer data to (and from) memory space to 
 * an external peripheral.
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:DMA
 * @overview DMA
 */


/**
 * This directive controls how the external DMA channel 1 
 * signals are routed out the GPIO multiplexer.  These signals 
 * can stay internal (allowing GPIO or other special functions 
 * to multiplex), or can be routed to the primary or alternate 
 * GPIO pins.  
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:DMA
 * @see @link BSP_GPIO_MUX_INTERNAL_USE_ONLY
 * @see @link BSP_GPIO_MUX_EXTERNAL_DMA_INPUT
 * @see @link BSP_GPIO_MUX_EXTERNAL_DMA_OUTPUT
 */
#define BSP_GPIO_MUX_DMA_1                          BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if ((BSP_GPIO_MUX_DMA_1 != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
     (BSP_GPIO_MUX_DMA_1 != BSP_GPIO_MUX_EXTERNAL_DMA_INPUT) && \
     (BSP_GPIO_MUX_DMA_1 != BSP_GPIO_MUX_EXTERNAL_DMA_OUTPUT)) 
#error BSP_GPIO_MUX_DMA_1 is not correctly defined.
#endif

/**
 * This directive controls how the external DMA channel 2 
 * signals are routed out the GPIO multiplexer.  These signals 
 * can stay internal (allowing GPIO or other special functions 
 * to multiplex), or can be routed to the primary or alternate 
 * GPIO pins.  
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:DMA
 * @see @link BSP_GPIO_MUX_INTERNAL_USE_ONLY
 * @see @link BSP_GPIO_MUX_EXTERNAL_DMA_INPUT
 * @see @link BSP_GPIO_MUX_EXTERNAL_DMA_OUTPUT
 */
#define BSP_GPIO_MUX_DMA_2                          BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if ((BSP_GPIO_MUX_DMA_2 != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
     (BSP_GPIO_MUX_DMA_2 != BSP_GPIO_MUX_EXTERNAL_DMA_INPUT) && \
     (BSP_GPIO_MUX_DMA_2 != BSP_GPIO_MUX_EXTERNAL_DMA_OUTPUT)) 
#error BSP_GPIO_MUX_DMA_2 is not correctly defined.
#endif

/**
 * The NS7520 contains 2 serial channels that can be multiplexed.  These  
 * channels can also be wired according to the type of serial 
 * communication needed. 
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:SerialInterface
 * @overview Serial
 */

/**
 * The NS7520 contains 2 serial channels that can be multiplexed.
 * 
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:SerialInterface:SerialChannels
 * @overview Channels
 */


/**
 * This directive controls how Serial Port A is multiplexed.
 * The signals can stay internal (allowing GPIO or other 
 * special functions to multiplex) or can be routed to GPIO 
 * pins based on the particular serial configuration.
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:SerialInterface:SerialChannels
 * @see @link BSP_GPIO_MUX_INTERNAL_USE_ONLY
 * @see @link BSP_GPIO_MUX_SERIAL_2_WIRE_UART
 * @see @link BSP_GPIO_MUX_SERIAL_4_WIRE_UART
 * @see @link BSP_GPIO_MUX_SERIAL_6_WIRE_UART
 * @see @link BSP_GPIO_MUX_SERIAL_8_WIRE_UART
 * @see @link BSP_GPIO_MUX_SERIAL_SPI_APP
 */
#if (BSP_SERIAL_PORT_1 == BSP_SERIAL_UART_DRIVER)
    #define BSP_GPIO_MUX_SERIAL_A                       BSP_GPIO_MUX_SERIAL_8_WIRE_UART
#else
    #define BSP_GPIO_MUX_SERIAL_A                       BSP_GPIO_MUX_INTERNAL_USE_ONLY
#endif
#if (BSP_GPIO_MUX_SERIAL_A != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_MUX_SERIAL_A != BSP_GPIO_MUX_SERIAL_2_WIRE_UART) && \
    (BSP_GPIO_MUX_SERIAL_A != BSP_GPIO_MUX_SERIAL_4_WIRE_UART) && \
    (BSP_GPIO_MUX_SERIAL_A != BSP_GPIO_MUX_SERIAL_6_WIRE_UART) && \
    (BSP_GPIO_MUX_SERIAL_A != BSP_GPIO_MUX_SERIAL_8_WIRE_UART) && \
    (BSP_GPIO_MUX_SERIAL_A != BSP_GPIO_MUX_SERIAL_SPI_MASTER) && \
    (BSP_GPIO_MUX_SERIAL_A != BSP_GPIO_MUX_SERIAL_SPI_SLAVE)
#error Invalid Serial Port A configuration
#endif




/**
 * This directive determines if pin A4 is configured 
 * for use as a serial port clock.  The pin is automatically setup
 * for use as an clock if serial port A is configured for SPI.  This
 * directive is only relevant if the port is not configured for SPI.
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:SerialInterface:SerialChannels
 * @see @link BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_INPUT
 * @see @link BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_OUTPUT
 */
#define BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_A        BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if (BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_A != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_A != BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_INPUT) && \
    (BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_A != BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_OUTPUT) 
#error Invalid setting for BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_A
#endif



/**
 * This directive determines if pin A0 is configured 
 * for use as a serial port clock.  The pin is automatically setup
 * for use as an clock if serial port A is configured for SPI.  This
 * directive is only relevant if the port is not configured for SPI.
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:SerialInterface:SerialChannels
 * @see @link BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_INPUT
 * @see @link BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_OUTPUT
 */
#define BSP_GPIO_MUX_SERIAL_EXTERNAL_TX_CLOCK_A        BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if (BSP_GPIO_MUX_SERIAL_EXTERNAL_TX_CLOCK_A != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_MUX_SERIAL_EXTERNAL_TX_CLOCK_A != BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_INPUT) 
#error Invalid setting for BSP_GPIO_MUX_SERIAL_EXTERNAL_TX_CLOCK_A
#endif
#if (BSP_GPIO_MUX_SERIAL_EXTERNAL_TX_CLOCK_A != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_MUX_SERIAL_A == BSP_GPIO_MUX_SERIAL_8_WIRE_UART) 
#error Conflict between BSP_GPIO_MUX_SERIAL_EXTERNAL_TX_CLOCK_A and BSP_GPIO_MUX_SERIAL_A.
#error The external transmitter clock function on port A cannot be used if an 8-wire
#error configuration is selected because the same pin is used for the DCD signal.
#endif

/**
 * This directive controls how Serial Port B is multiplexed.
 * The signals can stay internal (allowing GPIO or other 
 * special functions to multiplex) or can be routed to GPIO 
 * pins based on the particular serial configuration.
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:SerialInterface:SerialChannels
 * @see @link BSP_GPIO_MUX_INTERNAL_USE_ONLY
 * @see @link BSP_GPIO_MUX_SERIAL_2_WIRE_UART
 * @see @link BSP_GPIO_MUX_SERIAL_4_WIRE_UART
 * @see @link BSP_GPIO_MUX_SERIAL_6_WIRE_UART
 * @see @link BSP_GPIO_MUX_SERIAL_8_WIRE_UART
 * @see @link BSP_GPIO_MUX_SERIAL_SPI_APP
 */
#if (BSP_SERIAL_PORT_2 == BSP_SERIAL_UART_DRIVER)
    #define BSP_GPIO_MUX_SERIAL_B                       BSP_GPIO_MUX_SERIAL_8_WIRE_UART
#else
    #define BSP_GPIO_MUX_SERIAL_B                       BSP_GPIO_MUX_INTERNAL_USE_ONLY
#endif
#if (BSP_GPIO_MUX_SERIAL_B != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_MUX_SERIAL_B != BSP_GPIO_MUX_SERIAL_2_WIRE_UART) && \
    (BSP_GPIO_MUX_SERIAL_B != BSP_GPIO_MUX_SERIAL_4_WIRE_UART) && \
    (BSP_GPIO_MUX_SERIAL_B != BSP_GPIO_MUX_SERIAL_6_WIRE_UART) && \
    (BSP_GPIO_MUX_SERIAL_B != BSP_GPIO_MUX_SERIAL_8_WIRE_UART) && \
    (BSP_GPIO_MUX_SERIAL_B != BSP_GPIO_MUX_SERIAL_SPI_MASTER) && \
    (BSP_GPIO_MUX_SERIAL_B != BSP_GPIO_MUX_SERIAL_SPI_SLAVE)
#error Invalid Serial Port B configuration
#endif



/**
 * This directive determines if pin B4 is configured 
 * for use as a serial port clock.  The pin is automatically setup
 * for use as an clock if serial port B is configured for SPI.  This
 * directive is only relevant if the port is not configured for SPI.
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:SerialInterface:SerialChannels
 * @see @link BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_INPUT
 * @see @link BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_OUTPUT
 */
#define BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_B        BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if (BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_B != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_B != BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_INPUT) && \
    (BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_B != BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_OUTPUT) 
#error Invalid setting for pin BSP_GPIO_MUX_SERIAL_EXTERNAL_RX_CLOCK_B
#endif


/**
 * This directive determines if pin C5 is configured 
 * for use as a serial port clock.  The pin is automatically setup
 * for use as an clock if serial port B is configured for SPI.  This
 * directive is only relevant if the port is not configured for SPI.
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:SerialInterface:SerialChannels
 * @see @link BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_INPUT
 * @see @link BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_OUTPUT
 */
#define BSP_GPIO_MUX_SERIAL_EXTERNAL_TX_CLOCK_B        BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if (BSP_GPIO_MUX_SERIAL_EXTERNAL_TX_CLOCK_B != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_MUX_SERIAL_EXTERNAL_TX_CLOCK_B != BSP_GPIO_MUX_SERIAL_EXTERNAL_CLOCK_INPUT) 
#error Invalid setting for pin C5
#endif


/**
 * This directive determines if the IRQ output signal is supported.
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO
 * @see @link BSP_GPIO_MUX_USE_PRIMARY_PATH
 */
#define BSP_GPIO_IRQ_OUT                        BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if (BSP_GPIO_IRQ_OUT != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_IRQ_OUT != BSP_GPIO_MUX_USE_PRIMARY_PATH) 
#error Invalid setting for BSP_GPIO_IRQ_OUT  
#endif


/**
 * This directive determines if the RESET_OUT signal is supported.
 *
 * @since 6.2
 * @external
 * @category Mux_and_GPIO
 * @see @link BSP_GPIO_MUX_USE_PRIMARY_PATH
 */
#define BSP_GPIO_RESET_OUT                      BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if (BSP_GPIO_RESET_OUT != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_RESET_OUT != BSP_GPIO_MUX_USE_PRIMARY_PATH) 
#error Invalid setting for BSP_GPIO_RESET_OUT
#endif



/**
 * This directive determines if pins C5 and C2 should be configured
 * to support an external Ethernet CAM.
 *
 * @since 6.2
 * @external
 * @category Mux_and_GPIO
 * @see @link BSP_GPIO_MUX_USE_PRIMARY_PATH
 */
#define BSP_GPIO_ETHERNET_CAM                   BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if (BSP_GPIO_ETHERNET_CAM != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
    (BSP_GPIO_ETHERNET_CAM != BSP_GPIO_MUX_USE_PRIMARY_PATH) 
#error Invalid setting for BSP_GPIO_ETHERNET_CAM
#endif


/**
 * The NET50 has the ability to multiplex 40 signals to trigger 
 * a call to interrupt service routines.  4 pins can detect
 * high to low or low to high edges, 16 pins can be programmed
 * as active high or active low interrupt inputs, and the 
 * remaining 16 pins can be used as active low interrupt inputs.
 *
 * @since 6.2
 * @external
 * @category Mux_and_GPIO:IRQ
 * @overview IRQ
 */

/**
 * This directive controls how the C0 IRQ signal is multiplexed.
 * This signal can stay internal (allowing GPIO or other special 
 * functions to multiplex) or be configured to be an active high
 * or active low interrupt.
 *
 * @since 6.2
 * @external
 * @category Mux_and_GPIO:IRQ
 * @see @link BSP_GPIO_MUX_INTERNAL_USE_ONLY
 * @see @link BSP_GPIO_MUX_IRQ_ACTIVE_HIGH
 * @see @link BSP_GPIO_MUX_IRQ_ACTIVE_LOW
 */
#define BSP_GPIO_MUX_IRQ_C0                          BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if ((BSP_GPIO_MUX_IRQ_C0 != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
     (BSP_GPIO_MUX_IRQ_C0 != BSP_GPIO_MUX_IRQ_ACTIVE_HIGH) && \
     (BSP_GPIO_MUX_IRQ_C0 != BSP_GPIO_MUX_IRQ_ACTIVE_LOW))
#error BSP_GPIO_MUX_IRQ_C0 is not correctly configured.
#endif


/**
 * This directive controls how the C1 IRQ signal is multiplexed.
 * This signal can stay internal (allowing GPIO or other special 
 * functions to multiplex) or be configured to be an active high
 * or active low interrupt.
 *
 * @since 6.2
 * @external
 * @category Mux_and_GPIO:IRQ
 * @see @link BSP_GPIO_MUX_INTERNAL_USE_ONLY
 * @see @link BSP_GPIO_MUX_IRQ_RISING_EDGE
 * @see @link BSP_GPIO_MUX_IRQ_FALLING_EDGE
 * @see @link BSP_GPIO_MUX_IRQ_ACTIVE_HIGH
 * @see @link BSP_GPIO_MUX_IRQ_ACTIVE_LOW
 */
#define BSP_GPIO_MUX_IRQ_C1                          BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if ((BSP_GPIO_MUX_IRQ_C1 != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
     (BSP_GPIO_MUX_IRQ_C1 != BSP_GPIO_MUX_IRQ_ACTIVE_HIGH) && \
     (BSP_GPIO_MUX_IRQ_C1 != BSP_GPIO_MUX_IRQ_ACTIVE_LOW))
#error BSP_GPIO_MUX_IRQ_C1 is not correctly configured.
#endif


/**
 * This directive controls how the C2 IRQ signal is multiplexed.
 * This signal can stay internal (allowing GPIO or other special 
 * functions to multiplex) or be configured to be an active high
 * or active low interrupt.
 *
 * @since 6.2
 * @external
 * @category Mux_and_GPIO:IRQ
 * @see @link BSP_GPIO_MUX_INTERNAL_USE_ONLY
 * @see @link BSP_GPIO_MUX_IRQ_RISING_EDGE
 * @see @link BSP_GPIO_MUX_IRQ_FALLING_EDGE
 * @see @link BSP_GPIO_MUX_IRQ_ACTIVE_HIGH
 * @see @link BSP_GPIO_MUX_IRQ_ACTIVE_LOW
 */
#define BSP_GPIO_MUX_IRQ_C2                          BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if ((BSP_GPIO_MUX_IRQ_C2 != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
     (BSP_GPIO_MUX_IRQ_C2 != BSP_GPIO_MUX_IRQ_ACTIVE_HIGH) && \
     (BSP_GPIO_MUX_IRQ_C2 != BSP_GPIO_MUX_IRQ_ACTIVE_LOW))
#error BSP_GPIO_MUX_IRQ_C2 is not correctly configured.
#endif


/**
 * This directive controls how the C3 IRQ signal is multiplexed.
 * This signal can stay internal (allowing GPIO or other special 
 * functions to multiplex) or be configured to be an active high
 * or active low interrupt.
 *
 * @since 6.2
 * @external
 * @category Mux_and_GPIO:IRQ
 * @see @link BSP_GPIO_MUX_INTERNAL_USE_ONLY
 * @see @link BSP_GPIO_MUX_IRQ_RISING_EDGE
 * @see @link BSP_GPIO_MUX_IRQ_FALLING_EDGE
 * @see @link BSP_GPIO_MUX_IRQ_ACTIVE_HIGH
 * @see @link BSP_GPIO_MUX_IRQ_ACTIVE_LOW
 */
#define BSP_GPIO_MUX_IRQ_C3                         BSP_GPIO_MUX_INTERNAL_USE_ONLY
#if ((BSP_GPIO_MUX_IRQ_C3 != BSP_GPIO_MUX_INTERNAL_USE_ONLY) && \
     (BSP_GPIO_MUX_IRQ_C3 != BSP_GPIO_MUX_IRQ_ACTIVE_HIGH) && \
     (BSP_GPIO_MUX_IRQ_C3 != BSP_GPIO_MUX_IRQ_ACTIVE_LOW))
#error BSP_GPIO_MUX_IRQ_C3 is not correctly configured.
#endif




/**
 * Discrete signals can be controlled or sensed using a General Purpose 
 * Input/Output (GPIO) pin.  This pin is able to drive logic or read 
 * levels.  This functionality is available on all Signal Multiplexed pins.
 *
 * When a MUX pin is not configured (to a serial, or Timer, etc), then
 * the functionality defaults to GPIO.  In other words, to setup a pin
 * to have GPIO functionality, all of the associated multiplexer groups 
 * must be defined as BSP_GPIO_MUX_INTERNAL_USE_ONLY.
 *
 * When the MUX pin is set to GPIO, three initial states are available:
 * one for receiver, one for a driver initially set to logic 0, and
 * another for a driver set to logic 1.
 *
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:GPIOInterface
 * @overview GPIO
 */

/**
 * Altering the BSP_GPIO_INITIAL_STATE_PINx
 *
 * The following BSP_GPIO_INITIAL_STATE_PINx directives control the 
 * I/O configuration if and only if the MUX has not been configured
 * to some other available function.  
 *
 * When a MUX pin is not configured (to a serial, or Timer, etc), then
 * the functionality multiplexed is GPIO.  In other words, to setup a pin
 * to have GPIO functionality, all of the associated multiplexer groups 
 * must be defined as BSP_GPIO_MUX_INTERNAL_USE_ONLY.
 *
 * Addtionally, if the MUX is configured to use something other than the 
 * GPIO functionality, then the definitions below will be ignored on that 
 * particalur multiplexer pin.
 *
 * When the MUX pin is set to GPIO, three initial states are available:
 * one for receiver, one for a driver initially set to logic 0, and
 * another for a driver set to logic 1.
 *
 * @name GPIO_INITIAL_STATE_PINx "GPIO Initial Pin State"
 *
 * @note The safe setting for this is BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER,
 * which programs the pin to a receive configuration.
 *
 * @param BSP_GPIO_INITIAL_STATE_PIN_A0   "Initial state of GPIO A0" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_A1   "Initial state of GPIO A1" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_A2   "Initial state of GPIO A2" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_A3   "Initial state of GPIO A3" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_A4   "Initial state of GPIO A4" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_A5   "Initial state of GPIO A5" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_A6   "Initial state of GPIO A6" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_A7   "Initial state of GPIO A7" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_C0   "Initial state of GPIO C0" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_C1   "Initial state of GPIO C1" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_C2   "Initial state of GPIO C2" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_C3   "Initial state of GPIO C3" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_C4   "Initial state of GPIO C4" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_C5   "Initial state of GPIO C5" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_C6   "Initial state of GPIO C6" 
 * @param BSP_GPIO_INITIAL_STATE_PIN_C7   "Initial state of GPIO C7" 
 *
 * @since 6.2
 * @external
 * @category Mux_and_GPIO:GPIOInterface
 * @see @link BSP_GPIO_INITIAL_STATE_OUTPUT_DRIVER_LOGIC0
 * @see @link BSP_GPIO_INITIAL_STATE_OUTPUT_DRIVER_LOGIC1
 * @see @link BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
 */
#define BSP_GPIO_INITIAL_STATE_PIN_A0                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_A1                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_A2                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_A3                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_A4                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_A5                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_A6                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_A7                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER

#define BSP_GPIO_INITIAL_STATE_PIN_C0                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_C1                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER

/*
 * The Yellow LED is turned on.  This is only true if the serial port B is disabled.
 */
#define BSP_GPIO_INITIAL_STATE_PIN_C2                 BSP_GPIO_INITIAL_STATE_OUTPUT_DRIVER_LOGIC0

#define BSP_GPIO_INITIAL_STATE_PIN_C3                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_C4                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
#define BSP_GPIO_INITIAL_STATE_PIN_C5                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER

/*
 * Ethernet activity LED.
 */
#define BSP_GPIO_INITIAL_STATE_PIN_C6                 BSP_GPIO_INITIAL_STATE_OUTPUT_DRIVER_LOGIC0

#define BSP_GPIO_INITIAL_STATE_PIN_C7                 BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER


/**
 * These defines assign pin numbers to the GPIO pins on the NS7520
 * processor.  These pin numbers can be passed to the NAconfigureGPIOpin,
 * NAgetGPIOpin, and NAsetGPIOpin.
 *
 * @name N7520_PIN_ASSIGNMENTS  "NS7520 GPIO Pin Assignments"
 *
 *
 * @param BSP_GPIO_PIN_A0                 "GPIO pin A0 is assigned 0" 
 * @param BSP_GPIO_PIN_A1                 "GPIO pin A1 is assigned 1"
 * @param BSP_GPIO_PIN_A2                 "GPIO pin A2 is assigned 2"    
 * @param BSP_GPIO_PIN_A3                 "GPIO pin A3 is assigned 3"    
 * @param BSP_GPIO_PIN_A4                 "GPIO pin A4 is assigned 4"    
 * @param BSP_GPIO_PIN_A5                 "GPIO pin A5 is assigned 5"    
 * @param BSP_GPIO_PIN_A6                 "GPIO pin A6 is assigned 6"    
 * @param BSP_GPIO_PIN_A7                 "GPIO pin A7 is assigned 7"    
 * @param BSP_GPIO_PIN_C0                 "GPIO pin C0 is assigned 8"   
 * @param BSP_GPIO_PIN_C1                 "GPIO pin C1 is assigned 9"   
 * @param BSP_GPIO_PIN_C2                 "GPIO pin C2 is assigned 10"   
 * @param BSP_GPIO_PIN_C3                 "GPIO pin C3 is assigned 11"   
 * @param BSP_GPIO_PIN_C4                 "GPIO pin C4 is assigned 12"   
 * @param BSP_GPIO_PIN_C5                 "GPIO pin C5 is assigned 13"   
 * @param BSP_GPIO_PIN_C6                 "GPIO pin C6 is assigned 14"   
 * @param BSP_GPIO_PIN_C7                 "GPIO pin C7 is assigned 15"   
 *
 * @since 6.1
 * @external
 * @category Mux_and_GPIO:GPIOInterface
 * @see @link BSP_GPIO_INITIAL_STATE_OUTPUT_DRIVER_LOGIC0
 * @see @link BSP_GPIO_INITIAL_STATE_OUTPUT_DRIVER_LOGIC1
 * @see @link BSP_GPIO_INITIAL_STATE_INPUT_RECEIVER
 */
#define BSP_GPIO_PIN_A0                 0
#define BSP_GPIO_PIN_A1                 1
#define BSP_GPIO_PIN_A2                 2
#define BSP_GPIO_PIN_A3                 3
#define BSP_GPIO_PIN_A4                 4
#define BSP_GPIO_PIN_A5                 5
#define BSP_GPIO_PIN_A6                 6
#define BSP_GPIO_PIN_A7                 7
#define BSP_GPIO_PIN_C0                 8  
#define BSP_GPIO_PIN_C1                 9  
#define BSP_GPIO_PIN_C2                 10 
#define BSP_GPIO_PIN_C3                 11 
#define BSP_GPIO_PIN_C4                 12 
#define BSP_GPIO_PIN_C5                 13 
#define BSP_GPIO_PIN_C6                 14 
#define BSP_GPIO_PIN_C7                 15 
                                        
                                        
#ifdef __cplusplus                      
}                                       
#endif


#endif






