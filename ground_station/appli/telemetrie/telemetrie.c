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

// ----------------- fonctions hors sub telemetrie -----------------------------
void TELEMETRIE_send_consigne_base(uint8_t consigne){
	uint8_t bytes[1] ;
	bytes[0] = consigne ;
	TELEMETRIE_Send_data(ID_BASE_CONSIGNE_BASE, bytes, 1);
}

void TELEMETRIE_send_high_lvl_transi(uint8_t transi){
	uint8_t bytes[1] ;
	bytes[0] = transi ;
	TELEMETRIE_Send_data(ID_PC_HIGH_LVL_TRANSITION, bytes, 1);
}

//Double
void TELEMETRIE_send_double(float value, uint8_t id){
	uint8_t bytes[4] = {0};
	int32_t int_value = (int32_t)( value * (float) 1000000);
	bytes[0] = (uint8_t)((int_value >> 24) & 0b11111111) ;
	bytes[1] = (uint8_t)((int_value >> 16) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_value >> 8) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_value ) & 0b11111111) ;
	TELEMETRIE_Send_data(id, bytes, 4);
}
void TELEMETRIE_send_double_16b(float value, uint8_t id){
	uint8_t  bytes[2] = {0} ;
	int16_t int_value =  (int16_t)( value * (double) 100);
	bytes[0] = (uint8_t)(int_value >> 8);
	bytes[2] = (uint8_t)(int_value & 0b11111111);
	TELEMETRIE_Send_data(id, bytes, 2);
}

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

