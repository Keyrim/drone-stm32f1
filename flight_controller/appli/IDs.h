/*
 * IDs.h
 *
 *  Created on: 15 juin 2020
 *      Author: Théo
 */

#ifndef IDS_H_
#define IDS_H_

//Def de tous ids pour la télémétrie
//Voir le fichier "addressage com drone" dans le dossier STM32
//L'appli sur le pc connait elle aussi tous ces id

typedef enum{
	ID_NOMBRE_ID =							13,

	//Id pour le drone
	ID_DRONE_CONSIGNE = 					0,

	ID_DRONE_PITCH_ROLL_BASE =				20,

	ID_DRONE_ROLL_KP =						60,
	ID_DRONE_ROLL_KI =						61,
	ID_DRONE_ROLL_KD =						62,
	ID_DRONE_PITCH_KP =						63,
	ID_DRONE_PITCH_KI =						64,
	ID_DRONE_PITCH_KD =						65,
	//Id pour le pc
	ID_PC_BATTERIE = 	 					100,
	ID_PC_STATE_FLYING = 					101,
	ID_PC_EVERY_IS_OK =						102,
	ID_PC_HIGH_LVL_TRANSITION =				103,

	ID_PC_ANGLE_X_Y =						120,
	ID_PC_X_Y_BASE =						121,
	ID_PC_ANGLE_X_Y_ACC =					122,
	ID_PC_ANGLE_Z =							123,
	ID_PC_ROLL_KP =							124,
	ID_PC_ROLL_KI =							125,
	ID_PC_ROLL_KD =							126,
	ID_PC_PITCH_KP =						127,
	ID_PC_PITCH_KI =						128,
	ID_PC_PITCH_KD =						129,

	ID_PC_ANGLE_X_Y_Z_RATE = 				140,

	ID_PC_MOTEUR_ALL = 						160,
	ID_PC_CHAN_1_4 =						161,
	ID_PC_CHAN_5_8	 =						162,
	ID_PC_LONGITUDE =						163,
	ID_PC_LATTITUDE	 =						164,
	ID_PC_ACC_Z	 =							165,
	ID_PC_PID_D_ROLL =						166,
	ID_PC_PID_P_ROLL =						167,
	ID_PC_PID_ROLL =						168,
	ID_PC_PID_PITCH	 =						169,
	ID_PC_PID_YAW =							170,
	ID_PC_ALTITUDE =						171,

	//Ids  pour la base
	ID_BASE_CONSIGNE_BASE =					200
}Ids_t;

typedef enum{
	//sub ids drone consigne
	SUB_ID_DRONE_CONSIGNE_STOP_MOTEUR = 		0,
	SUB_ID_DRONE_CONSIGNE_CALIBRATE_MPU = 		1,
	SUB_ID_DRONE_CONSIGNE_MANUAL_PC =			2,
	SUB_ID_DRONE_CONSIGNE_MOTOR_UP =			3,
	SUB_ID_DRONE_CONSIGNE_MOTOR_DOWN =			4
}SUB_ids_drone_consigne_t;

typedef enum{
	//sub ids base consigne
	SUB_ID_BASE_CONSIGNE_START_SENDING_ANGLES = 	0 ,
	SUB_ID_CONSIGNE_BASE_STOP_SENDING_ANGLES =		1
}sub_ids_base_consigne_t;

typedef enum{
	//Sous ID_PC_HIGH_LVL_TRANSITION:
	SUB_ID_PC_HIGH_LVL_TRANSITION_ARM_SWITCH =		0,
	SUB_ID_PC_HIGH_LVL_TRANSITION_PPM_ISNT_OK =		1,
	SUB_ID_PC_HIGH_LVL_TRANSITION_SUB_ENDED =		2,
	SUB_ID_PC_HIGH_LVL_TRANSITION_SWITCH =			3,
	SUB_ID_PC_HIGH_LVL_TRANSITION_THROTTLE_LOW =	4,
	SUB_ID_PC_HIGH_LVL_TRANSITION_PC_REQUEST =		5
}sub_ids_pc_high_lvl_transition_t;


//uint8_t liste_id[] = {0, 20, 100, 101, 102, 120, 160, 161, 162, 163, 164, 165, 200};
#endif /* IDS_H_ */
