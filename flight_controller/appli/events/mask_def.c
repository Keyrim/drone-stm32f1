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

	//Mask 2 : from any flight mode, if a stop motors request occurs
	MASK_set_flag(&event->mask_or[1], FLAG_REQUEST_STOP_MOTORS);

	//Mask 3 : from manual hand control, same as manual
	MASK_set_flag(&event->mask_and[2], FLAG_STATE_MANUAL_HAND_CONTROL);

	MASK_set_flag(&event->mask_or[2], FLAG_TIMEOUT_PPM);
	MASK_set_flag(&event->mask_or[2], FLAG_REQUEST_STOP_MOTORS);
	MASK_set_flag(&event->mask_or[2], FLAG_CHAN_5_POS_1);
	MASK_set_flag(&event->mask_or[2], FLAG_THROTTLE_NULL);

	//Mask 4 : from Manual accro, same as manual
	MASK_set_flag(&event->mask_and[3], FLAG_STATE_MANUAL_ACCRO);

	MASK_set_flag(&event->mask_or[3], FLAG_TIMEOUT_PPM);
	MASK_set_flag(&event->mask_or[3], FLAG_REQUEST_STOP_MOTORS);
	MASK_set_flag(&event->mask_or[3], FLAG_CHAN_5_POS_1);
	MASK_set_flag(&event->mask_or[3], FLAG_THROTTLE_NULL);

	//Mask 5 : from parachute, if parachute déclenchement si sub_parachute_over
	MASK_set_flag(&event->mask_and[4], FLAG_STATE_PARACHUTE);

	MASK_set_flag(&event->mask_or[4], FLAG_SUB_PARACHUTE_OVER);

	//Mask 6 : from imu calib : imu done calib
	MASK_set_flag(&event->mask_and[5], FLAG_STATE_CALIBRATE_MPU);

	MASK_set_flag(&event->mask_or[5], FLAG_IMU_DONE_CALIB);

	//Mask 7 : from change pid settings, if switch 3 pos 1
	MASK_set_flag(&event->mask_and[6], FLAG_STATE_CHANGE_PID_SETTINGS);

	MASK_set_flag(&event->mask_or[6], FLAG_CHAN_7_POS_1);
}
void mask_def_manual(Event_t * event){
	//Mask 1: from on the ground : flag on_the_ground and flag switch 1 pos 3 and battery at least medium
	MASK_set_flag(&event->mask_and[0], FLAG_STATE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[0], FLAG_CHAN_5_POS_3);
	MASK_set_flag(&event->mask_and[0], FLAG_CHAN_6_POS_1);
#if USE_BATTERIE_CHECK
	MASK_set_flag(&event->mask_and[0], FLAG_BATTERY_MEDIUM);
#endif

	MASK_set_flag(&event->mask_or[0], FLAG_THROTTLE_LOW);

	//Mask 2 : from_manual_hand_control, if the right switch is in the right position
	MASK_set_flag(&event->mask_and[1], FLAG_STATE_MANUAL_HAND_CONTROL);

	MASK_set_flag(&event->mask_or[1], FLAG_CHAN_5_POS_1);

	//Mask 3 : from_manual accro, same idea as hand control
	MASK_set_flag(&event->mask_and[2], FLAG_STATE_MANUAL_ACCRO);

	MASK_set_flag(&event->mask_or[2], FLAG_CHAN_5_POS_1);

	//Mask 4 : from parachute, same idea
	MASK_set_flag(&event->mask_and[3], FLAG_STATE_PARACHUTE);

	MASK_set_flag(&event->mask_or[3], FLAG_CHAN_5_POS_1);
}
void mask_def_manual_pc(Event_t * event){
	//Mask 1 : from on the ground : flag on the ground switch 1 pos 3 v_bat medium et manual pc request
	MASK_set_flag(&event->mask_and[0], FLAG_STATE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[0], FLAG_CHAN_5_POS_3);
#if USE_BATTERIE_CHECK
	MASK_set_flag(&event->mask_and[0], FLAG_BATTERY_MEDIUM);
#endif

	MASK_set_flag(&event->mask_or[0], FLAG_REQUEST_MANUAL_PC);
}
void mask_def_manual_hand_control(Event_t * event){
	//Pas activable pour le moment
}
void mask_def_manual_accro(Event_t * event){
	//Mask 1 : From on the ground, comme pour tous les manuels et switch 2 pos 2
	MASK_set_flag(&event->mask_and[0], FLAG_STATE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[0], FLAG_CHAN_5_POS_3);
	MASK_set_flag(&event->mask_and[0], FLAG_CHAN_6_POS_2);
#if USE_BATTERIE_CHECK
	MASK_set_flag(&event->mask_and[0], FLAG_BATTERY_MEDIUM);
#endif

	MASK_set_flag(&event->mask_or[0], FLAG_THROTTLE_LOW);

	//Mask 2 : From manual, if the switch right way
	MASK_set_flag(&event->mask_and[1], FLAG_STATE_MANUAL);

	MASK_set_flag(&event->mask_or[1], FLAG_CHAN_6_POS_2);

	//Mask 3 : From manual hand, if the switch right way
	MASK_set_flag(&event->mask_and[2], FLAG_STATE_MANUAL_HAND_CONTROL);

	MASK_set_flag(&event->mask_or[2], FLAG_CHAN_6_POS_2);

	//Mask 4 : From parachute, if the switch right way
	MASK_set_flag(&event->mask_and[3], FLAG_STATE_PARACHUTE);

	MASK_set_flag(&event->mask_or[3], FLAG_CHAN_6_POS_2);
}
void mask_def_parachute(Event_t * event){

}
void mask_def_calibrate_mpu(Event_t * event){

}
void mask_def_error_sensors(Event_t * event){

}
void mask_def_change_pid_settings(Event_t * event){

}
