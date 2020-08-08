/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
  * @brief   Default main function.
  ******************************************************************************
*/
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
#include "MAE.h"
#include "../lib/lib_perso/sequence_led.h"
#include "../ressources/sequences_led.h"


//	-------------------------- Global variables ----------------------------------

//Security
#define SECURITY_IS_OK	ppm_is_ok
#define EVERY_IS_OK ((uint8_t)( 2 * ppm_is_ok +  gps_is_ok))

//Uart
uart_struct_e uart_telem ;


//Gps
gps_datas_t gps ;
#define UART_GPS UART2_ID
uint32_t time_last_read_gps = 0 ;
bool_e gps_is_ok = 0 ;
nmea_frame_e nmea_frame ;


//Trucs
uint32_t compteur_loop = 0 ;
double v_bat = 12.0 ;

//Consignes
double roll_consigne = 0 ;
double pitch_consigne = 0 ;
double yaw_consigne   = 0 ;
double throttle_consigne = 0 ;

//Consigne depuis la base
double roll_consigne_base = 0 ;
double pitch_consigne_base = 0 ;


//PPM variables
uint16_t channels [NB_CHANNEL] ;
uint16_t channels_average[NB_CHANNEL][5];
uint32_t time_last_read_ppm = 0 ;
bool_e ppm_is_ok = TRUE ;

// PWM escs signal variables
ESC_e escs[4];

//Main loop frequency setting
uint32_t previous_time_loop = 0 ;

//MPU 6050 variables
MPU6050_t mpu_data ;
MPU6050_Result_t mpu_result ;
bool_e mpu_is_ok = TRUE ;
COMP_FILTER_angles_e mpu_angles ;

//Pid variables
PID_t pid_roll ;
PID_t pid_pitch ;
PID_t pid_yaw ;


uint32_t time = 0 ;
uint32_t time_start = 0 ;
uint32_t time_delta = 0;
uint32_t time_end = 0 ;


High_Level_SM state_high_level = WAIT_LOOP ;
High_Level_SM previous_state_high_level = PWM_LOW ;


//Nos machines à état
DataSend_SM state_data_send = TO_SEND_1 ;

Flying_SM state_flying = ON_THE_GROUND ;
Flying_SM previous_state_flying = FLYING ;
bool_e entrance_state_flying = FALSE ;

Flight_Mode_SM state_flight_mode = MANUAL ;
Flight_Mode_SM previous_state_flight_mode = MANUAL_STYLEE ;
bool_e entrance_flight_mode = FALSE ;

//Sequence led d'etat
sequence_led_t led_etat ;

