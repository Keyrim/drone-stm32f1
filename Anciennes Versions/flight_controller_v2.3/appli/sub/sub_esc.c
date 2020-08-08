/*
 * sub_esc.c
 *
 *  Created on: 1 juil. 2020
 *      Author: Théo
 */

#include "sub_action.h"


running_e sub_esc_end_pulse(ESC_e* escs){
	running_e to_return = IN_PROGRESS ;
	bool_e escs_high = FALSE ;
	for(uint8_t e = 0; e < 4 ; e ++)
		escs_high |= ESC_End_pulse(&escs[e]);
	if(!escs_high)
		to_return = END_OK ;
	return to_return ;
}

void sub_esc_start_pulse(ESC_e * escs){
	for(uint8_t e = 0; e < 4 ; e ++)
		ESC_Start_pulse(&escs[e]);
}
