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
Mask_t flags ;

//Etats du drone, recquis par les fonctions d'event
State_drone_t * drone ;

//Si false, init du module event
bool_e initialized = FALSE ;

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

static void EVENT_init(){
	initialized = TRUE ;
	mask_flight_mode_clear = MASK_not(MASK_create(flags_flight_mode_clear, 9));
}





static void event_function_on_the_ground(void){
	CLEAR_FLAG_FLIGHT_MODE ;
	EVENT_Set_flag(FLAG_STATE_ON_THE_GROUND);
	drone->soft.state_flight_mode = ON_THE_GROUND ;
}

static void event_function_manual(void){
	CLEAR_FLAG_FLIGHT_MODE ;
	EVENT_Set_flag(FLAG_STATE_MANUAL);
	drone->soft.state_flight_mode = MANUAL ;
}



#define DEFINE_EVENT(event_function_param, nb_mask_param){  	\
		.function = event_function_param ,						\
		.nb_mask = nb_mask_param 								\
}

//Définitions des events
Event_t events[EVENT_COUNT] ={
		[EVENT_TRANSIT_ON_THE_GROUND] 			= DEFINE_EVENT(event_function_on_the_ground, 2),
		[EVENT_TRANSIT_MANUAL] 					= DEFINE_EVENT(event_function_manual, 1),
		//Fonctions pas def pour l instant
		[EVENT_TRANSIT_MANUAL_HAND_CONTROL]		= DEFINE_EVENT(event_function_on_the_ground, 0),
		[EVENT_TRANSIT_MANUAL_ACCRO] 			= DEFINE_EVENT(event_function_on_the_ground, 0),
		[EVENT_TRANSIT_CALIBRATE_MPU] 			= DEFINE_EVENT(event_function_on_the_ground, 0)
};




//Déclenchement des event
void EVENT_process_events(){
	if(!initialized)
		EVENT_init();


	//Pout chaque event
	for(uint32_t e = 0; e < EVENT_COUNT; e ++){
		uint32_t m = 0 ;
		bool_e function_did_run_once = FALSE ;
		//Pour chaque masques pour les events
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

bool_e EVENT_Set_flag(Flags_t flag){
	return MASK_set_flag(&flags, flag);
}
bool_e EVENT_Clean_flag(Flags_t flag){
	return MASK_clean_flag(&flags, flag);
}

void EVENT_init_module(State_drone_t * drone_){
	//On enregistre la structure de donnée pour le drone
	drone = drone_ ;

	//Def des mask pour tous les events
	//Appelle des fonctions de confifuration des masques pour chaques event
	mask_def_manual(&events[EVENT_TRANSIT_MANUAL]);
	mask_def_on_the_ground(&events[EVENT_TRANSIT_ON_THE_GROUND]);

}



