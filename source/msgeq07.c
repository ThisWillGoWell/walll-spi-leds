/*
 * msgeq07.c
 *
 *  Created on: Aug 21, 2019
 *      Author: will
 */


/*******************************************************************************
 *
 *
 *
 * MSGEQ07 STUFFS
 *
 *
 *
 ******************************************************************************/


//#include "perfs.h"
//#include "board.h"
//#include "fsl_device_registers.h"
//#include "fsl_dspi_edma.h"
//#include "fsl_edma.h"
//#include "fsl_dmamux.h"
//#include "fsl_adc16.h"
//#include "fsl_tpm.h"
//#include "clock_config.h"
//#include "fsl_dspi.h"

#include "fsl_debug_console.h"
//#include "fsl_i2c.h"
#include "fsl_port.h"

#include "pin_mux.h"
#include "fsl_adc16.h"
#include "fsl_tpm.h"
#include "clock_config.h"
#include "fsl_gpio.h"
#include "msgeq07.h"
/*******************************************************************************
 * ADC
 ******************************************************************************/
#define ADC16_BASE ADC1
#define ADC16_CHANNEL_GROUP 0U
#define ADC16_USER_CHANNEL 18U // left (tip of aux)
#define ADC16_CHANNEL_2 23U // right

#define ADC16_IRQn ADC1_IRQn
#define ADC16_IRQ_HANDLER_FUNC ADC1_IRQHandler

/*******************************************************************************
 * GPIO defines
 ******************************************************************************/

/*******************************************************************************
 * Timer/Pulse Width Modulation Module
 ******************************************************************************/
#define BOARD_TPM TPM2
#define BOARD_TPM_IRQ_NUM TPM2_IRQn
#define BOARD_TPM_HANDLER TPM2_IRQHandler
#define TPM_SOURCE_CLOCK (CLOCK_GetFreq(kCLOCK_PllFllSelClk)/4)
#define TPM_PRESCALER kTPM_Prescale_Divide_4

/*******************************************************************************
 * MSGEQQ07 Timing
 ******************************************************************************/
#define OUTPUT_SETTLE_US 36U
#define RESET_PULSE_WIDTH_US 100U
#define STROBE_TO_STROBE_US 72U
#define NUM_ACD_READS 14
/************************
 * MSGEQQ07 Read State Machine
 ************************/
#define RESET_HIGH_STATE 0
#define STROBE_HIGH_STATE 1
#define STROBE_LOW_STATE 2
#define STROBE_OUTPUT_SETTLE 3
#define READ_CHANNEL_1 4
#define READ_CHANNEL_2 5

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void setResetPin(uint8_t onOff);
void setStrobePin(uint8_t onOff);
void printSpectrum(void);
void startRead(void);
void setNextTimerInterrupt(uint32_t);

/*******************************************************************************
 * Variables
 ******************************************************************************/
adc16_channel_config_t adc16Channel1ConfigStruct;
adc16_channel_config_t adc16Channel2ConfigStruct;
uint8_t state;
uint32_t spectrum[NUM_ACD_READS] = {0};
uint8_t currentRead = 0;
msgeq07_callback current_callback;
void ADC16_IRQ_HANDLER_FUNC(void)
{
	/* Read conversion result to clear the conversion completed flag. */
	spectrum[currentRead] = ADC16_GetChannelConversionValue(ADC16_BASE, ADC16_CHANNEL_GROUP);

	if (state == READ_CHANNEL_2){
		setStrobePin(1);
	}

	if (currentRead != NUM_ACD_READS -1 ) {
		currentRead += 1;
		if(state == READ_CHANNEL_1) {
			state = READ_CHANNEL_2;
			ADC16_SetChannelConfig(ADC16_BASE, ADC16_CHANNEL_GROUP, &adc16Channel2ConfigStruct);
		}else {

			state = STROBE_HIGH_STATE;
			setNextTimerInterrupt(STROBE_TO_STROBE_US);
		}


	} else{
		currentRead = 0;
		current_callback(spectrum);
	}

	 __DSB();
}


