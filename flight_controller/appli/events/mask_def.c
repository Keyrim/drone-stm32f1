/*
 * mask_def.c
 *
 *  Created on: 17 sept. 2020
 *      Author: Theo
 */

#include "mask_def.h"

void mask_def_on_the_ground(Event_t * event){

	//Mask 1: mode de vol manuel : ppm_timeout | flag_chan_5_pos_1 | request_stop_motors | throttle_null
	MASK_set_flag(&event->mask_and[0], FLAG_STATE_MANUAL);

	MASK_set_flag(&event->mask_or[0], FLAG_TIMEOUT_PPM);
	MASK_set_flag(&event->mask_or[0], FLAG_REQUEST_STOP_MOTORS);
	MASK_set_flag(&event->mask_or[0], FLAG_CHAN_5_POS_1);
	MASK_set_flag(&event->mask_or[0], FLAG_THROTTLE_NULL);


	//Mask 2 : from imu calib : imu done calib
	MASK_set_flag(&event->mask_and[1], FLAG_STATE_CALIBRATE_MPU);

	MASK_set_flag(&event->mask_or[1], FLAG_IMU_DONE_CALIB);
}

void mask_def_manual(Event_t * event){
	//Mask 1: from on the ground : flag on_the_ground and flag switch 1 pos 3 and battery at least medium
	MASK_set_flag(&event->mask_and[0], FLAG_STATE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[0], FLAG_CHAN_5_POS_3);
	//MASK_set_flag(&event->mask_and[0], FLAG_BATTERY_MEDIUM);

	MASK_set_flag(&event->mask_or[0], FLAG_THROTTLE_LOW);
}
