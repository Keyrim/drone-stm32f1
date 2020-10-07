/*
 * telemetrie.h
 *
 *  Created on: 6 oct. 2020
 *      Author: Theo
 */


#ifndef TELEMETRIE_TELEMETRIE_H_
#define TELEMETRIE_TELEMETRIE_H_


#include "../IDs.h"
#include "stm32f1_uart.h"

/*
 * Structure de la sous couche télémétrie :
 *
 * Variable statique de cette sous couche logiciel :
 * 		UART_TELEM
 * 		FREQUENCE d'appel de la tâche send data
 * 		BAUD_RATE permet de limiter le nombre d'octet envoyer par seconde
 *
 * Fonction d'init :
 * 		Config de l uart
 * 		Config de la structure
 *
 * Fonction télmétrie send : prend un id, une taille et transmet à l'uart avec la bonne forme (octet de début, id et checksum)
 *
 * Fonctions utiliser par la sub send data :
 * 		Fonctions statiques car pas appelées hier
 *
 * Fonctions "publiques"
 * 		Send double
 *
 *
 */

typedef struct{
	uart_id_e uart ;
	uint32_t baud_rate ;
	uint32_t periode_send_data ;
}telemetrie_t;

void TELEMETRIE_Init(telemetrie_t * telemetrie, uart_id_e uart_telemetrie, uint32_t baud_rate, uint32_t periode_send_data);
void TELEMETRIE_Send_data(Ids_t id, uint8_t * data, uint8_t len);





#endif /* TELEMETRIE_TELEMETRIE_H_ */