void BOARD_TPM_HANDLER(void)
{
	// Stop the timer for now
    TPM_StopTimer(BOARD_TPM);

    switch(state){
    case RESET_HIGH_STATE: // the only diffrence between a first read and subsequent is you set reset 0
    	setResetPin(0);
    case STROBE_HIGH_STATE:
    	setStrobePin(0);
    	state = OUTPUT_SETTLE_US;
    	setNextTimerInterrupt(OUTPUT_SETTLE_US);
    	break;
    case OUTPUT_SETTLE_US:
    	state = READ_CHANNEL_1;
    	// trigger a read
    	ADC16_SetChannelConfig(ADC16_BASE, ADC16_CHANNEL_GROUP, &adc16Channel1ConfigStruct);

    	break;
    }
    /* Clear interrupt flag.*/
    TPM_ClearStatusFlags(BOARD_TPM, kTPM_TimeOverflowFlag);

    __DSB();
}



void getFrequency(msgeq07_callback callback){

	current_callback = callback;
	state = RESET_HIGH_STATE;
	//reset high
	setResetPin(1);
	setStrobePin(0);
	setNextTimerInterrupt(RESET_PULSE_WIDTH_US);
}

/*******************************************************************************
 * Timer
 ******************************************************************************/

void initMsgeq07Timer(void){
	/* TPM known issue of KL81, enable clock of TPM0 to use other TPM module */
	CLOCK_EnableClock(kCLOCK_Tpm2);
	/* Select the clock source for the TPM counter as kCLOCK_PllFllSelClk */
	CLOCK_SetTpmClock(1U);

	tpm_config_t tpmInfo;
	TPM_GetDefaultConfig(&tpmInfo);
	/* TPM clock divide by TPM_PRESCALER */
	tpmInfo.prescale = TPM_PRESCALER;


	TPM_Init(BOARD_TPM, &tpmInfo);/* Initialize TPM module */

	// enable the TPM interrupt out of all interrupts
	EnableIRQ(BOARD_TPM_IRQ_NUM);
}

void initMsgEq07Pins(void){

	gpio_pin_config_t output_config = {
		kGPIO_DigitalOutput, 0,
	};

	CLOCK_EnableClock(STROBE_CLOCK);
	CLOCK_EnableClock(RESET_CLOCK);

	PORT_SetPinMux(RESET_PORT, RESET_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(STROBE_PORT, STROBE_PIN, kPORT_MuxAsGpio);

	GPIO_PinInit(RESET_GPIO, RESET_PIN, &output_config);
	GPIO_PinInit(STROBE_GPIO, STROBE_PIN, &output_config);
}

void initMsgeq07Adc(){
	adc16_config_t adc16ConfigStruct;

	EnableIRQ(ADC16_IRQn);
	ADC16_GetDefaultConfig(&adc16ConfigStruct);
	ADC16_Init(ADC16_BASE, &adc16ConfigStruct);
	ADC16_EnableHardwareTrigger(ADC16_BASE, false); /* Make sure the software trigger is used. */

	if (kStatus_Success == ADC16_DoAutoCalibration(ADC16_BASE))
	{
		PRINTF("ADC16_DoAutoCalibration() Done.\r\n");
	}
	else
	{
		PRINTF("ADC16_DoAutoCalibration() Failed.\r\n");
	}

	adc16Channel1ConfigStruct.channelNumber = ADC16_USER_CHANNEL;
	adc16Channel1ConfigStruct.enableInterruptOnConversionCompleted = true; /* Enable the interrupt. */
	adc16Channel1ConfigStruct.enableDifferentialConversion = false;

	adc16Channel2ConfigStruct.channelNumber = ADC16_CHANNEL_2;
	adc16Channel2ConfigStruct.enableInterruptOnConversionCompleted = true; /* Enable the interrupt. */
	adc16Channel2ConfigStruct.enableDifferentialConversion = false;
}

void setNextTimerInterrupt(uint32_t us) {
    TPM_SetTimerPeriod(BOARD_TPM, USEC_TO_COUNT(us, TPM_SOURCE_CLOCK));
    TPM_EnableInterrupts(BOARD_TPM, kTPM_TimeOverflowInterruptEnable);
    TPM_StartTimer(BOARD_TPM, kTPM_SystemClock);
}


void setStrobePin(uint8_t onOff){
	if(onOff){
		GPIO_PortSet(STROBE_GPIO, 1u << STROBE_PIN);
	} else {
		GPIO_PortClear(STROBE_GPIO, 1u << STROBE_PIN);
	}
}

void setResetPin(uint8_t onOff){
	if(onOff){
		GPIO_PortSet(RESET_GPIO, 1u << RESET_PIN);
	} else {
		GPIO_PortClear(RESET_GPIO, 1u << RESET_PIN);
	}
}

void initMsgeq07(){
	initMsgeq07Timer();
	initMsgeq07Adc();
	initMsgEq07Pins();
}
