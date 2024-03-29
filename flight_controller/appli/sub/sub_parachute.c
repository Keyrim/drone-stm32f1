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

//Sub parachute, elle remet le drone � l'horizontal puis fait de l'altitude au drone jusqu'� toucher le sol, ou autre chose :)
running_e sub_parachute(State_drone_t * drone){
	running_e progression = IN_PROGRESS ;
	switch(state_parachute){
		case PARACHUTE_INIT :
			drone->consigne.roll = 0 ;
			drone->consigne.pitch = 0 ;
			state_parachute = PARACHUTE_LAND ;
			break;
		case PARACHUTE_LAND :
			//D'apr�s une "�tude " faite par moi m�me (enregistrement du 30 / 05 / 2020 sur google drive)
			//L'acc ne peut �tre n�gative uniquement quand on atterit ou si le drone est � l'envers .. )
			//Et dans un vol m�me un peu agro, l'acc ne d�passe pas 3.5 g)
			if(drone->capteurs.mpu.z_acc_filtered < 0 || drone->capteurs.mpu.z_acc_filtered > 3.5){
				//On a toucher le sol, ou qqchose.. Coupure des moteurs puis fin de la sub
				drone->consigne.throttle = 0 ;
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

