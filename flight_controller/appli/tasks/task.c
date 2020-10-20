/*
 * task.c
 *
 *  Created on: 13 août 2020
 *      Author: Théo
 */
#include <high_lvl/high_lvl.h>
#include "task.h"
#include "scheduler/scheduler.h"
#include "../sub/sub_action.h"
#include "../telemetrie/telemetrie_periodic_send_functions.h"


static State_drone_t * drone ;
static State_base_t * base ;

void tasks_init(State_drone_t * drone_, State_base_t * base_){
	//Structures qui servent pour les différentes tâches
	drone = drone_ ;
	base = base_ ;

	//Activation des tâches périodiques
	task_enable(TASK_IMU, TRUE);
	task_enable(TASK_GYRO_FILTERING, TRUE);
	//task_enable(TASK_PRINTF, TRUE);
	task_enable(TASK_IBUS, TRUE);
	//task_enable(TASK_ESCS_IBUS_TEST, TRUE);
	task_enable(TASK_SEND_DATA, TRUE);
	task_enable(TASK_RECEIVE_DATA, TRUE);
	task_enable(TASK_VERIF_SYSTEM, TRUE);
	task_enable(TASK_STABILISATION, TRUE);
	task_enable(TASK_HIGH_LVL, TRUE);
	task_enable(TASK_MS5611, TRUE);
	task_enable(TASK_LED, TRUE);

}

void task_function_imu(uint32_t current_time_us){
	UNUSED(current_time_us);
	IMU_update_mpu6050(&drone->capteurs.mpu);
}

void task_function_gyro_filtering(uint32_t current_time_us){
	UNUSED(current_time_us);
	IMU_acc_low_filter(&drone->capteurs.mpu);
	IMU_gyro_low_filter(&drone->capteurs.mpu);
	IMU_complementary_filter(&drone->capteurs.mpu);
}

void task_function_stabilisation(uint32_t current_time_us){
	UNUSED(current_time_us);
	//Si on vole, on stabilise le drone tel que le souhaite le mode de vol actuel
	float roll_output = 0 ;
	float pitch_output = 0 ;
	float yaw_output = 0 ;


	//On peut stabiliser par rapprot à l'angle,  par rapport à la vitesse angulaire ou ne pas stabiliser (si le drone vole pas)
	switch(drone->stabilisation.stab_mode){

		case STAB_OFF :
			//Si on ne stabilise pas, on stop tous les moteurs
			ESCS_set_period(PULSATION_MIN, PULSATION_MIN, PULSATION_MIN, PULSATION_MIN);
			break;

		case LEVELLED :
			//On calcule les consigne des moteurs selon les consignes d'angles
			drone->consigne.roll_rate 	= -PID_compute(&drone->stabilisation.pid_roll, drone->consigne.roll, drone->capteurs.mpu.y);
			drone->consigne.pitch_rate  = -PID_compute(&drone->stabilisation.pid_pitch, drone->consigne.pitch, drone->capteurs.mpu.x);

			//On calcule les consigne des moteurs selon les consignes d'angles
			roll_output 	= PID_compute(&drone->stabilisation.pid_roll_rate, drone->consigne.roll_rate, drone->capteurs.mpu.y_gyro_filtered);
			pitch_output 	= PID_compute(&drone->stabilisation.pid_pitch_rate, drone->consigne.pitch_rate, drone->capteurs.mpu.x_gyro_filtered);
			yaw_output 		= PID_compute(&drone->stabilisation.pid_yaw_rate, drone->consigne.yaw_rate, drone->capteurs.mpu.z_gyro_filtered);

			//Et on envoit aux moteurs en vérifiant qu'on ne dépasse les valeurs autorisées
			ESCS_set_period((uint16_t)(1000 + drone->consigne.throttle + (int16_t)(- roll_output + pitch_output - yaw_output)),
							(uint16_t)(1000 + drone->consigne.throttle + (int16_t)(+ roll_output + pitch_output + yaw_output)),
							(uint16_t)(1000 + drone->consigne.throttle + (int16_t)(- roll_output - pitch_output + yaw_output)),
							(uint16_t)(1000 + drone->consigne.throttle + (int16_t)(+ roll_output - pitch_output - yaw_output)));

			break;

		case ACCRO :
			//On calcule les consigne des moteurs selon les consignes d'angles
			roll_output 	= PID_compute(&drone->stabilisation.pid_roll_rate, drone->consigne.roll_rate, drone->capteurs.mpu.y_gyro_filtered);
			pitch_output 	= PID_compute(&drone->stabilisation.pid_pitch_rate, drone->consigne.pitch_rate, drone->capteurs.mpu.x_gyro_filtered);
			yaw_output 		= PID_compute(&drone->stabilisation.pid_yaw_rate, drone->consigne.yaw_rate, drone->capteurs.mpu.z_gyro_filtered);


			//Et on envoit aux moteurs en vérifiant qu'on ne dépasse les valeurs autorisées
			ESCS_set_period((uint16_t)(1000 + drone->consigne.throttle + (int16_t)(- roll_output + pitch_output - yaw_output)),
							(uint16_t)(1000 + drone->consigne.throttle + (int16_t)(+ roll_output + pitch_output + yaw_output)),
							(uint16_t)(1000 + drone->consigne.throttle + (int16_t)(- roll_output - pitch_output + yaw_output)),
							(uint16_t)(1000 + drone->consigne.throttle + (int16_t)(+ roll_output - pitch_output - yaw_output)));
			break;

		default:
			break;
	}



}


