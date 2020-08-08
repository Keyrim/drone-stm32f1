/*
 * high_lvl_cases.c
 *
 *  Created on: 29 juin 2020
 *      Author: Théo
 */

#include "high_lvl_cases.h"
#include "../../ressources/sequences_led.h"
#include "../lib/btm/Telemetrie.h"
#include "../lib/btm/Test_transition.h"

//Fonction de transition
void transition_high_lvl(State_drone_t * drone);

//Fonctions où on test des états
//Ces fonctions ne font pas de changement d'état
int32_t test_ppm(State_drone_t * drone, int32_t working);
int32_t test_arm_switch(State_drone_t * drone, int32_t armed);
int32_t test_throttle_really_low(State_drone_t * drone, int32_t superior);
int32_t test_throttle_low(State_drone_t * drone, int32_t superior);
int32_t test_imu_init(State_drone_t * drone, int32_t is_init);

//Fonctions où on regarde si il ya une demande pour une action
//Ces fonction effectues elles mêmes les changements d'état
bool_e check_calibrate_mpu_request(State_drone_t * drone);
bool_e check_manual_pc_request(State_drone_t * drone);
bool_e check_manual_accro_request(State_drone_t * drone);
bool_e check_stop_motor_request(State_drone_t * drone);
bool_e check_parachute_request(State_drone_t * drone);
bool_e check_manual_request(State_drone_t * drone);
bool_e check_manual_hand_control_request(State_drone_t * drone);




int32_t test_ppm(State_drone_t * drone, int32_t working){
	//exemple :
	//ppm.is_ok = true
	//test = ok = 1 ;
	//1^1 = 0
	//On renvoit true

	if(drone->communication.ibus.is_ok ^ working)
		return 	0 ;
	else
		return 1 ;



}
int32_t test_arm_switch(State_drone_t * drone, int32_t armed){
	static uint32_t last_update = 0 ;
	int32_t to_return = 0;


	if((drone->communication.ibus.channels[SWITCH_1] < 1200) ^ armed)
		to_return = 1 ;
	else
		to_return = 0 ;


	if(drone->communication.ibus.last_update != last_update){
		last_update = drone->communication.ibus.last_update;
	}
	else
		to_return += 2 ;

	return to_return ;
}
int32_t test_throttle_low(State_drone_t * drone, int32_t superior){

	static uint32_t last_update = 0 ;
	int32_t to_return = 0;


	if((drone->communication.ibus.channels[THROTTLE] < 1100) ^ superior)
		to_return = 1 ;
	else
		to_return = 0 ;


	if(drone->communication.ibus.last_update != last_update){
		last_update = drone->communication.ibus.last_update;
	}
	else
		to_return += 2 ;

	return to_return ;
}
int32_t test_throttle_really_low(State_drone_t * drone, int32_t superior){
	static uint32_t last_update = 0 ;
	int32_t to_return = 0;


	if((drone->communication.ibus.channels[THROTTLE] < 1070) ^ superior)
		to_return = 1 ;
	else
		to_return = 0 ;


	if(drone->communication.ibus.last_update != last_update){
		last_update = drone->communication.ibus.last_update;
	}
	else
		to_return += 2 ;

	return to_return ;
}
int32_t test_imu_init(State_drone_t * drone, int32_t is_init){
	bool_e to_return = 0 ;
	if(drone->capteurs.mpu.mpu_result ^ is_init)
		to_return = 1 ;
	return to_return ;
}


