/*
 * dht11.h
 *
 *  Created on: 5 juin 2019
 *      Author: Nirgal
 */

#ifndef DHT11_H_
#define DHT11_H_
#include "config.h"
#if USE_DHT11

#include <stdint.h>
#include "macro_types.h"
#include "stm32f1xx_hal.h"

void DHT11_demo(void);

void DHT11_init(GPIO_TypeDef * GPIOx, uint16_t GPIO_PIN_x);

running_e DHT11_state_machine_get_datas(uint8_t * humidity_int, uint8_t * humidity_dec, uint8_t * temperature_int, uint8_t * temperature_dec);

#endif /* DHT11_H_ */
#endif
