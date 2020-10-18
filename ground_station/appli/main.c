/**
  ******************************************************************************
  * @file    main.c
  * @author  Theo
  * @date    01-July-2020
  * @brief   Default main function.
  ******************************************************************************
*/

//Include de base pour la stm
#include "stm32f1xx_hal.h"
#include "stm32f1_sys.h"

//Include des deux machines à état principales (qui elles include bcp de choses)
#include "scheduler/scheduler.h"
#include "events/events.h"

//Fichier de ref pour les configurations / branchements
#include "branchement.h"
#include "settings.h"
#include "../ressources/sequences_led.h"





//	-------------------------- Global variables ----------------------------------
State_ground_station_t ground_station ;


int main(void)
{

	//	-------------------------------------------- Setup -----------------------------------------------------------
	HAL_Init();


	//Init sequence led
	LED_SEQUENCE_init(&ground_station.ihm.led_etat, GPIO_STATE_LED, GPIO_PIN_STATE_LED, SEQUENCE_LED_1, 200, 12, 1);

	//On laisse du temps à tout le monde pour bien démarer
	HAL_Delay(100);


	//------------------Init de la télémétrie
	TELEMETRIE_Init(&ground_station.communication.telemetrie_pc, UART_TELEMETRIE, 57600, 1);


	//------------------Init du MPU et du complementary filer
	Mpu_imu_init(&ground_station.capteurs.mpu,MPU6050_Accelerometer_16G, MPU6050_Gyroscope_500s, 0.998f, 250);
	//Si le mpu ne s'est pas init on démarre dans la high lvl imu non init
	if(!ground_station.capteurs.mpu.mpu_result)
		scheduler_enable_gyro();

	//Init module events
	EVENT_init(&ground_station);

	//Init pids pour le mode "angle / levelled "
	HAL_Delay(50);


	//---------------------------------------------- Séquenceur -----------------------------------------------------

	scheduler_init(&ground_station);
	while(1){
		scheduler();
	}



}