//Tâche qui s'occupe du haut niveau dans le drone
void task_function_high_lvl(uint32_t current_time_us){
	UNUSED(current_time_us);

	//Mise à jour des flags
	HIGH_LVL_Update_Flags(drone);

	//Appels fonction gestion des events (transition et erreur haut niveau)
	EVENT_process_events();

	//Appel fonction mode de vol actuel
	HIGH_LVL_Switch(drone, base);
}


void task_function_printf(uint32_t current_time_us){
	UNUSED(current_time_us);
	printf("%lu\t%lu\n", TASK_get_task(TASK_HIGH_LVL)->execution_duration_us, get_cpu_load());
}

void task_function_ibus(uint32_t current_time_us){
	UNUSED(current_time_us);
	uint32_t compteur = 0 ;
	//On peut lire jusqu à X octets par appel de la tâche dans l optique de garder une durée de tâche descente
	while(IBUS_check_data(&drone->communication.ibus) && compteur < 25)
		compteur ++ ;
	//Avant de quitter on fait applique notre "filtrage" sur les channels si les valeurs sont neuves
	channel_analysis_process(&drone->communication.ch_analysis);
}

void task_function_escs_ibus_test(uint32_t current_time_us){
	UNUSED(current_time_us);
	uint16_t m1 = (uint16_t)drone->communication.ibus.channels[THROTTLE] ;
	ESCS_set_period(m1, m1, m1, m1);
}

void task_function_send_data(uint32_t current_time_us){
	UNUSED(current_time_us);
	if(drone->communication.ibus.channels[SWITCH_4] > 1500)
		TELEMETRIE_Periodic_send(drone);
}

void task_function_receive_data(uint32_t current_time_us){
	UNUSED(current_time_us);
	uint32_t compteur = 0 ;
	//Compteur permet de lire X Octets par appel de la tâche
	while(UART_data_ready(drone->communication.telemetrie.uart) && compteur < 10){
		sub_receive_data(UART_get_next_byte(drone->communication.telemetrie.uart), drone, base);
		compteur ++ ;
	}
}

void task_function_verif_system(uint32_t current_time_us){

	//Time out sur l ibus

	if(current_time_us > drone->communication.ibus.last_update + TIME_OUT_IBUS){
		drone->communication.ibus.is_ok = FALSE ;
		EVENT_Set_flag(FLAG_TIMEOUT_PPM);
		EVENT_Clean_flag(FLAG_PPM_OK);
	}
	else{
		drone->communication.ibus.is_ok = TRUE ;
		EVENT_Clean_flag(FLAG_TIMEOUT_PPM);
		EVENT_Set_flag(FLAG_PPM_OK);

	}


	//Time out du gps
	if(current_time_us > drone->capteurs.gps.last_time_read_gps + TIME_OUT_GPS)
		drone->capteurs.gps.is_ok = FALSE ;
	else
		drone->capteurs.gps.is_ok = TRUE ;

	//Mesure batterie
	BATTERIE_update(&drone->capteurs.batterie);
	switch(drone->capteurs.batterie.batterie_lvl){
		case BATTERIE_LVL_HIGH :
			EVENT_Set_flag(FLAG_BATTERY_HIGH);
			EVENT_Clean_flag(FLAG_BATTERY_MEDIUM);
			EVENT_Clean_flag(FLAG_BATTERY_LOW);
			EVENT_Clean_flag(FLAG_BATTERY_LOW_CUTOF);
			EVENT_Clean_flag(FLAG_BATTERY_NO_BATT);
			break;
		case BATTERIE_LVL_MEDIUM :
			EVENT_Clean_flag(FLAG_BATTERY_HIGH);
			EVENT_Set_flag(FLAG_BATTERY_MEDIUM);
			EVENT_Clean_flag(FLAG_BATTERY_LOW);
			EVENT_Clean_flag(FLAG_BATTERY_LOW_CUTOF);
			EVENT_Clean_flag(FLAG_BATTERY_NO_BATT);
			break;
		case BATTERIE_LVL_LOW :
			EVENT_Clean_flag(FLAG_BATTERY_HIGH);
			EVENT_Clean_flag(FLAG_BATTERY_MEDIUM);
			EVENT_Set_flag(FLAG_BATTERY_LOW);
			EVENT_Clean_flag(FLAG_BATTERY_LOW_CUTOF);
			EVENT_Clean_flag(FLAG_BATTERY_NO_BATT);
			break;
		case BATTERIE_LVL_LOW_CUTOF :
			EVENT_Clean_flag(FLAG_BATTERY_HIGH);
			EVENT_Clean_flag(FLAG_BATTERY_MEDIUM);
			EVENT_Clean_flag(FLAG_BATTERY_LOW);
			EVENT_Set_flag(FLAG_BATTERY_LOW_CUTOF);
			EVENT_Clean_flag(FLAG_BATTERY_NO_BATT);
			break;
		case BATTERIE_LVL_NULL :
			EVENT_Clean_flag(FLAG_BATTERY_HIGH);
			EVENT_Clean_flag(FLAG_BATTERY_MEDIUM);
			EVENT_Clean_flag(FLAG_BATTERY_LOW);
			EVENT_Clean_flag(FLAG_BATTERY_LOW_CUTOF);
			EVENT_Set_flag(FLAG_BATTERY_NO_BATT);
			break;
	}

}

