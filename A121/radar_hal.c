/*
 * radar_hal.c
 *
 *  Created on: 13 Mar 2024
 *      Author: mhrehor
 */

#include <stdarg.h>
#include <stdio.h>

#include "../spi_c.h"

#include "A121/radar_hal.h"

const char* Str_Dbglevel[] = {"ERROR", "WARNING", "INFO", "VERBOSE", "DEBUG"};
#define LOG_BUFFER_MAX_SIZE 150U

//Register the Hardware Abstraction Layer for the Radar Subsystem
bool Radar_RegisterHAL()
{

	static const acc_hal_a121_t hal =
		{
			.max_spi_transfer_size = 64,

			.mem_alloc = malloc,
			.mem_free  = free,

			.transfer = RSS_Transfer8,
			.log      = RSS_Log,

			//.optimization.transfer16 = RSS_Transfer16,
			.optimization.transfer16 = NULL,
		};

	return acc_rss_hal_register(&hal);
}

void RSS_Log(acc_log_level_t level, const char *module, const char *format, ...)
{
	char log_buffer[LOG_BUFFER_MAX_SIZE];
	va_list ap;
	va_start(ap, format);

	int ret = vsnprintf(log_buffer, LOG_BUFFER_MAX_SIZE, format, ap);
	if (ret >= LOG_BUFFER_MAX_SIZE)
	{
		log_buffer[LOG_BUFFER_MAX_SIZE - 4] = '.';
		log_buffer[LOG_BUFFER_MAX_SIZE - 3] = '.';
		log_buffer[LOG_BUFFER_MAX_SIZE - 2] = '.';
		log_buffer[LOG_BUFFER_MAX_SIZE - 1] = 0;
	}

	printf("[%s]: (%s): %s\r\n", Str_Dbglevel[level], module, log_buffer);
}

void RSS_Transfer8(acc_sensor_id_t sensor_id, uint8_t *buffer, size_t buffer_size)
{
	_Radar_CS_LOW();
	while(buffer_size)
	{
		buffer_size--;
		uint8_t rx = SPIC_Transfer8(RSPI, *buffer);
		*buffer = rx;
		buffer++;
	}
	_Radar_CS_HIGH();
}

/*void RSS_Transfer16(acc_sensor_id_t sensor_id, uint16_t *buffer, size_t buffer_length)
{
	_Radar_CS_LOW();
	while(buffer_length)
	{
		buffer_length--;
		uint16_t rx = SPIC_Transfer16(RSPI, *buffer);
		*buffer = rx;
		buffer++;
	}
	_Radar_CS_HIGH();
}*/
