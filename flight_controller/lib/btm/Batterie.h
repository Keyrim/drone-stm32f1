/*
 * DRONE_batterie.h
 *
 *  Created on: 2 juil. 2020
 *      Author: Théo
 */

#ifndef LIB_PERSO_DRONE_BATTERIE_H_
#define LIB_PERSO_DRONE_BATTERIE_H_

#include "stm32f1_adc.h"

typedef struct{
	double voltage ;
	adc_id_e adc_voltage ;
	double voltage_coef ;
}DRONE_batterie_t;

void Batterie_init(DRONE_batterie_t * batterie, adc_id_e adc, double voltage_coef);
void Batterie_update(DRONE_batterie_t * batterie);

#endif /* LIB_PERSO_DRONE_BATTERIE_H_ */
