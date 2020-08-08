/*
 * DRONE_batterie.c
 *
 *  Created on: 2 juil. 2020
 *      Author: Théo
 */

#include "Batterie.h"

void Batterie_init(DRONE_batterie_t * batterie, adc_id_e adc, double voltage_coef){
	ADC_init() ;
	batterie->voltage_coef = voltage_coef ;
	batterie->adc_voltage = adc ;
}
void Batterie_update(DRONE_batterie_t * batterie){
	batterie->voltage = (double)ADC_getValue(batterie->adc_voltage) * batterie->voltage_coef ;
}
