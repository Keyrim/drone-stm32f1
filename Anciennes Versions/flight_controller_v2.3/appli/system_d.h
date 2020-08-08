/*
 * system_d.h
 *
 *  Created on: 29 juin 2020
 *      Author: Théo
 */



#include "settings.h"
#include "macro_types.h"
#include "MAE.h"
#include "../lib/btm\esc.h"
#include "../lib/btm/sequence_led.h"
#include "../lib/btm/DRONE_gps.h"
#include "../lib/btm/DRONE_mpu6050.h"
#include "../lib/btm/pid.h"
#include "../lib/btm/uart_lib.h"
#include "../lib/btm/ppm.h"
#include "../lib/btm/DRONE_batterie.h"
#include "../lib/btm/DRONE_consigne.h"

#ifndef SYSTEM_D_H_
#define SYSTEM_D_H_

//Struct capteur
typedef struct{
	gps_datas_t gps ;
	DRONE_batterie_t batterie ;
	DRONE_mpu6050_t mpu ;
}DRONE_capteurs_t;

//Struct stabilisation
typedef struct{
	ESC_e escs[4];
	PID_t pid_roll ;
	PID_t pid_pitch ;
	PID_t pid_yaw ;
	bool_e stabilize ;
}DRONE_stabilisation_t;

//Struct communication
typedef struct{
	DRONE_ppm_t ppm ;
	uart_struct_e uart_telem ;
}DRONE_communication_t;

//Struct soft
typedef struct{
	Flight_Mode_SM state_flight_mode ;
	bool_e entrance_flight_mode ;

	Low_Level_SM state_low_level ;

	uint32_t previous_time_loop ;
	uint32_t free_time ;

	bool_e flag_request_calib_mpu ;
	bool_e flag_request_manual_pc ;
	bool_e flag_request_stop_motor ;
}DRONE_soft_t;

//Struct ihm
typedef struct{
	sequence_led_t led_etat ;
}DRONE_ihm_t;



// Struct du drone
typedef struct{

	DRONE_capteurs_t capteurs ;
	DRONE_stabilisation_t stabilisation ;
	DRONE_consigne_t consigne ;
	DRONE_communication_t communication ;
	DRONE_soft_t soft ;
	DRONE_ihm_t ihm ;

}State_drone_t;

typedef struct{
	double angle_x ;
	double angle_y ;
}State_base_t;

#endif /* SYSTEM_D_H_ */
