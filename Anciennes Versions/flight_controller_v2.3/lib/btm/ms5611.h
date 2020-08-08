/*
 * ms5611.h
 *
 *  Created on: 22 juil. 2020
 *      Author: Théo
 */

#ifndef BTM_MS5611_H_
#define BTM_MS5611_H_

#include "macro_types.h"
#include "stm32f1_i2c.h"

#define MS5611_ADRESSE  0x77

/*  MS5611 registers */
#define MS5611_PROM                 0xA0
#define MS5611_TEMP                 0X58
#define MS5611_PRESSURE             0X48
#define MS5611_READ_DATA            0X00

typedef struct{
	uint16_t calibration_values[6];
	uint32_t raw_pressure ;
	uint32_t raw_temperature ;
	int32_t temperature ;
	uint32_t presure ;

}ms5611_t;

void MS5611_get_calibration_values(ms5611_t * ms5611, bool_e init_i2c);


#endif /* BTM_MS5611_H_ */
