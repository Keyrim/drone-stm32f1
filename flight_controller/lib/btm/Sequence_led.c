/*
 * sequence_led.c
 *
 *  Created on: 22 juin 2020
 *      Author: Théo
 */

#include "Sequence_led.h"

#include "stm32f1_gpio.h"
#include "systick.h"

//On initialise les paramètre et on configure le pin de sortie
void LED_SEQUENCE_init(sequence_led_t * seq_led, GPIO_TypeDef* gpio, uint16_t gpio_pin, int32_t init_seq, uint32_t periode, int8_t length_sequence, bool_e output_logic){
	//Init des varaibles
	seq_led->compteur = 0;
	seq_led->gpio = gpio ;
	seq_led->gpio_pin = gpio_pin ;
	seq_led->length_sequence = length_sequence ;
	seq_led->periode = periode ;
	seq_led->output_logic = output_logic ;
	if(seq_led->output_logic == 1)
		//Si logique négative on inverse la séquence
		init_seq = ~init_seq ;
	seq_led->sequence = init_seq ;
	//Init de la sortie gpio
	BSP_GPIO_PinCfg(seq_led->gpio, seq_led->gpio_pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH );



}
//On change la séquence active
void LED_SEQUENCE_set_sequence(sequence_led_t * seq_led, int32_t seq){
	if(seq_led->output_logic == 1)
		//Si logique négative on inverse la séquence
		seq = ~seq ;
	seq_led->compteur = 0 ;
	seq_led->sequence = seq ;
}

//On change la sortie de la led par rapport à la séquence active
void LED_SEQUENCE_play(sequence_led_t * seq_led, uint32_t current_time_us){
	if(current_time_us / 1000 > seq_led->previous_time + seq_led->periode){
		seq_led->previous_time += seq_led->periode ;
		//on récupère le niveau logique de la sortie
		bool_e output = ((seq_led->sequence) >> ((seq_led->length_sequence - seq_led->compteur) - 1)) & 1 ;
		//On update la sortie
		if(output)
			seq_led->gpio->BSRR = seq_led->gpio_pin ;
		else
			seq_led->gpio->BSRR = (uint32_t)(seq_led->gpio_pin) << 16U ;
		//on update le compteur
		seq_led->compteur ++ ;
		//TODO : verif à l'init si length > 0
		if(seq_led->compteur == seq_led->length_sequence)
			seq_led->compteur = 0 ;
	}
}

