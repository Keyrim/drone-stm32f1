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
	float x_acc;
	float y_acc;
	float z_acc;
	//Offset acc
	float x_acc_offset;
	float y_acc_offset;
	float z_acc_offset;
	//Vitesse angualaire
	float x_gyro;
	float y_gyro;
	float z_gyro;
	//dans le cas d un filtrage du gyro on stock les valeurs brute
	float x_gyro_unfiltered ;
	float y_gyro_unfiltered ;
	float z_gyro_unfiltered ;
	//Offset gyro
	float x_gyro_offset;
	float y_gyro_offset;
	float z_gyro_offset;
	//Angle obtenu par trigo sur l'acc
	float x_acc_angle;
	float y_acc_angle;
	//float du complementary filter
	float alpha;
	//Sensi gyro et acc
	float gyro_sensi;
	float acc_sensi;
	//Si c'est la première fois qu'on lis, on utilise juste les angles du complementary filter
	bool_e first_read;
	//Permet de savoir si le mpu va bien
	bool_e mpu_is_ok;
	//Filtre gyros
	Filter_second_order_t gyro_x_filter ;
	Filter_second_order_t gyro_y_filter ;
	Filter_second_order_t gyro_z_filter ;
	//Structure avec les données telle quelle du mpu
	MPU6050_t raw_data_mpu ;
	MPU6050_Result_t mpu_result;
	//Fréquence d'utilisation du mpu
	int32_t frequency;
	float periode ;

}DRONE_mpu6050_t;

void Mpu_imu_init(DRONE_mpu6050_t * angles, MPU6050_Accelerometer_t acc_sensi, MPU6050_Gyroscope_t gyro_sensi, float alpha, int32_t frequency);
void Mpu_imu_update_angles(DRONE_mpu6050_t * angles);
void Mpu_gyro_filtering(DRONE_mpu6050_t * angles);
bool_e Mpu_imu_calibrate(DRONE_mpu6050_t * angles, uint16_t epoch);	//Return true si c fini, faux si il est en train de calibrer


#endif /* COMPLEMENTARY_FILTER_H_ */
