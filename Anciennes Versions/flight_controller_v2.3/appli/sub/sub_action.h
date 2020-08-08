/*
 * sub_action.h
 *
 *  Created on: 19 avr. 2020
 *      Author: Theo
 */

#ifndef SUB_SUB_ACTION_H_
#define SUB_SUB_ACTION_H_

#include "macro_types.h"
#include "../btm/uart_lib.h"
#include "../system_d.h"
#include "../btm/esc.h"
#include "../IDs.h"



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

//Sub escs
running_e sub_esc_end_pulse(ESC_e* escs);
void sub_esc_start_pulse(ESC_e * escs);

//Subs de données (télémétrie)
void sub_send_data(State_drone_t * drone);
void sub_receive_data(uint8_t c, State_drone_t * drone, State_base_t * base);

//Sub smart, trèss smart
running_e sub_parachute(State_drone_t * drone);





#endif /* SUB_SUB_ACTION_H_ */
