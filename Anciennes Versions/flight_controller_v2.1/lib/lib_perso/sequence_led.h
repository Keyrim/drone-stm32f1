/*
 * sequence_led.h
 *
 *  Created on: 22 juin 2020
 *      Author: Théo
 */

#ifndef LIB_PERSO_SEQUENCE_LED_H_
#define LIB_PERSO_SEQUENCE_LED_H_

#include "stm32f1_gpio.h"
#include "macro_types.h"

typedef struct{
	GPIO_TypeDef* gpio;
	uint16_t gpio_pin ;
	uint32_t periode ;
	int32_t sequence ;		//Sequences lues de gauche à droite
	int8_t compteur ;
	int8_t length_sequence ;
	uint32_t previous_time ;
	bool_e output_logic ;	// true means positive and 0 means positive

}sequence_led_t;

void LED_SEQUENCE_init(sequence_led_t * seq_led, GPIO_TypeDef* gpio, uint16_t gpio_pin, int32_t init_seq, uint32_t periode, int8_t length_sequence, bool_e output_logic);
void LED_SEQUENCE_set_sequence(sequence_led_t * seq_led, int32_t seq);
void LED_SEQUENCE_play(sequence_led_t * seq_led);

#endif /* LIB_PERSO_SEQUENCE_LED_H_ */
