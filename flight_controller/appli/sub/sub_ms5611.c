/*
 * sub_ms5611.c
 *
 *  Created on: 29 juil. 2020
 *      Author: Théo
 */
#include "sub_action.h"

//On alterne entre 15(periode_temperature) mesure de la pression et une mesure de température car la température ne change vite
#define PERIODE_TEMPERATURE 15

//Gestion des requêtes de donnée, calcul de pression température et altitude pour le baromètre
uint32_t sub_ms5611(State_drone_t * drone, uint32_t current_time_us){
	static int32_t read_state = 0 ;
	static uint32_t wait_time = 0 ;
	static uint32_t previous_time = 0 ;

	static int32_t compteur_temp = 0 ;

	if(current_time_us >= wait_time + previous_time){
		switch(read_state){
			case 0:
				MS5611_request_temp();
				wait_time = 10000 ;
				read_state = 1 ;
				break;

			case 1:
				MS5611_read_temp(&drone->capteurs.ms5611);
				MS5611_calculate_temperature(&drone->capteurs.ms5611);
				wait_time = 200 ;
				read_state = 2;
				break;
			case 2:
				MS5611_request_pressure();
				wait_time = 10000 ;
				read_state = 3 ;
				break;
			case 3 :
				MS5611_read_pressure(&drone->capteurs.ms5611);	//154 µs
				wait_time = 200 ;
				read_state = 4 ;
				break;

			case 4 :
				MS5611_calculate_pressure(&drone->capteurs.ms5611); // 160 µs
				read_state = 5 ;
				break;

			case 5 :
				MS5611_calculate_altitude(&drone->capteurs.ms5611); //200 µs
				compteur_temp ++ ;
				read_state = (compteur_temp == PERIODE_TEMPERATURE) ? 0 : 2;
				break;
		}
		previous_time = current_time_us ;
	}

	return wait_time ;

}

