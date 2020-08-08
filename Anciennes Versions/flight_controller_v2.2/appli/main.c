/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
  * @brief   Default main function.
  ******************************************************************************
*/
#include "MAE.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "systick.h"
#include "stm32f1_extit.h"
#include "../lib/lib_perso\ppm.h"
#include "branchement.h"
#include "settings.h"
#include "../lib/lib_perso\esc.h"
#include "stm32f1_mpu6050.h"
#include "../lib/lib_perso\complementary_filter.h"
#include "../lib/lib_perso\pid.h"
#include "../lib/lib_perso\telemetrie.h"
#include "stm32f1_adc.h"
#include "sub/sub_action.h"
#include "../lib/lib_perso/GPS.h"
#include "WS2812S.h"
#include "../lib/lib_perso/state_machine.h"
#include "global.h"
#include "../lib/lib_perso/uart_lib.h"
#include "../lib/lib_perso/sequence_led.h"
#include "../ressources/sequences_led.h"
#include "system_d.h"
#include "high_lvl_cases.h"
#include "low_lvl_cases.h"

//	-------------------------- Global variables ----------------------------------
State_drone_t drone ;
State_base_t base ;


int main(void)
{
	//	-------------------------------------------- Setup -----------------------------------------------------------
	HAL_Init();
	ADC_init();

	//Init mae drone
	drone.state_flight_mode = ON_THE_GROUND ;
	drone.state_low_level = WAIT_LOOP ;

	LED_SEQUENCE_init(&drone.led_etat, GPIO_STATE_LED, GPIO_PIN_STATE_LED, SEQUENCE_LED_1, 200, 12, 1);
	//On laisse du temps à tout le monde pour bien démarer
	HAL_Delay(20);
	//------------------Init serial uart
	uart_init(&drone.uart_telem, UART_TELEMETRIE, 57600, 10);
	SYS_set_std_usart(UART_TELEMETRIE, UART_TELEMETRIE, UART_TELEMETRIE);

	//Init du gps, on passe sur une fréquence de 5hz sur l'envoit de données et d'autre trucs
	GPS_congif(UART_GPS);

	//------------------Init du MPU et du complementary filer
	drone.mpu_result = MPU6050_Init(&drone.mpu_data, NULL, GPIO_PIN_12, MPU6050_Device_0, MPU6050_Accelerometer_16G, MPU6050_Gyroscope_500s);
	COMP_FILTER_init(&drone.mpu_data, &drone.mpu_angles,MPU6050_Accelerometer_16G, MPU6050_Gyroscope_500s, 0.998, 250 );

	//------------------Init ppm module
	PPM_init(drone.channels, PIN_NUMBER, GPIO_PPM, GPIO_PIN_PPM, TRUE, &drone.time_last_read_ppm);

	//------------------Init pwm escs module
	drone.escs[0].gpio = esc0_gpio ;
	drone.escs[1].gpio = esc1_gpio ;
	drone.escs[2].gpio = esc2_gpio ;
	drone.escs[3].gpio = esc3_gpio ;
	drone.escs[0].gpio_pin = esc0_pin ;
	drone.escs[1].gpio_pin = esc1_pin ;
	drone.escs[2].gpio_pin = esc2_pin ;
	drone.escs[3].gpio_pin = esc3_pin ;
	drone.escs[0].pulsation = 1000;
	drone.escs[1].pulsation = 1000;
	drone.escs[2].pulsation = 1000;
	drone.escs[3].pulsation = 1000;
	ESC_init(drone.escs, 4);

	//Init pids
	PID_init(&drone.pid_roll, kp_roll, ki_roll, kd_roll, 250, max_pid_output);
	PID_init(&drone.pid_pitch, kp_pitch, ki_pitch, kd_pitch, 250, max_pid_output);
	PID_init(&drone.pid_yaw, kp_yaw, ki_yaw, kd_yaw, 250, max_pid_output);


	HAL_Delay(100);




	//	--------------------------------------------- Main Loop	-----------------------------------------------------
	while(1)
	{
		switch(drone.state_low_level){
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
		if(drone.free_time > 15){
			//Si on reçu des données du gps
			if(UART_data_ready(UART_GPS)){
				if(GPS_process_rx(UART_get_next_byte(UART_GPS), &drone.gps) >= 3){
					drone.time_last_read_gps = SYSTICK_get_time_us() ;
				}
			}
			//global.free_time -= 5 ;

			//Si on a reçu des données de la base
			if(UART_data_ready(UART_TELEMETRIE)){
				sub_receive_data(UART_get_next_byte(UART_TELEMETRIE), &drone, &base);
			}
			uart_send(&drone.uart_telem);

			//led d'etat
			LED_SEQUENCE_play(&drone.led_etat);
		}

	}






}
