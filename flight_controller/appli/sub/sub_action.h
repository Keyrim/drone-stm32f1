/*
 * sub_action.h
 *
 *  Created on: 19 avr. 2020
 *      Author: Theo
 */

#ifndef SUB_SUB_ACTION_H_
#define SUB_SUB_ACTION_H_

#include "macro_types.h"
#include "../system_d.h"
#include "../IDs.h"
#include "../../lib/btm/Esc.h"

//Ensemble des sub actions

void sub_channels_analysis(State_drone_t * drone);
uint32_t sub_ms5611(State_drone_t * drone, uint32_t current_time_us);
void sub_free_time(State_drone_t * drone, State_base_t * base);
running_e sub_esc_end_pulse(ESC_e* escs);
void sub_esc_start_pulse(ESC_e * escs);
void sub_receive_data(uint8_t c, State_drone_t * drone, State_base_t * base);
running_e sub_parachute(State_drone_t * drone);





#endif /* SUB_SUB_ACTION_H_ */
