/*
 * utils.h
 *
 *  Created on: 5 Mar 2024
 *      Author: mhrehor
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <stdbool.h>

#include "uart.h"

static inline void UART_WriteStr(mxc_uart_regs_t* uart,  const char* str)
{
	while(*str)
	{
		MXC_UART_WriteCharacter(uart, *str);
		str++;
	}
}

/*
 *
 */

#define _CYCLES_PER_WAIT_LOOP 7
#define _CYCLE_TIME_US ((1.0/60000000UL)*1000000UL)
#define uS *1
#define mS *1000
#define S *1000000
bool _TimeoutWaitBitSet_Loops(uint32_t loops, const volatile uint32_t* source, uint32_t bitmaskCondition);
#define TimeoutWaitBitSet(delay, source, conditionBitmask) _TimeoutWaitBitSet_Loops((delay) / _CYCLES_PER_WAIT_LOOP / _CYCLE_TIME_US , source, conditionBitmask)

#endif /* UTILS_H_ */
