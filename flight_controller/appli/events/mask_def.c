/*
 * mask_def.c
 *
 *  Created on: 17 sept. 2020
 *      Author: Theo
 */

#include "mask_def.h"

void mask_def_on_the_ground(Event_t * event){

	//Manuel : ppm_timeout | flag_chan_5_pos_1 | request_stop_motors | throttle_null
	MASK_set_flag(&event->mask_and[ON_THE_GROUND_MANUAL], FLAG_STATE_MANUAL);

	MASK_set_flag(&event->mask_or[ON_THE_GROUND_MANUAL], FLAG_TIMEOUT_PPM);
	MASK_set_flag(&event->mask_or[ON_THE_GROUND_MANUAL], FLAG_CHAN_5_POS_1);
	MASK_set_flag(&event->mask_or[ON_THE_GROUND_MANUAL], FLAG_THROTTLE_NULL);

	//Stop motors request occurs
	MASK_set_flag(&event->mask_or[ON_THE_GROUND_STOP_MOTORS_REQUEST], FLAG_REQUEST_STOP_MOTORS);

	//Manual accro, same as manual
	MASK_set_flag(&event->mask_and[ON_THE_GROUND_MANUAL_ACCRO], FLAG_STATE_MANUAL_ACCRO);

	MASK_set_flag(&event->mask_or[ON_THE_GROUND_MANUAL_ACCRO], FLAG_TIMEOUT_PPM);
	MASK_set_flag(&event->mask_or[ON_THE_GROUND_MANUAL_ACCRO], FLAG_CHAN_5_POS_1);
	MASK_set_flag(&event->mask_or[ON_THE_GROUND_MANUAL_ACCRO], FLAG_THROTTLE_NULL);

	//Parachute, if parachute déclenchement si sub_parachute_over
	MASK_set_flag(&event->mask_and[ON_THE_GROUND_PARACHUTE], FLAG_STATE_PARACHUTE);

	MASK_set_flag(&event->mask_or[ON_THE_GROUND_PARACHUTE], FLAG_SUB_PARACHUTE_OVER);

	//IMU calib : imu done calib
	MASK_set_flag(&event->mask_and[ON_THE_GROUND_IMU_CALIB], FLAG_STATE_CALIBRATE_MPU);

	MASK_set_flag(&event->mask_or[ON_THE_GROUND_IMU_CALIB], FLAG_IMU_DONE_CALIB);

	//Change pid settings, press button left
	MASK_set_flag(&event->mask_and[ON_THE_GROUND_CHANGE_PID_SETTINGS], FLAG_STATE_CHANGE_PID_SETTINGS);

	MASK_set_flag(&event->mask_or[ON_THE_GROUND_CHANGE_PID_SETTINGS], FLAG_CHAN_9_PUSH);

}
void mask_def_manual(Event_t * event){
	//On the ground : flag on_the_ground and flag switch 1 pos 3, button right OFF and battery at least medium
	MASK_set_flag(&event->mask_and[MANUAL_ON_THE_GROUND], FLAG_STATE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[MANUAL_ON_THE_GROUND], FLAG_CHAN_5_POS_3);
	MASK_set_flag(&event->mask_and[MANUAL_ON_THE_GROUND], FLAG_CHAN_6_POS_1);
	MASK_set_flag(&event->mask_and[MANUAL_ON_THE_GROUND], FLAG_CHAN_7_POS_1);
	MASK_set_flag(&event->mask_and[MANUAL_ON_THE_GROUND], FLAG_CHAN_10_OFF);
#if USE_BATTERIE_CHECK
	MASK_set_flag(&event->mask_and[MANUAL_ON_THE_GROUND], FLAG_BATTERY_MEDIUM);
#endif

	MASK_set_flag(&event->mask_or[MANUAL_ON_THE_GROUND], FLAG_THROTTLE_LOW);


	//Manual accro
	MASK_set_flag(&event->mask_and[MANUAL_MANUAL_ACCRO], FLAG_STATE_MANUAL_ACCRO);
	MASK_set_flag(&event->mask_and[MANUAL_MANUAL_ACCRO], FLAG_CHAN_10_OFF);

	MASK_set_flag(&event->mask_or[MANUAL_MANUAL_ACCRO], FLAG_CHAN_6_POS_1);

}
void mask_def_manual_pc(Event_t * event){
	//Mask 1 : from on the ground : flag on the ground switch 1 pos 3 v_bat medium et manual pc request
	MASK_set_flag(&event->mask_and[MANUAL_PC_ON_THE_GROUND], FLAG_STATE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[MANUAL_PC_ON_THE_GROUND], FLAG_CHAN_5_POS_3);
#if USE_BATTERIE_CHECK
	MASK_set_flag(&event->mask_and[MANUAL_PC_ON_THE_GROUND], FLAG_BATTERY_MEDIUM);
#endif

	MASK_set_flag(&event->mask_or[MANUAL_PC_ON_THE_GROUND], FLAG_REQUEST_MANUAL_PC);
}
void mask_def_manual_hand_control(Event_t * event){
	UNUSED(event);
	//Pas activable pour le moment
}
void mask_def_manual_accro(Event_t * event){
	//On the ground, comme pour tous les manuels et switch 2 pos 2
	MASK_set_flag(&event->mask_and[MANUAL_ACCRO_ON_THE_GROUND], FLAG_STATE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[MANUAL_ACCRO_ON_THE_GROUND], FLAG_CHAN_5_POS_3);
	MASK_set_flag(&event->mask_and[MANUAL_ACCRO_ON_THE_GROUND], FLAG_CHAN_6_POS_2);
	MASK_set_flag(&event->mask_and[MANUAL_ACCRO_ON_THE_GROUND], FLAG_CHAN_7_POS_1);
	MASK_set_flag(&event->mask_and[MANUAL_ACCRO_ON_THE_GROUND], FLAG_CHAN_10_OFF);
#if USE_BATTERIE_CHECK
	MASK_set_flag(&event->mask_and[MANUAL_ACCRO_ON_THE_GROUND], FLAG_BATTERY_MEDIUM);
#endif

	MASK_set_flag(&event->mask_or[MANUAL_ACCRO_ON_THE_GROUND], FLAG_THROTTLE_LOW);

	//Manual, if the switch right way
	MASK_set_flag(&event->mask_and[MANUAL_ACCRO_MANUAL], FLAG_STATE_MANUAL);
	MASK_set_flag(&event->mask_and[MANUAL_ACCRO_MANUAL], FLAG_CHAN_7_POS_1);

	MASK_set_flag(&event->mask_or[MANUAL_ACCRO_MANUAL], FLAG_CHAN_6_POS_2);


}
void mask_def_parachute(Event_t * event){
	//Manual if we press the left button
	MASK_set_flag(&event->mask_and[PARACHUTE_MANUAL], FLAG_STATE_MANUAL);

	MASK_set_flag(&event->mask_or[PARACHUTE_MANUAL], FLAG_CHAN_9_PUSH);

	//Manual accro if we press the left button
	MASK_set_flag(&event->mask_and[PARACHUTE_MANUAL_ACCRO], FLAG_STATE_MANUAL);

	MASK_set_flag(&event->mask_or[PARACHUTE_MANUAL_ACCRO], FLAG_CHAN_9_PUSH);
}
void mask_def_calibrate_mpu(Event_t * event){
	//On the ground, if we request it from the pc
	MASK_set_flag(&event->mask_and[CALIBRATE_MPU_PC_REQUEST], FLAG_STATE_ON_THE_GROUND);

	MASK_set_flag(&event->mask_or[CALIBRATE_MPU_PC_REQUEST], FLAG_REQUEST_MPU_CALIBRATION);

	//On the ground, if we press the button left and switch 2 pos 1
	MASK_set_flag(&event->mask_and[CALIBRATE_MPU_RADIO_REQUEST], FLAG_STATE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[CALIBRATE_MPU_RADIO_REQUEST], FLAG_CHAN_6_POS_1);

	MASK_set_flag(&event->mask_or[CALIBRATE_MPU_RADIO_REQUEST], FLAG_CHAN_9_PUSH);

}
void mask_def_error_sensors(Event_t * event){
	UNUSED(event);
}
void mask_def_change_pid_settings(Event_t * event){
	//On the ground, if we press the button left and switch 2 pos 1
	MASK_set_flag(&event->mask_and[CHANGE_PID_SETTINGS_RADIO_REQUEST], FLAG_STATE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[CHANGE_PID_SETTINGS_RADIO_REQUEST], FLAG_CHAN_6_POS_2);

	MASK_set_flag(&event->mask_or[CHANGE_PID_SETTINGS_RADIO_REQUEST], FLAG_CHAN_9_PUSH);
}















