/*
 * nfc03a1.c
 *
 *  Created on: 2 janv. 2017
 *      Author: Julie
 *      Code exemple pour le lecteur NFC - Lecture de l'identifiant d'une carte étudiante ESEO
 */

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#if USE_NFC03A1
#include "stdint.h"
#include "stdbool.h"
#include "nfc03a1.h"
#include "stm32f1_gpio.h"

/* Code exemple du lecteur NFC03A1 : voir fichier NFC_example.txt--------------*/

/*
 * 1 - Inclure le header de la librairie NFC avec son chemin complet :
 *
 *		#include "NFC03A1/nfc03a1.h"
 */

/*
 * 2 - Déclarer le mode d'utilisation du module NFC, 2 modes sont disponibles
 * 	   dans cette librairie : Proximity Coupling Device (PCD), mode de lecture/
 * 	   écriture, et Proximity Inductive Coupling Card (PICC), mode d'émulation
 * 	   de tags.
 *
 *		extern DeviceMode_t devicemode;
 */


/* Private variables ---------------------------------------------------------*/


/* TT1 (PCD only)*/
uint8_t TT1Tag[NFCT1_MAX_TAGMEMORY];

/* TT2 */
uint8_t TT2Tag[NFCT2_MAX_TAGMEMORY];

/* TT3 */
uint8_t TT3Tag[NFCT3_MAX_TAGMEMORY];
uint8_t *TT3AttribInfo = TT3Tag, *TT3NDEFfile = &TT3Tag[NFCT3_ATTRIB_INFO_SIZE];

/* TT4 */
uint8_t CardCCfile      [NFCT4_MAX_CCMEMORY];
uint8_t CardNDEFfileT4A [NFCT4_MAX_NDEFMEMORY];
uint8_t CardNDEFfileT4B [NFCT4_MAX_NDEFMEMORY];

/* TT5 (PCD only)*/
uint8_t TT5Tag[NFCT5_MAX_TAGMEMORY];


#include "stm32f1_timer.h"
#if USE_BSP_TIMER == 0
	#error "vous devez activer USE_BSP_TIMER pour le NFC"
#endif
#define NFC_TIMER_FOR_DELAY_ID	TIMER4_ID
static bool_e initialized = FALSE;
extern DeviceMode_t devicemode;

//dm : PCD or PICC
void NFC03A1_Init(DeviceMode_t dm)
{
	devicemode = dm;
	/* 95HF HW Init */
	BSP_GPIO_PinCfg(NFC_INTERFACE_GPIO_PORT, NFC_INTERFACE_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
	#if USE_NFC_WITH_UART
		/* Set interface pin to select UART */
		HAL_GPIO_WritePin(NFC_INTERFACE_GPIO_PORT, NFC_INTERFACE_PIN, GPIO_PIN_RESET);
	#endif
	#if USE_NFC_WITH_SPI
		/* Set interface pin to select SPI */
		HAL_GPIO_WritePin(NFC_INTERFACE_GPIO_PORT, NFC_INTERFACE_PIN, GPIO_PIN_SET);
	#endif /*  */

	ConfigManager_HWInit();
	TIMER_run_us(NFC_TIMER_FOR_DELAY_ID, 40000, FALSE);
	initialized = TRUE;
}

//Fonction de test, blocante.
void NFC03A1_demo(void)
{
	uint8_t tag;
	if(!initialized)
		NFC03A1_Init(PCD);

	while(1)
	{
		tag = ConfigManager_TagHunting(TRACK_ALL);
		switch(tag)
		{
			case TRACK_NFCTYPE4A:{
				ISO14443A_CARD infos;
				NFC03A1_get_ISO14443A_infos(&infos);
				uint8_t i;
				printf("uid = ");
				for(i=0; i<infos.UIDsize;i++)
					printf("%02x ",infos.UID[i]);
				printf("\n");
				break;}
			default:
				break;
		}
	}

}

extern ISO14443A_CARD ISO14443A_Card;
void NFC03A1_get_ISO14443A_infos(ISO14443A_CARD * infos)
{
	*infos = ISO14443A_Card;
}


/**
 *	@brief  This function starts timer delay
 *  @param  delay : delay in microseconds (1us).
 *  @retval None
 */

void NFC03A1_Delay_Us(uint16_t delay)
{
  uint32_t initial_time;
  initial_time = (uint32_t)TIMER_read(NFC_TIMER_FOR_DELAY_ID);
  while(((uint32_t)TIMER_read(NFC_TIMER_FOR_DELAY_ID))-initial_time < delay);
}


#endif

