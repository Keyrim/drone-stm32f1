/*
 * ms5611.c
 *
 *  Created on: 22 juil. 2020
 *      Author: Théo
 */

#include "stm32f1_i2c.h"
#include "math.h"
#include "MS5611.h"

//Chaque ms5611 à des valeurs de calibration qu'on doit récupérer avant de l'utiliser
//C'est ce que fais cette fonction
void MS5611_get_calibration_values(ms5611_t * ms5611, bool_e init_i2c){
	if(init_i2c)
		I2C_Init(I2C1, 400000);

	uint8_t data[2] ;
	for(uint8_t d = 1; d <7; d++)
	{
		I2C_ReadMulti(I2C1, MS5611_ADRESSE, (uint8_t)(MS5611_PROM + (2*d)), data, 2);
		ms5611->calibration_values[d-1] = (uint16_t)((data[0] << 8) | data[1]);
	}
}

//Demande pour une mesure de température
void MS5611_request_temp(){
	I2C_WriteNoRegister(I2C1, MS5611_ADRESSE, MS5611_TEMP);
}

//Demande pour une mesure de pressio
void MS5611_request_pressure(){
	I2C_WriteNoRegister(I2C1, MS5611_ADRESSE, MS5611_PRESSURE);
}

//Lecture de la température
void MS5611_read_temp(ms5611_t * ms5611){
	uint8_t data[3];
	I2C_ReadMulti(I2C1, MS5611_ADRESSE, MS5611_READ_DATA, data, 3);
	ms5611->raw_temperature = (uint32_t)(data[0]<<16 | data[1]<<8 | data[2]);
}

//Lecture de la pression
void MS5611_read_pressure(ms5611_t * ms5611){
	uint8_t data[3];
	I2C_ReadMulti(I2C1, MS5611_ADRESSE, MS5611_READ_DATA, data, 3);
	ms5611->raw_pressure = (uint32_t)(data[0]<<16 | data[1]<<8 | data[2]);
}

//Calcul de la température
void MS5611_calculate_temperature(ms5611_t * ms5611){
	ms5611->dT = ms5611->raw_temperature - (int32_t)ms5611->calibration_values[4] * 256;
	ms5611->temperature = 2000 +  ((int64_t) ms5611->dT * ms5611->calibration_values[5]) / 8388608 ;
}

//Calcul de la pression
void MS5611_calculate_pressure(ms5611_t * ms5611){
	 int64_t OFF = (int64_t)ms5611->calibration_values[1] * 65536 + (int64_t)ms5611->calibration_values[3] * ms5611->dT / 128 ;
	 int64_t SENS = (int64_t)ms5611->calibration_values[0] * 32768 + (int64_t)ms5611->calibration_values[2] * ms5611->dT / 256 ;
	 ms5611->pressure = (uint32_t)(ms5611->raw_pressure * SENS / 2097152 - OFF) / 32768 ;
}

//Calcul de l'altitude
void MS5611_calculate_altitude(ms5611_t * ms5611){
	ms5611->altitude = 44330.0 * (1.0 - pow((double)ms5611->pressure / 101325.0, 1.0/5.255)) - ms5611->altitude_shift ;	// 260 µs lel
	//Si on a le shift en altitude qui est nul, on l'init
	if(!ms5611->altitude_shift)
		ms5611->altitude_shift = ms5611->altitude ;
}
