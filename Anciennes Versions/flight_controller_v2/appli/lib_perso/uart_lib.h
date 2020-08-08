/*
 * uart_lib.h
 *
 *  Created on: 11 juin 2020
 *      Author: Théo
 */

#ifndef UART_LIB_H_
#define UART_LIB_H_
#include "stm32f1_uart.h"
#include "macro_types.h"
#include "systick.h"
#define TX_BUFFER_SIZE 50

typedef struct{
	uint8_t tx_buffer [TX_BUFFER_SIZE];
	uart_id_e uart_id ;
	uint32_t baud_rate ;
	uint32_t periode_send ;		//Période d'envoit en ms
	uint16_t car_per_transmit ;
	uint16_t index_buffer;
	uint32_t last_time_sent ;
}uart_struct_e;

void uart_init(uart_struct_e * uart, uart_id_e uart_id_, uint32_t baud_rate_, uint32_t periode_);
bool_e uart_add_one(uart_struct_e * uart, uint8_t c);	//on ajoute un caractère au buffer
bool_e uart_add_few(uart_struct_e * uart, uint8_t * str, uint16_t len);	//on ajoute plusieurs caractère aux buffer
void uart_send(uart_struct_e * uart);		//on envoit les caractère en fonction

#endif /* UART_LIB_H_ */
