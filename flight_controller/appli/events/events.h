/*
 * events.h
 *
 *  Created on: 13 sept. 2020
 *      Author: Théo
 */

#ifndef EVENTS_EVENTS_H_
#define EVENTS_EVENTS_H_

#include "../system_d.h"
#include "mask.h"


#define EVENT_NB_MASK_PER_EVENT_MAX 2

typedef enum{

	//Tous les events de transition high lvl
	EVENT_TRANSIT_ON_THE_GROUND,
	EVENT_TRANSIT_MANUAL,
	EVENT_TRANSIT_MANUAL_PC,
	EVENT_TRANSIT_MANUAL_HAND_CONTROL,
	EVENT_TRANSIT_MANUAL_ACCRO,
	EVENT_TRANSIT_PARACHUTE,
	EVENT_TRANSIT_CALIBRATE_MPU,
	EVENT_TRANSIT_ERROR_SENSOR,
	EVENT_TRANSIT_CHANGE_PID_SETTINGS,


	EVENT_COUNT
}Id_Events_t;

typedef struct{
	Mask_t mask_and[EVENT_NB_MASK_PER_EVENT_MAX] ;
	Mask_t mask_or[EVENT_NB_MASK_PER_EVENT_MAX] ;
	uint32_t nb_mask ;
	void (*function)(void);
}Event_t;


void EVENT_process_events();
void EVENT_init(State_drone_t * drone_);

//Set and clean flags
bool_e EVENT_Set_flag(Flags_t flag);
bool_e EVENT_Clean_flag(Flags_t flag);



#endif /* EVENTS_EVENTS_H_ */
