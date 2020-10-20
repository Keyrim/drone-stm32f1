/*
 * pid.h
 *
 *  Created on: 22 mars 2020
 *      Author: Theo
 */

#ifndef PID_H_
#define PID_H_
#include "macro_types.h"
#include "Filters.h"

typedef enum{
	 PID_KP = 0,
	 PID_KI,
	 PID_KD,
	 PID_FREQUENCY,
	 PID_MAX_OUTPUT,
	 PID_NB_SETTINGS,
}PID_Settings_name_e;


typedef struct {
	//Sorties
	float output ;
	float P;
	float I;
	float D;
	//Settings
	float settings[PID_NB_SETTINGS];
	//Error
	float error ;
	//Previous value
	float previous_measurement ;
	//Filters
	Filter_t d_filter ;
	Filter_order_e d_filter_order ;
}PID_t;

//Init function
void PID_init(PID_t * pid, float settings[PID_NB_SETTINGS], Filter_order_e d_filter_order, float settings_filter[3]);
//Pid update
float PID_compute(PID_t * pid, float consigne, float value);


#endif /* PID_H_ */
