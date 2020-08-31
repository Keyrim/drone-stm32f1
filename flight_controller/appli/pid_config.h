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
#define alpha 	0.15173867f
#define beta	1.50684931f
#define gamma  -0.65858798f

//Paramètres pid "levelled/angle"
#define PID_ANGLE_FREQUENCY 250
float PID_SETTINGS_ROLL [PID_NB_SETTINGS] =  {8.0f, 0.0f, 0.0f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};
float  PID_SETTINGS_PITCH [PID_NB_SETTINGS] = {10.0f, 0.0f, 0.0f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};

//float PID_SETTINGS_ROLL [PID_NB_SETTINGS] =  {1.5f, 0.0f, 0.29f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};
//float  PID_SETTINGS_PITCH [PID_NB_SETTINGS] = {2.47f, 0.0f, 0.4f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};

float  PID_SETTINGS_YAW [PID_NB_SETTINGS] = {6.0f, 0, 2.0f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_YAW} ;

//Paramètres pid "accro/angle rate"
#define PID_ANGLE_RATE	250
float PID_SETTINGS_ROLL_ACCRO[PID_NB_SETTINGS] = {0.5f, 1.0f, 0.0f, PID_ANGLE_RATE, 250};
float PID_SETTINGS_PITCH_ACCRO[PID_NB_SETTINGS] = {0.5f, 1.0f, 0.0f, PID_ANGLE_RATE, 250};
float PID_SETTINGS_YAW_ACCRO[PID_NB_SETTINGS] = {2.0f, 0.0f, 0.0f, PID_ANGLE_RATE, 250};

//Settings filters pid angle
float FILTER_SETTINGS_ANGLE[3] = {alpha, beta, gamma};











#endif /* PID_CONFIG_H_ */
