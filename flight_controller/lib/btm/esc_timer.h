/*
 * esc_timer.h
 *
 *  Created on: 16 août 2020
 *      Author: Théo
 */

#ifndef BTM_ESC_TIMER_H_
#define BTM_ESC_TIMER_H_

#include "stm32f1_gpio.h"
#include "stm32f103xb.h"
#include "macro_types.h"

//Configuration escs / channels du timer
#define ESC_1_CHANNEL	0
#define ESC_2_CHANNEL	2
#define ESC_3_CHANNEL	1
#define ESC_4_CHANNEL	3

typedef enum{
	ESC_OUTPUT_PWM,
	ESC_OUTPUT_ONE_SHOT_125
}Escs_output_t;

typedef struct {
	//Gpios
	GPIO_TypeDef * GPIO_escs ;
	//Pins
	uint16_t PIN_escs[4];
	//Motors duty from 0 0% to 1000 100%
	uint16_t Duty[4];
	//output mode
	Escs_output_t output_mode ;


}ESCs_t;

void ESCS_init(ESCs_t * escs_, Escs_output_t output_mode);
void ESCS_set_period(uint32_t period_esc_1, uint32_t period_esc_2, uint32_t period_esc_3, uint32_t period_esc_4);



#endif /* BTM_ESC_TIMER_H_ */
