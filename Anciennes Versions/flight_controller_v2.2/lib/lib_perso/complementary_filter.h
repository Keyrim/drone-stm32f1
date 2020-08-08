/*
 * complementary_filer.h
 *
 *  Created on: 21 mars 2020
 *      Author: Theo
 */

#ifndef COMPLEMENTARY_FILTER_H_
#define COMPLEMENTARY_FILTER_H_

#include "stm32f1_mpu6050.h"
#include "math.h"

typedef struct{
	double x;
	double y;
	double z;
	double x_acc;
	double y_acc;
	double z_acc;
	double x_gyro;
	double y_gyro;
	double z_gyro;
	double alpha;
	double gyro_sensi;
	double acc_sensi;
	bool_e first_read;
	MPU6050_t* raw_data_mpu ;


	uint16_t frequency;
}COMP_FILTER_angles_e;

void COMP_FILTER_init(MPU6050_t * mpu_data_struct, COMP_FILTER_angles_e* angles, MPU6050_Accelerometer_t acc_sensi, MPU6050_Gyroscope_t gyro_sensi, double alpha_, uint16_t frequency);
void COMP_FILTER_update_angles(COMP_FILTER_angles_e* angles);


#endif /* COMPLEMENTARY_FILTER_H_ */
