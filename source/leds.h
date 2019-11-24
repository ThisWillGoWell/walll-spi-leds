/*
 * leds.h
 *
 *  Created on: Aug 21, 2019
 *      Author: will
 */

#ifndef LEDS_H_
#define LEDS_H_


#define NUM_LEDS_PER_BUS 1156U
#define NUM_LEDS NUM_LEDS_PER_BUS * 3
#define TRANSFER_BAUDRATE 1450000U //1400000U /* Transfer baudrate - 14.5MHz */5
#define END_BYTES 100
#define START_BYTES 8
void initLeds();
void setLed(uint32_t tube, uint32_t i,uint8_t r, uint8_t g, uint8_t b);



#endif /* LEDS_H_ */
