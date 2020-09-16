/*
 * events.c
 *
 *  Created on: 13 sept. 2020
 *      Author: Théo
 */

#include "events.h"
//Ensemble des flags
#define NB_FLAG 2
uint32_t flags[2] = {0} ;


//Lève un flag et return true si le flag était baissé
bool_e EVENT_set_flag(Flags_t flag_set){

	uint32_t actual_flag = flag_set / 32 ;
	uint32_t flag_bit = (uint32_t)(1 << (flag_set % 32)) ;
	if(flags[actual_flag] & flag_bit)
	//Si le flag est déjà activè on renvoit faux
		return FALSE ;
	//Sinon on lève le flag et on renvoit true
	flags[actual_flag] |= flag_bit ;
	return TRUE ;
}

//Baisse un flag et renvoit vrai si il était levé
bool_e EVENT_clean_flag(Flags_t flag_clean){
	uint32_t actual_flag = flag_clean / 32 ;
	uint32_t flag_bit = (uint32_t)(1 << (flag_clean % 32)) ;
	if(flags[actual_flag] & flag_bit){
		//Si le flag est levé on le baisse et renvoit vrai
		flags[actual_flag] &= ~ flag_bit ;
		return TRUE ;
	}
	return FALSE ;
}

//Renvoit true si le flag est levé, false sinon
bool_e EVENT_read_flag(Flags_t flag_read){
	uint32_t actual_flag = flag_read / 32 ;
	uint32_t flag_bit = (uint32_t)(1 << (flag_read % 32)) ;
	if(flags[actual_flag] & flag_bit)
		//Si le flag est levé on renvoit vrai
		return TRUE ;
	return FALSE ;
}



static void event_function_timeout_ppm(void){

}

#define DEFINE_EVENT(mask_param, event_function_param){  \
		.mask = mask_param ,							 \
		.event_function = event_function_param 			 \
}

Event_t events[EVENT_COUNT] ={
		[EVENT_TIMEOUT_PPM] = DEFINE_EVENT(FLAG_TIMEOUT_PPM, event_function_timeout_ppm)
};


//On test le masque pour l event
//Et on execute la fonction
void EVENT_process_events(){
	for(uint32_t e = 0; e < EVENT_COUNT; e ++){
		if((events[e].mask & flags) == events[e].mask)
			events[e].event_function();
	}
}



