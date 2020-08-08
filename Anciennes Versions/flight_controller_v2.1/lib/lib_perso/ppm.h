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

void PPM_init(uint16_t* channels_array_, uint8_t pin_number, GPIO_TypeDef* gpio_, uint16_t gpio_pin_, bool_e enable_it, uint32_t *time_last_read_);
uint16_t PPM_get_average(uint8_t channel);
void enable_it(void);
void disable_it(void);

#endif /* PPM_H_ */
