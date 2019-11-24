/*
 * main.c
 *
 *  Created on: Jun 17, 2019
 *      Author: wggowell
 */
#include "board.h"
#include "leds.h"
#include "random.h"
#include "msgeq07.h"
#include "controller.h"
#include "pattern.h"
#include "rtc.h"
#include "power.h"
#include "fsl_debug_console.h"
#include "scheduler.h"

int main(void){
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    initLeds();
    initRgna();
    initMsgeq07();
    initController();
    initPower();
    initPatterns();
    //initRtc();
    //initScheduler();
    //EnableAuto();

    runPatterns();


    while(1){
    	updateTime();
    	PRINTF(stringTimeStamp());
    }
    return 0;

}

