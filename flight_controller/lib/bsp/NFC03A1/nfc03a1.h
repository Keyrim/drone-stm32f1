/*
 * nfc03a1.h
 *
 *  Created on: 1 janv. 2017
 *      Author: Julie
 */

#ifndef BSP_NFC03A1_EXAMPLES_NFC03A1_H_
#define BSP_NFC03A1_EXAMPLES_NFC03A1_H_

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#if USE_NFC03A1

#include "lib_nfc/common/lib_95HFConfigManager.h"
#include "../stm32f1_uart.h"
#include "../stm32f1_spi.h"
#include "lib_nfc/lib_pcd/lib_iso14443A.h"

#ifndef NFC_UART_ID
	#define NFC_UART_ID		UART1_ID
#endif
#ifndef NFC_SPI
	#define NFC_SPI			SPI1
#endif

#define USE_NFC_WITH_SPI	1
#define USE_NFC_WITH_UART	0

#if ((USE_NFC_WITH_UART+USE_NFC_WITH_SPI)!=1)
	#error	"you must chose one of the two modes : USE_NFC_WITH_SPI or USE_NFC_WITH_UART !"
#endif
#if USE_NFC_WITH_UART
	#warning "ce mode n'est pas fonctionnel... "
#endif

#define RFTRANS_95HF_SPI_NSS_GPIO_PORT	GPIOB
#define RFTRANS_95HF_SPI_NSS_PIN		GPIO_PIN_6
#define RFTRANS_95HF_NSS_LOW()			HAL_GPIO_WritePin(RFTRANS_95HF_SPI_NSS_GPIO_PORT, RFTRANS_95HF_SPI_NSS_PIN, 0)
#define RFTRANS_95HF_NSS_HIGH()			HAL_GPIO_WritePin(RFTRANS_95HF_SPI_NSS_GPIO_PORT, RFTRANS_95HF_SPI_NSS_PIN, 1)

//#define RFTRANS_95HF_IRQ_HANDLER        EXTI15_10_IRQHandler
#define IRQOUT_RFTRANS_95HF_PORT		GPIOA
#define IRQOUT_RFTRANS_95HF_PIN			GPIO_PIN_10
#define IRQIN_RFTRANS_GPIO_PORT			GPIOA
#define IRQIN_RFTRANS_95HF_PIN			GPIO_PIN_9

#define	RFTRANS_95HF_IRQIN_HIGH()		HAL_GPIO_WritePin(IRQIN_RFTRANS_GPIO_PORT, IRQIN_RFTRANS_95HF_PIN, 1)
#define	RFTRANS_95HF_IRQIN_LOW()		HAL_GPIO_WritePin(IRQIN_RFTRANS_GPIO_PORT, IRQIN_RFTRANS_95HF_PIN, 0)

#define NFC_INTERFACE_GPIO_PORT			GPIOC
#define NFC_INTERFACE_PIN				GPIO_PIN_7
/* Exported types ------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define BULK_MAX_PACKET_SIZE            0x00000040

/* Regarding board antenna (and matching) appropriate
value may be modified to optimized RF performances */
/* Analogue configuration register
 ARConfigB	bits  7:4	MOD_INDEX	Modulation index to modulator
                      3:0	RX_AMP_GAIN	Defines receiver amplifier gain
For type A you can also adjust the Timer Window
*/

/******************  PCD  ******************/
/* ISO14443A */
#define PCD_TYPEA_ARConfigA	        0x01
#define PCD_TYPEA_ARConfigB	        0xDF

#define PCD_TYPEA_TIMERW            0x5A

/* ISO14443B */
#define PCD_TYPEB_ARConfigA	        0x01
#define PCD_TYPEB_ARConfigB	        0x51

/* Felica */
#define PCD_TYPEF_ARConfigA	        0x01
#define PCD_TYPEF_ARConfigB	        0x51

/* ISO15693 */
#define PCD_TYPEV_ARConfigA	        0x01
#define PCD_TYPEV_ARConfigB	        0xD1

/******************  PICC  ******************/
/* ISO14443A */
#define PICC_TYPEA_ACConfigA            0x27  /* backscaterring */

/* ISO14443B */
#define PICC_TYPEB_ARConfigD            0x0E  /* card demodulation gain */
#define PICC_TYPEB_ACConfigA            0x17  /* backscaterring */

/* Felica */
#define PICC_TYPEF_ACConfigA            0x17  /* backscaterring */

/* Exported constants --------------------------------------------------------*/
#if USE_NFC_WITH_SPI
#define SPI_INTERRUPT_MODE_ACTIVATED
#endif

/**
 * @brief  TIMER definitions
 */

