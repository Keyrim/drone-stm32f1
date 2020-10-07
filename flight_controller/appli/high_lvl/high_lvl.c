/*
 * high_lvl_cases.c
 *
 *  Created on: 29 juin 2020
 *      Author: Théo
 */

#include <high_lvl/high_lvl.h>

#include "../../ressources/sequences_led.h"
#include "../telemetrie/telemetrie.h"
#include "../lib/btm/Test_transition.h"
#include "stdlib.h"




//Mode "on_the_ground" : on arrête les moteurs
void HIGH_LVL_On_The_Ground(State_drone_t * drone){

	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = STAB_OFF ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_1);
	}

}

//Mode "manual" : stablisation "levelled" et on met à jour les consignes d'angles par rapport à la télécomande radio
void HIGH_LVL_Manual(State_drone_t * drone){
	//			---------------------------- 		MAIN PART 			----------------------------------------
	if(drone->soft.entrance_flight_mode){
		drone->capteurs.mpu.z = 0 ;
		drone->stabilisation.stab_mode = LEVELLED ;
		drone->consigne.yaw = 0 ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_2);
	}

	drone->consigne.roll = ((float)(drone->communication.ibus.channels[ROLL]- 1500) / (float)500) * (float)SETPOINT_MAX_LVLED_PITCH_ROLL ;
	drone->consigne.pitch = ((float)(drone->communication.ibus.channels[PITCH]- 1500) / (float)500) * (float)SETPOINT_MAX_LVLED_PITCH_ROLL ;
	drone->consigne.yaw_rate = (float)(drone->communication.ibus.channels[YAW] - 1500) * (float)SETPOINT_MAX_ACCRO_YAW / (float)500 ;
	drone->consigne.throttle = (float)(drone->communication.ibus.channels[THROTTLE] - 1050)  ;

}

//Mode "manual_accro" : stabilisation "accro" et met à jour les consignes de vitesses angulaires par rapport à la télécomande radio
void HIGH_LVL_Manual_Accro(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = ACCRO ;
		drone->capteurs.mpu.z = 0 ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_8);
	}

	//			---------------------------- 		MAIN PART 			----------------------------------------
	drone->consigne.roll_rate = (float)(drone->communication.ibus.channels[ROLL]- 1500) * (float)SETPOINT_MAX_ACCRO_PITCH_ROLL / (float)500 ;
	drone->consigne.pitch_rate = (float)(drone->communication.ibus.channels[PITCH] - 1500) * (float)SETPOINT_MAX_ACCRO_PITCH_ROLL / (float)500 ;
	drone->consigne.yaw_rate = (float)(drone->communication.ibus.channels[YAW] - 1500) * (float)SETPOINT_MAX_ACCRO_YAW / (float)500 ;
	drone->consigne.throttle = (float)(drone->communication.ibus.channels[THROTTLE] - 1050) ;

}

//Mode "manual_hand_control" : stabilisation "levelled", les consignes d'angles sont reçu par la base (base équipée d'un mpu6050 positioné sur la main)
void HIGH_LVL_Manual_Hand_Control(State_drone_t * drone, State_base_t * base){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = LEVELLED ;
		TELEMETRIE_send_consigne_base(SUB_ID_BASE_CONSIGNE_START_SENDING_ANGLES);
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_4);
	}
	//			---------------------------- 		MAIN PART 			----------------------------------------
	drone->consigne.yaw = ((float)(drone->communication.ibus.channels[YAW] - 1500) / (float)500) * (float)SETPOINT_MAX_ACCRO_YAW ;
	drone->consigne.throttle = (float)(drone->communication.ibus.channels[THROTTLE] - 1050)  ;
	drone->consigne.roll = base->angle_x ;
	drone->consigne.pitch = base->angle_y ;



}

