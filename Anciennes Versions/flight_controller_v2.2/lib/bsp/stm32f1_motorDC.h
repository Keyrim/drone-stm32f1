/**
 * demo_motor.h
 *
 *  Created on: 27 octobre 2015
 *      Author: S. Poiraud
 */

#ifndef DEMO_MOTOR_H_
#define DEMO_MOTOR_H_

#include "macro_types.h"
#include "stdint.h"

typedef enum
{
	MOTOR1,
	MOTOR2,
	MOTOR3,
	MOTOR4,
	MOTOR_NB
}motor_e;


void MOTOR_set_duty(int16_t duty, motor_e motor);
void MOTOR_init(uint8_t nb_motors);
running_e DEMO_MOTOR_statemachine (bool_e ask_for_finish, char touch_pressed);


#endif /* DEMO_MOTOR_H_ */
