/*
 * task.h
 *
 *  Created on: 13 août 2020
 *      Author: Théo
 */

#ifndef TASKS_TASK_H_
#define TASKS_TASK_H_
#include "stdint.h"
#include "stdio.h"

#include "../lib/btm/Mpu_imu.h"
#include "../system_d.h"


typedef enum task_priority{
	PRIORITY_REAL_TIME 	= -1 ,
	PRIORITY_LOW		= 0 ,
	PRIOIRTY_MEDIUM	 	= 5 ,
	PRIORITY_HIGH		= 10
}task_priority_t;

typedef enum task_states{
	INIT,				//	Tâches existe
	READY,				// 	Tâches placées dans la file d attente pour être traité
	WAITING				//	En attente d un évenement / donnée
}task_states_t;

typedef enum task_ids {
	TASK_IMU,
	TASK_PRINTF,
	TASK_COUNT
}task_ids_t;

typedef struct task{
	task_priority_t static_priority ;
	void (*function)(uint32_t current_time_us);
	task_states_t state ;
	task_ids_t id ;
	uint32_t execution_time_us ;
	uint32_t desired_period_us ;
	uint32_t last_execution_us ;
}task_t;

void tasks_init(State_drone_t * drone_, State_base_t * base_);
task_t * get_task(task_ids_t id);

#endif /* TASKS_TASK_H_ */
