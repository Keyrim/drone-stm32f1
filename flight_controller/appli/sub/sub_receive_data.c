/*
 * sub_receive_data.c
 *
 *  Created on: 13 juin 2020
 *      Author: Théo
 */


#include "sub_action.h"
#include "../IDs.h"

//Macro pratique
#define SAVE_AND_INCREASE buffer[compteur] = c ;compteur++ ;

typedef enum {
	ATTENTE,
	RECEIVING
}sub_receive_e;


void sub_receive_data(uint8_t c, State_drone_t * drone, State_base_t * base){

	static uint8_t id = 0 ;
	static uint8_t compteur = 0 ;
	static uint8_t buffer[5] = {0};
	static uint8_t new_data = 1 ;

	PID_t * pid_roll ;
	PID_t * pid_pitch ;

#if SET_COEF_ON_RATE_PID
	pid_roll = &drone->stabilisation.pid_roll_rate ;
	pid_pitch = &drone->stabilisation.pid_pitch_rate ;
#else
	pid_roll = &drone->stabilisation.pid_roll ;
	pid_pitch = &drone->stabilisation.pid_pitch ;
#endif


	if(new_data){
		compteur = 0 ;
		id = c ;
		new_data = 0 ;
	}
	else
	{
		switch(id){
			case ID_DRONE_CONSIGNE :
				//Si on a reçu une consigne, on a que un octect à gérer donc pas besoin de compteur
				switch(c){
					case SUB_ID_DRONE_CONSIGNE_STOP_MOTEUR :
						EVENT_Set_flag(FLAG_REQUEST_STOP_MOTORS);
						break;
					case SUB_ID_DRONE_CONSIGNE_CALIBRATE_MPU :
						EVENT_Set_flag(FLAG_REQUEST_MPU_CALIBRATION);
						break;
					case SUB_ID_DRONE_CONSIGNE_MANUAL_PC :
						EVENT_Set_flag(FLAG_REQUEST_MANUAL_PC);
						break;
					case SUB_ID_DRONE_CONSIGNE_MOTOR_UP :
						drone->consigne.throttle = MIN(600, drone->consigne.throttle + 50);
						break;
					case SUB_ID_DRONE_CONSIGNE_MOTOR_DOWN :
						drone->consigne.throttle = MAX(0, drone->consigne.throttle - 50);
						break;
					default:
						break;
				}
				new_data = 1 ;
				break;
			case ID_DRONE_PITCH_ROLL_BASE:
				//On save la valeur et incrémente le compteur
				SAVE_AND_INCREASE ;
				//Si le compteur == 2, on a nos deux valeurs
				if(compteur == 2){
					base->angle_x = (float)(buffer[0] - 90)  ;
					base->angle_y = (float)(buffer[1] - 90)  ;
					new_data = 1 ;
				}
				break;

			case ID_DRONE_ROLL_KP :
				SAVE_AND_INCREASE ;
				if(compteur == 4){
					static int32_t k = 0;
					k += (int32_t)buffer[0] << 24;
					k += (int32_t)buffer[1] << 16;
					k += (int32_t)buffer[2] << 8;
					k += (int32_t)buffer[3] ;

					pid_roll->settings[PID_KP] = (float)k / (float)1000000;
				}
				break;
			case ID_DRONE_ROLL_KI :
				SAVE_AND_INCREASE ;
				if(compteur == 4){
					static int32_t k = 0;
					k += (int32_t)buffer[0] << 24;
					k += (int32_t)buffer[1] << 16;
					k += (int32_t)buffer[2] << 8;
					k += (int32_t)buffer[3] ;
					pid_roll->settings[PID_KI] = (float)k / (float)1000000;
				}
				break;

			case ID_DRONE_ROLL_KD :
				SAVE_AND_INCREASE ;
				if(compteur == 4){
					static int32_t k = 0;
					k += (int32_t)buffer[0] << 24;
					k += (int32_t)buffer[1] << 16;
					k += (int32_t)buffer[2] << 8;
					k += (int32_t)buffer[3] ;
					pid_roll->settings[PID_KD] = (float)k / (float)1000000;
				}
				break;
			case ID_DRONE_PITCH_KP :
				SAVE_AND_INCREASE ;
				if(compteur == 4){
					static int32_t k = 0;
					k += (int32_t)buffer[0] << 24;
					k += (int32_t)buffer[1] << 16;
					k += (int32_t)buffer[2] << 8;
					k += (int32_t)buffer[3] ;
					pid_pitch->settings[PID_KP] = (float)k / (float)1000000;
				}
				break;
			case ID_DRONE_PITCH_KI :
				SAVE_AND_INCREASE ;
				if(compteur == 4){
					static int32_t k = 0;
					k += (int32_t)buffer[0] << 24;
					k += (int32_t)buffer[1] << 16;
					k += (int32_t)buffer[2] << 8;
					k += (int32_t)buffer[3] ;
					pid_pitch->settings[PID_KI] = (float)k / (float)1000000;
				}
				break;
			case ID_DRONE_PITCH_KD :
				SAVE_AND_INCREASE ;
				if(compteur == 4){
					static int32_t k = 0;
					k += (int32_t)buffer[0] << 24;
					k += (int32_t)buffer[1] << 16;
					k += (int32_t)buffer[2] << 8;
					k += (int32_t)buffer[3] ;
					pid_pitch->settings[PID_KD] = (float)k / (float)1000000;
				}
				break;



			default:
				//Si on comprend pas on reset les choses
				compteur = 0 ;
				id = c ;
				new_data = 0 ;
				break;
		}
	}
}
