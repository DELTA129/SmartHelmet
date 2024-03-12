/*
 * Delta.h
 *
 *  Created on: 8 Mar 2024
 *      Author: mhrehor
 *
 *      Delta Time Library
 */

#ifndef DELTA_H_
#define DELTA_H_

#include "tmr.h"

extern float deltaMsFlt;
extern int deltaMsInt;

static inline void Delta_Init()
{
	mxc_tmr_cfg_t cfg;
	cfg.pres = TMR_PRES_2;
	cfg.mode = TMR_MODE_CONTINUOUS;
	cfg.bitMode = TMR_BIT_MODE_32;
	cfg.clock = MXC_TMR_APB_CLK;
	cfg.cmp_cnt = 0xFFFF-1;
	MXC_TMR_Init(MXC_TMR0, &cfg, false);
	MXC_TMR_Start(MXC_TMR0);
}

//To be called ONCE at the start of each main program loop
static inline void Delta_Update()
{
	int deltaTicks = MXC_TMR0->cnt;
	MXC_TMR0->cnt = 1;
#if defined(DELTA_FCPU_60M)
	deltaMsFlt = deltaTicks * ((1/2)/60000UL);
#elif defined(DELTA_FCPU_100M)
	deltaMsFlt = deltaTicks * ((1/2)/100000UL);
#else
#error "'DELTA_FCPU_60M' or 'DELTA_FCPU_100M' must be defined before the inclusion of Delta.h in order to determine the CPU frequency for Delta Timer"
#endif
	deltaMsInt = deltaMsFlt;
}


#endif /* DELTA_H_ */
