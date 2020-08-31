/*
 * pid.c
 *
 *  Created on: 22 mars 2020
 *      Author: Theo
 */

#include "Pid.h"

//renvoit la sortie du pid
float PID_compute(PID_t * pid, float consigne, float measurement){
	//Calcul de l'erreur
	pid->error = measurement - consigne ;

	//Calcul des P I D
	pid->P = pid->settings[PID_KP] * pid->error ;
	pid->I += pid->settings[PID_KI] * pid->error / pid->settings[PID_FREQUENCY];
	pid->D = pid->settings[PID_KD] * (measurement - pid->previous_measurement) * pid->settings[PID_FREQUENCY] ;

	//Filtrage
	if(pid->use_filter_on_p)
		pid->P = FILTER_second_order_process(pid->filter_p, pid->P);
	if(pid->use_filter_on_d)
		pid->D = FILTER_second_order_process(pid->filter_d, pid->D);

	//On somme le tout
	pid->output = pid->P + pid->I + pid->D ;

	//Check mid and max
	pid->output = MIN(pid->settings[PID_MAX_OUTPUT], pid->output);
	pid->output = MAX(-pid->settings[PID_MAX_OUTPUT], pid->output);

	//Mise à jour des valeurs antérieures
	pid->previous_measurement = measurement ;
	return pid->output ;
}



//Init des valeurs sur le pid donné
void PID_init(PID_t * pid, float settings[PID_NB_SETTINGS]){
	//Set settings
	for(uint8_t s = 0 ; s < PID_NB_SETTINGS; s++)
		pid->settings[s] = settings[s] ;

	//Init values
	pid->I = 0 ;
}

void PID_set_filter_p(PID_t * pid,Filter_second_order_t * filter){
	pid->filter_p = filter ;
	pid->use_filter_on_p = TRUE ;
}

void PID_set_filter_d(PID_t * pid,Filter_second_order_t * filter){
	pid->filter_d = filter ;
	pid->use_filter_on_d = TRUE ;
}
