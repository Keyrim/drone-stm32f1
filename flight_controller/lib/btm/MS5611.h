/*
 * ms5611.h
 *
 *  Created on: 22 juil. 2020
 *      Author: Théo
 */

#ifndef BTM_MS5611_H_
#define BTM_MS5611_H_

#include "macro_types.h"


#define MS5611_ADRESSE  0xEE

/*  MS5611 registers */
#define MS5611_PROM                 0xA0
#define MS5611_TEMP                 0X58
#define MS5611_PRESSURE             0X48
#define MS5611_READ_DATA            0X00

typedef struct{
	uint16_t calibration_values[6];
	uint32_t raw_pressure ;
	uint32_t raw_temperature ;
	uint32_t pressure ;
	int32_t temperature ;
	int32_t dT ;
	double altitude;
	double altitude_shift ;

}ms5611_t;

void MS5611_get_calibration_values(ms5611_t * ms5611, bool_e init_i2c);
void MS5611_request_temp();
void MS5611_request_pressure();
void MS5611_read_temp(ms5611_t * ms5611);
void MS5611_read_pressure(ms5611_t * ms5611);
void MS5611_calculate_temperature(ms5611_t * ms5611);
void MS5611_calculate_pressure(ms5611_t * ms5611);
void MS5611_calculate_altitude(ms5611_t * ms5611);


#endif /* BTM_MS5611_H_ */
