/*
 * dht11.c
 *
 *  Created on: 5 juin 2019
 *      Author: Nirgal
 */

#include "config.h"
#if USE_DHT11

#include "dht11.h"
#include "macro_types.h"
#include "stm32f1xx_hal.h"
#include "systick.h"
#include "stm32f1_extit.h"
#include <core_cm3.h>
#include "stm32f1_gpio.h"

/*
https://www.mouser.com/ds/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf

La ligne bidirectionelle de "data" du DHT11 doit être tirée au Vdd par une résistance de 4,7k.
Vdd doit être entre 3 et 5V.

Etant donné que le STM32F103 est alimenté en 3,3V, il est recommandé d'alimenter ce capteur en 3,3V.
On peut toutefois aller au delà, mais il faut s'assurer que la broche utilisée pour le dialogue soit tolérante 5V en entrée.




*/
static bool_e compute_frame(uint64_t datas, uint8_t * humidity_int, uint8_t * humidity_dec, uint8_t * temperature_int, uint8_t * temperature_dec);
static void DHT11_callback_exti(uint16_t pin);

static GPIO_TypeDef * DHT11_gpio;
static uint16_t DHT11_pin;
static bool_e initialized = FALSE;


void DHT11_init(GPIO_TypeDef * GPIOx, uint16_t GPIO_PIN_x)
{
	DHT11_gpio = GPIOx;
	DHT11_pin = GPIO_PIN_x;
	HAL_GPIO_WritePin(DHT11_gpio, DHT11_pin, 1);
	BSP_GPIO_PinCfg(DHT11_gpio, DHT11_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH);
	initialized = TRUE;
}
#define NB_BITS	42	//les deux bits de poids fort n'appartiennent pas aux données utiles. (il s'agit de la réponse du capteur avant la trame utile).
static volatile bool_e flag_end_of_reception = FALSE;
static volatile uint64_t trame;
static volatile uint8_t index = 0;
static volatile uint32_t t = 0;



void DHT11_demo(void)
{
	static uint8_t humidity_int;
	static uint8_t humidity_dec;
	static uint8_t temperature_int;
	static uint8_t temperature_dec;

	DHT11_init(GPIOA, GPIO_PIN_0);
	while(1)
	{

		switch(DHT11_state_machine_get_datas(&humidity_int, &humidity_dec, &temperature_int, &temperature_dec))
		{
			case END_OK:
 				debug_printf("DHT11 h=%d,%d | t=%d,%d\n",humidity_int, humidity_dec, temperature_int, temperature_dec);
 				HAL_Delay(1500);
				break;
			case END_ERROR:
				debug_printf("DHT11 read error (h=%d,%d | t=%d,%d)\n", humidity_int, humidity_dec, temperature_int, temperature_dec);
				HAL_Delay(1500);
				break;
			case END_TIMEOUT:
				debug_printf("DHT11 timeout (h=%d,%d | t=%d,%d)\n", humidity_int, humidity_dec, temperature_int, temperature_dec);
				HAL_Delay(1500);
				break;
			default:
				break;
		}
	}
}


static void DHT11_process_ms(void)
{
	if(t)
		t--;
}


