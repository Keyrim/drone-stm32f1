/*
 * scheduler.h
 *
 *  Created on: 12 août 2020
 *      Author: Theo
 */

#ifndef SCHEDULER_SCHEDULER_H_
#define SCHEDULER_SCHEDULER_H_

#include "../system_d.h"
#include "../tasks/task.h"
#include "string.h"

void scheduler_init(State_drone_t * drone, State_base_t * base);
void scheduler(void);

void queu_clear(void);
bool_e queu_contains(task_t * task);
bool_e queu_add(task_t * task);
bool_e queu_remove(task_t * task);


void task_enable(task_ids_t id, bool_e enable);
task_t * get_first_task(void);
task_t * get_next_task(void);
#endif /* SCHEDULER_SCHEDULER_H_ */