bool_e check_parachute_request(State_drone_t * drone){
	if(drone->communication.ibus.channels[SWITCH_2] > 1650){
		drone->soft.state_flight_mode = PARACHUTE ;
		TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_SWITCH, &drone->communication.uart_telem);
		return TRUE;
	}
	else
		return FALSE;
}
bool_e check_manual_hand_control_request(State_drone_t * drone){
	return FALSE ;
	if(drone->communication.ibus.channels[SWITCH_2] > 1350 && drone->communication.ibus.channels[SWITCH_2] < 1650){
		drone->soft.state_flight_mode = MANUAL_HAND_CONTROL ;
		TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_SWITCH, &drone->communication.uart_telem);
		return TRUE ;
	}
	else
		return FALSE ;
}
bool_e check_manual_request(State_drone_t * drone){
	if(drone->communication.ibus.channels[SWITCH_2] < 1350){
		drone->soft.state_flight_mode = MANUAL ;
		TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_SWITCH, &drone->communication.uart_telem);
		return TRUE ;
	}
	else
		return FALSE ;
}
bool_e check_manual_accro_request(State_drone_t * drone){
	if(drone->communication.ibus.channels[SWITCH_2] > 1350 && drone->communication.ibus.channels[SWITCH_2] < 1650 ){
		drone->soft.state_flight_mode = MANUAL_ACCRO ;
		TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_SWITCH, &drone->communication.uart_telem);
		return TRUE;
	}
	else
		return FALSE;
}
bool_e check_calibrate_mpu_request(State_drone_t * drone){
	if(drone->soft.flags[FLAG_CALIB_MPU]){
		drone->soft.flags[FLAG_CALIB_MPU] = 0 ;
		drone->soft.state_flight_mode = CALIBRATE_MPU6050 ;
		TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_PC_REQUEST, &drone->communication.uart_telem);
		return TRUE ;
	}
	else
		return FALSE ;
}
bool_e check_manual_pc_request(State_drone_t * drone){
	if(drone->soft.flags[FLAG_MANUAL_PC]){
		drone->soft.flags[FLAG_MANUAL_PC] = FALSE ;
		drone->soft.state_flight_mode = MANUAL_PC ;
		TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_PC_REQUEST, &drone->communication.uart_telem);
		return TRUE;
	}
	else
		return FALSE;
}
bool_e check_stop_motor_request(State_drone_t * drone){
	if(drone->soft.flags[FLAG_STOP_MOTOR]){
		drone->soft.flags[FLAG_STOP_MOTOR] = 0 ;
		drone->soft.state_flight_mode = ON_THE_GROUND ;
		TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_PC_REQUEST, &drone->communication.uart_telem);
		return TRUE ;
	}
	else
		return FALSE ;
}



//Fonction de transitons qui devrait se faire remplacer prochainnement par "high_lvl_transition"
test_t arm_switch_test ;
test_t throttle_low_test ;
test_t throttle_really_low_test ;
void transition_high_lvl(State_drone_t * drone){
	static bool_e transition_init = 0 ;
	if(!transition_init)
	{
		TRANSITION_init_test(&arm_switch_test, test_arm_switch);
		TRANSITION_init_test(&throttle_low_test, test_throttle_low);
		TRANSITION_init_test(&throttle_really_low_test, test_throttle_really_low);

		transition_init = 1;
	}


	switch(drone->soft.state_flight_mode){
		case ON_THE_GROUND :

			if(check_calibrate_mpu_request(drone))
				break;
			else if(check_manual_pc_request(drone))
				break;
			else if(test_ppm(drone, TRUE)){
				if(TRANSITION_test(&arm_switch_test, drone, TRUE, 1)){
					if(TRANSITION_test(&throttle_low_test, drone, FALSE, 1)){
						if(TRANSITION_test(&throttle_really_low_test, drone, TRUE, 1)){
							if(check_manual_request(drone))
								break;
							else if(check_manual_accro_request(drone))
								break;
						}

					}
				}
			}
			break;

		case MANUAL :
			if(test_ppm(drone, FALSE)){
				drone->soft.state_flight_mode = ON_THE_GROUND ;
				TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_PPM_ISNT_OK, &drone->communication.uart_telem);
			}

			else if(TRANSITION_test(&arm_switch_test, drone, FALSE, 5)){
				drone->soft.state_flight_mode = ON_THE_GROUND ;
				TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_ARM_SWITCH, &drone->communication.uart_telem);
			}

			else if(TRANSITION_test(&throttle_really_low_test, drone, FALSE, 5)){
				drone->soft.state_flight_mode = ON_THE_GROUND ;
				TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_THROTTLE_LOW, &drone->communication.uart_telem);
			}
			else if(check_stop_motor_request(drone))
				break;
			else if(check_manual_accro_request(drone))
				break;
			else if(check_parachute_request(drone))
				break;

			break;

		case MANUAL_ACCRO :
			if(test_ppm(drone, FALSE)){
				drone->soft.state_flight_mode = ON_THE_GROUND ;
				TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_PPM_ISNT_OK, &drone->communication.uart_telem);
			}

			else if(TRANSITION_test(&arm_switch_test, drone, FALSE, 5)){
				drone->soft.state_flight_mode = ON_THE_GROUND ;
				TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_ARM_SWITCH, &drone->communication.uart_telem);
			}

			else if(TRANSITION_test(&throttle_really_low_test, drone, FALSE, 5)){
				drone->soft.state_flight_mode = ON_THE_GROUND ;
				TELEMETRIE_send_high_lvl_transi(SUB_ID_PC_HIGH_LVL_TRANSITION_THROTTLE_LOW, &drone->communication.uart_telem);
			}
			else if(check_stop_motor_request(drone))
				break;
			else if(check_manual_request(drone))
				break;
			break;

		case MANUAL_HAND_CONTROL:
			break;

		case PARACHUTE:
			break;

		case ALTITUDE_HOLD:
			break;

		case POSITION_HOLD:
			break;

		case CALIBRATE_MPU6050:
			//Le state calibrate_mpu n'appelle pas la fonction de transition
			break;
		case IMU_FAILED_INIT:
			//Si l imu a failed, rien ne peut réparer un reset hardware
			break;
		case MANUAL_PC:
			check_stop_motor_request(drone);
			break;

		default:
			break;
	}
}

