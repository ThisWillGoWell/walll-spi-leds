/*
 * random.h
 *
 *  Created on: Aug 21, 2019
 *      Author: will
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdint.h>

uint32_t get_rand_uint32();
uint32_t get_rand_uint32_range(uint32_t start, uint32_t end);
uint8_t get_rand_uint8();
uint32_t get_rand_uint8_range(uint8_t start, uint8_t end);
void initRgna();

#endif /* RANDOM_H_ */
