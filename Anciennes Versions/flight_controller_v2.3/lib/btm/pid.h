/*
 * pid.h
 *
 *  Created on: 22 mars 2020
 *      Author: Theo
 */

#ifndef PID_H_
#define PID_H_
#include "macro_types.h"


typedef struct {
	double output ;
	double kP ;
	double kI ;
	double kD ;
	double kD_filter ;
	double error ;
	double previous_error ;
	double previous_angle ;
	uint16_t frequency ;
	double max_output ;
	double P;
	double I;
	double D;
	double D_filtered;
	bool_e use_D_filtered ;
}PID_t;

double PID_compute(PID_t* pid, double consigne, double angle);
void PID_init(PID_t* pid, double kp, double ki, double kd, double kd_filter, uint16_t frequency_, double max_ouput, bool_e use_D_filtered);

#endif /* PID_H_ */
