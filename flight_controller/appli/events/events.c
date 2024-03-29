/*
 * events.c
 *
 *  Created on: 13 sept. 2020
 *      Author: Th�o
 */

#include "mask_def.h"
#include "flags.h"
#include "events.h"

#define CLEAR_FLAG_FLIGHT_MODE flags = MASK_and(flags, mask_flight_mode_clear)

//Ensemble des flags
static Mask_t flags ;

//Etats du drone, recquis par les fonctions d'event
static State_drone_t * drone ;

//Si false, init du module event
static bool_e initialized = FALSE ;

//Masks flight mode clear
static Mask_t mask_flight_mode_clear ;
static Flags_t flags_flight_mode_clear [9] = {FLAG_STATE_ON_THE_GROUND,
		FLAG_STATE_MANUAL,
		FLAG_STATE_MANUAL_PC,
		FLAG_STATE_MANUAL_HAND_CONTROL,
		FLAG_STATE_MANUAL_ACCRO,
		FLAG_STATE_PARACHUTE,
		FLAG_STATE_CALIBRATE_MPU,
		FLAG_STATE_ERROR_SENSOR,
		FLAG_STATE_CHANGE_PID_SETTINGS};


//Fonction appel�es lors de la transition d'un event � un autre
static void event_function_on_the_ground(mask_def_ids_t mask_id){
	CLEAR_FLAG_FLIGHT_MODE ;
	EVENT_Set_flag(FLAG_STATE_ON_THE_GROUND);
	switch(mask_id){
		case ON_THE_GROUND_STOP_MOTORS_REQUEST :
			EVENT_Clean_flag(FLAG_REQUEST_STOP_MOTORS);
			break;
		case ON_THE_GROUND_PARACHUTE :
			EVENT_Clean_flag(FLAG_SUB_PARACHUTE_OVER);
			break;
		case ON_THE_GROUND_CHANGE_PID_SETTINGS :
			EVENT_Clean_flag(FLAG_CHAN_9_PUSH);
			break;
		case ON_THE_GROUND_IMU_TIMEOUT :
			EVENT_Clean_flag(FLAG_IMU_TIMEOUT);
			break;
		default :
			break;
	}
	drone->soft.state_flight_mode = ON_THE_GROUND ;
}
static void event_function_manual(mask_def_ids_t mask_id){
	UNUSED(mask_id);
	CLEAR_FLAG_FLIGHT_MODE ;
	EVENT_Set_flag(FLAG_STATE_MANUAL);
	drone->soft.state_flight_mode = MANUAL ;
}
static void event_function_manual_pc(mask_def_ids_t mask_id){
	CLEAR_FLAG_FLIGHT_MODE ;
	UNUSED(mask_id);
	EVENT_Set_flag(FLAG_STATE_MANUAL_PC);
	EVENT_Clean_flag(FLAG_REQUEST_MANUAL_PC);
	drone->soft.state_flight_mode = MANUAL_PC ;
}
static void event_function_manual_hand_control(mask_def_ids_t mask_id){
	//Pas cens� arriver l� pour l instant
	UNUSED(mask_id);
}
static void event_function_manual_accro(mask_def_ids_t mask_id){
	CLEAR_FLAG_FLIGHT_MODE ;
	UNUSED(mask_id);
	EVENT_Set_flag(FLAG_STATE_MANUAL_ACCRO);
	drone->soft.state_flight_mode = MANUAL_ACCRO ;
}
static void event_function_parachute(mask_def_ids_t mask_id){
	CLEAR_FLAG_FLIGHT_MODE ;
	UNUSED(mask_id);
	EVENT_Clean_flag(FLAG_CHAN_9_PUSH);
	EVENT_Set_flag(FLAG_STATE_PARACHUTE);
	drone->soft.state_flight_mode = PARACHUTE ;
}

static void event_function_calibration_mpu(mask_def_ids_t mask_id){
	CLEAR_FLAG_FLIGHT_MODE ;
	switch(mask_id){
		case CALIBRATE_MPU_PC_REQUEST:
			EVENT_Clean_flag(FLAG_REQUEST_MPU_CALIBRATION);
			break;
		case CALIBRATE_MPU_RADIO_REQUEST:
			EVENT_Clean_flag(FLAG_CHAN_9_PUSH);
			break;
		default:
			break;
	}
	EVENT_Set_flag(FLAG_STATE_CALIBRATE_MPU);
	drone->soft.state_flight_mode = CALIBRATE_MPU6050 ;
}
static void event_function_change_pid_settings(mask_def_ids_t mask_id){
	CLEAR_FLAG_FLIGHT_MODE ;
	UNUSED(mask_id);
	EVENT_Clean_flag(FLAG_CHAN_9_PUSH);
	EVENT_Set_flag(FLAG_STATE_CHANGE_PID_SETTINGS);
	drone->soft.state_flight_mode = PID_CHANGE_SETTINGS ;
}


#define DEFINE_EVENT(event_function_param, nb_mask_param, event_type_param){  	\
		.function = event_function_param ,						\
		.nb_mask = nb_mask_param , 								\
		.type = event_type_param 								\
}

