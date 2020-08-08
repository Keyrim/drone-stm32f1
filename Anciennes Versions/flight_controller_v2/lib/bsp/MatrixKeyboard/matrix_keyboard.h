/*
 * MatrixKeyboard.h
 *
 *  Created on: 24 août 2015
 *      Author: Nirgal
 */

#ifndef LIB_BSP_MATRIXKEYBOARD_MATRIX_KEYBOARD_H_
#define LIB_BSP_MATRIXKEYBOARD_MATRIX_KEYBOARD_H_
#include "stm32f1xx_hal.h"
#include "macro_types.h"
#include "config.h"
#if USE_MATRIX_KEYBOARD

	#define PORT_INPUT_0 	GPIOA
	#define PORT_INPUT_1 	GPIOA
	#define PORT_INPUT_2 	GPIOA
	#define PORT_INPUT_3 	GPIOA
	#define	PIN_INPUT_0		GPIO_PIN_4
	#define	PIN_INPUT_1		GPIO_PIN_5
	#define	PIN_INPUT_2		GPIO_PIN_6
	#define	PIN_INPUT_3		GPIO_PIN_7

	#define PORT_OUTPUT_0 	GPIOA
	#define PORT_OUTPUT_1 	GPIOA
	#define PORT_OUTPUT_2 	GPIOA
	#define PORT_OUTPUT_3 	GPIOA
	#define	PIN_OUTPUT_0	GPIO_PIN_8
	#define	PIN_OUTPUT_1	GPIO_PIN_9
	#define	PIN_OUTPUT_2	GPIO_PIN_10
	#define	PIN_OUTPUT_3	GPIO_PIN_11

	#define MATRIX_KEYBOARD_HAL_CLOCK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()

	#define CONFIG_PULL_UP
	//#define CONFIG_PULL_DOWN


	#define NO_KEY 		0xFF
	#define MANY_KEYS 	0xFE


	void KEYBOARD_init(const char * new_keyboard_keys);
	running_e DEMO_matrix_keyboard_process_main (bool_e ask_for_finish);
	void DEMO_matrix_keyboard_process_1ms(void);
	bool_e KEYBOARD_is_pressed(void);
	char KEYBOARD_get_key(void);
	bool_e KEYBOARD_is_pressed(void);
	void KEYBOARD_press_and_release_events(char * press_event, char * release_event, uint32_t * all_touchs_pressed);

#endif

#endif /* LIB_BSP_MATRIXKEYBOARD_MATRIX_KEYBOARD_H_ */
