/*
 * scheduler.c
 *
 *  Created on: 12 août 2020
 *      Author: Theo
 */
#include "scheduler.h"

static task_t * task_queu[TASK_COUNT];
static uint32_t task_queu_size = 0 ;
static uint32_t task_queu_position = 0 ;

void scheduler_init(State_drone_t * drone, State_base_t * base){
	tasks_init(drone, base);
}

void scheduler(void){
	uint32_t current_time_us = SYSTICK_get_time_us();
	task_t * task = get_first_task();
	while(task_queu_position < task_queu_size && task != NULL){
		if(current_time_us > task->last_execution_us + task->desired_period_us){
			task_execute(task, current_time_us);
		}
		task = get_next_task();
	}
}

task_t * get_first_task(void){
	task_queu_position = 0 ;
	return task_queu[0] ;
}

task_t * get_next_task(void){
	return task_queu[++task_queu_position];
}

uint32_t task_execute(task_t * task, uint32_t current_time_us){
	task->last_execution_us = current_time_us ;
	task->function(current_time_us);
	current_time_us = SYSTICK_get_time_us();
	task->execution_duration_us = current_time_us - task->last_execution_us ;
	return current_time_us ;
}

//Activation ou désactivation par ajout ou suppression dans la queu dans la queu
void task_enable(task_ids_t id, bool_e enable){
	if(enable && id < TASK_COUNT)
		queu_add(get_task(id));
	else
		queu_remove(get_task(id));
}

void task_reschedule(task_ids_t id, uint32_t new_period){
	get_task(id)->desired_period_us = new_period ;
	task_enable(id, TRUE);
}




void queu_clear(void){
	memset(task_queu, 0, sizeof(task_queu));
	task_queu_size = 0 ;
}

bool_e queu_contains(task_t * task){
	for(uint32_t t = 0; t < task_queu_size; t++)
		if(task_queu[t] == task)
			return TRUE ;
	return FALSE ;
}

bool_e queu_add(task_t * task){
	if(queu_contains(task) || task_queu_size >= TASK_COUNT)
		return FALSE ;

	for(uint32_t t = 0; t < task_queu_size + 1 ; t++){
		if(task_queu[t] == NULL || task->static_priority > task_queu[t]->static_priority){
			memmove(&task_queu[t+1], &task_queu[t], sizeof(task) * (task_queu_size +1 - t));
			task->state = READY ;
			task_queu[t] = task ;
			task_queu_size ++ ;
			return TRUE ;
		}
	}
	return FALSE ;
}

bool_e queu_remove(task_t * task){
	if(!queu_contains(task))
		return FALSE ;

	for(uint32_t t = 0; t < task_queu_size; t ++){
		if(task == task_queu[t]){
			memmove(&task_queu[t], &task_queu[t+1], sizeof(task) * (task_queu_size - t));
			task->state = INIT ;
			task_queu_size -- ;
			return TRUE ;
		}
	}
	return FALSE ;
}
