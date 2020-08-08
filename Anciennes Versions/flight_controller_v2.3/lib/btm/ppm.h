/*
 * ppm.h
 *
 *  Created on: 22 mars 2020
 *      Author: Theo
 */

#ifndef PPM_H_
#define PPM_H_

#include "macro_types.h"
#include "stm32f103xb.h"
#include "stm32f1_gpio.h"
#include "systick.h"
#include "settings.h"

typedef struct{
	uint8_t actual_channel ;
	uint16_t channels [NB_CHANNEL];
	uint32_t previous_time_rising ;
	uint32_t last_update ;
	//Pin
	uint8_t gpio_pin_number;
	uint16_t gpio_pin ;
	GPIO_TypeDef* GPIO ;
	IRQn_Type irqn ;
	bool_e is_ok ;
}DRONE_ppm_t;

void DRONE_ppm_init(DRONE_ppm_t * ppm, uint8_t pin_number, GPIO_TypeDef* gpio, uint16_t gpio_pin);
void DRONE_ppm_enable_it(DRONE_ppm_t * ppm);
void DRONE_ppm_disable_it(DRONE_ppm_t * ppm);

#endif /* PPM_H_ */
