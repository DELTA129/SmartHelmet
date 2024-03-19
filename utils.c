/*
 * utils.c
 *
 *  Created on: 13 Mar 2024
 *      Author: mhrehor
 */

#include "utils.h"


bool _TimeoutWaitBitSet_Loops(uint32_t loops, const volatile uint32_t* source, uint32_t bitmaskCondition)
{
	while(loops--)
	{
		if(*source & bitmaskCondition) return 1;
	}
	return 0;
}
