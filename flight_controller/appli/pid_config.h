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
float PID_SETTINGS_ROLL [PID_NB_SETTINGS] =  {4.0f, 0.0f, 0.0f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};
float  PID_SETTINGS_PITCH [PID_NB_SETTINGS] = {6.0f, 0.0f, 0.0f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};

//float PID_SETTINGS_ROLL [PID_NB_SETTINGS] =  {1.5f, 0.0f, 0.29f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};
//float  PID_SETTINGS_PITCH [PID_NB_SETTINGS] = {2.47f, 0.0f, 0.4f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};

float  PID_SETTINGS_YAW [PID_NB_SETTINGS] = {6.0f, 0.0f, 2.0f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_YAW} ;

//Paramètres pid "accro/angle rate"
#define PID_ANGLE_RATE	250
float PID_SETTINGS_ROLL_ACCRO[PID_NB_SETTINGS] = {0.6f, 0.5f, 0.0015f, PID_ANGLE_RATE, 250};
float PID_SETTINGS_PITCH_ACCRO[PID_NB_SETTINGS] = {0.5f, 0.5f, 0.001f, PID_ANGLE_RATE, 250};
float PID_SETTINGS_YAW_ACCRO[PID_NB_SETTINGS] = {1.0f, 1.0f, 0.0f, PID_ANGLE_RATE, 250};

//Settings filters pid angle$
// https://docs.google.com/spreadsheets/d/17qEF8PeXgOc0zejo5EkO0WPrfYT0EIATipH0eUmFORY/edit#gid=0
float FILTER_SETTINGS_ANGLE[3] = {alpha, beta, gamma};

//Settings fitlers gyro
// https://docs.google.com/spreadsheets/d/10sJPc1aYeCNcukzN5P1F0smk6_KCdXfGAj5ekmFCErw/edit#gid=0
float FILTER_SETTINGS_GYRO[3] = {0.1142857143f, 1.6f, -0.7142857143f}; 				//(dt 0.004 w0 100 ksi 0.3 )	//plus lisse donc un peu de latence
//float FILTER_SETTINGS_GYRO[3] = {0.1836734694f, 1.326530612f, -0.5102040816f}; 	//(dt 0.004 w0 150 ksi 0.5 )	//moins lisse, moins de latence











#endif /* PID_CONFIG_H_ */
