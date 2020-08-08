/*
 * settings.h
 *
 *  Created on: 28 mars 2020
 *      Author: Theo
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

//PPM settings
#define TIME_OUT_PPM 20000 	//(en micros seconde)
#define NB_CHANNEL 8
#define PITCH 1
#define ROLL 0
#define THROTTLE 2
#define YAW 3
#define SWITCH_1 4
#define SWITCH_2 5
#define SWITCH_3 6
#define SWITCH_4 7

//telemetrie
#define UART_TELEMETRIE UART3_ID

//Gps settings
#define TIME_OUT_GPS 2000000 //(en microsecondes)
#define UART_GPS UART2_ID

//Main loop settings
#define LOOP_PERIODE 4000

//PWM Settings
#define PULSATION_MIN 1000
#define PULSATION_MAX 2000

//pid max setpoint order (in degrees or degrees per second for the yaw)
#define roll_and_pitch_max_setpoint 70
#define yaw_max_setpoint 400

//PID settings
#define kp_roll 2.94 //2.94
#define ki_roll 0
#define kd_roll 0.4 //0.27

#define kp_pitch 2.94 //2.94
#define ki_pitch 0
#define kd_pitch 0.4 //0.27

#define kp_yaw 4
#define ki_yaw 0
#define kd_yaw 0

#define max_pid_output 200

//Rapport tension_batterie sur tension_mesurée en prenant en compte le adc qui renvoit entre 0 et 4095 pour 3.3 v
//2.139 a0 = 11.93 baterrie
//4096 = 3.3v
//v_bat = read_adc * (11.93 * 3.3) / (4096 * 2.139)
//v_bat = read_adc * 4.493 / 1000
#define BATTERIE_RESISTANCES_COEF ((double)0.00550)

#endif /* SETTINGS_H_ */
