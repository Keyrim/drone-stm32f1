/*
 * complementary_filer.h
 *
 *  Created on: 21 mars 2020
 *      Author: Theo
 */

#ifndef DRONE_MPU6050_H_
#define DRONE_MPU6050_H_

#include "stm32f1_mpu6050.h"
#include "math.h"
#include "Filters.h"

typedef struct{
	//Angles entre -180 et 180
	float x;
	float y;
	float z;
	//L'accéllération linéaire
	float x_acc_raw;
	float y_acc_raw;
	float z_acc_raw;
	//Filtre passe bas pour le gyro
	Filter_t x_acc_filter ;
	Filter_t y_acc_filter ;
	Filter_t z_acc_filter ;
	//Accéllération après passe bas
	float x_acc_filtered;
	float y_acc_filtered;
	float z_acc_filtered;
	//Offset acc
	float x_acc_offset;
	float y_acc_offset;
	float z_acc_offset;
	//Vitesse angualaire
	float x_gyro_raw;
	float y_gyro_raw;
	float z_gyro_raw;
	//Filtre passe bas pour le gyro
	Filter_t x_gyro_filter ;
	Filter_t y_gyro_filter ;
	Filter_t z_gyro_filter ;
	//dans le cas d un filtrage du gyro on stock les valeurs brute
	float x_gyro_filtered ;
	float y_gyro_filtered ;
	float z_gyro_filtered ;
	//Offset gyro
	float x_gyro_offset;
	float y_gyro_offset;
	float z_gyro_offset;
	//Angle obtenu par trigo sur l'acc
	float x_acc_angle;
	float y_acc_angle;
	//float du complementary filter
	float alpha;
	float one_minus_alpha;
	//Sensi gyro et acc
	float gyro_sensi;
	float acc_sensi;
	//Si c'est la première fois qu'on lit, on utilise juste les angles de l'acc obtenu par trigo
	bool_e first_read;
	//Permet de savoir si le mpu va bien
	bool_e mpu_is_ok;
	//Structure du mpu en brute brute (tel que lue en I2C )
	MPU6050_t raw_data_mpu ;
	MPU6050_Result_t mpu_result;
	//Fréquence d'utilisation du mpu
	int32_t frequency;
	float periode ;

}DRONE_mpu6050_t;

void Mpu_imu_init(DRONE_mpu6050_t * angles, MPU6050_Accelerometer_t acc_sensi, MPU6050_Gyroscope_t gyro_sensi, float alpha, int32_t frequency);
void IMU_update_mpu6050(DRONE_mpu6050_t * angles);
void IMU_gyro_low_filter(DRONE_mpu6050_t * angles); //Need filter gyro
void IMU_acc_low_filter(DRONE_mpu6050_t * angles);	//Need filter acc
void IMU_complementary_filter(DRONE_mpu6050_t * angles);
bool_e Mpu_imu_calibrate(DRONE_mpu6050_t * angles, uint16_t epoch);	//Return true si c fini, faux si il est en train de calibrer


#endif /* COMPLEMENTARY_FILTER_H_ */