//D�finitions des events
//Attention !!!! nb_mask <= EVENT_NB_MASK_PER_EVENT_MAX sinon d�rapage :)
static Event_t events[EVENT_COUNT] ={
		[EVENT_TRANSIT_ON_THE_GROUND] 			= DEFINE_EVENT(event_function_on_the_ground, 		ON_THE_GROUND_MASK_COUNT, 	EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_MANUAL] 					= DEFINE_EVENT(event_function_manual, 				MANUAL_MASK_COUNT, 			EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_MANUAL_PC] 				= DEFINE_EVENT(event_function_manual_pc, 			MANUAL_PC_MASK_COUNT, 		EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_MANUAL_HAND_CONTROL]		= DEFINE_EVENT(event_function_manual_hand_control, 	0, 							EVENT_TYPE_HIGH_LVL),	//TODO : Hand control
		[EVENT_TRANSIT_MANUAL_ACCRO] 			= DEFINE_EVENT(event_function_manual_accro, 		MANUAL_ACCRO_MASK_COUNT, 	EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_PARACHUTE] 				= DEFINE_EVENT(event_function_parachute, 			PARACHUTE_COUNT, 			EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_CALIBRATE_MPU] 			= DEFINE_EVENT(event_function_calibration_mpu, 		CALIBRATE_MPU_COUNT, 		EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_ERROR_SENSOR] 			= DEFINE_EVENT(event_function_on_the_ground, 		0, 							EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_CHANGE_PID_SETTINGS] 	= DEFINE_EVENT(event_function_change_pid_settings, 	CHANGE_PID_SETTINGS_COUNT, 	EVENT_TYPE_HIGH_LVL)
};

//D�finis les mask "high_lvl_state" pour les events
static void EVENT_init_high_lvl_state_mask(void){
	events[EVENT_TRANSIT_ON_THE_GROUND].high_lvl_state = MASK_create_single(FLAG_STATE_ON_THE_GROUND);
	events[EVENT_TRANSIT_MANUAL].high_lvl_state = MASK_create_single(FLAG_STATE_MANUAL);
	events[EVENT_TRANSIT_MANUAL_PC].high_lvl_state = MASK_create_single(FLAG_STATE_MANUAL_PC);
	events[EVENT_TRANSIT_MANUAL_HAND_CONTROL].high_lvl_state = MASK_create_single(FLAG_STATE_MANUAL_HAND_CONTROL);
	events[EVENT_TRANSIT_MANUAL_ACCRO].high_lvl_state = MASK_create_single(FLAG_STATE_MANUAL_ACCRO);
	events[EVENT_TRANSIT_PARACHUTE].high_lvl_state = MASK_create_single(FLAG_STATE_PARACHUTE);
	events[EVENT_TRANSIT_CALIBRATE_MPU].high_lvl_state = MASK_create_single(FLAG_STATE_CALIBRATE_MPU);
	events[EVENT_TRANSIT_ERROR_SENSOR].high_lvl_state = MASK_create_single(FLAG_STATE_ERROR_SENSOR);
	events[EVENT_TRANSIT_CHANGE_PID_SETTINGS].high_lvl_state = MASK_create_single(FLAG_STATE_CHANGE_PID_SETTINGS);
}


//D�clenchement des events
void EVENT_process_events(){
	if(initialized){
	//Pout chaque event
		for(uint32_t e = 0; e < EVENT_COUNT; e ++){
			//Si on a un event de transition
			if(events[e].type == EVENT_TYPE_HIGH_LVL){
				//on n'appelle pas l'event de transition vers l'�tat actuel
				if(!(Mask_test_and(events[e].high_lvl_state, flags))){
					uint32_t m = 0 ;
					bool_e function_did_run_once = FALSE ;
					//On test chaque paires de masque
					while(m < events[e].nb_mask && !function_did_run_once){
						if(Mask_test_and(events[e].mask_and[m], flags)){
							if(Mask_test_or(events[e].mask_or[m], flags)){
								events[e].function(m);
								function_did_run_once = TRUE ;
							}
						}
						m++ ;
					}
				}
			}
		}
	}
}

bool_e EVENT_Set_flag(Flags_t flag){
	return MASK_set_flag(&flags, flag);
}
bool_e EVENT_Clean_flag(Flags_t flag){
	return MASK_clean_flag(&flags, flag);
}

void EVENT_init(State_drone_t * drone_){
	initialized = TRUE ;

	//On enregistre la structure de donn�e pour le drone
	drone = drone_ ;

	//On commence en mode on the ground
	EVENT_Set_flag(FLAG_STATE_ON_THE_GROUND);

	//On init les mask d'�tats qui permette d'�viter de faire une transition dans l'�tat actuel
	EVENT_init_high_lvl_state_mask();

	//Def des mask pour tous les events
	//Appelle des fonctions de confifuration des masques pour chaques event
	mask_def_on_the_ground(&events[EVENT_TRANSIT_ON_THE_GROUND]);
	mask_def_manual(&events[EVENT_TRANSIT_MANUAL]);
	mask_def_manual_pc(&events[EVENT_TRANSIT_MANUAL_PC]);
	mask_def_manual_hand_control(&events[EVENT_TRANSIT_MANUAL_HAND_CONTROL]);
	mask_def_manual_accro(&events[EVENT_TRANSIT_MANUAL_ACCRO]);
	mask_def_parachute(&events[EVENT_TRANSIT_PARACHUTE]);

	mask_def_calibrate_mpu(&events[EVENT_TRANSIT_CALIBRATE_MPU]);
	mask_def_error_sensors(&events[EVENT_TRANSIT_ERROR_SENSOR]);
	mask_def_change_pid_settings(&events[EVENT_TRANSIT_CHANGE_PID_SETTINGS]);

	mask_flight_mode_clear = MASK_not(MASK_create(flags_flight_mode_clear, 9));

}



