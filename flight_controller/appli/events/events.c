/*
 * events.c
 *
 *  Created on: 13 sept. 2020
 *      Author: Th�o
 */

#include "events.h"
//Ensemble des flags
Mask_t flags ;

//Etats du drone, recquiert par les fonctions d'event
State_drone_t * drone ;

static void event_function_on_the_ground(void){

}



#define DEFINE_EVENT(event_function_param, nb_mask_param){  	\
		.function = event_function_param ,						\
		.nb_mask = nb_mask_param 								\
}

//D�finitions des events
Event_t events[EVENT_COUNT] ={
		[EVENT_TRANSIT_ON_THE_GROUND] 			= DEFINE_EVENT(event_function_on_the_ground, 1),
		[EVENT_TRANSIT_MANUAL] 					= DEFINE_EVENT(event_function_on_the_ground, 1),
		[EVENT_TRANSIT_MANUAL_HAND_CONTROL]		= DEFINE_EVENT(event_function_on_the_ground, 1),
		[EVENT_TRANSIT_MANUAL_ACCRO] 			= DEFINE_EVENT(event_function_on_the_ground, 1),
		[EVENT_TRANSIT_CALIBRATE_MPU] 			= DEFINE_EVENT(event_function_on_the_ground, 1)
};


//D�finitions des mask pour les events

//Pour chaque event pr�sent dans la liste
//On test le mask and : le mask de condition
//Si il est valid�
//On regarde le mask ou qui permet de d�clencher l'event
void EVENT_process_events(){
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
	//On enregistre la structure de donn�e pour le drone
	drone = drone_ ;

	//Def des mask pour tous les events
	//Appelle des fonctions de confifuration des masques pour chaques event


}