void task_function_ms5611(uint32_t current_time_us){
	task_reschedule(TASK_MS5611, sub_ms5611(drone,current_time_us));
}

void task_function_led(uint32_t current_time_us){
	LED_SEQUENCE_play(&drone->ihm.led_etat, current_time_us);
}

#define DEFINE_TASK(id_param, priority_param,  task_function_param, desired_period_us_param) { 	\
	.id = id_param,										\
	.static_priority = priority_param,					\
	.function = task_function_param,					\
	.desired_period_us = desired_period_us_param		\
}

#define PERIOD_US_FROM_HERTZ(hertz_param) (1000000 / hertz_param)

task_t tasks [TASK_COUNT] ={
		[TASK_IMU] = 			DEFINE_TASK(TASK_IMU ,				PRIORITY_REAL_TIME, 	task_function_imu, 				PERIOD_US_FROM_HERTZ(REGULATION_AND_MPU_FREQUENCY)),
		[TASK_GYRO_FILTERING] = DEFINE_TASK(TASK_GYRO_FILTERING ,	PRIORITY_REAL_TIME, 	task_function_gyro_filtering, 	PERIOD_US_FROM_HERTZ(REGULATION_AND_MPU_FREQUENCY)),
		[TASK_PRINTF] = 		DEFINE_TASK(TASK_PRINTF, 			PRIORITY_HIGH, 			task_function_printf, 			PERIOD_US_FROM_HERTZ(40)),
		[TASK_IBUS] = 			DEFINE_TASK(TASK_IBUS, 				PRIORITY_HIGH, 			task_function_ibus, 			PERIOD_US_FROM_HERTZ(1000)),
		[TASK_ESCS_IBUS_TEST] = DEFINE_TASK(TASK_ESCS_IBUS_TEST, 	PRIORITY_HIGH, 			task_function_escs_ibus_test, 	PERIOD_US_FROM_HERTZ(250)),
		[TASK_SEND_DATA] = 		DEFINE_TASK(TASK_SEND_DATA, 		PRIORITY_MEDIUM, 		task_function_send_data, 		PERIOD_US_FROM_HERTZ(250)),
		[TASK_RECEIVE_DATA] = 	DEFINE_TASK(TASK_RECEIVE_DATA, 		PRIORITY_MEDIUM, 		task_function_receive_data, 	PERIOD_US_FROM_HERTZ(500)),
		[TASK_VERIF_SYSTEM] = 	DEFINE_TASK(TASK_VERIF_SYSTEM, 		PRIORITY_MEDIUM, 		task_function_verif_system, 	PERIOD_US_FROM_HERTZ(10)),
		[TASK_STABILISATION] =	DEFINE_TASK(TASK_STABILISATION, 	PRIORITY_REAL_TIME, 	task_function_stabilisation, 	PERIOD_US_FROM_HERTZ(REGULATION_AND_MPU_FREQUENCY)),
		[TASK_HIGH_LVL] = 		DEFINE_TASK(TASK_HIGH_LVL, 			PRIORITY_HIGH,	 		task_function_high_lvl, 		PERIOD_US_FROM_HERTZ(250)),
		[TASK_MS5611] = 		DEFINE_TASK(TASK_MS5611, 			PRIORITY_MEDIUM,	 	task_function_ms5611, 			PERIOD_US_FROM_HERTZ(500)),
		[TASK_LED] = 			DEFINE_TASK(TASK_LED, 				PRIORITY_LOW,	 		task_function_led, 				PERIOD_US_FROM_HERTZ(10))
};

task_t * TASK_get_task(task_ids_t id){
	return &tasks[id];
}


