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

//Branchement ruban led
#define LED_MATRIX_PORT_DATA GPIOB
#define LED_MATRIX_PIN_DATA GPIO_PIN_13

//Uart pour le gps
#define UART_GPS UART2_ID

#endif /* BRANCHEMENT_H_ */
