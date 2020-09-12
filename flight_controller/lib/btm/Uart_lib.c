/*
 * uart_lib.c
 *
 *  Created on: 11 juin 2020
 *      Author: Théo
 */
#include "Uart_lib.h"



//On enregistre les différents paramète pour la structure donnée
void uart_init(uart_struct_e * uart, uart_id_e uart_id_, uint32_t baud_rate_, uint32_t periode_){
	//On init toutes les valeurs
	uart->baud_rate = baud_rate_ ;
	uart->periode_send = periode_ ;
	uart->index_buffer = 0 ;
	uart->last_time_sent = 0 ;
	uart->uart_id = uart_id_ ;
	uart->car_per_transmit = (uint16_t)(baud_rate_ / ( (1000 / periode_) * 10 )) ;	//On compte 10 bits par octet ce qui est faux mais bon il y a bit de debut et fin et pas lapein de jouer sur les limites
	for(uint16_t t = 0; t < TX_BUFFER_SIZE; t++)
		uart->tx_buffer[t] = 0 ;
	UART_init(uart_id_, baud_rate_);


}

//On rajoute un caractère au buffer
bool_e uart_add_one(uart_struct_e * uart, uint8_t c){
	//Si on a dépasser le tableau, on va éviter d'écrire on ne sait pas où
	if(uart->index_buffer < TX_BUFFER_SIZE){
		uart->tx_buffer[uart->index_buffer] = c ;
		uart->index_buffer ++ ;
		return 1;
	}
	else
		return 0 ;
}

//On rajoute plusieur caractère au buffer
bool_e uart_add_few(uart_struct_e * uart, uint8_t * str, uint16_t len){
	//Si on a ou va dépasser le tableau, on va éviter d'écrire on ne sait pas où
	if(uart->index_buffer + len - 1 < TX_BUFFER_SIZE){
		for(uint16_t i = 0 ; i < len; i ++)
		{
			uart->tx_buffer[uart->index_buffer] = str[i] ;
			uart->index_buffer ++ ;
		}
		return 1;
	}
	else
		return 0 ;
}

//On vide le buffer à intervalle de temps régulier de sorte de ne pas avoir d'overflow
void uart_send(uart_struct_e * uart, uint32_t current_time_us){
	//On check si le temps recquis est passé et si le tableau n'est pas vide
	if((current_time_us / 1000 > uart->last_time_sent + uart->periode_send && uart->index_buffer)){
		UART_puts_it(uart->uart_id, uart->tx_buffer, uart->index_buffer);
		uart->last_time_sent += uart->periode_send ;
		uart->index_buffer = 0 ;


	}
}



