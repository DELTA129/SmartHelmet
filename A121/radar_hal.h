/*
 * sh_hal.h
 *
 *  Created on: 13 Mar 2024
 *      Author: mhrehor
 */

#ifndef A121_RADAR_HAL_H_
#define A121_RADAR_HAL_H_

#include "rss/include/acc_rss_a121.h"
#include "radar_hwi.h"

#define RSPI MXC_SPI0

void RSS_Transfer8(acc_sensor_id_t sensor_id, uint8_t *buffer, size_t buffer_size);

void RSS_Transfer16(acc_sensor_id_t sensor_id, uint16_t *buffer, size_t buffer_length);

void RSS_Log(acc_log_level_t level, const char *module, const char *format, ...);

//Register the Hardware Abstraction Layer for the Radar Subsystem
bool Radar_RegisterHAL();




#endif /* A121_RADAR_HAL_H_ */
