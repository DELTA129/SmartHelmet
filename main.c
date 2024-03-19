/******************************************************************************
 * Copyright (C) 2023 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

/**
 * @file    main.c
 * @brief   Hello World!
 * @details This example uses the UART to print to a terminal and flashes an LED.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"

#define DELTA_FCPU_60M

#include "i2c_c.h"
#include "spi_c.h"
#include "utils.h"


#include "Delta.h"

#include "A121/radar.h"

#include "example_detector_distance.h"

/***** Definitions *****/

/***** Globals *****/

/***** Functions *****/

// *****************************************************************************

#define spi MXC_SPI0
#define P_SS (1 << 4)
#define P_MOSI (1 << 5)
#define P_MISO (1 << 6)
#define P_SCK (1 << 7)
int main(void)
{

	while(1);

	MXC_UART_Init(MXC_UART0, 115200, 0);
	SPIC_Init(MXC_SPI0);
    UART_WriteStr(MXC_UART0, "begin\r\n");

    bool radarInitSuccess = Radar_Init();
    printf("Radar init %s\r\n", radarInitSuccess ? "success" : "FAILED");
    printf("RSS version: %s\r\n", acc_version_get());

    int result = acc_example_detector_distance();
    printf("END, Result: %d\r\n", result);

    while(1);

    int loopctr = 0;
    while(1){
	   RSS_Log(ACC_LOG_LEVEL_INFO, "Main", "Loop %d", loopctr++);
	   SPIC_Send8(MXC_SPI0, 0x7A);

	   int flags = MXC_SPI0->intfl;
	   //sprintf(strbuf, "done TRx, flags: %d ctrl0: %d\r\n", flags, MXC_SPI0->ctrl0);
	   //UART_WriteStr(MXC_UART0, strbuf);
	   MXC_Delay(100000);
   }
}
