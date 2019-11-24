#include "fsl_uart.h"
#include "controller.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_port.h"
#include "fsl_debug_console.h"


void squareEvent();
void triEvent();
void circleEvent();
void xEvent();
void upEvent();
void downEvent();
void leftEvent();
void rightEvent();
void startEvent();
void selectEvent();

void nextMode();
void previousMode();

uint8_t mode = 0;

#define NUM_COMMANDS 10

void (*events[])() = {
    squareEvent, // 0
    triEvent, // 1
    circleEvent, // 2
    xEvent, // 3
    upEvent, // 4
    downEvent, // 5
    leftEvent, // 6
    rightEvent, // 7
    startEvent, // 8
    selectEvent // 9
};

int commandIndex = 0;
void UART_IRQHandler(void)
{
    uint8_t data;

    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART))
    {
    	data = UART_ReadByte(UART);
    	if(data == '?'){
    		if(commandIndex >= NUM_COMMANDS){
    			PRINTF("got invalid command index");
    		} else {
    			events[commandIndex]();
    			commandIndex = 0;
    		}
    	} else {
    		commandIndex = commandIndex * 10 + data - '0';
    	}
    }
    __DSB();
}

void squareEvent(){
	mode = 1;
}
void triEvent(){
	mode = 2;
}
void circleEvent(){
	mode = 3;
}
void xEvent(){
	mode = 4;
}
void upEvent(){

}
void downEvent(){

}
void leftEvent(){
	previousMode();
}
void rightEvent(){
	nextMode();
}
void startEvent(){

}
void selectEvent(){

}

void previousMode(){
    if(mode == 0){
        mode = NUM_MODES - 1;
    } else{
        mode = mode - 1;
    }

}

void nextMode(){

    if(mode == 0){
        mode = NUM_MODES - 1;
    } else{
        mode = mode - 1;
    }

}

void initUartPins(void)
{
    /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PORTB16 (pin E10) is configured as UART0_RX */
    PORT_SetPinMux(PORTC, 4U, kPORT_MuxAlt3);

    /* PORTB17 (pin E9) is configured as UART0_TX */
    PORT_SetPinMux(PORTC, 3U, kPORT_MuxAlt3);

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART0TXSRC_MASK)))

                  /* UART 0 transmit data source select: UART0_TX pin. */
                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));
}


void sendUart(uint8_t * send, int length ){
	for(int i=0;i<length;i++){
		while (kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(UART))
		{
			UART_WriteByte(UART, send[i]);
		}
	}

}


uint8_t getMode(){
	return mode;
}

void initController(){
	initUartPins();
	uart_config_t config;
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = UART_BAUD;
	config.enableTx = true;
	config.enableRx = true;

	UART_Init(UART, &config, UART_CLK_FREQ);

    UART_EnableInterrupts(UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(UART_IRQn);

}
