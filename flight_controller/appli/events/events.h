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


#define EVENT_NB_MASK_PER_EVENT_MAX 7

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

typedef enum{
	EVENT_TYPE_HIGH_LVL,
	EVENT_TYPE_ERROR
}Event_type_t;

typedef struct{
	Mask_t mask_and[EVENT_NB_MASK_PER_EVENT_MAX] ;	//Masques de conditions
	Mask_t mask_or[EVENT_NB_MASK_PER_EVENT_MAX] ;	//Masques de déclenchement
	uint32_t nb_mask ;								//Nombre de paires de masque
	Event_type_t type ;								//Type d'évenement
	Mask_t high_lvl_state ;							//Pour un event de type high lvl, flag qui correspond à l'état vers le quel vas l'event (pour éviter de test une transition si on est déjà dans l event)
	void (*function)(void);
}Event_t;


void EVENT_process_events();
void EVENT_init(State_drone_t * drone_);

//Set and clean flags
bool_e EVENT_Set_flag(Flags_t flag);
bool_e EVENT_Clean_flag(Flags_t flag);



#endif /* EVENTS_EVENTS_H_ */