//Mode "parachute" : stabilisation "levelled", on appel la sub parachute
void HIGH_LVL_Parachute(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = LEVELLED ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_3);
	}

	//			---------------------------- 		MAIN PART 			----------------------------------------
	drone->consigne.throttle = (float)(drone->communication.ibus.channels[THROTTLE] - 1050)  ;
	if(sub_parachute(drone) != IN_PROGRESS){
		EVENT_Set_flag(FLAG_SUB_PARACHUTE_OVER);
	}

}


//Mode "calibrate_mpu" :stabilisation off,  quand le drone est au sol on peut calibrer le mpu
void HIGH_LVL_Calibrate_MPU(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = STAB_OFF ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_6);
	}

	//			---------------------------- 		MAIN PART 			----------------------------------------
	if(Mpu_imu_calibrate(&drone->capteurs.mpu, 1000)){
		EVENT_Set_flag(FLAG_IMU_DONE_CALIB);
	}
}

//Mode "manual_pc" : stabilisation levelled, on control le "throttle" depuis le pc
void HIGH_LVL_Manual_Pc(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = ACCRO ;
		drone->consigne.throttle = 1000 ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_5);
	}
}

//Mode "imu_failed_init" : stabilisation off,  si le mpu ne s'est pas init, on reste bloqué au sol en attente d'un reset hard
void HIGH_LVL_IMU_Failed_Init(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
			drone->stabilisation.stab_mode = STAB_OFF ;
			LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_7);
	}
}

//Mode "change_pid_settings" :stabilisation off, drone au sol on modifi les valeurs des coefs pour les pids depuis la télécomande
void HIGH_LVL_Change_Pid_Settings(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
			drone->stabilisation.stab_mode = STAB_OFF ;
			LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_9);
	}

	PID_t * pid_roll ;
	PID_t * pid_pitch ;

	#if SET_COEF_ON_RATE_PID
		pid_roll = &drone->stabilisation.pid_roll_rate ;
		pid_pitch = &drone->stabilisation.pid_pitch_rate ;
	#else
		pid_roll = &drone->stabilisation.pid_roll ;
		pid_pitch = &drone->stabilisation.pid_pitch ;
	#endif

	const float divider = 1000000 ;
	if(drone->communication.ibus.channels[SWITCH_3] > 1300 && drone->communication.ibus.channels[SWITCH_3] < 1600){
		//Dead band pour ne pas increase pour rien
		if(abs(drone->communication.ibus.channels[8] - 1500) > 20)
			pid_roll->settings[PID_KP] += (float)(drone->communication.ibus.channels[8] - 1500) / divider ;
		if(abs(drone->communication.ibus.channels[9] - 1500) > 20)
			pid_roll->settings[PID_KD] += (float)(drone->communication.ibus.channels[9] - 1500) / divider ;

		//Pas de valeurs négative, on veut pas compenser à l'envers
		pid_roll->settings[PID_KP] = MAX(pid_roll->settings[PID_KP], 0);
		pid_roll->settings[PID_KD] = MAX(pid_roll->settings[PID_KD], 0);

		//Pas de valeur trop importantes non plus
		pid_roll->settings[PID_KP] = MIN(pid_roll->settings[PID_KP], 15);
		pid_roll->settings[PID_KD] = MIN(pid_roll->settings[PID_KD], 15);

	}
	else if(drone->communication.ibus.channels[SWITCH_3] > 1600){
		//Dead band pour ne pas increase pour rien
		if(abs(drone->communication.ibus.channels[8] - 1500) > 20)
			pid_pitch->settings[PID_KP] += (float)(drone->communication.ibus.channels[8] - 1500) / divider ;
		if(abs(drone->communication.ibus.channels[9] - 1500) > 20)
			pid_pitch->settings[PID_KD] += (float)(drone->communication.ibus.channels[9] - 1500) / divider ;

		//Pas de valeurs négative, on veut pas compenser à l'envers
		pid_pitch->settings[PID_KP] = MAX(pid_pitch->settings[PID_KP], 0);
		pid_pitch->settings[PID_KD] = MAX(pid_pitch->settings[PID_KD], 0);

		//Pas de valeur trop importantes non plus
		pid_pitch->settings[PID_KP] = MIN(pid_pitch->settings[PID_KP], 15);
		pid_pitch->settings[PID_KD] = MIN(pid_pitch->settings[PID_KD], 15);
	}

}

