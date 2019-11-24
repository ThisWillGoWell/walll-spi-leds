/*
 * power.h
 *
 *  Created on: Aug 21, 2019
 *      Author: will
 */

#ifndef POWER_H_
#define POWER_H_

#include "fsl_gpio.h"

#define POWER_GPIO GPIOC
#define POWER_PORT PORTC
#define POWER_PIN 6U
#define POWER_CLOCK kCLOCK_PortC

void EnablePower();
void DisablePower();
void initPower();

#endif /* POWER_H_ */
