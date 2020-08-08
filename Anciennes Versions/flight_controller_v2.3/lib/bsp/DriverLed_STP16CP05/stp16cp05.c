/*
 * stp16cp05.c
 *
 *  Created on: 17 janv. 2018
 *      Author: Nirgal
 *
 *      Le driver de LED STP16C05 pilote jusqu'à 16 LED en fournissant un courant régulé commun et réglable (via une résistance de référence).
 *
 *      Le composant intègre deux registres : 1 registre à décalage, et 1 registre de pilotage des leds.
 *      Afin de piloter ces leds, il faut :
 *      1- envoyer 16 bits les uns après les autres (au registre à décalage), séparés par un pulse de clock après chaque bit
 *      2- marquer un pulse de latch sur la broche 'LE' pour que les bits transmis soient pris en compte
 *      Et c'est tout ;)
 *
 *      Notons que le pulse de clock peut être bref (minimum 8ns !) une simple écriture ON suivie d'une écriture OFF suffit.
 */
#include "stm32f1_gpio.h"
#include "stp16cp05.h"

#define NB_LEDS 16
#define SDI_PIN		GPIOA, GPIO_PIN_2
#define CLK_PIN		GPIOA, GPIO_PIN_3
#define LE_PIN		GPIOA, GPIO_PIN_0

//Macros facilitant le portage du code.
#define Write(pin, state)	 			HAL_GPIO_WritePin(pin, state)
#define ConfigureAsOutput(port_pin)		BSP_GPIO_PinCfg(port_pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH)

/*
 * Initialisation des broches en sortie, et à 0.
 */
void STP16CP05_init(void){
	//Initialisation du SDI vers drive LED
	ConfigureAsOutput(SDI_PIN);
	ConfigureAsOutput(LE_PIN);
	ConfigureAsOutput(CLK_PIN);
	Write(SDI_PIN, 0);
	Write(LE_PIN, 0);
	Write(CLK_PIN, 0);
}

/*
 * Fonction de test permettant de vérifier le fonctionnement. On vous laisse deviner ce qu'elle fait ^^
 */
void STP16CP05_test(void){
	uint16_t p;
	uint16_t n;
	for (n = 0; n < 16; n++){
		p = 1 << n;
		STP16CP05_set_leds(p);
		HAL_Delay(300);
	}
}

/*
 * Fonction assurant la mise à jour des leds, selon le paramètre entier sur 16 bits : 'leds'
 * Chaque bit représente l'état d'une led.
 */
void STP16CP05_set_leds(uint16_t leds)
{
	uint8_t i;
	for(i=0; i<NB_LEDS; i++)
	{
		Write(SDI_PIN, (leds>>i)&1);
		Write(CLK_PIN, 1);
		Write(CLK_PIN, 0);
	}
	Write(LE_PIN, 1);
	Write(LE_PIN, 0);

}
