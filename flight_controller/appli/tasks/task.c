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
}

static void task_function_imu(uint32_t current_time_us){
	UNUSED(current_time_us);
	Mpu_imu_update_angles(&drone->capteurs.mpu);
}

static void task_function_printf(uint32_t current_time_us){
	UNUSED(current_time_us);
	printf("x\t%f\n", drone->capteurs.mpu.x);
}

#define DEFINE_TASK(id_param, priority_param,  task_function_param, desired_period_us_param) { 	\
	.id = id_param,										\
	.static_priority = priority_param,					\
	.function = task_function_param,					\
	.desired_period_us = desired_period_us_param		\
}



task_t tasks [TASK_COUNT] ={
		[TASK_IMU] = DEFINE_TASK(TASK_IMU ,PRIORITY_HIGH, task_function_imu, 4000),
		[TASK_PRINTF] = DEFINE_TASK(TASK_PRINTF, PRIORITY_LOW, task_function_printf, 4000)
};

task_t * get_task(task_ids_t id){
	return &tasks[id];
}

//tasks[TASK_IMU] = DEFINE_TASK(TASK_IMU, PRIORITY_REAL_TIME, tast_function_imu, 4000) ;