int main(void)
{
	//	-------------------------------------------- Setup -----------------------------------------------------------
	//Init pins
	HAL_Init();
	ADC_init();

	LED_SEQUENCE_init(&led_etat, GPIO_STATE_LED, GPIO_PIN_STATE_LED, SEQUENCE_LED_1, 100, 12, 1);
	//On laisse du temps à tout le monde pour bien démarer
	HAL_Delay(20);
	//------------------Init serial uart
	uart_init(&uart_telem, UART_TELEMETRIE, 57600, 10);
	SYS_set_std_usart(UART_TELEMETRIE, UART_TELEMETRIE, UART_TELEMETRIE);

	//Init du gps, on passe sur une fréquence de 5hz sur l'envoit de données et d'autre trucs
	GPS_congif(UART_GPS);

	//------------------Initialisation du port de la led Verte
	//BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Init led etat


	//------------------Init ruban de led
	//LED_MATRIX_init();
	//sub_clignotement_led(LED_MODE_1);

	//------------------Init du MPU et du complementary filer
	mpu_result = MPU6050_Init(&mpu_data, NULL, GPIO_PIN_12, MPU6050_Device_0, MPU6050_Accelerometer_16G, MPU6050_Gyroscope_500s);
	COMP_FILTER_init(&mpu_data, &mpu_angles,MPU6050_Accelerometer_16G, MPU6050_Gyroscope_500s, 0.998, 250 );

	//------------------Init ppm module
	PPM_init(channels, PIN_NUMBER, GPIO_PPM, GPIO_PIN_PPM, TRUE, &time_last_read_ppm);

	//------------------Init pwm escs module
	escs[0].gpio = esc0_gpio ;
	escs[1].gpio = esc1_gpio ;
	escs[2].gpio = esc2_gpio ;
	escs[3].gpio = esc3_gpio ;
	escs[0].gpio_pin = esc0_pin ;
	escs[1].gpio_pin = esc1_pin ;
	escs[2].gpio_pin = esc2_pin ;
	escs[3].gpio_pin = esc3_pin ;
	escs[0].pulsation = 1000;
	escs[1].pulsation = 1000;
	escs[2].pulsation = 1000;
	escs[3].pulsation = 1000;
	ESC_init(escs, 4);

	//Init pids
	PID_init(&pid_roll, kp_roll, ki_roll, kd_roll, 250, max_pid_output);
	PID_init(&pid_pitch, kp_pitch, ki_pitch, kd_pitch, 250, max_pid_output);
	PID_init(&pid_yaw, kp_yaw, ki_yaw, kd_yaw, 250, max_pid_output);


	HAL_Delay(100);




	//	--------------------------------------------- Main Loop	-----------------------------------------------------
	while(1)	//boucle de tâche de fond
	{
		//bool_e entrance = state_high_level != previous_state_high_level ;
		previous_state_high_level = state_high_level ;
		switch(state_high_level){
			case WAIT_LOOP :
				//When we start a new loop, free_time is always set to zero, it can be modified by the called function

				time = SYSTICK_get_time_us();
				if(time >= previous_time_loop + LOOP_PERIODE){
					previous_time_loop += LOOP_PERIODE ;
					state_high_level = PWM_HIGH ;
					global.free_time = 0 ;	//Si c'est l'heure on passe direct au state suivant
				}
				else
					global.free_time = previous_time_loop + LOOP_PERIODE - time ;	//si on a du temps dispo, on le dit

				break;
			case PWM_HIGH:
				//Starting pwm signals for the escs
				ESC_Start_pulse();
				state_high_level = UPDATE_ANGLES ;
				break;
			case UPDATE_ANGLES :
				//read mpu data and appli the filter
				MPU6050_ReadAll(&mpu_data);
				COMP_FILTER_update_angles(&mpu_angles);
				state_high_level = VERIF_SYSTEM ;
				break ;

			case VERIF_SYSTEM :
				time = SYSTICK_get_time_us();
				//Verif si le ppm est toujours bien reçu
				if(time > time_last_read_ppm + TIME_OUT_PPM)
					ppm_is_ok = FALSE ;
				else
					ppm_is_ok = TRUE ;

				//Verif si le gps est ok
				if(time > time_last_read_gps + TIME_OUT_GPS)
					gps_is_ok = FALSE ;
				else
					gps_is_ok = TRUE ;

				state_high_level = PWM_LOW ;
				break;

			case PWM_LOW :
				state_high_level = try_action(ESC_End_pulse(), state_high_level, ESCS_SETPOINTS, ERROR_HIGH_LEVEL);
				break;

			case ESCS_SETPOINTS :
				entrance_state_flying = state_flying != previous_state_flying ;
				previous_state_flying = state_flying ;
				switch(state_flying){
					case ON_THE_GROUND :
						if(entrance_state_flying){
							LED_SEQUENCE_set_sequence(&led_etat, SEQUENCE_LED_1);
						}
						//Si on est au sol on met la consigne des moteurs à zéro
						escs[0].pulsation = PULSATION_MIN ;
						escs[1].pulsation = PULSATION_MIN ;
						escs[2].pulsation = PULSATION_MIN ;
						escs[3].pulsation = PULSATION_MIN ;

						//Si on a le droit on passe en mode flying
						if(channels[SWITCH_1] > 1500 && channels[THROTTLE] < 1100 && channels[THROTTLE] > 1070 && SECURITY_IS_OK)
							//if(v_bat > 10.8)	//On démarre pas si il reste 20% de la batterie
								state_flying = FLYING ;
						break;

					case FLYING:
						//On détermine le mode de vol avec des if, le switch est probablement inutile ici
						if(channels[SWITCH_2] > 1650){
							state_flight_mode = PARACHUTE ;
						}else if(channels[SWITCH_2] > 1350){
							state_flight_mode = MANUAL_STYLEE ;
						}
						else{
							//SINON manuel
							state_flight_mode = MANUAL ;
						}

						//Détermine un changement de mode de vol
						entrance_flight_mode = state_flight_mode != previous_state_flight_mode ;
						previous_state_flight_mode = state_flight_mode ;
						switch(state_flight_mode){

							case MANUAL :
								if(entrance_flight_mode)
									LED_SEQUENCE_set_sequence(&led_etat, SEQUENCE_LED_2);
								//Si on est en mode manuel pour les consigne c'est facile
								roll_consigne = ((double)(channels[ROLL]- 1500) / (double)500) * (double)roll_and_pitch_max_setpoint ;
								pitch_consigne = ((double)(channels[PITCH]- 1500) / (double)500) * (double)roll_and_pitch_max_setpoint ;
								yaw_consigne = ((double)(channels[YAW] - 1500) / (double)500) * (double)yaw_max_setpoint ;		//Consigne en degré par seconde attention ^^
								throttle_consigne = (double)(channels[THROTTLE] - 1050)  ;
								break;
							case MANUAL_STYLEE:
								if(entrance_flight_mode){
									TELEMETRIE_send_consigne_base(SUB_ID_BASE_CONSIGNE_START_SENDING_ANGLES, &uart_telem);
									LED_SEQUENCE_set_sequence(&led_etat, SEQUENCE_LED_4);
								}

								yaw_consigne = ((double)(channels[YAW] - 1500) / (double)500) * (double)yaw_max_setpoint ;	//Consigne en degré par seconde attention ^^
								throttle_consigne = (double)(channels[THROTTLE] - 1050)  ;
								roll_consigne = roll_consigne_base ;
								pitch_consigne = pitch_consigne_base ;
								break;
							case PARACHUTE :
								throttle_consigne = (double)(channels[THROTTLE] - 1050)  ;
								if(sub_parachute(&roll_consigne, &pitch_consigne, &throttle_consigne, mpu_angles.z_acc) != IN_PROGRESS){
									state_flying = ON_THE_GROUND ;
								}

								break;
							case POSITION_HOLD:
								//TODO Position hold function
								break;
							case ALTITUDE_HOLD:
								//TODO Altitude hold function
								break;
						}

						//Now we have our setpoint so we can apply our pids
						double roll_output = PID_compute(&pid_roll,- roll_consigne + mpu_angles.y, mpu_angles.y);
						double pitch_output = PID_compute(&pid_pitch,- pitch_consigne + mpu_angles.x, mpu_angles.x);
						double yaw_output	= PID_compute(&pid_yaw, - yaw_consigne - mpu_angles.z  , 0);

						ESC_Set_pulse(&escs[0], (uint16_t)(1000 + throttle_consigne + (int16_t)(- roll_output - pitch_output + yaw_output)));
						ESC_Set_pulse(&escs[1], (uint16_t)(1000 + throttle_consigne + (int16_t)(+ roll_output - pitch_output - yaw_output)));
						ESC_Set_pulse(&escs[2], (uint16_t)(1000 + throttle_consigne + (int16_t)(- roll_output + pitch_output - yaw_output)));
						ESC_Set_pulse(&escs[3], (uint16_t)(1000 + throttle_consigne + (int16_t)(+ roll_output + pitch_output + yaw_output)));

						//On vérifi qu'on est toujours bien senscé être en mode flying et que les verif de sécurité sont bon
						if(channels[SWITCH_1] < 1500 || channels[THROTTLE] < 1070 || !SECURITY_IS_OK){
							state_flying = ON_THE_GROUND ;
						}
						break;

					}
				state_high_level = SEND_DATA ;
				break;

			case SEND_DATA :

				if(channels[SWITCH_4] > 1500)
					sub_send_data(&uart_telem, mpu_angles.z_acc, EVERY_IS_OK, gps.latitude_deg,gps.longitude_deg,mpu_angles.x, roll_consigne_base, channels[0], channels[1], channels[2], channels[3], channels[4], channels[5], channels[6], channels[7], v_bat, (uint8_t)state_flying, escs[0].pulsation, escs[1].pulsation, escs[2].pulsation, escs[3].pulsation);

				if((compteur_loop % 10) == 0)
					state_high_level = ONCE_EVERY_10 ;
				else
					state_high_level = END ;

				break;


			case ONCE_EVERY_10:
				//Mesure de la tension ne recquiert pas de se faire tout le temps
				v_bat = (double)ADC_getValue(ADC_9) * BATTERIE_RESISTANCES_COEF ;
				state_high_level = END ;
				break;

			case END:
				compteur_loop ++ ;
				state_high_level = WAIT_LOOP ;
				break;
			case ERROR_HIGH_LEVEL :
				printf("error\n");
				break;
		}



		//Là on fait des choses en prenant en compte le "free time" : je crois ça fonctionne )
		if(global.free_time > 15){
			//Si on reçu des données du gps
			if(UART_data_ready(UART_GPS)){
				if(GPS_process_rx(UART_get_next_byte(UART_GPS), &gps) >= 3){
					time_last_read_gps = SYSTICK_get_time_us() ;
				}
			}
			global.free_time -= 5 ;

			//Si on a reçu des données de la base
			if(UART_data_ready(UART_TELEMETRIE)){
				sub_receive_data(UART_get_next_byte(UART_TELEMETRIE), &state_flying, &roll_consigne_base, &pitch_consigne_base);
			}
			uart_send(&uart_telem);

			//led d'etat
			LED_SEQUENCE_play(&led_etat);
		}

	}






}