//Fonctions appelée par la tâche high qui appelle la fonction qui correspondant au mode de vol
void HIGH_LVL_Switch(State_drone_t * drone, State_base_t * base){
	//Détermine un changement de mode de vol
		static Flight_Mode_SM previous_state_flight_mode = PARACHUTE ;
		drone->soft.entrance_flight_mode = drone->soft.state_flight_mode != previous_state_flight_mode ;
		previous_state_flight_mode = drone->soft.state_flight_mode ;

		//Si changement mode de vol suite à

		//Switch sur les différents états de la high lvl
		switch(drone->soft.state_flight_mode){
			case ON_THE_GROUND :
				HIGH_LVL_On_The_Ground(drone);
				break;

			case MANUAL :
				HIGH_LVL_Manual(drone);
				break;

			case MANUAL_HAND_CONTROL:
				HIGH_LVL_Manual_Hand_Control(drone, base);
				break;

			case PARACHUTE :
				HIGH_LVL_Parachute(drone);
				break;

			case CALIBRATE_MPU6050:
				HIGH_LVL_Calibrate_MPU(drone);
				break;

			case MANUAL_PC:
				HIGH_LVL_Manual_Pc(drone);
				break;

			case MANUAL_ACCRO :
				HIGH_LVL_Manual_Accro(drone);
				break;

			case IMU_FAILED_INIT:
				HIGH_LVL_IMU_Failed_Init(drone);
				break;

			case PID_CHANGE_SETTINGS:
				HIGH_LVL_Change_Pid_Settings(drone);
				break;

			case POSITION_HOLD:
				//TODO Position hold function
				break;
			case ALTITUDE_HOLD:
				//TODO Altitude hold function
				break;
		}
}


