/*
 * telemetrie.c
 *
 *  Created on: 6 oct. 2020
 *      Author: Theo
 */

#include "telemetrie.h"
#include "stm32f1_sys.h"

//Une seule télémétrie dans le drone donc en save en dure le pointeur sur la struct telem qu on va use
static telemetrie_t * telemetrie ;

//Fonction d'init, configure l'uart et le print
void TELEMETRIE_Init(telemetrie_t * telemetrie_, uart_id_e uart_telemetrie, uint32_t baud_rate, uint32_t periode_send_data){
	//Save du pointeur
	telemetrie = telemetrie_ ;

	//Init variables
	telemetrie->uart = uart_telemetrie ;
	telemetrie->baud_rate = baud_rate ;
	telemetrie->periode_send_data = periode_send_data ;

	//Config uart
	UART_init(uart_telemetrie, baud_rate);

	//Printf config
	SYS_set_std_usart(uart_telemetrie, uart_telemetrie, uart_telemetrie);

}

//Fonction qui permet d'envoyer des données avec id et tableau d'octect
void TELEMETRIE_Send_data(Ids_t id, uint8_t * data, uint8_t len){
	uint8_t bytes[20] = {0};
	uint32_t checksum = 0 ;

	bytes[0] = '$' ;
	checksum += (uint32_t)bytes[0];

	bytes[1] = id ;
	checksum += (uint32_t)bytes[1];

	for(uint8_t b = 0; b < len; b++){
		bytes[2 + b] = data[b];
		checksum += (uint32_t)bytes[2 + b];
	}

	bytes[2 + len] = (uint8_t)(checksum % 256) ;
	UART_puts_it(telemetrie->uart, bytes, (uint8_t)(3 + len));
}

