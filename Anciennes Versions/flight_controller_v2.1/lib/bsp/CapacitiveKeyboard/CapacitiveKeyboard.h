/*
 * CapacitiveKeyboard.h
 *
 *  Created on: 4 mai 2016
 *      Author: Nirgal
 */

#ifndef BSP_CAPACITIVEKEYBOARD_CAPACITIVEKEYBOARD_H_
#define BSP_CAPACITIVEKEYBOARD_CAPACITIVEKEYBOARD_H_

	#include "macro_types.h"
	#include "stm32f1xx_hal.h"

	#define CAPACITIVE_KEYBOARD_SDO_PORT	GPIOA
	#define CAPACITIVE_KEYBOARD_SDO_PIN		GPIO_PIN_8
	#define CAPACITIVE_KEYBOARD_SCL_PORT	GPIOA
	#define CAPACITIVE_KEYBOARD_SCL_PIN		GPIO_PIN_9

	#define MATRIX_KEYBOARD_HAL_CLOCK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()



	void DEMO_CapacitiveKeyboard_process_1ms(void);
	void CapacitiveKeyboard_init(void);
	uint16_t CapacitiveKeyboard_read(void);
	running_e DEMO_CapacitiveKeyboard (bool_e ask_for_finish);


#endif /* BSP_CAPACITIVEKEYBOARD_CAPACITIVEKEYBOARD_H_ */
