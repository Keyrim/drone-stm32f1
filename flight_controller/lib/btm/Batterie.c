/*
 * DRONE_batterie.c
 *
 *  Created on: 2 juil. 2020
 *      Author: Théo
 */

#include "Batterie.h"

void BATTERIE_init(DRONE_batterie_t * batterie, adc_id_e adc, double voltage_coef){
	ADC_init() ;
	batterie->voltage_coef = voltage_coef ;
	batterie->adc_voltage = adc ;
}
void BATTERIE_update(DRONE_batterie_t * batterie){
	batterie->voltage = (double)ADC_getValue(batterie->adc_voltage) * batterie->voltage_coef ;

	if(batterie->voltage > 12)
		batterie->batterie_lvl = BATTERIE_LVL_HIGH ;
	else if (batterie->voltage > 11.4)
		batterie->batterie_lvl = BATTERIE_LVL_MEDIUM ;
	else if (batterie->voltage > 10.6)
		batterie->batterie_lvl = BATTERIE_LVL_LOW ;
	else if (batterie->voltage > 8)
		batterie->batterie_lvl = BATTERIE_LVL_LOW_CUTOF ;
	else
		batterie->batterie_lvl = BATTERIE_LVL_NULL ;
}
