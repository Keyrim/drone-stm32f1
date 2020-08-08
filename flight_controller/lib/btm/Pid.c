/*
 * pid.c
 *
 *  Created on: 22 mars 2020
 *      Author: Theo
 */

#include "Pid.h"

//renvoit la sortie du pid
double PID_compute(PID_t* pid, double consigne, double measurement){
	//Calcul de l'erreur
	pid->error = measurement - consigne ;

	//Calcul des P I D
	pid->P = pid->settings[PID_KP] * pid->error ;
	pid->I += pid->settings[PID_KI] * pid->error / pid->settings[PID_FREQUENCY];
	pid->D = pid->settings[PID_KD] * (measurement - pid->previous_measurement) * pid->settings[PID_FREQUENCY] ;


	//On somme le tout
	pid->output = pid->P + pid->I + pid->D ;

	//Check mid and max
	pid->output = MIN(pid->settings[PID_MAX_OUTPUT], pid->output);
	pid->output = MAX(-pid->settings[PID_MAX_OUTPUT], pid->output);

	//Mise à jour des valeurs antérieures
	pid->previous_measurement = measurement ;
	return pid->output ;
}

//Renvoit la sortie du pid avec filtre du premier ordre sur le D
double PID_compute_FOF_d(PID_t* pid, double consigne, double measurement){
	//Calcul de l'erreur
	pid->error = measurement - consigne ;

	//Calcul des P I D
	pid->P = pid->settings[PID_KP] * pid->error ;
	pid->I += pid->settings[PID_KI] * pid->error / pid->settings[PID_FREQUENCY];
	pid->D = pid->settings[PID_KD] * (measurement - pid->previous_measurement) * (double)pid->settings[PID_FREQUENCY] ;

	//Calcul du D lissé au premier ordre
	pid->D_filtered[0] = pid->settings[PID_ALPHA] * pid->D + pid->settings[PID_BETA] * pid->D_filtered[1] ;

	//Pit output
	pid->output = pid->P + pid->I + pid->D_filtered[0] ;

	//Check mid and max
	pid->output = MIN(pid->settings[PID_MAX_OUTPUT], pid->output);
	pid->output = MAX(-pid->settings[PID_MAX_OUTPUT], pid->output);

	//Mise à jour des valeurs antérieures
	pid->previous_measurement = measurement ;
	pid->D_filtered[1] = pid->D_filtered[0] ;
	return pid->output ;
}

//Renvoit la sortie du pid avec filtre du second ordre sur le D
double PID_compute_SOF_d(PID_t* pid, double consigne, double measurement){
	//Calcule error
	pid->error = measurement - consigne ;

	//Calcule P I D
	pid->P = pid->settings[PID_KP] * pid->error ;
	pid->I += pid->settings[PID_KI] * pid->error / pid->settings[PID_FREQUENCY];
	pid->D = pid->settings[PID_KD] * (measurement - pid->previous_measurement) * (double)pid->settings[PID_FREQUENCY] ;

	//Calcul D filtré au second ordre
	pid->D_filtered[0] = pid->settings[PID_ALPHA] * pid->D + pid->settings[PID_BETA] * pid->D_filtered[1] + pid->settings[PID_GAMMA] * pid->D_filtered[2];

	//PID output
	pid->output = pid->P + pid->I + pid->D_filtered[0] ;

	//Check mid and max
	pid->output = MIN(pid->settings[PID_MAX_OUTPUT], pid->output);
	pid->output = MAX(-pid->settings[PID_MAX_OUTPUT], pid->output);

	//Mise à jour des valeurs antérieures
	pid->previous_measurement = measurement ;
	pid->D_filtered[2] = pid->D_filtered[1] ;
	pid->D_filtered[1] = pid->D_filtered[0] ;
	return pid->output ;
}

//Init des valeurs sur le pid donné
void PID_init(PID_t* pid, double settings[PID_NB_SETTINGS]){
	//Set settings
	for(uint8_t s = 0 ; s < PID_NB_SETTINGS; s++)
		pid->settings[s] = settings[s] ;

	//Init values
	pid->I = 0 ;
	pid->D_filtered[1] = 0 ;
	pid->D_filtered[2] = 0 ;
}
