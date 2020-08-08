/*
 * esc.h
 *
 *  Created on: 23 mars 2020
 *      Author: Theo
 */

#ifndef ESC_H_
#define ESC_H_

#include "stm32f1_timer.h"
#define TIMER_ID_PWM TIMER1_ID

#define PERIODE_TIMER	5		//	Période de l'it du timer en µs
#define PERIODE_PWM 	4000	//	Période du signal pwm en µs, has to be a multiple of PERIODE_TIMER

typedef struct{
	uint16_t pulsation ;
	GPIO_TypeDef* gpio;
	uint16_t gpio_pin ;
	bool_e signal_state ;
}ESC_e;

typedef enum{
	ESC_State_LOW,
	ESC_State_HIGH,
	ESC_State_GOING_LOW,
}ESC_State_e;

running_e ESC_End_pulse(void);
void ESC_Start_pulse(void);
void ESC_Set_pulse(ESC_e* esc_,uint16_t pulsation_);
void ESC_init(ESC_e* escs_, uint8_t nb_escs_);


#endif /* ESC_H_ */
