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
uint32_t sub_ms5611(State_drone_t * drone){
	static int32_t read_state = 0 ;
	static uint32_t wait_time = 0 ;
	static uint32_t previous_time = 0 ;

	static int32_t compteur_temp = 0 ;

	uint32_t to_return = 0 ;

	uint32_t time = SYSTICK_get_time_us();
	if(time >= wait_time + previous_time){
		switch(read_state){
			case 0:
				//Prend 57 µs
				//Demande temperature
				MS5611_request_temp();
				//Attendre  8,3 ms
				wait_time = 10000 ;
				//Prochaine action prend 160 µs
				to_return = 180 ;
				read_state = 1 ;
				break;

			case 1:
				//Prend 160 µs
				MS5611_read_temp(&drone->capteurs.ms5611);
				MS5611_calculate_temperature(&drone->capteurs.ms5611);
				wait_time = 0 ;
				//Prochaine action prend 66 µs
				to_return = 80 ;
				read_state = 2;
				break;
			case 2:
				//Prend 66 µs
				MS5611_request_pressure();
				wait_time = 10000 ;
				//Prochaine action prend 410 µs
				to_return = 420 ;
				read_state = 3 ;
				break;
			case 3 :
				//Prend 410 µs
				MS5611_read_pressure(&drone->capteurs.ms5611);	//154 µs

				MS5611_calculate_pressure(&drone->capteurs.ms5611); // 160 µs
				MS5611_calculate_altitude(&drone->capteurs.ms5611); // 400 µs



				compteur_temp ++ ;
				if(compteur_temp == PERIODE_TEMPERATURE){
					//Prochaine action prend 57 µs
					to_return = 70 ;
					compteur_temp = 0;
					read_state = 0;
				}
				else{
					//Prochaine action prend 66 µs
					to_return = 80 ;
					read_state = 2 ;
				}


				break;
		}
		previous_time = time ;
	}

	return to_return ;

}

