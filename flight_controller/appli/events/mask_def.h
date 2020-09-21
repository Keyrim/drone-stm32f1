/*
 * mask_def.h
 *
 *  Created on: 17 sept. 2020
 *      Author: Theo
 */

#ifndef EVENTS_MASK_DEF_H_
#define EVENTS_MASK_DEF_H_

#include "events.h"

void mask_def_on_the_ground(Event_t * event);
void mask_def_manual(Event_t * event);

void mask_def_manual_pc(Event_t * event);
void mask_def_manual_hand_control(Event_t * event);
void mask_def_manual_accro(Event_t * event);
void mask_def_parachute(Event_t * event);
void mask_def_calibrate_mpu(Event_t * event);
void mask_def_error_sensors(Event_t * event);
void mask_def_change_pid_settings(Event_t * event);

#endif /* EVENTS_MASK_DEF_H_ */
