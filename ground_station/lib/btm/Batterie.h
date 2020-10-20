/*
 * DRONE_batterie.h
 *
 *  Created on: 2 juil. 2020
 *      Author: Théo
 */

#ifndef LIB_PERSO_DRONE_BATTERIE_H_
#define LIB_PERSO_DRONE_BATTERIE_H_

#include "stm32f1_adc.h"

typedef enum{
	BATTERIE_LVL_NULL,
	BATTERIE_LVL_LOW_CUTOF,
	BATTERIE_LVL_LOW,
	BATTERIE_LVL_MEDIUM,
	BATTERIE_LVL_HIGH
}batterie_lvl_t;

typedef struct{
	double voltage ;
	adc_id_e adc_voltage ;
	double voltage_coef ;
	batterie_lvl_t batterie_lvl ;
}DRONE_batterie_t;

void BATTERIE_init(DRONE_batterie_t * batterie, adc_id_e adc, double voltage_coef);
void BATTERIE_update(DRONE_batterie_t * batterie);

#endif /* LIB_PERSO_DRONE_BATTERIE_H_ */
