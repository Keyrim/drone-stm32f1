/*
 * sub_parachute.c
 *
 *  Created on: 30 mai 2020
 *      Author: Theo
 */
#include "sub_action.h"

typedef enum{
	PARACHUTE_INIT,
	PARACHUTE_LAND,
	PARACHUTE_LANDED
}STATES_sub_parachute_e;

static STATES_sub_parachute_e state_parachute = PARACHUTE_INIT ;

running_e sub_parachute(State_drone_t * drone){
	running_e progression = IN_PROGRESS ;
	switch(state_parachute){
		case PARACHUTE_INIT :
			drone->roll_consigne = 0 ;
			drone->pitch_consigne = 0 ;
			state_parachute = PARACHUTE_LAND ;
			break;
		case PARACHUTE_LAND :
			//D'après une "étude " faite par moi même (enregistrement du 30 / 05 / 2020)
			//L'acc ne peut être négative uniquement quand on atterit ou si le drone est à l'envers .. )
			//Et dans un vol même un peu agro, l'acc ne dépasse pas 3.5 g)
			if(drone->mpu_angles.z_acc < 0 || drone->mpu_angles.z_acc > 3.5){
				//On a toucher le sol, ou qqchoses .. Coupure des moteurs puis fin de la sub
				drone->throttle_consigne = 0 ;
				state_parachute = PARACHUTE_LANDED ;
			}
			break;
		case PARACHUTE_LANDED :
			state_parachute = PARACHUTE_INIT ;
			progression = END_OK ;
			break;
		default:
			progression = END_OK ;
			break;
	}

	return progression ;
}

