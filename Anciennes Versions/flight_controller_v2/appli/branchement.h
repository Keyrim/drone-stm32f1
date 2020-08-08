/*
 * branchement.h
 *
 *  Created on: 28 mars 2020
 *      Author: Theo
 */

#ifndef BRANCHEMENT_H_
#define BRANCHEMENT_H_




//Branchements ppm
#define GPIO_PPM GPIOB
#define GPIO_PIN_PPM GPIO_PIN_12
#define PIN_NUMBER 12

//Branchements escs

#define esc0_gpio GPIOA
#define esc1_gpio GPIOB
#define esc2_gpio GPIOB
#define esc3_gpio GPIOB

#define esc0_pin GPIO_PIN_15
#define esc1_pin GPIO_PIN_4
#define esc2_pin GPIO_PIN_3
#define esc3_pin GPIO_PIN_5

#define PIN_BATTERIE	GPIO_PIN_0
#define PORT_BATTERIE	GPIOA

//leds
#define LED_MATRIX_PORT_DATA GPIOB
#define LED_MATRIX_PIN_DATA GPIO_PIN_13
#endif /* BRANCHEMENT_H_ */
