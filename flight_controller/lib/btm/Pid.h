/*
 * pid.h
 *
 *  Created on: 22 mars 2020
 *      Author: Theo
 */

#ifndef PID_H_
#define PID_H_
#include "macro_types.h"


#define PID_KP 			0
#define PID_KI 			1
#define PID_KD 			2
#define PID_FREQUENCY 	3
#define PID_MAX_OUTPUT 	4
#define PID_ALPHA 		5
#define PID_BETA 		6
#define PID_GAMMA 		7
#define PID_NB_SETTINGS 8

typedef struct {
	//Sorties
	double output ;
	double P;
	double I;
	double D;
	//Settings
	double settings[PID_NB_SETTINGS] ;

	//Error
	double error ;
	//Previous value
	double previous_measurement ;
	double D_filtered[3];//D[0] coresspond à mtn, D[1] coresspond à D à t-1 et l'autre t-2
	bool_e use_D_filtered ;
}PID_t;

//Pid basic PID
double PID_compute(PID_t* pid, double consigne, double angle);
//Pid with first order filter on d
double PID_compute_FOF_d(PID_t* pid, double consigne, double angle);
//Pid with second order filter on d
double PID_compute_SOF_d(PID_t* pid, double consigne, double angle);
//Init function
void PID_init(PID_t* pid, double settings[PID_NB_SETTINGS]);

#endif /* PID_H_ */
