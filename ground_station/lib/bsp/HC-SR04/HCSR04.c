/*
 * HC-SR04.c
 *
 *  Created on: 22 mai 2019
 *      Author: Nirgal
 */

#include "config.h"
#if USE_HCSR04
#include "HCSR04.h"
#include "macro_types.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_timer.h"
#include "stm32f1_gpio.h"
#include "stm32f1_extit.h"

#ifndef HCSR04_NB_SENSORS
	#define HCSR04_NB_SENSORS	5
#endif
#ifndef HCSR04_TIMER
	#define HCSR04_TIMER		TIMER1_ID
#endif

//En cas d'absence de mesure, certains HCSR04 présentent un écho de l'ordre de 130ms.
//Si La période de mesure est inférieure, elle ne sera respectée que si le capteur à présenté un écho moins long.
#define PERIOD_MEASURE			100


#define HSCR04_TIMEOUT			150		//ms



#define US_SPEED_IN_AIR			344	//mm/ms (à environ 20°)
typedef enum
{
	HCSR04_STATE_INEXISTANT = 0,
	HCSR04_STATE_INITIALIZED,
	HCSR04_STATE_TRIG,
	HCSR04_STATE_WAIT_ECHO_RISING,
	HCSR04_STATE_WAIT_ECHO_FALLING,
	HCSR04_STATE_ECHO_RECEIVED,
	HCSR04_STATE_TIMEOUT,
	HCSR04_STATE_ERROR,
	HCSR04_STATE_IDLE
}hcsr04_state_t;


typedef struct
{
	GPIO_TypeDef * trig_gpio;
	uint16_t trig_pin;
	GPIO_TypeDef * echo_gpio;
	uint16_t echo_pin;
	hcsr04_state_t state;
	uint32_t ttrig;		//ms
	uint32_t tfalling;	//us
	uint32_t trising;	//us
	uint16_t distance;
}hcsr04_t;

static hcsr04_t sensors[HCSR04_NB_SENSORS];

static void HCSR04_callback(uint16_t pin);
static HAL_StatusTypeDef HCSR04_compute_distance(uint8_t id);
static void HCSR04_trig(uint8_t id);
static void HCSR04_RunTimerUs(void);
static uint32_t HCSR04_ReadTimerUs(void);



/*
 * Fonction de démonstration permettant d'apréhender l'utilisation de ce module logiciel
 * @pre	cette fonction doit être appelée en tâche de fond
 * @pre	il faut avoir préalablement initialisé un port série (cette fonction utilise printf)
 * @pre ATTENTION, le signal ECHO arrive en 5V. Il ne faut pas utiliser pour le recevoir une broche non tolérante 5V. (voir le classeur Ports_STM32F1.xls)
 * @post cette fonction effectue des mesures régulières avec un capteur HCSR04 placé sur les ports PC7 (Trig) et PB6 (Echo)
 * @note cette fonction n'utilise que des fonctions publiques. Elle peut donc être dupliquée à l'extérieur de ce module logiciel.
 */
void HCSR04_demo_state_machine(void)
{
	typedef enum
	{
		INIT,
		FAIL,
		LAUNCH_MEASURE,
		RUN,
		WAIT_DURING_MEASURE,
		WAIT_BEFORE_NEXT_MEASURE
	}state_e;

	static state_e state = INIT;
	static uint32_t tlocal;
	static uint8_t id_sensor;
	uint16_t distance;

	//ne pas oublier d'appeler en tâche de fond cette fonction.
	HCSR04_process_main();


	switch(state)
	{
		case INIT:
			if(HCSR04_add(&id_sensor, GPIOC, GPIO_PIN_7, GPIOB, GPIO_PIN_6) != HAL_OK)
			{
				printf("HCSR04 non ajouté - erreur gênante\n");
				state = FAIL;
			}
			else
			{
				printf("HCSR04 ajouté\n");
				state = LAUNCH_MEASURE;
			}
			break;
		case LAUNCH_MEASURE:
			HCSR04_run_measure(id_sensor);
			tlocal = HAL_GetTick();
			state = WAIT_DURING_MEASURE;
			break;
		case WAIT_DURING_MEASURE:
			switch(HCSR04_get_value(id_sensor, &distance))
			{
				case HAL_BUSY:
					//rien à faire... on attend...
					break;
				case HAL_OK:
					printf("sensor %d - distance : %d\n", id_sensor, distance);
					state = WAIT_BEFORE_NEXT_MEASURE;
					break;
				case HAL_ERROR:
					printf("sensor %d - erreur ou mesure non lancée\n", id_sensor);
					state = WAIT_BEFORE_NEXT_MEASURE;
					break;

				case HAL_TIMEOUT:
					printf("sensor %d - timeout\n", id_sensor);
					state = WAIT_BEFORE_NEXT_MEASURE;
					break;
			}
			break;
		case WAIT_BEFORE_NEXT_MEASURE:
			if(HAL_GetTick() > tlocal + PERIOD_MEASURE)
				state = LAUNCH_MEASURE;
			break;
		default:
			break;
	}
}

