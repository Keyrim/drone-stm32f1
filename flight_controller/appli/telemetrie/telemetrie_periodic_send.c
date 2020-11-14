/*
 * telemetrie_periodic_send.c
 *
 *  Created on: 6 oct. 2020
 *      Author: Theo
 */


/*
 * sub_send_data.c
 *
 *  Created on: 19 avr. 2020
 *      Author: Theo
 */

#include "telemetrie.h"
#include "telemetrie_periodic_send_functions.h"
#include "../system_d.h"

#define NB_OCTECT_MAX 30	//On envoit au plus 20 octects par loop

typedef struct{
	int32_t nb_octet;
	uint32_t periode;	//periode d'envoi par cycle ( 4 => un envoit tous les 4 cycles)
	bool_e to_send;
	void (*telemetrie_function)(State_drone_t * drone);
}Data_group_t;


//PLacer les donnees dans l'ordre de prioritees d'envoit
typedef enum{
	DATA_ANGLES,
	DATA_ANGLE_Z,
	DATA_RADIO_1,
	DATA_RADIO_2,
	DATA_FLIGHT_MODE,
	DATA_BATTERIE,
	DATA_MOTEURS,
	DATA_LATTITUDE,
	DATA_LONGITUDE,
	DATA_EVERY_IS_OK,
	DATA_ACC_Z,
	DATA_ANGLES_ACC,
	DATA_PID_ROLL,
	DATA_PID_PITCH,
	DATA_PID_YAW,
	DATA_PID_ROLL_P,
	DATA_PID_ROLL_D,
	DATA_ALTITUDE,
	DATA_X_Y_Z_RATE,
	DATA_ROLL_KP,
	DATA_ROLL_KI,
	DATA_ROLL_KD,
	DATA_PITCH_KP,
	DATA_PITCH_KI,
	DATA_PITCH_KD,
	DATA_CPU_USED,
	DATA_TASK_PERIODE,
	DATA_GROUP_COUNT

}names_data_group_t;



#define DEFINE_DATA_GROUP(nb_octet_param, periode_param, telemetrie_function_param) { \
	.nb_octet = nb_octet_param,				\
	.periode = periode_param,				\
	.telemetrie_function = telemetrie_function_param \
}


Data_group_t data_groups[DATA_GROUP_COUNT] = {
		//PIDS
		[DATA_PID_ROLL] = DEFINE_DATA_GROUP(		5, 			0, 		TELEMETRIE_send_pid_roll),
		[DATA_PID_PITCH] = DEFINE_DATA_GROUP(		5, 			0, 		TELEMETRIE_send_pid_pitch),
		[DATA_PID_YAW] = DEFINE_DATA_GROUP(			5, 			0, 			TELEMETRIE_send_pid_yaw),	//Utilisé pour Gy Roll atm
		[DATA_PID_ROLL_P] = DEFINE_DATA_GROUP(		5, 			5, 			TELEMETRIE_send_pid_roll_p),
		[DATA_PID_ROLL_D] = DEFINE_DATA_GROUP(		5, 			5, 			TELEMETRIE_send_pid_roll_d),
		//Pids coefs
		[DATA_ROLL_KP] = DEFINE_DATA_GROUP(			3, 			0, 			TELEMETRIE_send_pid_roll_kp),
		[DATA_ROLL_KI] = DEFINE_DATA_GROUP(			3, 			0, 			TELEMETRIE_send_pid_roll_ki),
		[DATA_ROLL_KD] = DEFINE_DATA_GROUP(			3, 			0, 			TELEMETRIE_send_pid_roll_kd),
		[DATA_PITCH_KP] = DEFINE_DATA_GROUP(		3, 			0, 			TELEMETRIE_send_pid_pitch_kp),
		[DATA_PITCH_KI] = DEFINE_DATA_GROUP(		3, 			0, 			TELEMETRIE_send_pid_pitch_ki),
		[DATA_PITCH_KD] = DEFINE_DATA_GROUP(		3, 			0, 			TELEMETRIE_send_pid_pitch_kd),
		//Altitude
		[DATA_ALTITUDE] = DEFINE_DATA_GROUP(		5, 			0, 			TELEMETRIE_send_altitude),
		//Angles
		[DATA_ANGLES] = DEFINE_DATA_GROUP(			3, 			50, 		TELEMETRIE_send_angle_x_y_as_int),
		[DATA_ANGLE_Z] = DEFINE_DATA_GROUP(			3, 			0, 		TELEMETRIE_send_angle_z_as_int),
		[DATA_ANGLES_ACC] = DEFINE_DATA_GROUP(		3, 			0, 		TELEMETRIE_send_angle_x_y_acc_as_int),
		[DATA_X_Y_Z_RATE] = DEFINE_DATA_GROUP(		4, 			10, 		TELEMETRIE_send_angle_x_y_z_rate_as_int),
		//Latitude & longitude
		[DATA_LATTITUDE] = DEFINE_DATA_GROUP(		5, 			0, 			TELEMETRIE_send_lat),
		[DATA_LONGITUDE] = DEFINE_DATA_GROUP(		5, 			0, 			TELEMETRIE_send_long),
		//Accï¿½lï¿½ration
		[DATA_ACC_Z] = DEFINE_DATA_GROUP(			5, 			0, 			TELEMETRIE_send_acc_z),
		//Radios
		[DATA_RADIO_1] = DEFINE_DATA_GROUP(			5, 			100, 	TELEMETRIE_send_channel_all_1_4),
		[DATA_RADIO_2] = DEFINE_DATA_GROUP(			5, 			100, 		TELEMETRIE_send_channel_all_5_8),
		//States
		[DATA_FLIGHT_MODE] = DEFINE_DATA_GROUP(		2, 			50, 		TELEMETRIE_send_state_flying),
		[DATA_BATTERIE] = DEFINE_DATA_GROUP(		2, 			1000, 		TELEMETRIE_send_v_bat),
		[DATA_EVERY_IS_OK] = DEFINE_DATA_GROUP(		2, 			50, 		TELEMETRIE_send_every_is_ok),
		//Moteurs
		[DATA_MOTEURS] = DEFINE_DATA_GROUP(			5, 			50, 			TELEMETRIE_send_moteur_all),
		//Cpu used
		[DATA_CPU_USED] = DEFINE_DATA_GROUP(		2, 			100, 		TELEMETRIE_send_cpu_pourcentage),
		[DATA_TASK_PERIODE] = DEFINE_DATA_GROUP(	4, 			50, 		TELEMETRIE_send_periode_task)
};

