/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 *
 * Ce code est inspiré de la librairie fournie par Tilen Majerle...pour STM32F4
 * Un portage est ici réalisé pour STM32F103.
 */
#include "bmp180.h"
#include <stdio.h>

#include "config.h"
#if USE_BMP180
/* Multiple is faster than divide */
#define BMP180_1_16     ((float) 0.0625)
#define BMP180_1_256    ((float) 0.00390625)
#define BMP180_1_2048   ((float) 0.00048828125)
#define BMP180_1_4096   ((float) 0.000244140625)
#define BMP180_1_8192   ((float) 0.0001220703125)
#define BMP180_1_32768  ((float) 0.000030517578125)
#define BMP180_1_65536  ((float) 0.0000152587890625)
#define BMP180_1_101325 ((float) 0.00000986923266726)


//Fonction blocante, pour démo.
void BMP180_demo(void)
{
	char buffer[50];
	/* Working structure */
	BMP180_t BMP180_Data;

	/* Initialize BMP180 pressure sensor */
	if (BMP180_Init(&BMP180_Data) == BMP180_Result_Ok) {
		/* Init OK */
		printf("BMP180 configured and ready to use\n\n");
	} else {
		/* Device error */
		printf("BMP180 error\n\n");
		return;
	}

	/* Imagine, we are at 1000 meters above the sea */
	/* And we read pressure of 95000 pascals */
	/* Pressure right on the sea is */
	printf( "Pressure right above the sea: %ld pascals\n", BMP180_GetPressureAtSeaLevel(95000, 1000));
	printf("Data were calculated from pressure %ld pascals at know altitude %d meters\n\n\n", 95000, 1000);

	while (1)
	{
		/* Start temperature conversion */
		BMP180_StartTemperature(&BMP180_Data);

		/* Wait delay in microseconds */
		/* You can do other things here instead of delay */
		HAL_Delay(BMP180_Data.Delay/1000+1);

		/* Read temperature first */
		BMP180_ReadTemperature(&BMP180_Data);

		/* Start pressure conversion at ultra high resolution */
		BMP180_StartPressure(&BMP180_Data, BMP180_Oversampling_UltraHighResolution);

		/* Wait delay in microseconds */
		/* You can do other things here instead of delay */
		HAL_Delay(BMP180_Data.Delay/1000+1);

		/* Read pressure value */
		BMP180_ReadPressure(&BMP180_Data);

		/* Format data and print to USART */
		sprintf(buffer, "Temp: %2.3f degrees\nPressure: %6ld Pascals\nAltitude at current pressure: %3.2f meters\n\n",
			BMP180_Data.Temperature,
			BMP180_Data.Pressure,
			BMP180_Data.Altitude
		);
		/* Send to USART */
		printf(buffer);

		/* Some delay */
		HAL_Delay(1000);
	}
}


/* EEPROM values */
int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
uint16_t AC4, AC5, AC6, UT;
/* OK */
int32_t X1, X2, X3, B3, B5, B6, T, p;
uint32_t B4, B7, UP;
uint8_t lib_initialized = 0;

BMP180_Result_t BMP180_Init(BMP180_t* BMP180_Data) {
	uint8_t data[22];
	uint8_t i = 0;
	
	/* Initialize I2C */
	I2C_Init(BMP180_I2C, BMP180_I2C_SPEED);
	/* Test if device is connected */
	if (!I2C_IsDeviceConnected(BMP180_I2C, BMP180_I2C_ADDRESS)) {
		/* Device is not connected */
		return BMP180_Result_DeviceNotConnected;
	}
	
	/* Get default values from EEPROM */
	/* EEPROM starts at 0xAA address, read 22 bytes */
	I2C_ReadMulti(BMP180_I2C, BMP180_I2C_ADDRESS, BMP180_REGISTER_EEPROM, data, 22);
	
	/* Set configuration values */
	AC1 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC2 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC3 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC4 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC5 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC6 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
	B1 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	B2 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	MB = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	MC = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	MD = (int16_t)(data[i] << 8 | data[i + 1]);
	
	/* Initialized OK */
	lib_initialized = 1;
	
	/* Return OK */
	return BMP180_Result_Ok;
}

BMP180_Result_t BMP180_StartTemperature(BMP180_t* BMP180_Data) {
	/* Check for library initialization */
	if (!lib_initialized) {
		return BMP180_Result_LibraryNotInitialized;
	}
	/* Send to device */
	I2C_Write(BMP180_I2C, BMP180_I2C_ADDRESS, BMP180_REGISTER_CONTROL, BMP180_COMMAND_TEMPERATURE);
	/* Set minimum delay */
	BMP180_Data->Delay = BMP180_TEMPERATURE_DELAY;
	/* Return OK */
	return BMP180_Result_Ok;
}

