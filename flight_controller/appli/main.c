/**
  ******************************************************************************
  * @file    main.c
  * @author  Theo
  * @date    01-July-2020
  * @brief   Default main function.
  ******************************************************************************
*/

//Include de base pour la stm
#include <high_lvl/high_lvl.h>
#include "stm32f1xx_hal.h"
#include "stm32f1_sys.h"

//Include des deux machines à état principales (qui elles include bcp de choses)
#include "scheduler/scheduler.h"
#include "regulation_filtrage/regulation_filtrage.h"

//Fichier de ref pour les configurations / branchements
#include "branchement.h"
#include "settings.h"
#include "../ressources/sequences_led.h"





//	-------------------------- Global variables ----------------------------------
State_drone_t drone ;
State_base_t base ;


int main(void)
{
	//Init mae drone
	drone.soft.state_flight_mode = ON_THE_GROUND ;

	//	-------------------------------------------- Setup -----------------------------------------------------------
	HAL_Init();
	BATTERIE_init(&drone.capteurs.batterie, BATTERIE_ADC_VOLTAGE, BATTERIE_RESISTANCES_COEF);


	//Init sequence led
	LED_SEQUENCE_init(&drone.ihm.led_etat, GPIO_STATE_LED, GPIO_PIN_STATE_LED, SEQUENCE_LED_1, 200, 12, 1);

	//On laisse du temps à tout le monde pour bien démarer
	HAL_Delay(100);


	//------------------Init de la télémétrie
	TELEMETRIE_Init(&drone.communication.telemetrie, UART_TELEMETRIE, 57600, 1);

	//------------------Init du compas
//	HMC5883_init(&drone.capteurs.hmc, TRUE, 0, 0, 0, 0);
//
//	HAL_Delay(5);
//
//	while(1){
//		HMC5883_test();
//		HAL_Delay(500);
//	}


	//------------------Init du MPU et du complementary filer
	Mpu_imu_init(&drone.capteurs.mpu,MPU6050_Accelerometer_16G, MPU6050_Gyroscope_500s, 0.998f, REGULATION_AND_MPU_FREQUENCY);
	//Si le mpu ne s'est pas init on démarre dans la high lvl imu non init
	if(drone.capteurs.mpu.mpu_result)
		drone.soft.state_flight_mode = IMU_FAILED_INIT ;
	else
		scheduler_enable_gyro(TRUE);

	//------------------Init ibus
	IBUS_init(&drone.communication.ibus, UART_IBUS);

	//------------------Init channel analysis
	channel_analysis_init(&drone.communication.ch_analysis, 10, drone.communication.ibus.channels);

	//Init ms5611 baromètre
	HAL_Delay(50);
	MS5611_get_calibration_values(&drone.capteurs.ms5611, FALSE);

	//Init module events
	EVENT_init(&drone);

	REGU_FILTRAGE_filters_imu_config(&drone);
	REGU_FILTRAGE_pids_orientation_init(&drone);

	ESCS_init(&drone.stabilisation.escs_timer, ESC_OUTPUT_ONE_SHOT_125);

	HAL_Delay(50);


	//---------------------------------------------- Séquenceur -----------------------------------------------------

	scheduler_init(&drone, &base);
	while(1){
		scheduler();
	}



}