static void DHT11_callback_exti(uint16_t pin)
{
	static uint32_t rising_time_us = 0;

	if(pin == DHT11_pin)
	{
		if(index < NB_BITS)
		{
			if(HAL_GPIO_ReadPin(DHT11_gpio, DHT11_pin))
			{
				rising_time_us = SYSTICK_get_time_us();				//on enregistre la date du front montant (en microsecondes)
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
			}
			else
			{
				uint32_t falling_time_us;
				falling_time_us = SYSTICK_get_time_us(); //on conserve la différence entre le front montant et le front descendant
				if(falling_time_us - rising_time_us > 50)
					trame |= (uint64_t)(1) << (NB_BITS - 1 - index);
				index++;
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
			}
		}

		if(index == NB_BITS)
		{
			flag_end_of_reception = TRUE;
			EXTI_disable(EXTI_gpiopin_to_pin_number(DHT11_pin));
		}
	}
}
//Attention, fonction blocante pendant 4ms !
running_e DHT11_state_machine_get_datas(uint8_t * humidity_int, uint8_t * humidity_dec, uint8_t * temperature_int, uint8_t * temperature_dec)
{
	typedef enum
	{
		INIT,
		SEND_START_SIGNAL,
		WAIT_DHT_ANSWER,
		TIMEOUT,
		END_OF_RECEPTION,
		WAIT_BEFORE_NEXT_ASK
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	bool_e entrance;
	entrance = (state != previous_state)?TRUE:FALSE;
	previous_state = state;
	running_e ret = IN_PROGRESS;


	switch(state)
	{
		case INIT:
			if(initialized)
			{
				state = SEND_START_SIGNAL;
				Systick_add_callback_function(&DHT11_process_ms);
				EXTIT_set_callback(&DHT11_callback_exti, EXTI_gpiopin_to_pin_number(DHT11_pin), FALSE);
			}
			else
			{
				debug_printf("You should call DHT11_init(...)\n");
				ret = END_ERROR;
			}
			break;

		case SEND_START_SIGNAL:
			if(entrance)
			{
				t = 20;
				index = 0;
				trame = 0;
				flag_end_of_reception = FALSE;
				BSP_GPIO_PinCfg(DHT11_gpio, DHT11_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH);
				HAL_GPIO_WritePin(DHT11_gpio, DHT11_pin, 0);
			}
			if(!t)
			{
				__HAL_GPIO_EXTI_CLEAR_IT(DHT11_gpio);
				BSP_GPIO_PinCfg(DHT11_gpio, DHT11_pin, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH);
				HAL_GPIO_WritePin(DHT11_gpio, DHT11_pin, 1);
				EXTIT_enable(EXTI_gpiopin_to_pin_number(DHT11_pin));
				state = WAIT_DHT_ANSWER;
				//début de la surveillance des fronts
			}
			break;
		case WAIT_DHT_ANSWER:
			if(entrance)
			{
				t = 10;
			}
			if(flag_end_of_reception)
				state = END_OF_RECEPTION;
			if(!t)
				state = TIMEOUT;
			break;
		case TIMEOUT:
			ret = END_TIMEOUT;
			t = 100;
			state = WAIT_BEFORE_NEXT_ASK;
			break;
		case END_OF_RECEPTION:
			debug_printf("%llx\n",trame);
			if(compute_frame(trame, humidity_int, humidity_dec, temperature_int, temperature_dec))
				ret = END_OK;
			else
				ret = END_ERROR;
			t = 1000;
			state = WAIT_BEFORE_NEXT_ASK;
			break;
		case WAIT_BEFORE_NEXT_ASK:
			if(!t)
				state = SEND_START_SIGNAL;
			break;
		default:
			break;

	}
	return ret;
}

static bool_e compute_frame(uint64_t datas, uint8_t * humidity_int, uint8_t * humidity_dec, uint8_t * temperature_int, uint8_t * temperature_dec)
{
	bool_e ret = FALSE;
	*humidity_int = (uint8_t)(datas >> 32);
	*humidity_dec = (uint8_t)(datas >> 24);
	*temperature_int = (uint8_t)(datas >> 16);
	*temperature_dec = (uint8_t)(datas >> 8);
	debug_printf(" - %d+%d+%d+%d= %d (%d)", *humidity_int, *humidity_dec, *temperature_int, *temperature_dec, (uint8_t)(*humidity_int + *humidity_dec + *temperature_int + *temperature_dec), (uint8_t)(datas));
	//checksum
	if((uint8_t)(*humidity_int + *humidity_dec + *temperature_int + *temperature_dec) == (uint8_t)(datas))
		ret = TRUE;
	return ret;
}

#endif
