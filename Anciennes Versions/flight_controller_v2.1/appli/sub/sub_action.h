/*
 * sub_action.h
 *
 *  Created on: 19 avr. 2020
 *      Author: Theo
 */

#ifndef SUB_SUB_ACTION_H_
#define SUB_SUB_ACTION_H_

#include "macro_types.h"
#include "../lib_perso/uart_lib.h"



typedef enum{
	LED_MODE_1,
	LED_MODE_ETEINT
}LedMode_e;

typedef struct {
	int8_t speed;
	uint8_t position;
	uint32_t color;

}Mooving_light;

//Sub pour les leds
void sub_clignotement_led(LedMode_e mode);

//Subs de données (télémétrie)
void sub_send_data(uart_struct_e * uart_telem , double acc_z_, uint8_t every_is_ok_, double latitude_, double longitude_, double x, double y, int32_t ch1, int32_t ch2, int32_t ch3, int32_t ch4, int32_t ch5, int32_t ch6, int32_t ch7, int32_t ch8, double batterie_, uint8_t state, uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);
void sub_receive_data(uint8_t c, uint8_t * state_flying, double * consigne_roll_base, double * consigne_pitch_base);

//Sub autonomies
running_e sub_parachute();




#endif /* SUB_SUB_ACTION_H_ */
