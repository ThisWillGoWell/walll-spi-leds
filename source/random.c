/*
 * random.c
 *
 *  Created on: Aug 21, 2019
 *      Author: will
 */

/*******************************************************************************
 * rand
 ******************************************************************************/
#include "fsl_rnga.h"
#include "fsl_debug_console.h"

uint32_t data[1] = {};
void initRgna(){
    RNGA_Init(RNG);
}


uint32_t get_rand_uint32(){
	status_t status = RNGA_GetRandomData(RNG, data, 1);
	if (status == kStatus_Success)
	{
		return data[0];
	}
	else
	{
		PRINTF("RNGA failed!\r\n");
	}
	return 0;
}

uint8_t get_rand_uint8(){
	status_t status = RNGA_GetRandomData(RNG, data, 1);
	if (status == kStatus_Success)
	{
		return 0x000000FF & data[0];
	}
	else
	{
		PRINTF("RNGA failed!\r\n");
	}
	return 0;
}

// end is exclusive
uint32_t get_rand_uint32_range(uint32_t start, uint32_t end){
	return (get_rand_uint32() % (end - start)) + start;
}

uint32_t get_rand_uint8_range(uint8_t start, uint8_t end){
	return (get_rand_uint8() % (end - start)) + start;
}
