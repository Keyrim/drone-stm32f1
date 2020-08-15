/*
 * task.c
 *
 *  Created on: 13 août 2020
 *      Author: Théo
 */
#include "task.h"
#include "scheduler/scheduler.h"



static State_drone_t * drone ;
static State_base_t * base ;

void tasks_init(State_drone_t * drone_, State_base_t * base_){
	//Structures qui servent pour les différentes tâches
	drone = drone_ ;
	base = base_ ;

	//Activation des tâches périodiques
	task_enable(TASK_IMU, TRUE);
	task_enable(TASK_PRINTF, TRUE);
	task_enable(TASK_IBUS, TRUE);
}

void task_function_imu(uint32_t current_time_us){
	UNUSED(current_time_us);
	Mpu_imu_update_angles(&drone->capteurs.mpu);
}

void task_function_printf(uint32_t current_time_us){
	UNUSED(current_time_us);
	printf("%lu\t%lu\n", get_task(TASK_IBUS)->execution_duration_us, get_cpu_load());
}

void task_function_ibus(uint32_t current_time_us){
	UNUSED(current_time_us);
	uint32_t compteur = 0 ;
	//On peut lire jusqu à 5 octets par appel de la tâche
	while(IBUS_check_data(&drone->communication.ibus) && compteur < 18)
		compteur ++ ;

}

#define DEFINE_TASK(id_param, priority_param,  task_function_param, desired_period_us_param) { 	\
	.id = id_param,										\
	.static_priority = priority_param,					\
	.function = task_function_param,					\
	.desired_period_us = desired_period_us_param		\
}

#define PERIOD_US_FROM_HERTZ(hertz_param) (1000000 / hertz_param)

task_t tasks [TASK_COUNT] ={
		[TASK_IMU] = DEFINE_TASK(TASK_IMU ,PRIORITY_REAL_TIME, task_function_imu, PERIOD_US_FROM_HERTZ(250)),
		[TASK_PRINTF] = DEFINE_TASK(TASK_PRINTF, PRIORITY_LOW, task_function_printf, PERIOD_US_FROM_HERTZ(40)),
		[TASK_IBUS] = DEFINE_TASK(TASK_IBUS, PRIORITY_HIGH, task_function_ibus, PERIOD_US_FROM_HERTZ(100)),
};

task_t * get_task(task_ids_t id){
	return &tasks[id];
}

//tasks[TASK_IMU] = DEFINE_TASK(TASK_IMU, PRIORITY_REAL_TIME, tast_function_imu, 4000) ;

