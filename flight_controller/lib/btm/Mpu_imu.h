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

typedef struct{
	//Angles
	double x;
	double y;
	double z;
	//L'accéllération linéaire
	double x_acc;
	double y_acc;
	double z_acc;
	//Offset acc
	double x_acc_offset;
	double y_acc_offset;
	double z_acc_offset;
	//Vitesse angualaire
	double x_gyro;
	double y_gyro;
	double z_gyro;
	//Offset gyro
	double x_gyro_offset;
	double y_gyro_offset;
	double z_gyro_offset;
	//Angle obtenu par trigo sur l'acc
	double x_acc_angle;
	double y_acc_angle;
	//float du complementary filter
	double alpha;
	//Sensi gyro et acc
	double gyro_sensi;
	double acc_sensi;
	//Si c'est la première fois qu'on lis, on utilise juste les angles du complementary filter
	bool_e first_read;
	//Permet de savoir si le mpu va bien
	bool_e mpu_is_ok;
	//Structure avec les données telle quelle du mpu
	MPU6050_t raw_data_mpu ;
	MPU6050_Result_t mpu_result;
	//Fréquence d'utilisation du mpu
	uint16_t frequency;

}DRONE_mpu6050_t;

void Mpu_imu_init(DRONE_mpu6050_t * angles, MPU6050_Accelerometer_t acc_sensi, MPU6050_Gyroscope_t gyro_sensi, double alpha, uint16_t frequency);
void Mpu_imu_update_angles(DRONE_mpu6050_t * angles);
bool_e Mpu_imu_calibrate(DRONE_mpu6050_t * angles, uint16_t epoch);	//Return true si c fini, faux si il est en train de calibrer


#endif /* COMPLEMENTARY_FILTER_H_ */
