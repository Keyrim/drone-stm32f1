/*
 * mask_def.c
 *
 *  Created on: 17 sept. 2020
 *      Author: Theo
 */

#include "mask_def.h"

void mask_def_on_the_ground(Event_t * event){

	//Mask 1: mode de vol manuel : ppm_timeout | flag_chan_5_pos_1 | request_stop_motors
	MASK_set_flag(&event->mask_and[0], FLAG_STATE_MANUAL);

	MASK_set_flag(&event->mask_or[0], FLAG_TIMEOUT_PPM);
	MASK_set_flag(&event->mask_or[0], FLAG_REQUEST_STOP_MOTORS);
	MASK_set_flag(&event->mask_or[0], FLAG_CHAN_5_POS_1);


	//Mask 2 : mode de vol imu calib :
	MASK_set_flag(&event->mask_and[1], FLAG_STATE_MANUAL);

	MASK_set_flag(&event->mask_or[1], FLAG_TIMEOUT_PPM);
}

void mask_def_manual(Event_t * event){
	//Mask 1: mode de vol au sol :
	MASK_set_flag(&event->mask_and[0], FLAG_STATE_MANUAL);

	MASK_set_flag(&event->mask_or[0], FLAG_TIMEOUT_PPM);
	MASK_set_flag(&event->mask_or[0], FLAG_REQUEST_STOP_MOTORS);
	MASK_set_flag(&event->mask_or[0], FLAG_CHAN_5_POS_1);
}
