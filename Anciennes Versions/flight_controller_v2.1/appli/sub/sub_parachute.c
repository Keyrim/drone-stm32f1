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

running_e sub_parachute(double * consigne_roll, double * consigne_pitch, double * consigne_throtle, double acc_z){
	running_e progression = IN_PROGRESS ;
	switch(state_parachute){
		case PARACHUTE_INIT :
			*consigne_roll = 0 ;
			*consigne_pitch = 0 ;
			state_parachute = PARACHUTE_LAND ;
			break;
		case PARACHUTE_LAND :
			//D'après une "étude " faite par moi même (enregistrement du 30 / 05 / 2020)
			//L'acc ne peut être négative uniquement quand on atterit ou si le drone est à l'envers .. )
			//Et dans un vol même un peu agro, l'acc ne dépasse pas 3.5 g)
			if(acc_z < 0 || acc_z > 3.5){
				//On a toucher le sol, ou qqchoses .. Coupure des moteurs puis fin de la sub
				*consigne_throtle = 0 ;
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

