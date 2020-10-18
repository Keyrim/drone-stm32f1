/*
 * task.c
 *
 *  Created on: 13 août 2020
 *      Author: Théo
 */
#include "task.h"
#include "scheduler/scheduler.h"


static State_ground_station_t * ground_station ;

void tasks_init(State_ground_station_t * ground_station_){
	//Structures qui servent pour les différentes tâches
	ground_station = ground_station_ ;

	//Activation des tâches périodiques
	task_enable(TASK_LED, TRUE);

}


void task_function_led(uint32_t current_time_us){
	LED_SEQUENCE_play(&ground_station->ihm.led_etat, current_time_us);
}

#define DEFINE_TASK(id_param, priority_param,  task_function_param, desired_period_us_param) { 	\
	.id = id_param,										\
	.static_priority = priority_param,					\
	.function = task_function_param,					\
	.desired_period_us = desired_period_us_param		\
}

#define PERIOD_US_FROM_HERTZ(hertz_param) (1000000 / hertz_param)

task_t tasks [TASK_COUNT] ={
		[TASK_LED] = 			DEFINE_TASK(TASK_LED, 				PRIORITY_LOW,	 		task_function_led, 				PERIOD_US_FROM_HERTZ(10))
};

task_t * TASK_get_task(task_ids_t id){
	return &tasks[id];
}


