/*
 * regulation_filtrage.c
 *
 *  Created on: 20 oct. 2020
 *      Author: Theo
 */
#include "regulation_filtrage.h"

//Paramètres
//Apprès analyse des courbes des sorties de pid sur un google sheet
//Un filtre du second ordre avec ces paramètres fonctionne bien
#define alpha 	0.15173867f
#define beta	1.50684931f
#define gamma  -0.65858798f

//Paramètres pid "levelled/angle"
#define PID_ANGLE_FREQUENCY REGULATION_AND_MPU_FREQUENCY
float PID_SETTINGS_ROLL [PID_NB_SETTINGS] =  {6.0f, 0.0f, 0.15f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};
float  PID_SETTINGS_PITCH [PID_NB_SETTINGS] = {6.0f, 0.0f, 0.3f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};

//float PID_SETTINGS_ROLL [PID_NB_SETTINGS] =  {1.5f, 0.0f, 0.29f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};
//float  PID_SETTINGS_PITCH [PID_NB_SETTINGS] = {2.47f, 0.0f, 0.4f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_PITCH_ROLL};

float  PID_SETTINGS_YAW [PID_NB_SETTINGS] = {6.0f, 0.0f, 2.0f, PID_ANGLE_FREQUENCY, SETPOINT_MAX_ACCRO_YAW} ;

//Paramètres pid "accro/angle rate"
#define PID_ANGLE_RATE_FREQUENCY	REGULATION_AND_MPU_FREQUENCY
float PID_SETTINGS_ROLL_ACCRO[PID_NB_SETTINGS] = {0.6f, 0.3f, 0.0f, PID_ANGLE_RATE_FREQUENCY, 250};
float PID_SETTINGS_PITCH_ACCRO[PID_NB_SETTINGS] = {0.5f, 0.25f, 0.0f, PID_ANGLE_RATE_FREQUENCY, 250};
float PID_SETTINGS_YAW_ACCRO[PID_NB_SETTINGS] = {1.0f, 1.0f, 0.0f, PID_ANGLE_RATE_FREQUENCY, 250};

//Settings filters pid angle$
// https://docs.google.com/spreadsheets/d/17qEF8PeXgOc0zejo5EkO0WPrfYT0EIATipH0eUmFORY/edit#gid=0
float FILTER_SETTINGS_ANGLE[3] = {alpha, beta, gamma};

float FILTER_first_order_settings[3] = {0.1f, 0.9f, 0.0f};


//Settings fitlers gyro
// https://docs.google.com/spreadsheets/d/10sJPc1aYeCNcukzN5P1F0smk6_KCdXfGAj5ekmFCErw/edit#gid=0
float FILTER_SETTINGS_GYRO[3] = {0.1142857143f, 1.6f, -0.7142857143f}; 				//(dt 0.004 w0 100 ksi 0.3 )	//plus lisse donc un peu de latence
//float FILTER_SETTINGS_GYRO[3] = {0.1836734694f, 1.326530612f, -0.5102040816f}; 	//(dt 0.004 w0 150 ksi 0.5 )	//moins lisse, moins de latence


void REGU_FILTRAGE_pids_orientation_init(State_drone_t * drone){

	//Init pids pour le mode "angle / levelled "
	PID_init(&drone->stabilisation.pid_roll, PID_SETTINGS_ROLL, FILTER_NO_FILTERING, NULL);
	PID_init(&drone->stabilisation.pid_pitch,  PID_SETTINGS_PITCH, FILTER_NO_FILTERING, NULL);
	PID_init(&drone->stabilisation.pid_yaw, PID_SETTINGS_YAW, FILTER_NO_FILTERING, NULL);

	//Init pids pour le mode "accro / rate"
	float FILTER_SETTINGS_pid_angle_rate[3] = {0.2f, 0.8f, 0.0f};
	PID_init(&drone->stabilisation.pid_roll_rate, PID_SETTINGS_ROLL_ACCRO, FILTER_FIRST_ORDER, FILTER_SETTINGS_pid_angle_rate);
	PID_init(&drone->stabilisation.pid_pitch_rate, PID_SETTINGS_PITCH_ACCRO, FILTER_FIRST_ORDER, FILTER_SETTINGS_pid_angle_rate);
	PID_init(&drone->stabilisation.pid_yaw_rate, PID_SETTINGS_YAW_ACCRO, FILTER_FIRST_ORDER, FILTER_SETTINGS_pid_angle_rate);
}
void REGU_FILTRAGE_filters_imu_config(State_drone_t * drone){
	DRONE_mpu6050_t * imu = &drone->capteurs.mpu ;
	//Filtre de l'acc
	float FILTER_SETTINGS_acc[3] = {0.1f, 0.9f, 0.0f};
	FILTER_init(&imu->x_acc_filter, FILTER_SETTINGS_acc, FILTER_FIRST_ORDER);
	FILTER_init(&imu->y_acc_filter, FILTER_SETTINGS_acc, FILTER_FIRST_ORDER);
	FILTER_init(&imu->z_acc_filter, FILTER_SETTINGS_acc, FILTER_FIRST_ORDER);
	//Filtre du gyro
	float FILTER_SETTINGS_gyro[3] = {0.2f, 0.8f, 0.0f};
	FILTER_init(&imu->x_gyro_filter, FILTER_SETTINGS_gyro, FILTER_FIRST_ORDER);
	FILTER_init(&imu->y_gyro_filter, FILTER_SETTINGS_gyro, FILTER_FIRST_ORDER);
	FILTER_init(&imu->z_gyro_filter, FILTER_SETTINGS_gyro, FILTER_FIRST_ORDER);

}

void REGULATION_procces(State_drone_t * drone){
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

