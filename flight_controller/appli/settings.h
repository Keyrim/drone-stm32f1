/*
 * settings.h
 *
 *  Created on: 28 mars 2020
 *      Author: Theo
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

//radio settings
#define TIME_OUT_IBUS 20000 	//(en micros seconde)

//#define NB_CHANNEL 10
#define PITCH 1
#define ROLL 0
#define THROTTLE 2
#define YAW 3
#define SWITCH_1 4
#define SWITCH_2 5
#define SWITCH_3 6
#define SWITCH_4 7

//On puet modifer les coefs des pids depuis la base
//Ici on choisit si on modifi les pids "angle rate" ou "angle"
#define SET_COEF_ON_RATE_PID 1

//Paramètre du gps
#define TIME_OUT_GPS 2000000 //(en microsecondes)
#define GPS_RELAY	FALSE

//Regulation settings
#define REGULATION_AND_MPU_FREQUENCY 500

//Temps à l'état haut mini et maxi pour les signaux destinés aux escs
#define PULSATION_MIN 1000
#define PULSATION_MAX 2000

//Consigne max possible
#define SETPOINT_MAX_LVLED_PITCH_ROLL 	70
#define SETPOINT_MAX_ACCRO_PITCH_ROLL 	400
#define SETPOINT_MAX_LVLED_YAW 			150
#define SETPOINT_MAX_ACCRO_YAW			400


//Batterie
#define USE_BATTERIE_CHECK FALSE
#define BATTERIE_RESISTANCES_COEF ((double)0.00550)

#endif /* SETTINGS_H_ */
