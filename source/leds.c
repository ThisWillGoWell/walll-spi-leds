
/*******************************************************************************
 *
 *
 * DMA STUFFS
 *
 *
 *
 ******************************************************************************/

/*******************************************************************************
 * DMA Defines
 *  - DSPI
 ******************************************************************************/
#include "board.h"
#include "fsl_dspi_edma.h"
#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "fsl_dspi.h"
#include "fsl_debug_console.h"
#include "leds.h"

#define BRIGHTNESS 0xFF

#define DMA_MUX_BASEADDR DMAMUX
#define DSPI_MASTER_DMA_BASEADDR DMA0
#define DSPIn_MASTER_PCS_FOR_INIT kDSPI_Pcs0
#define DSPI_MASTER_PCS_FOR_TRANSFER kDSPI_MasterPcs0
#define TRANSFER_SIZE NUM_LEDS_PER_BUS * 4 + START_BYTES + END_BYTES /* Transfer dataSize */

#define DSPI0_MASTER_CLK_SRC DSPI0_CLK_SRC
#define DSPI0_MASTER_CLK_FREQ CLOCK_GetFreq(DSPI0_CLK_SRC)

#define DSPI0_MASTER_BASEADDR SPI0
#define DSPI0_MASTER_DMA_RX_REQUEST_SOURCE kDmaRequestMux0SPI0Rx
#define DSPI0_MASTER_DMA_TX_REQUEST_SOURCE kDmaRequestMux0SPI0Tx

#define DSPI1_MASTER_BASEADDR SPI1
#define DSPI1_MASTER_DMA_RX_REQUEST_SOURCE kDmaRequestMux0SPI1Rx
#define DSPI1_MASTER_DMA_TX_REQUEST_SOURCE kDmaRequestMux0SPI1Tx

#define DSPI2_MASTER_BASEADDR SPI2
#define DSPI2_MASTER_DMA_RX_REQUEST_SOURCE kDmaRequestMux0SPI2Rx
#define DSPI2_MASTER_DMA_TX_REQUEST_SOURCE kDmaRequestMux0SPI2Tx

#define SPI0_RX_CHANNEL 0U
#define SPI0_TX_INTER_CHANNEL 1U
#define SPI0_TX_CHANNEL 2U

#define SPI1_RX_CHANNEL 3U
#define SPI1_TX_INTER_CHANNEL 4U
#define SPI1_TX_CHANNEL 5U

#define SPI2_RX_CHANNEL 6U
#define SPI2_TX_INTER_CHANNEL 7U
#define SPI2_TX_CHANNEL 8U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* DSPI user callback */
void DSPI0_Callback(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *userData);
void DSPI1_Callback(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *userData);
void DSPI2_Callback(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *userData);


/******************************************************************************
 * DMA Variables
 ******************************************************************************/
edma_handle_t dspiEdmaMasterRxRegToRxDataHandle;
edma_handle_t dspiEdmaMasterTxDataToIntermediaryHandle;
edma_handle_t dspiEdmaMasterIntermediaryToTxRegHandle;

edma_handle_t dspi1EdmaMasterRxRegToRxDataHandle;
edma_handle_t dspi1EdmaMasterTxDataToIntermediaryHandle;
edma_handle_t dspi1EdmaMasterIntermediaryToTxRegHandle;

edma_handle_t dspi2EdmaMasterRxRegToRxDataHandle;
edma_handle_t dspi2EdmaMasterTxDataToIntermediaryHandle;
edma_handle_t dspi2EdmaMasterIntermediaryToTxRegHandle;

dspi_master_edma_handle_t g_dspi0_edma_m_handle;
dspi_master_edma_handle_t g_dspi1_edma_m_handle;
dspi_master_edma_handle_t g_dspi2_edma_m_handle;

// DSPI transfer objects
dspi_transfer_t spi0Xfer;
dspi_transfer_t spi1Xfer;
dspi_transfer_t spi2Xfer;

