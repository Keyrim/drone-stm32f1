/*
 * events.c
 *
 *  Created on: 13 sept. 2020
 *      Author: Théo
 */

#include "events.h"
#include "mask_def.h"

#define CLEAR_FLAG_FLIGHT_MODE flags = MASK_and(flags, mask_flight_mode_clear)

//Ensemble des flags
static Mask_t flags ;

//Etats du drone, recquis par les fonctions d'event
static State_drone_t * drone ;

//Si false, init du module event
static bool_e initialized = FALSE ;

//Masks flight mode clear
Mask_t mask_flight_mode_clear ;
Flags_t flags_flight_mode_clear [9] = {FLAG_STATE_ON_THE_GROUND,
		FLAG_STATE_MANUAL,
		FLAG_STATE_MANUAL_PC,
		FLAG_STATE_MANUAL_HAND_CONTROL,
		FLAG_STATE_MANUAL_ACCRO,
		FLAG_STATE_PARACHUTE,
		FLAG_STATE_CALIBRATE_MPU,
		FLAG_STATE_ERROR_SENSOR,
		FLAG_STATE_CHANGE_PID_SETTINGS};


//Fonctions liées aux events
static void event_function_on_the_ground(void){
	CLEAR_FLAG_FLIGHT_MODE ;
	EVENT_Set_flag(FLAG_STATE_ON_THE_GROUND);
	EVENT_Clean_flag(FLAG_REQUEST_STOP_MOTORS);
	EVENT_Clean_flag(FLAG_SUB_PARACHUTE_OVER);
	drone->soft.state_flight_mode = ON_THE_GROUND ;
}
static void event_function_manual(void){
	CLEAR_FLAG_FLIGHT_MODE ;
	EVENT_Set_flag(FLAG_STATE_MANUAL);
	drone->soft.state_flight_mode = MANUAL ;
}
static void event_function_manual_pc(void){
	CLEAR_FLAG_FLIGHT_MODE ;
	EVENT_Set_flag(FLAG_STATE_MANUAL_PC);
	EVENT_Clean_flag(FLAG_REQUEST_MANUAL_PC);
	drone->soft.state_flight_mode = MANUAL_PC ;
}
static void event_function_manual_hand_control(void){
	//Pas censé arriver là pour l instant
}
static void event_function_manual_accro(void){
	CLEAR_FLAG_FLIGHT_MODE ;
	EVENT_Set_flag(FLAG_STATE_MANUAL_ACCRO);
	drone->soft.state_flight_mode = MANUAL_ACCRO ;
}
static void event_function_parachute(void){
	CLEAR_FLAG_FLIGHT_MODE ;
	EVENT_Clean_flag(FLAG_CHAN_9_PUSH);
	EVENT_Set_flag(FLAG_STATE_PARACHUTE);
	drone->soft.state_flight_mode = PARACHUTE ;
}


#define DEFINE_EVENT(event_function_param, nb_mask_param, event_type_param){  	\
		.function = event_function_param ,						\
		.nb_mask = nb_mask_param , 								\
		.type = event_type_param 								\
}

//Définitions des events
//Attention !!!! nb_mask < EVENT_NB_MASK_PER_EVENT_MAX sinon dérapage :)
Event_t events[EVENT_COUNT] ={
		[EVENT_TRANSIT_ON_THE_GROUND] 			= DEFINE_EVENT(event_function_on_the_ground, 		7, EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_MANUAL] 					= DEFINE_EVENT(event_function_manual, 				3, EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_MANUAL_PC] 				= DEFINE_EVENT(event_function_manual_pc, 			1, EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_MANUAL_HAND_CONTROL]		= DEFINE_EVENT(event_function_manual_hand_control, 	0, EVENT_TYPE_HIGH_LVL),	//TODO : Hand control
		[EVENT_TRANSIT_MANUAL_ACCRO] 			= DEFINE_EVENT(event_function_manual_accro, 		3, EVENT_TYPE_HIGH_LVL),

		[EVENT_TRANSIT_PARACHUTE] 				= DEFINE_EVENT(event_function_parachute, 			1, EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_CALIBRATE_MPU] 			= DEFINE_EVENT(event_function_on_the_ground, 		0, EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_ERROR_SENSOR] 			= DEFINE_EVENT(event_function_on_the_ground, 		0, EVENT_TYPE_HIGH_LVL),
		[EVENT_TRANSIT_CHANGE_PID_SETTINGS] 	= DEFINE_EVENT(event_function_on_the_ground, 		0, EVENT_TYPE_HIGH_LVL)
};

//Définis les mask "high_lvl_state" pour les events
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


//Déclenchement des event
void EVENT_process_events(){
	if(initialized){
	//Pout chaque event
		for(uint32_t e = 0; e < EVENT_COUNT; e ++){
			//Si on a un event de transition
			if(events[e].type == EVENT_TYPE_HIGH_LVL){
				//on n'appelle pas l'event de transition vers l'état actuel
				if(!(Mask_test_and(events[e].high_lvl_state, flags))){
					uint32_t m = 0 ;
					bool_e function_did_run_once = FALSE ;
					//On test chaque paires de masque
					while(m < events[e].nb_mask && !function_did_run_once){
						if(Mask_test_and(events[e].mask_and[m], flags)){
							if(Mask_test_or(events[e].mask_or[m], flags)){
								events[e].function();
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

	//On enregistre la structure de donnée pour le drone
	drone = drone_ ;

	//On commence en mode on the ground
	EVENT_Set_flag(FLAG_STATE_ON_THE_GROUND);

	//On init les mask d'états qui permette d'éviter de faire une transition dans l'état actuel
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



