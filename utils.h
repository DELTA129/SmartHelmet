/*
 * utils.h
 *
 *  Created on: 5 Mar 2024
 *      Author: mhrehor
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "uart.h"

void UART_WriteStr(mxc_uart_regs_t* uart,  const char* str)
{
	while(*str)
	{
		MXC_UART_WriteCharacter(uart, *str);
		str++;
	}
}

#endif /* UTILS_H_ */