uint8_t activeTransfer;
// DSPI transfer arrays
uint8_t leds0[2][TRANSFER_SIZE] = {0};
uint8_t leds1[2][TRANSFER_SIZE] = {0};
uint8_t leds2[2][TRANSFER_SIZE] = {0};



void DSPI0_Callback(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *userData)
{
	 DSPI_MasterTransferEDMA(DSPI0_MASTER_BASEADDR, &g_dspi0_edma_m_handle, &spi0Xfer);
	 __DSB();
}

void DSPI1_Callback(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *userData)
{
	DSPI_MasterTransferEDMA(DSPI1_MASTER_BASEADDR, &g_dspi1_edma_m_handle, &spi1Xfer);
	__DSB();
}

void DSPI2_Callback(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *userData)
{
	DSPI_MasterTransferEDMA(DSPI2_MASTER_BASEADDR, &g_dspi2_edma_m_handle, &spi2Xfer);
	__DSB();

}

void Update(){

	 spi0Xfer.txData = leds0[activeTransfer];
	 spi1Xfer.txData = leds1[activeTransfer];
	 spi2Xfer.txData = leds2[activeTransfer];
	 activeTransfer = (activeTransfer + 1) % 2;

	 while(DSPI_MasterTransferEDMA(DSPI0_MASTER_BASEADDR, &g_dspi0_edma_m_handle, &spi0Xfer) == kStatus_DSPI_Busy);
	 while(DSPI_MasterTransferEDMA(DSPI1_MASTER_BASEADDR, &g_dspi1_edma_m_handle, &spi1Xfer) == kStatus_DSPI_Busy);
	 while(DSPI_MasterTransferEDMA(DSPI2_MASTER_BASEADDR, &g_dspi2_edma_m_handle, &spi2Xfer) == kStatus_DSPI_Busy);

}


