/*
 * ibus.c
 *
 *  Created on: 25 juil. 2020
 *      Author: Théo
 */

#include "Ibus.h"
#include "systick.h"


//Fonction privé qui test si le checsum est bon sur un buffer
uint32_t check_sum_f(uint8_t * buffer);
uint32_t check_sum_f(uint8_t * buffer){
	uint32_t sum = 0 ;
	for(uint8_t b=0; b <30; b++)
		sum += (uint32_t)buffer[b];
	return sum ;
}

//Init de l'uart
void IBUS_init(ibus_t * ibus, uart_id_e uart){
	ibus->uart = uart ;
	UART_init(ibus->uart, 115200);
}

//On lit un caractère si il y en a un de dispo et on l'ajoute au buffer
//Quand on a tous noc caractère on met à jours les valeurs de du tableau radio
bool_e IBUS_check_data(ibus_t * ibus){
	bool_e to_return = FALSE ;
	if(UART_data_ready(ibus->uart)){
		uint8_t c = UART_get_next_byte(ibus->uart);
		switch(ibus->state){
			case LENGTH :
				//ON sait que si on reçoit pas une lentgh de 0x20 (32) c pas bon
				if(c == 0x20){
					ibus->state = COMMAND ;
					ibus->buffer[0] = c ;
				}
				break;
			case COMMAND :
				//Seul commande qu'on connait btw
				if(c == 0x40){
					ibus->state = CHANNELS ;
					ibus->buffer[1] = c ;
					ibus->compteur = 2 ;
				}
				//ON check que du coup c pas la commande de départ au cas ou et sinon retour au début
				else if(c != 0x20)
						ibus->state = LENGTH ;
				break ;

			case CHANNELS :
				ibus->buffer[ibus->compteur++] = c ;
				if(ibus->compteur == 32){
					uint32_t cs = (uint32_t)(ibus->buffer[30] | (ibus->buffer[31] << 8)) ;
					if(0xFFFF - check_sum_f(ibus->buffer)== cs)
					{
						for(uint32_t radio = 0; radio < 28; radio +=2)
							ibus->channels[radio / 2] = (int32_t)(ibus->buffer[radio+2] | (ibus->buffer[radio + 3] << 8));
						ibus->last_update = SYSTICK_get_time_us();

					}
					ibus->state = LENGTH ;
				}
				break;

		}
		to_return = TRUE ;
	}
	return to_return ;

}
