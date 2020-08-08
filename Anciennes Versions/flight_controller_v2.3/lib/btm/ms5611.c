/*
 * ms5611.c
 *
 *  Created on: 22 juil. 2020
 *      Author: Théo
 */

#include "ms5611.h"


void MS5611_get_calibration_values(ms5611_t * ms5611, bool_e init_i2c){
	uint8_t data[12] ;
	I2C_ReadMulti(I2C1, MS5611_ADRESSE, MS5611_PROM+2, data, 12);
	ms5611->calibration_values[0] = (int16_t)(data[0] << 8 || data[1]);
	ms5611->calibration_values[1] = (int16_t)(data[2] << 8 || data[3]);
	ms5611->calibration_values[2] = (int16_t)(data[4] << 8 || data[5]);
	ms5611->calibration_values[3] = (int16_t)(data[6] << 8 || data[7]);
	ms5611->calibration_values[4] = (int16_t)(data[8] << 8 || data[9]);
	ms5611->calibration_values[5] = (int16_t)(data[10] << 8 || data[11]);


}