void initEdma(void){

    uint32_t DSPI0TxChannel,DSPI0RxChannel, DSPI0IntermediaryChannel;
	uint32_t DSPI1TxChannel, DSPI1RxChannel, DSPI1IntermediaryChannel;
	uint32_t DSPI2TxChannel, DSPI2RxChannel, DSPI2IntermediaryChannel;

	DSPI0TxChannel = 1U;
	DSPI0IntermediaryChannel = 2U;
	DSPI0RxChannel = 0U;

	DSPI1TxChannel = 3U;
	DSPI1IntermediaryChannel = 4U;
	DSPI1RxChannel = 5U;

	DSPI2TxChannel = 6U;
	DSPI2IntermediaryChannel = 7U;
	DSPI2RxChannel = 8U;


	DMAMUX_Init(DMA_MUX_BASEADDR);


    // enable DMA channels
    // SPI0 Source
	// SPI0 Rx
    DMAMUX_SetSource(DMA_MUX_BASEADDR, DSPI0RxChannel,
                     (uint8_t)DSPI0_MASTER_DMA_RX_REQUEST_SOURCE);
    DMAMUX_EnableChannel(DMA_MUX_BASEADDR, DSPI0RxChannel);
    // SP0 TX
    DMAMUX_SetSource(DMA_MUX_BASEADDR, DSPI0TxChannel,
                     (uint8_t)DSPI0_MASTER_DMA_TX_REQUEST_SOURCE);
    DMAMUX_EnableChannel(DMA_MUX_BASEADDR, DSPI0TxChannel);

    // SPI1 Source
    // SPI1 Rx
    DMAMUX_SetSource(DMA_MUX_BASEADDR, DSPI1RxChannel,
    	                     (uint8_t)DSPI1_MASTER_DMA_RX_REQUEST_SOURCE);
	DMAMUX_EnableChannel(DMA_MUX_BASEADDR, DSPI1RxChannel);
	//SPI1Tx
    DMAMUX_SetSource(DMA_MUX_BASEADDR, DSPI1TxChannel,
   	                     (uint8_t)DSPI1_MASTER_DMA_TX_REQUEST_SOURCE);
	DMAMUX_EnableChannel(DMA_MUX_BASEADDR, DSPI1TxChannel);


	// SPI2 Source
	// SPI2 Rx
	DMAMUX_SetSource(DMA_MUX_BASEADDR, DSPI2RxChannel,
							 (uint8_t)DSPI2_MASTER_DMA_RX_REQUEST_SOURCE);
	DMAMUX_EnableChannel(DMA_MUX_BASEADDR, DSPI2RxChannel);
	//SPI2Tx
	DMAMUX_SetSource(DMA_MUX_BASEADDR, DSPI2TxChannel,
						 (uint8_t)DSPI2_MASTER_DMA_TX_REQUEST_SOURCE);
	DMAMUX_EnableChannel(DMA_MUX_BASEADDR, DSPI2TxChannel);

	/* EDMA init */
	/*
	 * userConfig.enableRoundRobinArbitration = false;
	 * userConfig.enableHaltOnError = true;
	 * userConfig.enableContinuousLinkMode = false;
	 * userConfig.enableDebugMode = false;
	 */
    edma_config_t userConfig;
	EDMA_GetDefaultConfig(&userConfig);

	EDMA_Init(DSPI_MASTER_DMA_BASEADDR, &userConfig);



	// configure DataHandle
		    // SPI0 RX
		    memset(&(dspiEdmaMasterRxRegToRxDataHandle), 0, sizeof(dspiEdmaMasterRxRegToRxDataHandle));
		    // SPI0 TX: Data -> intermediary
		    memset(&(dspiEdmaMasterTxDataToIntermediaryHandle), 0, sizeof(dspiEdmaMasterTxDataToIntermediaryHandle));
		    // SPI0 TX: intermediary -> TX
		    memset(&(dspiEdmaMasterIntermediaryToTxRegHandle), 0, sizeof(dspiEdmaMasterIntermediaryToTxRegHandle));


		    EDMA_CreateHandle(&(dspiEdmaMasterRxRegToRxDataHandle), DSPI_MASTER_DMA_BASEADDR, DSPI0RxChannel);
		    EDMA_CreateHandle(&(dspiEdmaMasterTxDataToIntermediaryHandle), DSPI_MASTER_DMA_BASEADDR,
		                      DSPI0IntermediaryChannel);
		    EDMA_CreateHandle(&(dspiEdmaMasterIntermediaryToTxRegHandle), DSPI_MASTER_DMA_BASEADDR, DSPI0TxChannel);

		    // Create the MAsterTransferHandle
		    DSPI_MasterTransferCreateHandleEDMA(DSPI0_MASTER_BASEADDR, &g_dspi0_edma_m_handle, DSPI0_Callback,
		                                        NULL, &dspiEdmaMasterRxRegToRxDataHandle,
		                                        &dspiEdmaMasterTxDataToIntermediaryHandle,
		                                        &dspiEdmaMasterIntermediaryToTxRegHandle);

		    // SPI1 RX
		    memset(&(dspi1EdmaMasterRxRegToRxDataHandle), 0, sizeof(dspi1EdmaMasterRxRegToRxDataHandle));
			EDMA_CreateHandle(&(dspi1EdmaMasterRxRegToRxDataHandle), DSPI_MASTER_DMA_BASEADDR, DSPI1RxChannel);

			//SPI1: Data -> intermediary
		    memset(&(dspi1EdmaMasterTxDataToIntermediaryHandle), 0, sizeof(dspi1EdmaMasterTxDataToIntermediaryHandle));
		    EDMA_CreateHandle(&(dspi1EdmaMasterTxDataToIntermediaryHandle), DSPI_MASTER_DMA_BASEADDR, DSPI1IntermediaryChannel);

		    //SPI0 TX: intermediary -> TX
		    memset(&(dspi1EdmaMasterIntermediaryToTxRegHandle), 0, sizeof(dspi1EdmaMasterIntermediaryToTxRegHandle));
		    EDMA_CreateHandle(&(dspi1EdmaMasterIntermediaryToTxRegHandle), DSPI_MASTER_DMA_BASEADDR, DSPI1TxChannel);
		    // Create the MasterTransferHandle
		    DSPI_MasterTransferCreateHandleEDMA(DSPI1_MASTER_BASEADDR, &g_dspi1_edma_m_handle, DSPI1_Callback,
		                                        NULL, &dspi1EdmaMasterRxRegToRxDataHandle,
		                                        &dspi1EdmaMasterTxDataToIntermediaryHandle,
		                                        &dspi1EdmaMasterIntermediaryToTxRegHandle);

		    // SPI2 RX
			memset(&(dspi2EdmaMasterRxRegToRxDataHandle), 0, sizeof(dspi2EdmaMasterRxRegToRxDataHandle));
			EDMA_CreateHandle(&(dspi2EdmaMasterRxRegToRxDataHandle), DSPI_MASTER_DMA_BASEADDR, DSPI2RxChannel);

			//SPI2: Data -> intermediary
			memset(&(dspi2EdmaMasterTxDataToIntermediaryHandle), 0, sizeof(dspi2EdmaMasterTxDataToIntermediaryHandle));
			EDMA_CreateHandle(&(dspi2EdmaMasterTxDataToIntermediaryHandle), DSPI_MASTER_DMA_BASEADDR, DSPI2IntermediaryChannel);

			//SPI2 TX: intermediary -> TX
			memset(&(dspi2EdmaMasterIntermediaryToTxRegHandle), 0, sizeof(dspi2EdmaMasterIntermediaryToTxRegHandle));
			EDMA_CreateHandle(&(dspi2EdmaMasterIntermediaryToTxRegHandle), DSPI_MASTER_DMA_BASEADDR, DSPI2TxChannel);
			// Create the MasterTransferHandle
			DSPI_MasterTransferCreateHandleEDMA(DSPI2_MASTER_BASEADDR, &g_dspi2_edma_m_handle, DSPI2_Callback,
												NULL, &dspi2EdmaMasterRxRegToRxDataHandle,
												&dspi2EdmaMasterTxDataToIntermediaryHandle,
												&dspi2EdmaMasterIntermediaryToTxRegHandle);
}


