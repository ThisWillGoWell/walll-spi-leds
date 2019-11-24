/*
 * alarm.c
 *
 *  Created on: Aug 23, 2019
 *      Author: will
 */

#include "scheduler.h"
#include "rtc.h"
#include "power.h"

#include "fsl_ftm.h"

#define BOARD_FTM_BASEADDR FTM3

/* Interrupt number and interrupt handler for the FTM instance used */
#define BOARD_FTM_IRQ_NUM FTM3_IRQn
#define BOARD_FTM_HANDLER FTM3_IRQHandler

/* Get source clock for FTM driver */
#define FTM_SOURCE_CLOCK (CLOCK_GetFreq(kCLOCK_BusClk)/4)

uint32_t milisecondCounts = 0U;

void initScheduler(){
    ftm_config_t ftmInfo;
    FTM_GetDefaultConfig(&ftmInfo);

	/* Divide FTM clock by 4 */
	ftmInfo.prescale = kFTM_Prescale_Divide_4;

	/* Initialize FTM module */
	FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);

	/*
	 * Set timer period.
	*/
	FTM_SetTimerPeriod(BOARD_FTM_BASEADDR, USEC_TO_COUNT(1000U, FTM_SOURCE_CLOCK));

	FTM_EnableInterrupts(BOARD_FTM_BASEADDR, kFTM_TimeOverflowInterruptEnable);

	EnableIRQ(BOARD_FTM_IRQ_NUM);

	FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);

    FTM_SetTimerPeriod(BOARD_FTM_BASEADDR, USEC_TO_COUNT(1000U, FTM_SOURCE_CLOCK));


}


bool autoEnabled;
void EnableAuto(){
	autoEnabled = true;
}

void DisableAuto(){
	autoEnabled = false;
}

void autoPower(){
	updateTime();
	uint8_t currentHour = getHours();
	uint8_t currentMin = getMinutes();
	if(currentHour >= OnHour){
		if(currentMin >= OnMin){
			EnablePower();
		} else{
			DisablePower();
		}
	}else if (currentHour <= OffHour){
		if(currentMin < OffMin) {
			EnablePower();
		} else{
			DisablePower();
		}
	} else{
		DisablePower();
	}
}

void nextPattern(){

}
/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_FTM_HANDLER(void)
{
    /* Clear interrupt flag.*/
    FTM_ClearStatusFlags(BOARD_FTM_BASEADDR, kFTM_TimeOverflowFlag);


    if(autoEnabled){
    	if(milisecondCounts % PATTERN_CHANGE_RATE == 0){
    		nextPattern();
    	}
    	 if(milisecondCounts % POWER_CHECK_RATE == 0){
 			autoPower();
		}

    }

    __DSB();
}



