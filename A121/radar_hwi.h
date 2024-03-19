/*
 * radar_hw.h
 * Radar Hardware interface
 *
 *  Created on: 13 Mar 2024
 *      Author: mhrehor
 */

#ifndef A121_RADAR_HWI_H_
#define A121_RADAR_HWI_H_

#include <stdbool.h>

#include "max78000.h"
#include "gpio.h"
#include "mxc_delay.h"

//SPI CS Pin
#define RADAR_CS_GPIO MXC_GPIO1
#define RADAR_CS_PIN_INDEX 2
#define RADAR_CS_PIN (1 << RADAR_CS_PIN_INDEX)

//Radar Enable Pin
#define RADAR_EN_GPIO MXC_GPIO1
#define RADAR_EN_PIN_INDEX 1
#define RADAR_EN_PIN (1 << RADAR_EN_PIN_INDEX)

//Radar interrupt pin
#define RADAR_INT_GPIO MXC_GPIO1
#define RADAR_INT_PIN_INDEX 0
#define RADAR_INT_PIN (1 << RADAR_INT_PIN_INDEX)

static inline void Radar_InitPins()
{
	RADAR_EN_GPIO->vssel |= RADAR_EN_PIN; //Set IO voltage to 3.3V
	RADAR_EN_GPIO->out_clr = RADAR_EN_PIN; //Pre-set EN output LOW
	RADAR_EN_GPIO->outen_set = RADAR_EN_PIN; //Set EN as output;

	RADAR_CS_GPIO->vssel |= RADAR_CS_PIN; //Set IO voltage to 3.3V
	RADAR_CS_GPIO->out_set = RADAR_CS_PIN; //Pre-set CS output HIGH
	RADAR_CS_GPIO->outen_set = RADAR_CS_PIN; //Set CS as output;

	RADAR_INT_GPIO->vssel |= RADAR_INT_PIN; //Set IO voltage to 3.3V
	RADAR_INT_GPIO->outen_clr = RADAR_INT_PIN; //Ensure output driver is disabled
	//RADAR_INT_GPIO->padctrl1 |= RADAR_INT_PIN; //Enable pulldown
	//RADAR_INT_GPIO->ps |= RADAR_INT_PIN; //Strong pulldown
	RADAR_INT_GPIO->inen |= RADAR_INT_PIN; //Enable pin as input
}

static inline void _Radar_CS_LOW()
{
	RADAR_CS_GPIO->out_clr = RADAR_CS_PIN;
}
static inline void _Radar_CS_HIGH()
{
	RADAR_CS_GPIO->out_set = RADAR_CS_PIN;
}

//Hardware-enable Radar Sensor
static inline void Radar_HW_Enable()
{
	RADAR_EN_GPIO->out_set = RADAR_EN_PIN;
	MXC_Delay(10000);
}

//Hardware-disable radar sensor
static inline void Radar_HW_Disable()
{
	RADAR_EN_GPIO->out_clr = RADAR_EN_PIN;
	MXC_Delay(10000);
}

bool Radar_WaitForInterrupt(unsigned int ms);


#endif /* A121_RADAR_HWI_H_ */
