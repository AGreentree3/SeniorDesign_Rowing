/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** ============================================================================
 *  @file       ROW_CUSTOM.h
 *
 *  @brief      ROW_CUSTOM Board Specific header file.
 *
 *  The ROW_CUSTOM header file should be included in an application as
 *  follows:
 *  @code
 *  #include "ROW_CUSTOM.h"
 *  @endcode
 *
 *  This board file is made for the 5x5 mm QFN package, to convert this board
 *  file to use for other smaller device packages please refer to the table
 *  below which lists the max IOID values supported by each package. All other
 *  unused pins should be set to IOID_UNUSED.
 *
 *  Furthermore the board file is also used
 *  to define a symbol that configures the RF front end and bias.
 *  See the comments below for more information.
 *  For an in depth tutorial on how to create a custom board file, please refer
 *  to the section "Running the SDK on Custom Boards" with in the Software
 *  Developer's Guide.
 *
 *  Refer to the datasheet for all the package options and IO descriptions:
 *  http://www.ti.com/lit/ds/symlink/cc2640r2f.pdf
 *
 *  +-----------------------+------------------+-----------------------+
 *  |     Package Option    |  Total GPIO Pins |   MAX IOID            |
 *  +=======================+==================+=======================+
 *  |     7x7 mm QFN        |     31           |   IOID_30             |
 *  +-----------------------+------------------+-----------------------+
 *  |     5x5 mm QFN        |     15           |   IOID_14             |
 *  +-----------------------+------------------+-----------------------+
 *  |     4x4 mm QFN        |     10           |   IOID_9              |
 *  +-----------------------+------------------+-----------------------+
 *  |     2.7 x 2.7 mm WCSP |     14           |   IOID_13             |
 *  +-----------------------+------------------+-----------------------+
 *  ============================================================================
 */
#ifndef __ROW_CUSTOM_BOARD_H__
#define __ROW_CUSTOM_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <ti/drivers/PIN.h>
#include <ti/devices/cc26x0r2/driverlib/ioc.h>

/* Externs */
extern const PIN_Config BoardGpioInitTable[];

/* Defines */
#ifndef ROW_CUSTOM
  #define ROW_CUSTOM
#endif /* ROW_CUSTOM */

/*
 *  ============================================================================
 *  RF Front End and Bias configuration symbols for TI reference designs and
 *  kits. This symbol sets the RF Front End configuration in ble_user_config.h
 *  and selects the appropriate PA table in ble_user_config.c.
 *  Other configurations can be used by editing these files.
 *
 *  Define only one symbol:
 *  CC2650EM_7ID    - Differential RF and internal biasing
                      (default for CC2640R2 LaunchPad)
 *  CC2650EM_5XD    � Differential RF and external biasing
 *  CC2650EM_4XS    � Single-ended RF on RF-P and external biasing
 *  CC2640R2EM_CXS  - WCSP: Single-ended RF on RF-N and external biasing
 *                    (Note that the WCSP is only tested and characterized for
 *                     single ended configuration, and it has a WCSP-specific
 *                     PA table)
 *
 *  Note: CC2650EM_xxx reference designs apply to all CC26xx devices.
 *  ==========================================================================
 */
#define CC2640R2EM_CXS

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>                <pin mapping>
 */


/* LEDs */
#define ROW_CUSTOM_LED_ON                    0 // on low
#define ROW_CUSTOM_LED_OFF                   1
#define ROW_CUSTOM_LED                           IOID_3        	


/* I2C */
#define ROW_CUSTOM_I2C_SDA                       IOID_9
#define ROW_CUSTOM_I2C_SCL                       IOID_10

/* Clock Out */
#define ROW_CUSTOM_CLOCK_OUT                     IOID_8

/* IMU */
#define ROW_CUSTOM_IMU_BOOTN                     IOID_14         
#define ROW_CUSTOM_IMU_RSTN                      IOID_13
#define ROW_CUSTOM_IMU_INT                       IOID_12   


/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 */
void ROW_CUSTOM_initGeneral(void);

/*!
 *  @def    ROW_CUSTOM_CryptoName
 *  @brief  Enum of Crypto names
 */
typedef enum ROW_CUSTOM_CryptoName {
    ROW_CUSTOM_CRYPTO0 = 0,

    ROW_CUSTOM_CRYPTOCOUNT
} ROW_CUSTOM_CryptoName;

/*!
 *  @def    ROW_CUSTOM_GPTimerName
 *  @brief  Enum of GPTimer parts
 */
typedef enum ROW_CUSTOM_GPTimerName {
    ROW_CUSTOM_GPTIMER0A = 0,
    ROW_CUSTOM_GPTIMER0B,
    ROW_CUSTOM_GPTIMER1A,
    ROW_CUSTOM_GPTIMER1B,
    ROW_CUSTOM_GPTIMER2A,
    ROW_CUSTOM_GPTIMER2B,
    ROW_CUSTOM_GPTIMER3A,
    ROW_CUSTOM_GPTIMER3B,

    ROW_CUSTOM_GPTIMERPARTSCOUNT
} ROW_CUSTOM_GPTimerName;

/*!
 *  @def    ROW_CUSTOM_GPTimers
 *  @brief  Enum of GPTimers
 */
typedef enum ROW_CUSTOM_GPTimers {
    ROW_CUSTOM_GPTIMER0 = 0,
    ROW_CUSTOM_GPTIMER1,
    ROW_CUSTOM_GPTIMER2,
    ROW_CUSTOM_GPTIMER3,

    ROW_CUSTOM_GPTIMERCOUNT
} ROW_CUSTOM_GPTimers;

/*!
 *  @def    ROW_CUSTOM_UDMAName
 *  @brief  Enum of DMA buffers
 */
typedef enum ROW_CUSTOM_UDMAName {
    ROW_CUSTOM_UDMA0 = 0,

    ROW_CUSTOM_UDMACOUNT
} ROW_CUSTOM_UDMAName;

/*!
 *  @def    ROW_CUSTOM_I2CName
 *  @brief  Enum of I2C names
 */
typedef enum ROW_CUSTOM_I2CName {
    ROW_CUSTOM_I2C0 = 0,

    ROW_CUSTOM_I2CCOUNT
} ROW_CUSTOM_I2CName;

/*!
 *  @def    ROW_CUSTOM_WatchdogName
 *  @brief  Enum of Watchdogs
 */
typedef enum ROW_CUSTOM_WatchdogName {
    ROW_CUSTOM_WATCHDOG0 = 0,

    ROW_CUSTOM_WATCHDOGCOUNT
} ROW_CUSTOM_WatchdogName;

/*!
 *  @def    CC2650_LAUNCHXL_TRNGName
 *  @brief  Enum of TRNG names on the board
 */
typedef enum ROW_CUSTOM_TRNGName {
    ROW_CUSTOM_TRNG0 = 0,
    ROW_CUSTOM_TRNGCOUNT
} ROW_CUSTOM_TRNGName;

#ifdef __cplusplus
}
#endif

#endif /* __ROW_CUSTOM_BOARD_H__ */