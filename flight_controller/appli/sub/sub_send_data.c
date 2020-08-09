/*
 * sub_send_data.c
 *
 *  Created on: 19 avr. 2020
 *      Author: Theo
 */

#include "sub_action.h"

#include "../../lib/btm/Telemetrie.h"

#define NB_OCTECT_MAX 20	//On envoit au plus 20 octects par loop
#define NB_DATA_GROUP 18
static bool_e initialialized = FALSE ;



typedef struct{
	int32_t nb_octet;
	uint32_t periode;	//periode d'envoi par cycle ( 4 => un envoit tous les 4 cycles)
	bool_e to_send;
	void (*telemetrie_function)(State_drone_t * drone);
}Data_group_t;


//PLacer les données dans l'ordre de priorité d'envoit
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
	DATA_ALTITUDE
}names_data_group_t;

Data_group_t data_groups[NB_DATA_GROUP] ;


//Sub qui envoit des données par télémétrie à qui veut l'entendre ^^
void sub_send_data(State_drone_t * drone){
	if(!initialialized){

		//PIDS
		data_groups[DATA_PID_ROLL].nb_octet = 5 ;
		data_groups[DATA_PID_PITCH].nb_octet = 5 ;
		data_groups[DATA_PID_YAW].nb_octet = 5 ;
		data_groups[DATA_PID_ROLL_D].nb_octet = 5 ;
		data_groups[DATA_PID_ROLL_P].nb_octet = 5 ;

		data_groups[DATA_PID_ROLL].periode = 0 ;
		data_groups[DATA_PID_PITCH].periode = 0 ;
		data_groups[DATA_PID_YAW].periode = 0 ;
		data_groups[DATA_PID_ROLL_D].periode = 0 ;
		data_groups[DATA_PID_ROLL_P].periode = 0 ;

		data_groups[DATA_PID_ROLL].telemetrie_function = TELEMETRIE_send_pid_roll ;
		data_groups[DATA_PID_PITCH].telemetrie_function = TELEMETRIE_send_pid_pitch ;
		data_groups[DATA_PID_YAW].telemetrie_function = TELEMETRIE_send_pid_yaw ;
		data_groups[DATA_PID_ROLL_P].telemetrie_function = TELEMETRIE_send_pid_roll_p ;
		data_groups[DATA_PID_ROLL_D].telemetrie_function = TELEMETRIE_send_pid_roll_d ;

		//Altitude
		data_groups[DATA_ALTITUDE].nb_octet = 5 ;
		data_groups[DATA_ALTITUDE].periode = 0 ;
		data_groups[DATA_ALTITUDE].telemetrie_function = TELEMETRIE_send_altitude ;

		//Angles
		data_groups[DATA_ANGLES].nb_octet = 3 ;
		data_groups[DATA_ANGLE_Z].nb_octet = 3 ;
		data_groups[DATA_ANGLES_ACC].nb_octet = 3 ;

		data_groups[DATA_ANGLES].periode = 5 ;
		data_groups[DATA_ANGLE_Z].periode = 0 ;
		data_groups[DATA_ANGLES_ACC].periode = 10 ;

		data_groups[DATA_ANGLES].telemetrie_function = TELEMETRIE_send_angle_x_y_as_int ;
		data_groups[DATA_ANGLE_Z].telemetrie_function = TELEMETRIE_send_angle_z_as_int ;
		data_groups[DATA_ANGLES_ACC].telemetrie_function = TELEMETRIE_send_angle_x_y_acc_as_int ;


		//Latitude & longitude
		data_groups[DATA_LATTITUDE].nb_octet = 5 ;
		data_groups[DATA_LONGITUDE].nb_octet = 5 ;

		data_groups[DATA_LATTITUDE].periode = 0 ;
		data_groups[DATA_LONGITUDE].periode = 0 ;

		data_groups[DATA_LATTITUDE].telemetrie_function = TELEMETRIE_send_lat ;
		data_groups[DATA_LONGITUDE].telemetrie_function = TELEMETRIE_send_long ;

		//Accélération
		data_groups[DATA_ACC_Z].nb_octet = 5 ;
		data_groups[DATA_ACC_Z].periode = 0 ;
		data_groups[DATA_ACC_Z].telemetrie_function = TELEMETRIE_send_acc_z ;

		//radios
		data_groups[DATA_RADIO_1].nb_octet = 5 ;
		data_groups[DATA_RADIO_2].nb_octet = 5 ;

		data_groups[DATA_RADIO_1].periode = 20 ;
		data_groups[DATA_RADIO_2].periode = 50;

		data_groups[DATA_RADIO_1].telemetrie_function = TELEMETRIE_send_channel_all_1_4 ;
		data_groups[DATA_RADIO_2].telemetrie_function = TELEMETRIE_send_channel_all_5_8 ;

		//state_global v_bat et "every_is_ok"
		data_groups[DATA_FLIGHT_MODE].nb_octet = 2 ;
		data_groups[DATA_BATTERIE].nb_octet = 2 ;
		data_groups[DATA_EVERY_IS_OK].nb_octet = 2 ;

		data_groups[DATA_FLIGHT_MODE].periode = 250 ;
		data_groups[DATA_BATTERIE].periode = 250 ;
		data_groups[DATA_EVERY_IS_OK].periode = 250;

		data_groups[DATA_FLIGHT_MODE].telemetrie_function = TELEMETRIE_send_state_flying ;
		data_groups[DATA_BATTERIE].telemetrie_function = TELEMETRIE_send_v_bat ;
		data_groups[DATA_EVERY_IS_OK].telemetrie_function = TELEMETRIE_send_every_is_ok ;

		//moteurs
		data_groups[DATA_MOTEURS].nb_octet = 5;
		data_groups[DATA_MOTEURS].periode = 3;
		data_groups[DATA_MOTEURS].telemetrie_function = TELEMETRIE_send_moteur_all ;

		initialialized = TRUE;
	}

	static uint32_t compteur = 0 ;
	int32_t compteur_octet = NB_OCTECT_MAX ;

	for(uint8_t data = 0; data < NB_DATA_GROUP; data ++){
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

















