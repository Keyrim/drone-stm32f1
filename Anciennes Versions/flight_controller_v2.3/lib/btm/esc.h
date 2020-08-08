/*
 * esc.h
 *
 *  Created on: 23 mars 2020
 *      Author: Theo
 */

#ifndef ESC_H_
#define ESC_H_
#include "macro_types.h"
#include "stm32f103xb.h"

typedef struct{
	uint16_t pulsation ;
	GPIO_TypeDef* gpio;
	uint16_t gpio_pin ;
	bool_e signal_state ;
	uint32_t pulse_start_time ;
}ESC_e;



bool_e ESC_End_pulse(ESC_e * esc);
void ESC_Start_pulse(ESC_e * esc);
void ESC_Set_pulse(ESC_e * esc_, uint16_t pulsation);
void ESC_init(ESC_e * esc, GPIO_TypeDef* gpio, uint16_t gpio_pin);


#endif /* ESC_H_ */