/*
 * Fonctionnement de la high lvl :
 * 	->	Si on rentre dans un nouvel état on change la séquence de la led et on change le mode de stabilisation
 * 	->	Chaque état fait ce qu'il à faire : changement des consignes etc
 * 	->	Enfin on regarde avec la fonction de transition si on doit changer d'état
 */


//Mode "on_the_ground" : on arrête les moteurs
void HIGH_LVL_On_The_Ground(State_drone_t * drone){

	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = STAB_OFF ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_1);
	}
	//Si on est au sol on met la consigne des moteurs à zéro
	drone->stabilisation.escs[0].pulsation = PULSATION_MIN ;
	drone->stabilisation.escs[1].pulsation = PULSATION_MIN ;
	drone->stabilisation.escs[2].pulsation = PULSATION_MIN ;
	drone->stabilisation.escs[3].pulsation = PULSATION_MIN ;

	transition_high_lvl(drone);

}

//Mode "manual" : stablisation "levelled" et on met à jour les consignes d'angles par rapport à la télécomande radio
void HIGH_LVL_Manual(State_drone_t * drone){
	//			---------------------------- 		MAIN PART 			----------------------------------------
	if(drone->soft.entrance_flight_mode){
		drone->capteurs.mpu.z = 0 ;
		drone->stabilisation.stab_mode = LEVELLED ;
		drone->consigne.yaw = 0 ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_2);
	}

	drone->consigne.roll = ((double)(drone->communication.ibus.channels[ROLL]- 1500) / (double)500) * (double)SETPOINT_MAX_LVLED_PITCH_ROLL ;
	drone->consigne.pitch = ((double)(drone->communication.ibus.channels[PITCH]- 1500) / (double)500) * (double)SETPOINT_MAX_LVLED_PITCH_ROLL ;
	double yaw_consigne_degre_sec = ((double)(drone->communication.ibus.channels[YAW] - 1500) / (double)500) * (double)SETPOINT_MAX_ACCRO_YAW ;
	drone->consigne.yaw += yaw_consigne_degre_sec / (double)250 ;
	drone->consigne.throttle = (double)(drone->communication.ibus.channels[THROTTLE] - 1050)  ;

	//			---------------------------- Condition de transition 	----------------------------------------
	transition_high_lvl(drone);
}

