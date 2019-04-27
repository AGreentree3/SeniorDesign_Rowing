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

#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Watchdog.h>

#include "ROW_CUSTOM.h"

#define Board_initGeneral()     ROW_CUSTOM_initGeneral()

/* These #defines allow us to reuse TI-RTOS across other device families */

#define Board_3V3_EN            ROW_CUSTOM_3V3_EN

#define Board_CRYPTO0           ROW_CUSTOM_CRYPTO0

#define Board_GPIO_BUTTON0      ROW_CUSTOM_GPIO_S1
#define Board_GPIO_BUTTON1      ROW_CUSTOM_GPIO_S2
#define Board_GPIO_LED0         ROW_CUSTOM_GPIO_LED3
#define Board_GPIO_LED1         ROW_CUSTOM_GPIO_LED4
#define Board_GPIO_LED2         ROW_CUSTOM_GPIO_LED4
#define Board_GPIO_LED3         ROW_CUSTOM_GPIO_LED3
#define Board_GPIO_LED4         ROW_CUSTOM_GPIO_LED4
#define Board_GPIO_LED_ON       ROW_CUSTOM_GPIO_LED_ON
#define Board_GPIO_LED_OFF      ROW_CUSTOM_GPIO_LED_OFF

#define Board_GPTIMER0A         ROW_CUSTOM_GPTIMER0A
#define Board_GPTIMER0B         ROW_CUSTOM_GPTIMER0B
#define Board_GPTIMER1A         ROW_CUSTOM_GPTIMER1A
#define Board_GPTIMER1B         ROW_CUSTOM_GPTIMER1B
#define Board_GPTIMER2A         ROW_CUSTOM_GPTIMER2A
#define Board_GPTIMER2B         ROW_CUSTOM_GPTIMER2B
#define Board_GPTIMER3A         ROW_CUSTOM_GPTIMER3A
#define Board_GPTIMER3B         ROW_CUSTOM_GPTIMER3B

#define Board_KEY_DOWN          ROW_CUSTOM_KEY_DOWN
#define Board_KEY_LEFT          ROW_CUSTOM_KEY_LEFT
#define Board_KEY_RIGHT         ROW_CUSTOM_KEY_RIGHT
#define Board_KEY_SELECT        ROW_CUSTOM_KEY_SELECT
#define Board_KEY_UP            ROW_CUSTOM_KEY_UP

#define Board_LCD_CSN           ROW_CUSTOM_LCD_CSN
#define Board_LCD_MODE          ROW_CUSTOM_LCD_MODE
#define Board_LCD_RST           ROW_CUSTOM_LCD_RST

#define Board_PIN_BUTTON0       ROW_CUSTOM_KEY_UP
#define Board_PIN_BUTTON1       ROW_CUSTOM_KEY_DOWN
#define Board_PIN_LED0          ROW_CUSTOM_PIN_LED3
#define Board_PIN_LED1          ROW_CUSTOM_PIN_LED4
#define Board_PIN_LED2          ROW_CUSTOM_PIN_LED4
#define Board_PIN_LED3          ROW_CUSTOM_PIN_LED3
#define Board_PIN_LED4          ROW_CUSTOM_PIN_LED4

#define Board_PWM0              ROW_CUSTOM_PWM0
#define Board_PWM1              ROW_CUSTOM_PWM1
#define Board_PWM2              ROW_CUSTOM_PWM2
#define Board_PWM3              ROW_CUSTOM_PWM3
#define Board_PWM4              ROW_CUSTOM_PWM4
#define Board_PWM5              ROW_CUSTOM_PWM5
#define Board_PWM6              ROW_CUSTOM_PWM6
#define Board_PWM7              ROW_CUSTOM_PWM7

#define Board_SPI0              ROW_CUSTOM_SPI0
#define Board_SPI1              ROW_CUSTOM_SPI1

#define Board_UART0             ROW_CUSTOM_UART0

#define Board_WATCHDOG0         ROW_CUSTOM_WATCHDOG0

/*
 * These macros are provided for backwards compatibility.
 * Please use the <Driver>_init functions directly rather
 * than Board_init<Driver>.
 */
#define Board_initGPIO()        GPIO_init()
#define Board_initPWM()         PWM_init()
#define Board_initSPI()         SPI_init()
#define Board_initUART()        UART_init()
#define Board_initWatchdog()    Watchdog_init()

/*
 * These macros are provided for backwards compatibility.
 * Please use the 'Board_PIN_xxx' macros to differentiate
 * them from the 'Board_GPIO_xxx' macros.
 */
#define Board_BUTTON0           Board_PIN_BUTTON0
#define Board_BUTTON1           Board_PIN_BUTTON1
#define Board_LED0              Board_PIN_LED0
#define Board_LED1              Board_PIN_LED1
#define Board_LED2              Board_PIN_LED2
#define Board_LED3              Board_PIN_LED3
#define Board_LED4              Board_PIN_LED4
#define Board_LED_ON            Board_GPIO_LED_ON
#define Board_LED_OFF           Board_GPIO_LED_OFF

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */