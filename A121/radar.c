/*
 * radar.c
 *
 *  Created on: 13 Mar 2024
 *      Author: mhrehor
 */

#include "../utils.h"

#include "radar_hwi.h"

bool Radar_WaitForInterrupt(unsigned int ms)
{

	return TimeoutWaitBitSet(ms mS, &RADAR_INT_GPIO->in, RADAR_INT_PIN);
}
