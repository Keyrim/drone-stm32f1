/*
 * system_d.h
 *
 *  Created on: 29 juin 2020
 *      Author: Théo
 */


#include "../lib/lib_perso/GPS.h"
#include "stm32f1_mpu6050.h"
#include "../lib/lib_perso\complementary_filter.h"
#include "settings.h"
#include "macro_types.h"
#include "MAE.h"
#include "../lib/lib_perso\esc.h"
#include "../../lib_perso/sequence_led.h"
#include "../lib/lib_perso/pid.h"
#include "../lib/lib_perso/uart_lib.h"

#ifndef SYSTEM_D_H_
#define SYSTEM_D_H_

// Struct du drone
typedef struct{

	//Gps
	gps_datas_t gps ;
	bool_e gps_is_ok  ;
	uint32_t time_last_read_gps ;

	//Batterie
	double v_bat ;

	//Consignes
	double roll_consigne ;
	double pitch_consigne ;
	double yaw_consigne ;
	double throttle_consigne ;

	// PWM escs signal variables
	ESC_e escs[4];

	//PPM variables
	uint16_t channels [NB_CHANNEL] ;
	bool_e ppm_is_ok  ;
	uint32_t time_last_read_ppm  ;

	//MPU 6050 variables
	MPU6050_t mpu_data ;
	MPU6050_Result_t mpu_result ;
	bool_e mpu_is_ok  ;
	COMP_FILTER_angles_e mpu_angles ;

	//Flight mode
	Flight_Mode_SM state_flight_mode ;
	bool_e entrance_flight_mode ;

	//low_lvl
	Low_Level_SM state_low_level ;

	//Main loop frequency setting
	uint32_t previous_time_loop ;

	//Free time
	uint32_t free_time ;

	//led etat
	sequence_led_t led_etat ;

	//Pid variables
	PID_t pid_roll ;
	PID_t pid_pitch ;
	PID_t pid_yaw ;

	//Uart
	uart_struct_e uart_telem ;


}State_drone_t;

typedef struct{
	double angle_x ;
	double angle_y ;
}State_base_t;

#endif /* SYSTEM_D_H_ */