static void Flag_channel_analysis(State_drone_t * drone){
	//Pour simplifier les lignes suivante
	channel_analysis_t  * ch_analysis = &drone->communication.ch_analysis ;
	//Switch 1

	if(ch_analysis->switch_pos[4] == SWITCH_POS_1){
		EVENT_Set_flag(FLAG_CHAN_5_POS_1);
		EVENT_Clean_flag(FLAG_CHAN_5_POS_3);
	}
	else {
		EVENT_Set_flag(FLAG_CHAN_5_POS_3);
		EVENT_Clean_flag(FLAG_CHAN_5_POS_1);
	}
	//Switch 2
	if(ch_analysis->switch_pos[5] == SWITCH_POS_1){
		EVENT_Set_flag(FLAG_CHAN_6_POS_1);
		EVENT_Clean_flag(FLAG_CHAN_6_POS_2);
		EVENT_Clean_flag(FLAG_CHAN_6_POS_3);
	}
	else if (ch_analysis->switch_pos[5] == SWITCH_POS_2){
		EVENT_Clean_flag(FLAG_CHAN_6_POS_1);
		EVENT_Set_flag(FLAG_CHAN_6_POS_2);
		EVENT_Clean_flag(FLAG_CHAN_6_POS_3);
	}
	else {
		EVENT_Clean_flag(FLAG_CHAN_6_POS_1);
		EVENT_Clean_flag(FLAG_CHAN_6_POS_2);
		EVENT_Set_flag(FLAG_CHAN_6_POS_3);
	}

	//Switch 3
	if(ch_analysis->switch_pos[6] == SWITCH_POS_1){
		EVENT_Set_flag(FLAG_CHAN_7_POS_1);
		EVENT_Clean_flag(FLAG_CHAN_7_POS_2);
		EVENT_Clean_flag(FLAG_CHAN_7_POS_3);
	}
	else if (ch_analysis->switch_pos[6] == SWITCH_POS_2){
		EVENT_Clean_flag(FLAG_CHAN_7_POS_1);
		EVENT_Set_flag(FLAG_CHAN_7_POS_2);
		EVENT_Clean_flag(FLAG_CHAN_7_POS_3);
	}
	else {
		EVENT_Clean_flag(FLAG_CHAN_7_POS_1);
		EVENT_Clean_flag(FLAG_CHAN_7_POS_2);
		EVENT_Set_flag(FLAG_CHAN_7_POS_3);
	}

	//Switch 4
	if(ch_analysis->switch_pos[7] == SWITCH_POS_1){
		EVENT_Set_flag(FLAG_CHAN_8_POS_1);
		EVENT_Clean_flag(FLAG_CHAN_8_POS_3);
	}
	else {
		EVENT_Set_flag(FLAG_CHAN_8_POS_3);
		EVENT_Clean_flag(FLAG_CHAN_8_POS_1);
	}

	//Throttle lvl
	if(ch_analysis->throttle_lvl[2] == THROTTLE_NULL){
		EVENT_Set_flag(FLAG_THROTTLE_NULL);
		EVENT_Clean_flag(FLAG_THROTTLE_LOW);
		EVENT_Clean_flag(FLAG_THROTTLE_HIGH);
	}
	else if(ch_analysis->throttle_lvl[2] == THROTTLE_LOW){
		EVENT_Clean_flag(FLAG_THROTTLE_NULL);
		EVENT_Set_flag(FLAG_THROTTLE_LOW);
		EVENT_Clean_flag(FLAG_THROTTLE_HIGH);
	}
	else {
		EVENT_Clean_flag(FLAG_THROTTLE_NULL);
		EVENT_Clean_flag(FLAG_THROTTLE_LOW);
		EVENT_Set_flag(FLAG_THROTTLE_HIGH);
	}

	//Button 1
	if(ch_analysis->analysis_mode[8]==ANALYSIS_BUTTON_ON_OFF){
		if(ch_analysis->button_on_off[8] == BUTTON_ON){
			EVENT_Set_flag(FLAG_CHAN_9_ON);
			EVENT_Clean_flag(FLAG_CHAN_9_OFF);
		}
		else{
			EVENT_Set_flag(FLAG_CHAN_9_OFF);
			EVENT_Clean_flag(FLAG_CHAN_9_ON);
		}
	}
	else if(ch_analysis->analysis_mode[8]==ANALYSIS_BUTTON_PUSH){
		if(ch_analysis->button_pushed[8] == BUTTON_PUSHED_REQUEST){
			ch_analysis->button_pushed[8] = BUTTON_PUSHED_NO_REQUEST ;
			EVENT_Set_flag(FLAG_CHAN_9_PUSH);
		}
	}
	//Button 2
	if(ch_analysis->analysis_mode[9]==ANALYSIS_BUTTON_ON_OFF){
		if(ch_analysis->button_on_off[9] == BUTTON_ON){
			EVENT_Set_flag(FLAG_CHAN_10_ON);
			EVENT_Clean_flag(FLAG_CHAN_10_OFF);
		}
		else{
			EVENT_Set_flag(FLAG_CHAN_10_OFF);
			EVENT_Clean_flag(FLAG_CHAN_10_ON);
		}
	}
	else if(ch_analysis->analysis_mode[9]==ANALYSIS_BUTTON_PUSH){
		if(ch_analysis->button_pushed[9] == BUTTON_PUSHED_REQUEST){
			ch_analysis->button_pushed[9] = BUTTON_PUSHED_NO_REQUEST ;
			EVENT_Set_flag(FLAG_CHAN_10_PUSH);
		}
	}
}


void HIGH_LVL_Update_Flags(State_drone_t * drone){
	Flag_channel_analysis(drone);
}











