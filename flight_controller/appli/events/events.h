/*
 * events.h
 *
 *  Created on: 13 sept. 2020
 *      Author: Théo
 */

#ifndef EVENTS_EVENTS_H_
#define EVENTS_EVENTS_H_

#include "../system_d.h"


//Pour l'instant la variable flag est un int 32 donc 32 flag max
typedef enum{
	//Flags pour les channels
	FLAG_CHAN_5_POS_1,
	FLAG_CHAN_5_POS_3,

	FLAG_CHAN_6_POS_1,
	FLAG_CHAN_6_POS_2,
	FLAG_CHAN_6_POS_3,

	FLAG_CHAN_7_POS_1,
	FLAG_CHAN_7_POS_2,
	FLAG_CHAN_7_POS_3,

	FLAG_CHAN_8_POS_1,
	FLAG_CHAN_8_POS_3,

	FLAG_CHAN_9_PUSH,

	FLAG_CHAN_10_PUSH,

	//Flags high lvl states
	FLAG_REQUEST_MPU_CALIBRATION,
	FLAG_REQUEST_MANUAL_PC,
	FLAG_REQUEST_MANUAL,
	FLAG_REQUEST_MANUAL_ACCRO,
	FLAG_REQUEST_MANUAL_HAND,

	//Flags état périphérique
	FLAG_IMUT_FAIL,
	FLAG_TIMEOUT_PPM,

	//Flag batterie
	FLAG_BATTERY_REALLY_LOW,
	FLAG_BATTERY_LOW,
	FLAG_BATTERY_MEDIUM,
	FLAG_BATTERY_HIGH,

	//Flags pour l'état de la high lvl
	FLAG_STATE_ON_THE_GROUND,
	FLAG_STATE_MANUAL,
	FLAG_STATE_MANUAL_PC,
	FLAG_STATE_MANUAL_HAND_CONTROL,
	FLAG_STATE_MANUAL_ACCRO,
	FLAG_STATE_PARACHUTE,
	FLAG_STATE_CALIBRATE_MPU,
	FLAG_STATE_ERROR_SENSOR,
	FLAG_STATE_CHANGE_PID_SETTINGS,

	FLAG_COUNT
}Flags_t;

typedef enum{
	EVENT_TIMEOUT_PPM ,

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
	uint32_t mask ;
	void (*event_function)(void);
}Event_t;

bool_e EVENT_set_flag(Flags_t flag);
bool_e EVENT_clean_flag(Flags_t flag);
bool_e EVENT_read_flag(Flags_t flag);

void EVENT_process_events();
void EVENT_init(State_drone_t * drone);

#endif /* EVENTS_EVENTS_H_ */
