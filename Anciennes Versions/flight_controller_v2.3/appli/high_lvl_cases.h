/*
 * high_lvl_cases.h
 *
 *  Created on: 29 juin 2020
 *      Author: Théo
 */

#include "system_d.h"
#include "MAE.h"
#include "settings.h"
#include "sub/sub_action.h"

#ifndef HIGH_LVL_CASES_H_
#define HIGH_LVL_CASES_H_


void HIGH_LVL_On_The_Ground(State_drone_t * drone);
void HIGH_LVL_Manual(State_drone_t * drone);
void HIGH_LVL_Manual_Stylee(State_drone_t * drone, State_base_t * base);
void HIGH_LVL_Parachute(State_drone_t * drone);
void HIGH_LVL_Calibrate_MPU(State_drone_t * drone);
void HIGH_LVL_Manual_Pc(State_drone_t * drone);
void HIGH_LVL_IMU_Failed_Init(State_drone_t * drone);


#endif /* HIGH_LVL_CASES_H_ */
