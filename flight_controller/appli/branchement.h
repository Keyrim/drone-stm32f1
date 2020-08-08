/*
 * branchement.h
 *
 *  Created on: 28 mars 2020
 *      Author: Theo
 */

#ifndef BRANCHEMENT_H_
#define BRANCHEMENT_H_

//Batterie
#define BATTERIE_ADC_VOLTAGE ADC_9
#define PIN_BATTERIE	GPIO_PIN_1
#define PORT_BATTERIE	GPIOB

//Led d'état
#define GPIO_STATE_LED GPIOC
#define GPIO_PIN_STATE_LED GPIO_PIN_13

//Branchements pour l'ibus
#define UART_IBUS UART1_ID

//Uart pour la télémetrie
#define UART_TELEMETRIE UART3_ID

//Branchements escs
#define esc0_gpio GPIOA
#define esc1_gpio GPIOB
#define esc2_gpio GPIOB
#define esc3_gpio GPIOB

#define esc0_pin GPIO_PIN_15
#define esc1_pin GPIO_PIN_4
#define esc2_pin GPIO_PIN_3
#define esc3_pin GPIO_PIN_5

//Branchement ruban led
#define LED_MATRIX_PORT_DATA GPIOB
#define LED_MATRIX_PIN_DATA GPIO_PIN_13

//Uart pour le gps
#define UART_GPS UART2_ID

#endif /* BRANCHEMENT_H_ */
