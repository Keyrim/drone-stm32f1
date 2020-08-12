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
#include "high_lvl_cases.h"
#include "low_lvl_cases.h"


//Fichier de ref pour les configurations / branchements
#include "branchement.h"
#include "settings.h"
#include "pid_config.h"





//	-------------------------- Global variables ----------------------------------
State_drone_t drone ;
State_base_t base ;


int main(void)
{
	//Init mae drone
	drone.soft.state_flight_mode = ON_THE_GROUND ;
	drone.soft.state_low_level = WAIT_LOOP ;

	//	-------------------------------------------- Setup -----------------------------------------------------------
	HAL_Init();
	Batterie_init(&drone.capteurs.batterie, BATTERIE_ADC_VOLTAGE, BATTERIE_RESISTANCES_COEF);


	//Init sequence led
	LED_SEQUENCE_init(&drone.ihm.led_etat, GPIO_STATE_LED, GPIO_PIN_STATE_LED, SEQUENCE_LED_1, 200, 12, 1);

	//On laisse du temps à tout le monde pour bien démarer
	HAL_Delay(100);
	//------------------Init serial uart
	uart_init(&drone.communication.uart_telem, UART_TELEMETRIE, 57600, 8);
	SYS_set_std_usart(UART_TELEMETRIE, UART_TELEMETRIE, UART_TELEMETRIE);

	//Init du gps
	GPS_congif(UART_GPS);

	//------------------Init du MPU et du complementary filer
	Mpu_imu_init(&drone.capteurs.mpu,MPU6050_Accelerometer_16G, MPU6050_Gyroscope_500s, 0.998f, 250 );
	//Si le mpu ne s'est pas init on démarre dans la high lvl imu non init
	if(drone.capteurs.mpu.mpu_result)
		drone.soft.state_flight_mode = IMU_FAILED_INIT ;

	//------------------Init ibus
	IBUS_init(&drone.communication.ibus, UART_IBUS);
	//------------------Init channel analysis
	channel_analysis_init(&drone.communication.ch_analysis, 10, drone.communication.ibus.channels);
	//------------------Init pwm escs
	ESC_init(&drone.stabilisation.escs[0], esc0_gpio, esc0_pin);
	ESC_init(&drone.stabilisation.escs[1], esc1_gpio, esc1_pin);
	ESC_init(&drone.stabilisation.escs[2], esc2_gpio, esc2_pin);
	ESC_init(&drone.stabilisation.escs[3], esc3_gpio, esc3_pin);

	//Init ms5611 baromètre
	HAL_Delay(50);
	MS5611_get_calibration_values(&drone.capteurs.ms5611, FALSE);

	//Init pids pour le mode "angle / levelled "
	PID_init(&drone.stabilisation.pid_roll, PID_SETTINGS_ROLL);
	PID_init(&drone.stabilisation.pid_pitch,  PID_SETTINGS_PITCH);
	PID_init(&drone.stabilisation.pid_yaw, PID_SETTINGS_YAW);

	//Init pids pour le mode "accro / rate"
	PID_init(&drone.stabilisation.pid_roll_rate, PID_SETTINGS_ROLL_ACCRO);
	PID_init(&drone.stabilisation.pid_pitch_rate, PID_SETTINGS_PITCH_ACCRO);
	PID_init(&drone.stabilisation.pid_yaw_rate, PID_SETTINGS_YAW_ACCRO);

	HAL_Delay(50);


	//	--------------------------------------------- Main Loop	-----------------------------------------------------
	while(1)
	{
		//On passe dans chacuns des case de la low lvl à 250 hertz
		switch(drone.soft.state_low_level){
			case WAIT_LOOP :
				LOW_LVL_Wait_Loop(&drone);
				break;

			case PWM_HIGH:
				LOW_LVL_Pwm_High(&drone);
				break;

			case UPDATE_ANGLES :
				LOW_LVL_Update_Angles(&drone);
				break ;

			case VERIF_SYSTEM :
				LOW_LVL_Verif_System(&drone);
				break;

			case PWM_LOW :
				LOW_LVL_Pwm_Low(&drone);
				break;

			case HIGH_LVL :
				LOW_LVL_Process_High_Lvl(&drone, &base);
				break;

			case STABILISATION :
				LOW_Lvl_Stabilisation(&drone);
				break;

			case SEND_DATA :
				LOW_LVL_Send_Data(&drone);
				break;

			case ERROR_HIGH_LEVEL :
				printf("error\n");
				break;
		}



		//Cette fonction est appelée à chaque boucle et regarde si le drone à du temps dispo
		//Si elle a du temps elle l'utilise (gestion led état, uart, ibus, etc)
		sub_free_time(&drone, &base);



	}


}
