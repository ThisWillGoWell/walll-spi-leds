/*
 * power.c
 *
 *  Created on: Aug 21, 2019
 *      Author: will
 */

#ifndef POWER_C_
#define POWER_C_

#include "fsl_port.h"
#include "pin_mux.h"
#include "power.h"

void initPower(){

	gpio_pin_config_t output_config = {
		kGPIO_DigitalOutput, 0,
	};

    CLOCK_EnableClock(kCLOCK_PortC);
	PORT_SetPinMux(POWER_PORT, POWER_PIN, kPORT_MuxAsGpio);

	GPIO_PinInit(POWER_GPIO, POWER_PIN, &output_config);

	 CLOCK_EnableClock(kCLOCK_PortC);
		PORT_SetPinMux(PORTC, 7, kPORT_MuxAsGpio);

		GPIO_PinInit(GPIOC, 7, &output_config);

}

void EnablePower(){
	GPIO_PortClear(POWER_GPIO, 1u << POWER_PIN);
}

void DisablePower(){
	GPIO_PortSet(POWER_GPIO, 1u << POWER_PIN);
}

#endif /* POWER_C_ */