/*
 * @pre	il ne peut pas y avoir plusieurs capteurs sur un même numéro de broche (par exemple PA0 et PB0 !)
 */
HAL_StatusTypeDef HCSR04_add(uint8_t * id, GPIO_TypeDef * TRIG_GPIO, uint16_t TRIG_PIN, GPIO_TypeDef * ECHO_GPIO, uint16_t ECHO_PIN)
{
	HAL_StatusTypeDef ret;
	ret = HAL_ERROR;
	for(uint8_t i = 0; i<HCSR04_NB_SENSORS; i++)
	{
		if(sensors[i].state == HCSR04_STATE_INEXISTANT)
		{
			//on a trouvé une case libre.
			*id = i;
			sensors[i].state = HCSR04_STATE_INITIALIZED;
			sensors[i].trig_gpio = TRIG_GPIO;
			sensors[i].trig_pin = TRIG_PIN;
			sensors[i].echo_gpio = ECHO_GPIO;
			sensors[i].echo_pin = ECHO_PIN;
			BSP_GPIO_PinCfg(ECHO_GPIO, ECHO_PIN, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLDOWN, GPIO_SPEED_FREQ_HIGH);
			BSP_GPIO_PinCfg(TRIG_GPIO, TRIG_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
			EXTIT_set_callback(&HCSR04_callback, EXTI_gpiopin_to_pin_number(ECHO_PIN), TRUE);
			ret = HAL_OK;
			break;
		}
	}

	return ret;
}

static bool_e timer_is_running = FALSE;

void HCSR04_run_measure(uint8_t id)
{
	if(sensors[id].state != HCSR04_STATE_INEXISTANT)
	{
		if(!timer_is_running)
			HCSR04_RunTimerUs();
		HCSR04_trig(id);
	}
}


/*
 * @brief Fonction de callback devant être appelée uniquement par les routines d'interruptions.
 */
static void HCSR04_callback(uint16_t pin)
{
	uint8_t i;
	for(i=0; i<HCSR04_NB_SENSORS; i++)
	{
		if(sensors[i].echo_pin == pin)	//trouvé !
		{
			if(sensors[i].state == HCSR04_STATE_WAIT_ECHO_RISING)
			{
				if(HAL_GPIO_ReadPin(sensors[i].echo_gpio, sensors[i].echo_pin) == 1)
				{
					sensors[i].trising = HCSR04_ReadTimerUs();
					sensors[i].state = HCSR04_STATE_WAIT_ECHO_FALLING;
				}
			}
			else if(sensors[i].state == HCSR04_STATE_WAIT_ECHO_FALLING)
			{
				if(HAL_GPIO_ReadPin(sensors[i].echo_gpio, sensors[i].echo_pin) == 0)
				{
					sensors[i].tfalling = HCSR04_ReadTimerUs();
					sensors[i].state = HCSR04_STATE_ECHO_RECEIVED;
				}
			}
			break;
		}
	}
}



static void HCSR04_trig(uint8_t id)
{
	if(sensors[id].state != HCSR04_STATE_INEXISTANT)
	{
		uint32_t tlocal;
		sensors[id].state = HCSR04_STATE_TRIG;
		HAL_GPIO_WritePin(sensors[id].trig_gpio, sensors[id].trig_pin, 1);	//trig on
		tlocal = HCSR04_ReadTimerUs();
		while(HCSR04_ReadTimerUs() - tlocal < 10);	//délai d'au moins 10us
		HAL_GPIO_WritePin(sensors[id].trig_gpio, sensors[id].trig_pin, 0);	//trig off
		sensors[id].state = HCSR04_STATE_WAIT_ECHO_RISING;
		sensors[id].ttrig = HAL_GetTick();
	}
}

#define HCSR04_PERIOD_TIMER		(40000)				//on compte jusqu'à 40000 * 2.5us = 100ms (soit 34m)
#define HCSR04_PRESCALER_TIMER	(64*10/4)			//on compte des [2.5us] Cette résolution correspond à 0.85mm

static uint32_t HCSR04_ReadTimerUs(void)
{
	return TIMER_read(HCSR04_TIMER);
}

static void HCSR04_RunTimerUs(void)
{
	TIMER_run_us(HCSR04_TIMER, 10000, FALSE);
	TIMER_set_prescaler(HCSR04_TIMER, HCSR04_PRESCALER_TIMER);
	TIMER_set_period(HCSR04_TIMER, HCSR04_PERIOD_TIMER);
	timer_is_running = TRUE;
}

void HCSR04_process_main(void)
{
	uint8_t i;
	for(i=0; i<HCSR04_NB_SENSORS; i++)
	{
		switch(sensors[i].state)
		{
			case HCSR04_STATE_INEXISTANT:
				break;
			case HCSR04_STATE_INITIALIZED:
				//rien à faire, on attend une demande de mesure via HCSR04_run_measure()
				break;
			case HCSR04_STATE_TRIG:
				//neven happen
				break;
			case HCSR04_STATE_WAIT_ECHO_RISING:	//no break;
			case HCSR04_STATE_WAIT_ECHO_FALLING:
				//on attend l'IT...
				if(HAL_GetTick() - sensors[i].ttrig > HSCR04_TIMEOUT)
				{
					sensors[i].state = HCSR04_STATE_TIMEOUT;
				}
				break;
			case HCSR04_STATE_ECHO_RECEIVED:
				//on a correctement reçu un echo
				//calcul de la distance...
				if(HCSR04_compute_distance(i) == HAL_OK)
					sensors[i].state = HCSR04_STATE_IDLE;
				else
					sensors[i].state = HCSR04_STATE_ERROR;
				break;
			case HCSR04_STATE_ERROR:
				//rien à faire, on attend une demande de mesure via HCSR04_run_measure()
				break;
			case HCSR04_STATE_TIMEOUT:
				//rien à faire, on attend une demande de mesure via HCSR04_run_measure()
				break;
			case HCSR04_STATE_IDLE:
				//rien à faire, on attend une demande de mesure via HCSR04_run_measure()
				break;
			default:
				break;
		}
	}
}

static HAL_StatusTypeDef HCSR04_compute_distance(uint8_t id)
{
	uint32_t distance;

	sensors[id].distance = (uint16_t)0;	//hypothèse tant qu'on a pas une valeur correcte.

	if(sensors[id].state != HCSR04_STATE_ECHO_RECEIVED)
		return HAL_ERROR;

	if(sensors[id].tfalling < sensors[id].trising)
		sensors[id].tfalling += HCSR04_PERIOD_TIMER;

	if(sensors[id].tfalling < sensors[id].trising)
		return HAL_ERROR;

	distance = sensors[id].tfalling - sensors[id].trising;
	distance *=  HCSR04_PRESCALER_TIMER;	//distance est exprimé ici en pulses de timer purs

	uint32_t freq;
	if(HCSR04_TIMER == TIMER1_ID)
	{
		//Fréquence du TIMER1 est PCLK2 lorsque APB2 Prescaler vaut 1, sinon : PCLK2*2
		freq = HAL_RCC_GetPCLK2Freq();
		if((RCC->CFGR & RCC_CFGR_PPRE2) >> 11 != RCC_HCLK_DIV1)
			freq *= 2;
	}
	else
	{
		//Fréquence des TIMERS 2,3,4 est PCLK1 lorsque APB1 Prescaler vaut 1, sinon : PCLK1*2
		freq = HAL_RCC_GetPCLK1Freq();
		if((RCC->CFGR & RCC_CFGR_PPRE1) >> 8 != RCC_HCLK_DIV1)
			freq *= 2;
	}
	freq /= 1000000;	//fréquence exprimée en MHz
	if(!freq)
		return HAL_ERROR;

	distance /= freq;				//[us]
	distance *= US_SPEED_IN_AIR;	//[um]
	distance /= 1000;				//distance aller-retour [mm]
	distance /= 2;					//distance aller simple [mm]

	if(distance > 5000)//au delà 5m, on considère que la distance n'a pas été acquise (ou bien est infinie)
		distance = 0;

	sensors[id].distance = (uint16_t)distance;

	return HAL_OK;
}

HAL_StatusTypeDef HCSR04_get_value(uint8_t id, uint16_t * distance)
{
	HAL_StatusTypeDef ret = HAL_BUSY;
	switch(sensors[id].state)
	{
		case HCSR04_STATE_IDLE:	//on a reçu une distance
			*distance = sensors[id].distance;
			ret = HAL_OK;
			break;
		case HCSR04_STATE_TIMEOUT:
			ret = HAL_TIMEOUT;
			break;
		case HCSR04_STATE_INEXISTANT:	//no break;		//il est anormal de demander la valeur d'un capteur non initialisé
		case HCSR04_STATE_INITIALIZED:	//no break;		//il est anormal de demander la valeur d'un capteur non lancé en mesure.
		case HCSR04_STATE_ERROR:						//erreur interne lors du calcul de distance
			ret = HAL_ERROR;
			break;
		default:
			ret = HAL_BUSY;	// tout les autres cas sont 'busy'
			break;
	}
	return ret;
}

#endif
