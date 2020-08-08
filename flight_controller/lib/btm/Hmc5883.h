/*
 * hmc5883.h
 *
 *  Created on: 1 août 2020
 *      Author: Theo
 */

#ifndef BTM_HMC5883_H_
#define BTM_HMC5883_H_

#include "macro_types.h"
#include "stm32f1_i2c.h"

//Register map

#define HMC5883_I2C_ADRESSE	0x3C		//Original is 0x1E, but must be shifter by one to the right in the i2c_lib
#define HMC5883_WRITE 		0x3C
#define HMC5883_READ 		0x3D
#define HMC5883_CONF_A		0X00
#define HMC5883_CONF_B		0X01
#define HMC5883_MODE		0X02
#define HMC5883_X_MSB		0x03
#define HMC5883_X_LSB		0X04
#define HMC5883_Z_MSB		0X05
#define HMC5883_Z_LSB		0x06
#define HMC5883_Y_MSB		0x07
#define HMC5883_Y_LSB		0x08
#define HMC5883_STATUS		0x09
#define HMC5883_IDENTIFICATION_A 0x0A
#define HMC5883_IDENTIFICATION_B 0c0B
#define HMC5883_IDENTIFICATION_C 0x0C


//Configuration register A
#define HMC55883_average_shift 				5
#define HMC5883_output_rate_shift 			2
#define HMC5883_measurement_config_shift	0

//Sample average for register config
#define sample_average_one  	0x00
#define sample_average_two		0x01
#define sample_average_four		0x02
#define sample_average_eight	0x03

//Output rate for register config
#define output_rate_0_75 		0x00
#define output_rate_1_5			0x01
#define output_rate_3_0			0x02
#define output_rate_7_5			0x03
#define output_rate_15_0		0x04
#define output_rate_30_0		0x05
#define output_rate_75_0		0x07

//Measurement mode for register config
#define measurement_mode_normal 		0x00
#define measurement_mode_positive_bias	0x01
#define measurement_mode_negative_bias	0x01

//Configuration register B
#define HMC5883_gain_configuration_shift 5

//Gain configuration
#define gain_configuration_0_88 0x00
#define gain_configuration_1_3	0x01
#define gain_configuration_1_9 0x02
#define gain_configuration_2_5 0x03
#define gain_configuration_4_0 0x04
#define gain_configuration_4_7 0x05
#define gain_configuration_5_6 0x06
#define gain_configuration_8_1 0x07

//Compas sensi
#define compas_sensi_0_88 	0.73
#define compas_sensi_1_3	0.92
#define compas_sensi_1_9	1.22
#define compas_sensi_2_5	1.5
#define compas_sensi_4_0	2.27
#define compas_sensi_4_7	2.56
#define compas_sensi_5_6	3.03
#define compas_sensi_8_1	4.35

//Mode register
#define HMC5883_mode_shift			0
#define HMC5883_high_speed_i2c 		7

#define operating_mode_continuous 			0x00
#define operating_mode_single_measurement	0x01
#define operating_mode_idle					0x02 	//(also 0x03)



//Compas struct
typedef struct{
	double sensi ;
	int16_t x_raw ;
	int16_t y_raw ;
	int16_t z_raw ;
	double x ;
	double y ;
	double z ;
}compas_struct_t;

void HMC5883_init(compas_struct_t * compas, bool_e init_i2c, uint8_t operating_mode, uint8_t measurement_rate, uint8_t gain_config, uint8_t sample_average);
void HMC5883_read(compas_struct_t * compas);





























#endif /* BTM_HMC5883_H_ */
