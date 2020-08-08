/*
 * low_lvl_cases.c
 *
 *  Created on: 29 juin 2020
 *      Author: Théo
 */


#include "low_lvl_cases.h"

void LOW_LVL_Wait_Loop(State_drone_t * drone){

	uint32_t time = SYSTICK_get_time_us();
	if(time >= drone->previous_time_loop + LOOP_PERIODE){
		drone->previous_time_loop += LOOP_PERIODE ;
		drone->state_low_level = PWM_HIGH ;
		drone->free_time = 0 ;	//Si c'est l'heure on passe direct au state suivant
	}
	else
		drone->free_time = drone->previous_time_loop + LOOP_PERIODE - time ;	//si on a du temps dispo, on le dit
}
void LOW_LVL_Pwm_High(State_drone_t * drone){
	//Starting pwm signals for the escs
	ESC_Start_pulse();
	drone->state_low_level = UPDATE_ANGLES ;
}

void LOW_LVL_Update_Angles(State_drone_t * drone){
	//read mpu data and appli the filter
	MPU6050_ReadAll(&drone->mpu_data);
	COMP_FILTER_update_angles(&drone->mpu_angles);
	drone->state_low_level = VERIF_SYSTEM ;
}

void LOW_LVL_Verif_System(State_drone_t * drone){
	uint32_t time = SYSTICK_get_time_us();
	//Verif si le ppm est toujours bien reçu
	if(time > drone->time_last_read_ppm + TIME_OUT_PPM)
		drone->ppm_is_ok = FALSE ;
	else
		drone->ppm_is_ok = TRUE ;

	//Verif si le gps est ok
	if(time > drone->time_last_read_gps + TIME_OUT_GPS)
		drone->gps_is_ok = FALSE ;
	else
		drone->gps_is_ok = TRUE ;

	//Mesure batterie
	drone->v_bat = (double)ADC_getValue(ADC_9) * BATTERIE_RESISTANCES_COEF ;

	drone->state_low_level = PWM_LOW ;
}

void LOW_LVL_Pwm_Low(State_drone_t * drone){
	drone->state_low_level = try_action(ESC_End_pulse(), drone->state_low_level, ESCS_SETPOINTS, ERROR_HIGH_LEVEL);
}


void LOW_LVL_Escs_Setpoints(State_drone_t * drone, State_base_t * base){
	//Détermine un changement de mode de vol
	static Flight_Mode_SM previous_state_flight_mode = PARACHUTE ;
	drone->entrance_flight_mode = drone->state_flight_mode != previous_state_flight_mode ;
	previous_state_flight_mode = drone->state_flight_mode ;
	switch(drone->state_flight_mode){

		case ON_THE_GROUND :
			if(drone->entrance_flight_mode)
				LED_SEQUENCE_set_sequence(&drone->led_etat, SEQUENCE_LED_1);
			HIGH_LVL_On_The_Ground(drone);

			break;

		case MANUAL :
			if(drone->entrance_flight_mode)
					LED_SEQUENCE_set_sequence(&drone->led_etat, SEQUENCE_LED_2);
			HIGH_LVL_Manual(drone);

			break;
		case MANUAL_STYLEE:
			if(drone->entrance_flight_mode){
				TELEMETRIE_send_consigne_base(SUB_ID_BASE_CONSIGNE_START_SENDING_ANGLES, &drone->uart_telem);
				LED_SEQUENCE_set_sequence(&drone->led_etat, SEQUENCE_LED_4);
			}
			HIGH_LVL_Manual_Stylee(drone, base);
			break;
		case PARACHUTE :
			if(drone->entrance_flight_mode)
				LED_SEQUENCE_set_sequence(&drone->led_etat, SEQUENCE_LED_3);
			HIGH_LVL_Parachute(drone);

			break;
		case POSITION_HOLD:
			//TODO Position hold function
			break;
		case ALTITUDE_HOLD:
			//TODO Altitude hold function
			break;
	}


	//If we fly, we compute our outputs, according to our targets, to keep the drone leveled
	if(drone->state_flight_mode){
		double roll_output 	= PID_compute(&drone->pid_roll,- drone->roll_consigne + drone->mpu_angles.y, drone->mpu_angles.y);
		double pitch_output = PID_compute(&drone->pid_pitch,- drone->pitch_consigne + drone->mpu_angles.x, drone->mpu_angles.x);
		double yaw_output	= PID_compute(&drone->pid_yaw, - drone->yaw_consigne - drone->mpu_angles.z  , 0);

		ESC_Set_pulse(&drone->escs[0], (uint16_t)(1000 + drone->throttle_consigne + (int16_t)(- roll_output - pitch_output + yaw_output)));
		ESC_Set_pulse(&drone->escs[1], (uint16_t)(1000 + drone->throttle_consigne + (int16_t)(+ roll_output - pitch_output - yaw_output)));
		ESC_Set_pulse(&drone->escs[2], (uint16_t)(1000 + drone->throttle_consigne + (int16_t)(- roll_output + pitch_output - yaw_output)));
		ESC_Set_pulse(&drone->escs[3], (uint16_t)(1000 + drone->throttle_consigne + (int16_t)(+ roll_output + pitch_output + yaw_output)));
	}

	drone->state_low_level = SEND_DATA ;
}

void LOW_LVL_Send_Data(State_drone_t * drone){
	if(drone->channels[SWITCH_4] > 1500)
		sub_send_data(drone);
	drone->state_low_level = WAIT_LOOP ;
}