//Mode "manual_accro" : stabilisation "accro" et met à jour les consignes de vitesses angulaires par rapport à la télécomande radio
void HIGH_LVL_Manual_Accro(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = ACCRO ;
		drone->capteurs.mpu.z = 0 ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_8);
	}

	//			---------------------------- 		MAIN PART 			----------------------------------------
	drone->consigne.roll_rate = (double)(drone->communication.ibus.channels[ROLL]- 1500) * (double)SETPOINT_MAX_ACCRO_PITCH_ROLL / (double)500 ;
	drone->consigne.pitch_rate = (double)(drone->communication.ibus.channels[PITCH] - 1500) * (double)SETPOINT_MAX_ACCRO_PITCH_ROLL / (double)500 ;
	drone->consigne.yaw_rate = (double)(drone->communication.ibus.channels[YAW] - 1500) * (double)SETPOINT_MAX_ACCRO_YAW / (double)500 ;
	drone->consigne.throttle = (double)(drone->communication.ibus.channels[THROTTLE] - 1050) ;

	//			---------------------------- Condition de transition 	----------------------------------------
	transition_high_lvl(drone);
}

//Mode "manual_hand_control" : stabilisation "levelled", les consignes d'angles sont reçu par la base (base équipée d'un mpu6050 positioné sur la main)
void HIGH_LVL_Manual_Hand_Control(State_drone_t * drone, State_base_t * base){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = LEVELLED ;
		TELEMETRIE_send_consigne_base(SUB_ID_BASE_CONSIGNE_START_SENDING_ANGLES, &drone->communication.uart_telem);
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_4);
	}
	//			---------------------------- 		MAIN PART 			----------------------------------------
	drone->consigne.yaw = ((double)(drone->communication.ibus.channels[YAW] - 1500) / (double)500) * (double)SETPOINT_MAX_ACCRO_YAW ;
	drone->consigne.throttle = (double)(drone->communication.ibus.channels[THROTTLE] - 1050)  ;
	drone->consigne.roll = base->angle_x ;
	drone->consigne.pitch = base->angle_y ;

	//			---------------------------- Condition de transition 	----------------------------------------
	transition_high_lvl(drone);

}

//Mode "parachute" : stabilisation "levelled", on appel la sub parachute
void HIGH_LVL_Parachute(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = LEVELLED ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_3);
	}

	//			---------------------------- 		MAIN PART 			----------------------------------------
	drone->consigne.throttle = (double)(drone->communication.ibus.channels[THROTTLE] - 1050)  ;
	if(sub_parachute(drone) != IN_PROGRESS){
		drone->soft.state_flight_mode = ON_THE_GROUND ;
	}
	else
		transition_high_lvl(drone);
}


//Mode "calibrate_mpu" :stabilisation off,  quand le drone est au sol on peut calibrer le mpu
void HIGH_LVL_Calibrate_MPU(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = STAB_OFF ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_6);
	}

	//			---------------------------- 		MAIN PART 			----------------------------------------
	if(Mpu_imu_calibrate(&drone->capteurs.mpu, 1000)){
		drone->soft.state_flight_mode = ON_THE_GROUND ;
	}
}

//Mode "manual_pc" : stabilisation levelled, on control le "throttle" depuis le pc
void HIGH_LVL_Manual_Pc(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
		drone->stabilisation.stab_mode = LEVELLED ;
		drone->consigne.throttle = 1000 ;
		LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_5);
	}

	transition_high_lvl(drone);
}

//Mode "imu_failed_init" : stabilisation off,  si le mpu ne s'est pas init, on reste bloqué au sol en attente d'un reset hard
void HIGH_LVL_IMU_Failed_Init(State_drone_t * drone){
	if(drone->soft.entrance_flight_mode){
			drone->stabilisation.stab_mode = STAB_OFF ;
			LED_SEQUENCE_set_sequence(&drone->ihm.led_etat, SEQUENCE_LED_7);
		}
	transition_high_lvl(drone);
}















