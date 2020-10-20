/*
 * events.c
 *
 *  Created on: 13 sept. 2020
 *      Author: Théo
 */

#include "mask_def.h"
#include "flags.h"
#include "events.h"



//Ensemble des flags
static Mask_t flags ;

//Etats du drone, recquis par les fonctions d'event
static State_ground_station_t * ground_station ;

//Si false, init du module event
static bool_e initialized = FALSE ;




#define DEFINE_EVENT(event_function_param, nb_mask_param, event_type_param){  	\
		.function = event_function_param ,						\
		.nb_mask = nb_mask_param , 								\
		.type = event_type_param 								\
}

//Définitions des events
//Attention !!!! nb_mask < EVENT_NB_MASK_PER_EVENT_MAX sinon dérapage :)
static Event_t events[EVENT_COUNT] ={0};

//Définis les mask "high_lvl_state" pour les events
static void EVENT_init_high_lvl_state_mask(void){

}


//Déclenchement des events
void EVENT_process_events(){
	if(initialized){
	//Pout chaque event
		for(uint32_t e = 0; e < EVENT_COUNT; e ++){
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

bool_e EVENT_Set_flag(Flags_t flag){
	return MASK_set_flag(&flags, flag);
}
bool_e EVENT_Clean_flag(Flags_t flag){
	return MASK_clean_flag(&flags, flag);
}

void EVENT_init(State_ground_station_t * ground_station_){
	initialized = TRUE ;

	//On enregistre la structure de donnée pour le drone
	ground_station = ground_station_ ;

	//On init les mask d'états qui permette d'éviter de faire une transition dans l'état actuel
	EVENT_init_high_lvl_state_mask();



}