void initDspi(void){
	 /* Port B Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortB);
	/* Port D Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortD);

    /* PORTD1 (pin D4) is configured as SPI0_SCK */
    PORT_SetPinMux(PORTD, 1U, kPORT_MuxAlt2); // SPI0 Clock
    PORT_SetPinMux(PORTD, 2U, kPORT_MuxAlt2); // SPI0 Data

	PORT_SetPinMux(PORTD, 6U, kPORT_MuxAlt7); // SPI1 Data
	PORT_SetPinMux(PORTD, 5U, kPORT_MuxAlt7);// SPI1 Clock

	PORT_SetPinMux(PORTD, 12U, kPORT_MuxAlt2); //SPI2 clock
	PORT_SetPinMux(PORTD, 13U, kPORT_MuxAlt2); // SPI2 Data

	dspi_master_config_t masterConfig;
	uint32_t srcClock_Hz;

	masterConfig.whichCtar = kDSPI_Ctar0;
	masterConfig.ctarConfig.baudRate = TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.bitsPerFrame = 8;
	masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
	masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
	masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
	masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 0;
	masterConfig.whichPcs = DSPIn_MASTER_PCS_FOR_INIT;
	masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
	masterConfig.enableContinuousSCK = false;
	masterConfig.enableRxFifoOverWrite = false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint = kDSPI_SckToSin0Clock;
	srcClock_Hz = DSPI0_MASTER_CLK_FREQ;

	DSPI_MasterInit(DSPI0_MASTER_BASEADDR, &masterConfig, srcClock_Hz);
	DSPI_MasterInit(DSPI1_MASTER_BASEADDR, &masterConfig, srcClock_Hz);
	DSPI_MasterInit(DSPI2_MASTER_BASEADDR, &masterConfig, srcClock_Hz);

	// init transfer objects
	spi0Xfer.txData = leds0;
	spi0Xfer.rxData = NULL;
	spi0Xfer.dataSize = TRANSFER_SIZE;
	spi0Xfer.configFlags = kDSPI_MasterCtar0 | DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;

	spi1Xfer.txData = leds1;
	spi1Xfer.rxData = NULL;
	spi1Xfer.dataSize = TRANSFER_SIZE;
	spi1Xfer.configFlags = kDSPI_MasterCtar0 | DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;


	spi2Xfer.txData = leds2;
	spi2Xfer.rxData = NULL;
	spi2Xfer.dataSize = TRANSFER_SIZE;
	spi2Xfer.configFlags = kDSPI_MasterCtar0 | DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;




}

