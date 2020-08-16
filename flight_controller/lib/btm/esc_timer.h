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

typedef struct {
	//Gpios
	GPIO_TypeDef * GPIO_escs ;
	//Pins
	uint16_t PIN_escs[4];
	//Motors duty
	uint16_t Duty[4];




}ESCs_t;

void ESCS_init(ESCs_t * escs_);
void ESCS_set_period(uint32_t period_esc_1, uint32_t period_esc_2, uint32_t period_esc_3, uint32_t period_esc_4);



#endif /* BTM_ESC_TIMER_H_ */
