/*
= * CapacitiveKeyboard.c
 *
 *  Created on: 4 mai 2016
 *      Author: Nirgal
 */

/*
 * Ce driver est destiné au pilotage du clavier capacitif 16 touches embarquant le composant TTP229 (marquage : B228BSF)
 * 	Ce clavier est configuré par défaut pour 8 touches, en mono-appui.
 *
 * 	Pour activer les 8 touches de 9 à 16, il faut ajouter une liaison (avec un cavalier)
 * 		- entre les broches 3 et 6 du connecteur P1.
 *
 * 	Pour activer le multi-appui, il faut ajouter deux liaisons :
 * 		- entre les broches 4 et 5 du connecteur P1.
 * 		- entre les broches 4 et 5 du connecteur P2.
 *
 *	Notons que ces configurations sont prises en compte à l'alimentation du clavier. Il faut donc resetter son alimentation à chaque changement des cavaliers.
 *
 * 	L'image suivante illustre ces modifications :
 * 		http://2.bp.blogspot.com/-8HEhBIwDGtk/VH04gBM3cNI/AAAAAAAAADw/IBK7bJmtqGs/s1600/IMG_20141201_192455_1.jpg
 *
 *
 */
#include "config.h"
#if USE_CAPACITIVE_KEYBOARD
#include "CapacitiveKeyboard.h"
#include "systick.h"

#define DELAY_US_DURATION	100
volatile static uint32_t t = 0;
static bool_e initialized = FALSE;
#define WRITE_CLK(x)	HAL_GPIO_WritePin(CAPACITIVE_KEYBOARD_SCL_PORT,CAPACITIVE_KEYBOARD_SCL_PIN,x)
#define READ_SDO()		HAL_GPIO_ReadPin(CAPACITIVE_KEYBOARD_SDO_PORT,CAPACITIVE_KEYBOARD_SDO_PIN)

/*
 * Initialisation du clavier capacitif : Horloge du GPIOA, SDO et SDL
 */
void CapacitiveKeyboard_init(void)
{
	MATRIX_KEYBOARD_HAL_CLOCK_ENABLE();
	HAL_GPIO_Init(CAPACITIVE_KEYBOARD_SCL_PORT, &(GPIO_InitTypeDef){CAPACITIVE_KEYBOARD_SCL_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});
	HAL_GPIO_Init(CAPACITIVE_KEYBOARD_SDO_PORT, &(GPIO_InitTypeDef){CAPACITIVE_KEYBOARD_SDO_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});
	initialized = TRUE;
}


void CapacitiveKeyboard_delay_us(uint32_t loop)
{
	while(loop--);
}

/*
 * @pre	cette fonction ne doit pas être appelée plus souvent que toutes les 5ms.
 */
uint16_t CapacitiveKeyboard_read(void)
{
	uint16_t ret;
	uint8_t i;
	ret = 0;
	for(i=0; i<16; i++)
	{
		WRITE_CLK(1);
		CapacitiveKeyboard_delay_us(DELAY_US_DURATION);
		WRITE_CLK(0);
		ret |= ((!READ_SDO())<<i);
		CapacitiveKeyboard_delay_us(DELAY_US_DURATION);
	}

	return ret;
}

/*
 * Machine à états du clavier capacitif
 * Cette fonction doit être appelée dans la boucle de tâche de fond
 * Initialisation du clavier puis toutes les 10ms, on vient lire l'appui réalisé
 */
running_e DEMO_CapacitiveKeyboard (bool_e ask_for_finish)
{
	typedef enum
	{
		INIT = 0,
		RUN
	}state_e;
	static state_e state = INIT;
	static uint16_t previous_key = 0;
	running_e ret;
	uint16_t current_keys;

	ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:
			Systick_add_callback_function(DEMO_CapacitiveKeyboard_process_1ms);
			CapacitiveKeyboard_init();	//Initialisation du clavier
			printf("To run this demo, you should plug a capacitive keyboard on the right ports. See CapacitiveKeyboard.h\n");
			state = RUN;
			break;
		case RUN:
			if(!t)	//A chaque fois que t vaut 0 (toutes les 10ms)...
			{
				t = 10;							//[ms] On recharge le chronomètre t pour 10ms...
				current_keys = CapacitiveKeyboard_read();
				if((current_keys != previous_key) && (current_keys != 0))
				{
					uint8_t i;
					printf("Key(s) :");
					for(i=0;i<16;i++)
						if((current_keys>>i)&1)
							printf(" %d",i+1);
					printf("\n");
				}
				previous_key = current_keys;
			}
			if(ask_for_finish)
			{
				state = INIT;
				Systick_remove_callback_function(DEMO_CapacitiveKeyboard_process_1ms);
				ret = END_OK;
			}
			break;
		default:
			break;
	}
	return ret;
}

//Cette fonction doit être appelée toutes les ms.
void DEMO_CapacitiveKeyboard_process_1ms(void)
{
	if(t)		//Si le chronomètre est "chargé", on décompte... (comme un minuteur de cuisine !)
		t--;
}
#endif

