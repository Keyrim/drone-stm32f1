/*
 * hmc5883.c
 *
 *  Created on: 1 août 2020
 *      Author: Theo
 */

#include "Hmc5883.h"

//Init de la sensi et de l'i2c si besoin
void HMC5883_init(compas_struct_t * compas, bool_e init_i2c, uint8_t operating_mode, uint8_t measurement_rate, uint8_t gain_config, uint8_t sample_average){
	if(init_i2c)
		I2C_Init(I2C1, 400000);

	double sensi = 0 ;
	switch(gain_config){
		case 0 :
			sensi = compas_sensi_0_88 ;
			break ;
		case 1 :
			sensi = compas_sensi_1_3 ;
			break;
		case 2 :
			sensi = compas_sensi_1_9 ;
			break;
		case 3 :
			sensi = compas_sensi_2_5 ;
			break;
		case 4 :
			sensi = compas_sensi_4_0 ;
			break;
		case 5 :
			sensi = compas_sensi_4_7 ;
			break;
		case 6 :
			sensi = compas_sensi_5_6 ;
			break;
		case 7 :
			sensi = compas_sensi_8_1 ;
	}
	compas->sensi = sensi ;

	//Register A config
	uint8_t tmp = (uint8_t)((measurement_rate << HMC5883_output_rate_shift) + (sample_average << HMC55883_average_shift));
	//I2C_Write(I2C1, HMC5883_I2C_ADRESSE, HMC5883_CONF_A, tmp);
	I2C_Write(I2C1, HMC5883_I2C_ADRESSE, HMC5883_CONF_A, 0x78);
	//Register B config
	tmp = (uint8_t)(gain_config << HMC5883_gain_configuration_shift) ;
	//I2C_Write(I2C1, HMC5883_I2C_ADRESSE, HMC5883_CONF_B , 0xA0);
	I2C_Write(I2C1, HMC5883_I2C_ADRESSE, HMC5883_CONF_B , tmp);

	//register mode config
	tmp = operating_mode << HMC5883_mode_shift ;
	//I2C_Write(I2C1, HMC5883_I2C_ADRESSE, HMC5883_MODE , 0);
	I2C_Write(I2C1, HMC5883_I2C_ADRESSE, HMC5883_MODE , tmp);
}

//Fonction de test voir si le capteur répond
void HMC5883_test(){
	int8_t buffer[3] = {0};
	bool_e result = 0 ;
	result = I2C_ReadMulti(I2C1, HMC5883_I2C_ADRESSE, HMC5883_IDENTIFICATION_A, buffer, 3);
	printf("Retour : %d\t%d\t%d\t%d\r\n", result, buffer[0], buffer[1], buffer[2]);

//On lit les valeurs sur les trois axes
void HMC5883_read(compas_struct_t * compas){
	uint8_t data[6];
	I2C_ReadMulti(I2C1, HMC5883_I2C_ADRESSE, HMC5883_X_MSB, data, 6);
	compas->x_raw = (int16_t)(data[0]<<8 | data[1]);
	compas->z_raw = (int16_t)(data[2]<<8 | data[3]);
	compas->y_raw = (int16_t)(data[4]<<8 | data[5]);

}
