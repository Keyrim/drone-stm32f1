/*
 * telem_2.c
 *
 *  Created on: 8 sept. 2020
 *      Author: Théo
 */

#include "telem_2.h"
#include "../../appli/branchement.h"

void TELEM2_send_data(Ids_t id, uint8_t * data, uint8_t len){
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
	UART_puts_it(UART_TELEMETRIE, bytes, (uint8_t)(3 + len));
}
void TELEM2_send_float(Ids_t id, float value){
	UNUSED(id);
	UNUSED(value);
}
