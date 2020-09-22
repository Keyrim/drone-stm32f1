/*
 * system_d.h
 *
 *  Created on: 29 juin 2020
 *      Author: Théo
 */



#include "settings.h"
#include "macro_types.h"
#include "MAE.h"

#include "../lib/btm/Batterie.h"
#include "../lib/btm/Channel_annalysis.h"
#include "../lib/btm/Consigne.h"
#include "../lib/btm/Esc.h"
#include "../lib/btm/Gps2.h"
#include "../lib/btm/Ibus.h"
#include "../lib/btm/Mpu_imu.h"
#include "../lib/btm/MS5611.h"
#include "../lib/btm/Pid.h"
#include "../lib/btm/Sequence_led.h"
#include "../lib/btm/Uart_lib.h"
#include "../lib/btm/esc_timer.h"
#include "../lib/btm/Filters.h"
#include "events/events.h"

#ifndef SYSTEM_D_H_
#define SYSTEM_D_H_

//Structure capteurs
typedef struct{
	gps_datas_t gps ;
	DRONE_batterie_t batterie ;
	DRONE_mpu6050_t mpu ;
	ms5611_t ms5611 ;
}DRONE_capteurs_t;

//Structure fitler
typedef struct{
	//PID angle
	Filter_second_order_t pid_roll ;
	Filter_second_order_t pid_pitch ;
	Filter_second_order_t pid_yaw ;

	//Pid accro
	Filter_second_order_t pid_roll_rate ;
	Filter_second_order_t pid_pitch_rate ;
	Filter_second_order_t pid_yaw_rate ;
}DRONE_filters;

//Structure stabilisation
typedef struct{

	ESCs_t escs_timer ;
	//Pid for lvled mode
	PID_t pid_roll ;
	PID_t pid_pitch ;
	PID_t pid_yaw ;
	//Pid for accro mode
	PID_t pid_roll_rate ;
	PID_t pid_pitch_rate ;
	PID_t pid_yaw_rate ;
	//Mode de stabilisation
	Stabilisation_SM stab_mode ;
}DRONE_stabilisation_t;

//Structure communication
typedef struct{
	ibus_t ibus;
	uart_id_e uart_telem ;
	channel_analysis_t ch_analysis ;
}DRONE_communication_t;

//Structure pour le code
typedef struct{
	Flight_Mode_SM state_flight_mode ;
	bool_e entrance_flight_mode ;

	uint32_t dead_line ;
	bool_e flags[NB_FLAGS];
}DRONE_soft_t;

//Structure de l'ihm
typedef struct{
	sequence_led_t led_etat ;
}DRONE_ihm_t;

//Structure principale du drone
typedef struct{

	DRONE_capteurs_t capteurs ;
	DRONE_stabilisation_t stabilisation ;
	DRONE_consigne_t consigne ;
	DRONE_communication_t communication ;
	DRONE_soft_t soft ;
	DRONE_ihm_t ihm ;
	DRONE_filters filters ;

}State_drone_t;

//Structure de la base
typedef struct{
	float angle_x ;
	float angle_y ;
}State_base_t;

#endif /* SYSTEM_D_H_ */
