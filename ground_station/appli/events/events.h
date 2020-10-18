/*
 * events.h
 *
 *  Created on: 13 sept. 2020
 *      Author: Théo
 */


#ifndef EVENTS_EVENTS_H_
#define EVENTS_EVENTS_H_


#include "mask_def_enum.h"
#include "mask.h"
#include "../system_d.h"


#define EVENT_NB_MASK_PER_EVENT_MAX 7

typedef enum{

	//Tous les events de transition high lvl
	EVENT_SEND_ANGLES ,

	EVENT_COUNT
}Id_Events_t;


typedef struct{
	Mask_t mask_and[EVENT_NB_MASK_PER_EVENT_MAX] ;	//Masques de conditions
	Mask_t mask_or[EVENT_NB_MASK_PER_EVENT_MAX] ;	//Masques de déclenchement
	uint32_t nb_mask ;								//Nombre de paires de masque
	void (*function)(mask_def_ids_t mask_id);
}Event_t;


void EVENT_process_events();
void EVENT_init(State_ground_station_t * ground_station_);

//Set and clean flags
bool_e EVENT_Set_flag(Flags_t flag);
bool_e EVENT_Clean_flag(Flags_t flag);



#endif /* EVENTS_EVENTS_H_ */
