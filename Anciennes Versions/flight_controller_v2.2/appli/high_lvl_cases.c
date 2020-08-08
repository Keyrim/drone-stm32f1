/*
 * high_lvl_cases.c
 *
 *  Created on: 29 juin 2020
 *      Author: Théo
 */

#include "high_lvl_cases.h"

void HIGH_LVL_On_The_Ground(State_drone_t * drone){
	//Si on est au sol on met la consigne des moteurs à zéro
	drone->escs[0].pulsation = PULSATION_MIN ;
	drone->escs[1].pulsation = PULSATION_MIN ;
	drone->escs[2].pulsation = PULSATION_MIN ;
	drone->escs[3].pulsation = PULSATION_MIN ;

	//Conditions de transitions
	if(drone->channels[SWITCH_1] > 1500 && drone->ppm_is_ok && (drone->v_bat > 10.8 || !USE_BATTERIE_CHECK)){	//Conditions générales
		if(drone->channels[THROTTLE] < 1100 && drone->channels[THROTTLE] > 1070){								//Pour passer en manuel
			drone->state_flight_mode = MANUAL ;
		}
	}
}

void HIGH_LVL_Manual(State_drone_t * drone){
	//			---------------------------- 		MAIN PART 			----------------------------------------
	drone->roll_consigne = ((double)(drone->channels[ROLL]- 1500) / (double)500) * (double)roll_and_pitch_max_setpoint ;
	drone->pitch_consigne = ((double)(drone->channels[PITCH]- 1500) / (double)500) * (double)roll_and_pitch_max_setpoint ;
	drone->yaw_consigne = ((double)(drone->channels[YAW] - 1500) / (double)500) * (double)yaw_max_setpoint ;		//Consigne en degré par seconde attention ^^
	drone->throttle_consigne = (double)(drone->channels[THROTTLE] - 1050)  ;

	//			---------------------------- Condition de transition 	----------------------------------------
	if(drone->channels[SWITCH_1] < 1500 || drone->channels[THROTTLE] < 1070 || !drone->ppm_is_ok){		// Securité
		drone->state_flight_mode = ON_THE_GROUND ;
	}
	else if(drone->channels[SWITCH_2] > 1650)														//Passage en parachute
		drone->state_flight_mode = PARACHUTE ;
	else if(drone->channels[SWITCH_2] > 1350)														//Passage en manuel "stylé"
		drone->state_flight_mode = MANUAL_STYLEE ;
}

void HIGH_LVL_Manual_Stylee(State_drone_t * drone, State_base_t * base){

	//			---------------------------- 		MAIN PART 			----------------------------------------
	drone->yaw_consigne = ((double)(drone->channels[YAW] - 1500) / (double)500) * (double)yaw_max_setpoint ;	//Consigne en degré par seconde attention ^^
	drone->throttle_consigne = (double)(drone->channels[THROTTLE] - 1050)  ;
	drone->roll_consigne = base->angle_x ;
	drone->pitch_consigne = base->angle_y ;

	//			---------------------------- Condition de transition 	----------------------------------------
	if(drone->channels[SWITCH_1] < 1500 || drone->channels[THROTTLE] < 1070 || !drone->ppm_is_ok)		// Securité
		drone->state_flight_mode = ON_THE_GROUND ;
	else if(drone->channels[SWITCH_2] > 1650)														//Passage en parachute
		drone->state_flight_mode = PARACHUTE ;
	else if(drone->channels[SWITCH_2] < 1350 && drone->channels[SWITCH_2] > 1000 )														//Passage en manuel "stylé"
		drone->state_flight_mode = MANUAL ;

}

void HIGH_LVL_Parachute(State_drone_t * drone){

	//			---------------------------- 		MAIN PART 			----------------------------------------
	drone->throttle_consigne = (double)(drone->channels[THROTTLE] - 1050)  ;
	if(sub_parachute(drone) != IN_PROGRESS){
		drone->state_flight_mode = ON_THE_GROUND ;
	}

	//			---------------------------- Condition de transition 	----------------------------------------
	if(drone->channels[SWITCH_1] < 1500 || drone->channels[THROTTLE] < 1070 || !drone->ppm_is_ok)		// Securité
		drone->state_flight_mode = ON_THE_GROUND ;
	else if(drone->channels[SWITCH_2] > 1350 && drone->channels[SWITCH_2] < 1650)														//Passage en parachute
		drone->state_flight_mode = MANUAL_STYLEE ;
	else if(drone->channels[SWITCH_2] < 1350 && drone->channels[SWITCH_2] > 1000 )														//Passage en manuel "stylé"
		drone->state_flight_mode = MANUAL ;
}
















