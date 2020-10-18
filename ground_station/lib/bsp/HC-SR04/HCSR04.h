/*
 * HC-SR04.h
 *
 *  Created on: 22 mai 2019
 *      Author: Nirgal
 */

#ifndef HCSR04_H_
#define HCSR04_H_
#include "config.h"
#if USE_HCSR04

void HCSR04_demo_state_machine(void);

HAL_StatusTypeDef HCSR04_add(uint8_t * id, GPIO_TypeDef * TRIG_GPIO, uint16_t TRIG_PIN, GPIO_TypeDef * ECHO_GPIO, uint16_t ECHO_PIN);

HAL_StatusTypeDef HCSR04_get_value(uint8_t id, uint16_t * distance);

void HCSR04_run_measure(uint8_t id);

void HCSR04_process_main(void);

#endif
#endif /* HCSR04_H_ */
