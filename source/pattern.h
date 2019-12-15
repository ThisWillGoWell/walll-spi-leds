/*
 * pattern.h
 *
 *  Created on: Jun 17, 2019
 *      Author: wggowell
 */

#ifndef PATTERN_H_
#define PATTERN_H_
#include "stdint.h"
#include "colors.h"
/*****************************************************************************
 * Helper Prototypes
*****************************************************************************/
void setLedHsv(int tube, uint32_t i, HsvColor c);
void setLedRgb(int tube, uint32_t i, RgbColor c);


	#define NUM_TUBES 1
	#define LEDS_PER_TUBE 1
	void runPatterns(void);
	void initPatterns(void);
	void fade_all_and_set(uint8_t amount);
	typedef void (*PatternFunc)(uint32_t counter);

	typedef struct Pattern
	{
		uint32_t defaultMsDealy;
		PatternFunc func;
	} Pattern;


#endif /* PATTERN_H_ */
