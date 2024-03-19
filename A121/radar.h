/*
 * radar.h
 *
 *  Created on: 12 Mar 2024
 *      Author: mhrehor
 */

#ifndef RADAR_H_
#define RADAR_H_

#include <stdbool.h>

#include "rss/include/acc_definitions_a121.h"
#include "rss/include/acc_detector_distance.h"
#include "rss/include/acc_hal_definitions_a121.h"
#include "rss/include/acc_rss_a121.h"
#include "rss/include/acc_sensor.h"
#include "rss/include/acc_version.h"

#include "radar_hal.h"

static inline bool Radar_Init()
{
	Radar_InitPins();
	return Radar_RegisterHAL();
}

#endif /* RADAR_H_ */
