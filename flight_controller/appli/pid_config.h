/*
 * pid_config.h
 *
 *  Created on: 3 août 2020
 *      Author: Theo
 */

#ifndef PID_CONFIG_H_
#define PID_CONFIG_H_

#include "macro_types.h"

#include "../lib/btm/Pid.h"

//Apprès analyse des courbes des sorties de pid sur un google sheet
//Un filtre du second ordre avec ces paramètres fonctionne bien
#define alpha 	0.1517386723
#define beta	1.506849315
#define gamma	-0.6585879874

//Paramètres pid "levelled/angle"
#define PID_ANGLE_FREQUENCY 250
double PID_SETTINGS_ROLL [8] =  {1.5, 0, 0.29, PID_ANGLE_FREQUENCY, 250, alpha, beta, gamma} ;
double  PID_SETTINGS_PITCH [8] = {2.47, 0, 0.4, PID_ANGLE_FREQUENCY, 250, alpha, beta, gamma} ;
double  PID_SETTINGS_YAW [8] = {6, 0, 2, PID_ANGLE_FREQUENCY, 250, 0.1, 0.9, 0} ;

//Paramètres pid "accro/angle rate"
#define PID_ANGLE_RATE	250
double PID_SETTINGS_ROLL_ACCRO[8] = {0.5, 1, 0, PID_ANGLE_RATE, 250, 1, 0, 0};
double PID_SETTINGS_PITCH_ACCRO[8] = {0.5, 1, 0, PID_ANGLE_RATE, 250, 1, 0, 0};
double PID_SETTINGS_YAW_ACCRO[8] = {2, 0, 0, PID_ANGLE_RATE, 250, 0.2, 0.8, 0};









#endif /* PID_CONFIG_H_ */