//Sub qui envoit des donnï¿½es par telemetrie ï¿½ qui veut l'entendre ^^
void TELEMETRIE_Periodic_send(State_drone_t * drone){

	static uint32_t compteur = 0 ;
	int32_t compteur_octet = NB_OCTECT_MAX ;

	//On envoit les coefs des pids si on est en train de les changer
	if(drone->soft.state_flight_mode == PID_CHANGE_SETTINGS){
		if(drone->communication.ibus.channels[SWITCH_3] > 1600){
			data_groups[DATA_ROLL_KP].periode = 0 ;
			data_groups[DATA_ROLL_KI].periode = 0 ;
			data_groups[DATA_ROLL_KD].periode = 0 ;
			data_groups[DATA_PITCH_KP].periode = 10 ;
			data_groups[DATA_PITCH_KI].periode = 0 ;
			data_groups[DATA_PITCH_KD].periode = 10 ;
		}
		else{
			data_groups[DATA_ROLL_KP].periode = 10 ;
			data_groups[DATA_ROLL_KI].periode = 0 ;
			data_groups[DATA_ROLL_KD].periode = 10 ;
			data_groups[DATA_PITCH_KP].periode = 0 ;
			data_groups[DATA_PITCH_KI].periode = 0 ;
			data_groups[DATA_PITCH_KD].periode = 0 ;
		}

	}
	else{
		data_groups[DATA_ROLL_KP].periode = 0 ;
		data_groups[DATA_ROLL_KI].periode = 0 ;
		data_groups[DATA_ROLL_KD].periode = 0 ;
		data_groups[DATA_PITCH_KP].periode = 0 ;
		data_groups[DATA_PITCH_KI].periode = 0 ;
		data_groups[DATA_PITCH_KD].periode = 0 ;
	}

	for(uint8_t data = 0; data < DATA_GROUP_COUNT; data ++){
		if(data_groups[data].periode){
			if(!(compteur % data_groups[data].periode) || data_groups[data].to_send){
				if(compteur_octet >= data_groups[data].nb_octet){
					data_groups[data].telemetrie_function(drone);
					compteur_octet -= data_groups[data].nb_octet ;
					data_groups[data].to_send = FALSE ;
				}
				else
					data_groups[data].to_send  = TRUE ;
			}
		}
	}
	compteur ++ ;

}

















