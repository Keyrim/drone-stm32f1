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

#define TASK_STAT_AVERAGE_OVER 20		//Moyenne glissante sur X valeurs
#define TASK_DURATION_MAX	2500	//On considère qu une tâche ne peut durer plus de 2000 µs


typedef enum task_priority{
	PRIORITY_REAL_TIME 	= -1 ,
	//Priorité toujours positive > 0
	PRIORITY_LOW		= 1 ,
	PRIORITY_MEDIUM	 	= 5 ,
	PRIORITY_HIGH		= 10
}task_priority_t;

typedef enum task_states{
	INIT,				//	Tâches existe
	READY				// 	Tâches placées dans la file d attente pour être traité
}task_states_t;

typedef enum task_ids {
	TASK_IMU,
	TASK_IBUS,
	TASK_ESCS_IBUS_TEST,
	TASK_PRINTF,
	TASK_SEND_DATA,
	TASK_RECEIVE_DATA,
	TASK_VERIF_SYSTEM,
	TASK_STABILISATION,
	TASK_UART_SEND,
	TASK_HIGH_LVL,
	TASK_COUNT
}task_ids_t;

typedef struct task{
	task_priority_t static_priority ;
	void (*function)(uint32_t current_time_us);
	task_states_t state ;
	task_ids_t id ;
	uint32_t execution_duration_us ;
	uint32_t desired_period_us ;
	uint32_t real_period_us ;
	uint32_t last_execution_us ;

	//Tableau pour avoir les stats en moyenne glissante
	uint32_t real_period_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t real_period_us_average_sum ;
	uint32_t execution_duration_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t execution_duration_us_average_sum ;
	uint32_t average_index ;
}task_t;

void tasks_init(State_drone_t * drone_, State_base_t * base_);
task_t * get_task(task_ids_t id);

#endif /* TASKS_TASK_H_ */
