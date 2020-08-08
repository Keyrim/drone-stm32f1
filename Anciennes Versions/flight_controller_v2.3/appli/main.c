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


//Fichier de ref pour les configurations
#include "branchement.h"
#include "settings.h"





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
	DRONE_batterie_init(&drone.capteurs.batterie, BATTERIE_ADC_VOLTAGE, BATTERIE_RESISTANCES_COEF);


	//Init sequence led
	LED_SEQUENCE_init(&drone.ihm.led_etat, GPIO_STATE_LED, GPIO_PIN_STATE_LED, SEQUENCE_LED_1, 200, 12, 1);

	//On laisse du temps à tout le monde pour bien démarer
	HAL_Delay(100);
	//------------------Init serial uart
	uart_init(&drone.communication.uart_telem, UART_TELEMETRIE, 57600, 8);
	SYS_set_std_usart(UART_TELEMETRIE, UART_TELEMETRIE, UART_TELEMETRIE);

	//Init du gps, on passe sur une fréquence de 5hz sur l'envoit de données et d'autre trucs
	DRONE_GPS_congif(UART_GPS);

	//------------------Init du MPU et du complementary filer
	DRONE_mpu6050_init(&drone.capteurs.mpu,MPU6050_Accelerometer_16G, MPU6050_Gyroscope_500s, 0.998, 250 );
	//Si le mpu ne s'est pas init on démarre dans la high lvl imu non init
	if(drone.capteurs.mpu.mpu_result)
		drone.soft.state_flight_mode = IMU_FAILED_INIT ;

	//------------------Init ppm module
	DRONE_ppm_init(&drone.communication.ppm, PIN_NUMBER, GPIO_PPM, GPIO_PIN_PPM);
	//------------------Init pwm escs module
	ESC_init(&drone.stabilisation.escs[0], esc0_gpio, esc0_pin);
	ESC_init(&drone.stabilisation.escs[1], esc1_gpio, esc1_pin);
	ESC_init(&drone.stabilisation.escs[2], esc2_gpio, esc2_pin);
	ESC_init(&drone.stabilisation.escs[3], esc3_gpio, esc3_pin);

	//Init pids
	PID_init(&drone.stabilisation.pid_roll, kp_roll, ki_roll, kd_roll, kd_filter_roll, 250, PID_MAX_OUTPUT, TRUE);
	PID_init(&drone.stabilisation.pid_pitch, kp_pitch, ki_pitch, kd_pitch, kd_filter_pitch, 250, PID_MAX_OUTPUT, TRUE);
	PID_init(&drone.stabilisation.pid_yaw, kp_yaw, ki_yaw, kd_yaw, kd_filter_yaw, 250, PID_MAX_OUTPUT, TRUE);

	HAL_Delay(100);


	//	--------------------------------------------- Main Loop	-----------------------------------------------------
	while(1)
	{
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

			case ESCS_SETPOINTS :
				LOW_LVL_Escs_Setpoints(&drone, &base);
				break;

			case SEND_DATA :
				LOW_LVL_Send_Data(&drone);
				break;

			case ERROR_HIGH_LEVEL :
				printf("error\n");
				break;
		}



		//Là on fait des choses en prenant en compte le "free time" : je crois ça fonctionne )
		if(drone.soft.free_time > 15){
			//Si on reçu des données du gps
			if(UART_data_ready(UART_GPS)){
				uint8_t c = UART_get_next_byte(UART_GPS) ;
				if(DRONE_GPS_process_rx(c, &drone.capteurs.gps) >= 3){
					drone.capteurs.gps.last_time_read_gps = SYSTICK_get_time_us() ;
				}
				if(GPS_RELAY)
					uart_add_one(&drone.communication.uart_telem, c);
			}

			//Si on a reçu des données de la base
			if(UART_data_ready(UART_TELEMETRIE)){
				sub_receive_data(UART_get_next_byte(UART_TELEMETRIE), &drone, &base);
			}
			uart_send(&drone.communication.uart_telem);

			//led d'etat
			LED_SEQUENCE_play(&drone.ihm.led_etat);
		}

	}






}