// Power Calculations
#define mA 1000 // 1000 us
#define Amp 1000000
#define BASE_LED_COST  1000 // 1 mA, 1000uA when the lds are off to power
#define BASE_CURRENT  NUM_LEDS * BASE_LED_COST

#define UA_PER_LED  4531;

uint32_t currentPower = 0;


void set_led_strip(uint8_t *leds, int index,  uint8_t r, uint8_t g, uint8_t b){
	uint32_t startPower = currentPower;
	currentPower -= leds[index+1];
	currentPower -= leds[index+2];
	currentPower -= leds[index+3];

	if(currentPower > startPower){
		currentPower = 0;
	}
	leds[index + 1] =  b;
	leds[index + 2] = g;
	leds[index + 3] = r;

	currentPower += r;
	currentPower += g;
	currentPower += b;
}


void setLed(uint32_t tube, uint32_t i, uint8_t r, uint8_t g, uint8_t b){
	if (tube % 6 >= 3) {
		i = 68 - i - 1;
	}
	int index = (tube / 3) *  (68 * 4) + i * 4 + 4;
	switch(tube % 3){
	case 0:
		set_led_strip(&leds0[activeTransfer], index, r,g,b);
		break;
	case 1:
		set_led_strip(&leds1[activeTransfer], index, r,g,b);
		break;
	case 2:
		set_led_strip(&leds2[activeTransfer], index, r,g,b);
		break;
	}
}

void initLeds(){
	initEdma();
	initDspi();

	for(int j=0;j<2;j++){
		// set the global brightness of each bus
		for(int i=0;i<NUM_LEDS_PER_BUS;i++){
			leds0[j][START_BYTES+i*4] = BRIGHTNESS;
			leds1[j][START_BYTES+i*4] = BRIGHTNESS;
			leds2[j][START_BYTES+i*4] = BRIGHTNESS;
		}

		// initlize the end frame for each bus
		for(int i=TRANSFER_SIZE-1; i > TRANSFER_SIZE - END_BYTES-1; i--){
			leds0[j][i]=0xFF;
			leds1[j][i]=0xFF;
			leds2[j][i]=0xFF;
		}
	}



	if (kStatus_Success != DSPI_MasterTransferEDMA(DSPI0_MASTER_BASEADDR, &g_dspi0_edma_m_handle, &spi0Xfer))
	{
		PRINTF("There is error when start DSPI_MasterTransferEDMA \r\n ");
	}
	if (kStatus_Success != DSPI_MasterTransferEDMA(DSPI1_MASTER_BASEADDR, &g_dspi1_edma_m_handle, &spi1Xfer))
	{
		PRINTF("There is error when start DSPI_MasterTransferEDMA \r\n ");
	}

	if (kStatus_Success != DSPI_MasterTransferEDMA(DSPI2_MASTER_BASEADDR, &g_dspi2_edma_m_handle, &spi2Xfer))
	{
		PRINTF("There is error when start DSPI_MasterTransferEDMA \r\n ");
	}
}