BMP180_Result_t BMP180_ReadTemperature(BMP180_t* BMP180_Data) {
	uint8_t data[2];
	
	/* Check for library initialization */
	if (!lib_initialized) {
		return BMP180_Result_LibraryNotInitialized;
	}
	
	/* Read multi bytes from I2C */
	I2C_ReadMulti(BMP180_I2C, BMP180_I2C_ADDRESS, BMP180_REGISTER_RESULT, data, 2);
	
	/* Get uncompensated temperature */
	UT = data[0] << 8 | data[1];
	
	/* Calculate true temperature */
	X1 = (UT - AC6) * AC5 * BMP180_1_32768;
	X2 = MC * 2048 / (X1 + MD);
	B5 = X1 + X2;
	
	/* Get temperature in degrees */
	BMP180_Data->Temperature = (B5 + 8) / ((float)160);
	
	/* Return OK */
	return BMP180_Result_Ok;
}

BMP180_Result_t BMP180_StartPressure(BMP180_t* BMP180_Data, BMP180_Oversampling_t Oversampling) {
	uint8_t command;
	
	/* Check for library initialization */
	if (!lib_initialized) {
		return BMP180_Result_LibraryNotInitialized;
	}
	
	switch (Oversampling) {
		case BMP180_Oversampling_UltraLowPower :
			command = BMP180_COMMAND_PRESSURE_0;
			BMP180_Data->Delay = BMP180_PRESSURE_0_DELAY;
			break;
		case BMP180_Oversampling_Standard:
			command = BMP180_COMMAND_PRESSURE_1;
			BMP180_Data->Delay = BMP180_PRESSURE_1_DELAY;
			break;
		case BMP180_Oversampling_HighResolution:
			command = BMP180_COMMAND_PRESSURE_2;
			BMP180_Data->Delay = BMP180_PRESSURE_2_DELAY;
			break;
		case BMP180_Oversampling_UltraHighResolution:
			command = BMP180_COMMAND_PRESSURE_3;
			BMP180_Data->Delay = BMP180_PRESSURE_3_DELAY;
			break;
		default:
			command = BMP180_COMMAND_PRESSURE_0;
			BMP180_Data->Delay = BMP180_PRESSURE_0_DELAY;
			break;
	}
	/* Send to device */
	I2C_Write(BMP180_I2C, BMP180_I2C_ADDRESS, BMP180_REGISTER_CONTROL, command);
	/* Save selected oversampling */
	BMP180_Data->Oversampling = Oversampling;
	/* Return OK */
	return BMP180_Result_Ok;
}

BMP180_Result_t BMP180_ReadPressure(BMP180_t* BMP180_Data) {
	uint8_t data[3];
	
	/* Check for library initialization */
	if (!lib_initialized) {
		return BMP180_Result_LibraryNotInitialized;
	}
	
	/* Read multi bytes from I2C */
	I2C_ReadMulti(BMP180_I2C, BMP180_I2C_ADDRESS, BMP180_REGISTER_RESULT, data, 3);
	
	/* Get uncompensated pressure */
	UP = (data[0] << 16 | data[1] << 8 | data[2]) >> (8 - (uint8_t)BMP180_Data->Oversampling);

	/* Calculate true pressure */
	B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6 * BMP180_1_4096)) * BMP180_1_2048;
	X2 = AC2 * B6 * BMP180_1_2048;
	X3 = X1 + X2;
	B3 = (((AC1 * 4 + X3) << (uint8_t)BMP180_Data->Oversampling) + 2) * 0.25;
	X1 = AC3 * B6 * BMP180_1_8192;
	X2 = (B1 * (B6 * B6 * BMP180_1_4096)) * BMP180_1_65536;
	X3 = ((X1 + X2) + 2) * 0.25;
	B4 = AC4 * (uint32_t)(X3 + 32768) * BMP180_1_32768;
	B7 = ((uint32_t)UP - B3) * (50000 >> (uint8_t)BMP180_Data->Oversampling);
	if (B7 < 0x80000000) {
		p = (B7 * 2) / B4;
	} else {
		p = (B7 / B4) * 2;
	}
	X1 = ((float)p * BMP180_1_256) * ((float)p * BMP180_1_256);
	X1 = (X1 * 3038) * BMP180_1_65536;
	X2 = (-7357 * p) * BMP180_1_65536;
	p = p + (X1 + X2 + 3791) * BMP180_1_16;
	
	/* Save pressure */
	BMP180_Data->Pressure = p;
	
	/* Calculate altitude */
	BMP180_Data->Altitude = (float)44330.0 * (float)((float)1.0 - (float)pow((float)p * BMP180_1_101325, 0.19029495));
	
	/* Return OK */
	return BMP180_Result_Ok;
}

uint32_t BMP180_GetPressureAtSeaLevel(uint32_t pressure, float altitude) {
	return (uint32_t)((float)pressure / ((float)pow(1 - (float)altitude / (float)44330, 5.255)));
}

#endif