/* --------------------------------------------------------------------------
* Delay TIMER configuration (ms)
* -------------------------------------------------------------------------- */
#define TIMER_DELAY					TIM2
#define TIMER_DELAY_PERIOD			72
#define TIMER_DELAY_PRESCALER		1000

/* --------------------------------------------------------------------------
* Delay TIMER configuration (탎)
* --------------------------------------------------------------------------- */
#define TIMER_US_DELAY				TIM2
#define TIMER_US_DELAY_PERIOD		35
#define TIMER_US_DELAY_PRESCALER	1

/*----------------------------------------------------------------------------*/
#define TIMER_TIMEOUT               		TIM3
#define APPLI_TIMER_TIMEOUT					TIM4
#define TIMER_TIMEOUT_IRQ_CHANNEL			TIM3_IRQn
#define APPLI_TIMER_TIMEOUT_IRQ_CHANNEL		TIM4_IRQn
#define EXTI_RFTRANS_95HF_IRQ_CHANNEL		EXTI15_10_IRQn
#define TIMER_TIMEOUT_IRQ_HANDLER			TIM3_IRQHandler
#define APPLI_TIMER_TIMEOUT_IRQ_HANDLER		TIM4_IRQHandler


/*----------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
* timeout configuration (ms)
* --------------------------------------------------------------------------
* 72 MHz / 72 = 1MHz (1us )
* 1탎 * 1000 + 1탎 ~= 1ms
* -------------------------------------------------------------------------- */

#define TIMER_TIMEOUT_PERIOD		1000
#define TIMER_TIMEOUT_PRESCALER		72

/* --------------------------------------------------------------------------
* timeout configuration (ms)
* --------------------------------------------------------------------------
* 72 MHz / 72 = 1MHz (1us )
* 1탎 * 1000 + 1탎 ~= 1ms
* -------------------------------------------------------------------------- */

#define APPLI_TIMER_TIMEOUT_PERIOD		1000
#define APPLI_TIMER_TIMEOUT_PRESCALER	72

/**
 * @brief  NVIC definitions
 */

/**
 *	@brief  Interrupts configuration
|---------------|-----------------------|-------------------|-------------------|
|	 name	|preemption priority	|sub proiority	    |	channel	        |
|---------------|-----------------------|-------------------|-------------------|
| 95HF		|	0		|	0           |	EXTI15_10_IRQn	|
|---------------|-----------------------|-------------------|-------------------|
| delay		|	0		|	0           |	TIM2_IRQn	|
|---------------|-----------------------|-------------------|-------------------|
| timeout	|	0		|	0           |	TIM3_IRQn	|
|---------------|-----------------------|-------------------|-------------------|
| appli timeout	|	0		|	0           |	TIM4_IRQn	|
|---------------|-----------------------|-------------------|-------------------|
 */
#define EXTI_RFTRANS_95HF_PREEMPTION_PRIORITY   1
#define EXTI_RFTRANS_95HF_SUB_PRIORITY		1

#define TIMER_DELAY_PREEMPTION_PRIORITY		1
#define TIMER_DELAY_SUB_PRIORITY			3
#define TIMER_DELAY_IRQ_CHANNEL				TIM2_IRQn
#define TIMER_DELAY_PREEMPTION_HIGHPRIORITY	0
#define TIMER_DELAY_SUB_HIGHPRIORITY		0

#define TIMER_TIMEOUT_PREEMPTION_PRIORITY	0
#define TIMER_TIMEOUT_SUB_PRIORITY			0

#define APPLI_TIMER_TIMEOUT_PREEMPTION_PRIORITY	0
#define APPLI_TIMER_TIMEOUT_SUB_PRIORITY		1

/**
 * @brief  IRQ handler functions names
 */
#define TIMER_DELAY_IRQ_HANDLER			TIM2_IRQHandler


/* Exported functions ------------------------------------------------------- */
void RFTRANS_95HF_EXTI_Callback(uint16_t GPIO_Pin);

void drvInt_Enable_Reply_IRQ(void);
void drvInt_Enable_RFEvent_IRQ(void);
void drvInt_Disable_95HF_IRQ(void);

/* Exported functions ------------------------------------------------------- */
void HAL_Delay_Us(uint16_t delay);


void NFC03A1_get_ISO14443A_infos(ISO14443A_CARD * infos);

void NFC03A1_demo(void);

typedef enum {UNDEFINED_MODE=0,PICC,PCD}DeviceMode_t;

void NFC03A1_Init(DeviceMode_t dm);

void NFC03A1_Delay_Us(uint16_t delay);


#endif
#endif /* BSP_NFC03A1_EXAMPLES_NFC03A1_H_ */
