/*
 * complementary_filter.c
 *
 *  Created on: 21 mars 2020
 *      Author: Theo
 */

#include "complementary_filter.h"
#include "systick.h"
#include "stdio.h"





//Private functions
static double absolu(double x);
static double absolu(double x){
	if( x > 0)
		return x ;
	else
		return -x ;
}




void COMP_FILTER_update_angles(COMP_FILTER_angles_e* angles){

	COMP_FILTER_angles_e acc_angles ;
	angles->x_acc = (double)angles->raw_data_mpu->Accelerometer_Z / angles->acc_sensi ;
	angles->y_acc = (double)angles->raw_data_mpu->Accelerometer_Y / angles->acc_sensi ;
	angles->z_acc = (double)angles->raw_data_mpu->Accelerometer_X / angles->acc_sensi ;
	double acc_total = sqrt((angles->x_acc * angles->x_acc ) + (angles->y_acc * angles->y_acc) + (angles->z_acc * angles->z_acc));
	if(acc_total != 0  ){
		if(absolu(angles->x_acc) <= absolu(acc_total))
			acc_angles.x =   asin(angles->x_acc / acc_total ) * (double)57.32;
		if(absolu(angles->y_acc) <= absolu(acc_total))
			acc_angles.y = - asin(angles->y_acc / acc_total ) * (double)57.32;
	}

	//If it is the first reading we do, we initialize
	if(angles->first_read){
		angles->x = acc_angles.x ;
		angles->y = acc_angles.y ;
		angles->z = 0 ;
		angles->first_read = FALSE ;
	}
	//Otherwise, we use the gyro
	else{

		angles->x_gyro = (double)angles->raw_data_mpu->Gyroscope_Z / ((double)angles->frequency * angles->gyro_sensi );
		angles->y_gyro = (double)angles->raw_data_mpu->Gyroscope_Y / ((double)angles->frequency * angles->gyro_sensi );
		angles->z_gyro = (double)angles->raw_data_mpu->Gyroscope_X / ((double)angles->frequency * angles->gyro_sensi );

		angles->x += angles->x_gyro ;
		angles->y += angles->y_gyro  ;
		angles->z = (double)angles->frequency * angles->z_gyro ;		//On intégre pas car pour le yaw on bosse en deg/sec

		//Pour prendre en compte le transfert d'angle quand je fais une rotation sur le yaw
		angles->x -= sin(angles->z_gyro * 0.017) * angles->y ;
		angles->y += sin(angles->z_gyro * 0.017) * angles->x ;

		//Complementary filter
		//acc_x used with gyY makes sense dw (it really does btw)
		angles->x = angles->alpha * angles->x + (acc_angles.y ) * ((double)1 - angles->alpha);
		angles->y = angles->alpha * angles->y + (acc_angles.x ) * ((double)1 - angles->alpha);

	}
}

void COMP_FILTER_init(MPU6050_t* mpu_data_struct, COMP_FILTER_angles_e* angles, MPU6050_Accelerometer_t acc, MPU6050_Gyroscope_t gyro, double alpha_, uint16_t frequency_){

	angles->alpha = alpha_ ;
	angles->frequency = frequency_ ;
	angles->raw_data_mpu = mpu_data_struct ;
	switch(acc){
		case MPU6050_Accelerometer_16G :
			angles->acc_sensi = MPU6050_ACCE_SENS_16 ;
			break;
		case MPU6050_Accelerometer_8G :
			angles->acc_sensi = MPU6050_ACCE_SENS_8 ;
			break;
		case MPU6050_Accelerometer_4G :
			angles->acc_sensi = MPU6050_ACCE_SENS_4 ;
			break;
		case MPU6050_Accelerometer_2G :
			angles->acc_sensi = MPU6050_ACCE_SENS_2 ;
			break;
	}
	switch(gyro){
		case MPU6050_Gyroscope_2000s :
			angles->gyro_sensi = MPU6050_GYRO_SENS_2000 ;
			break;
		case MPU6050_Gyroscope_1000s :
			angles->gyro_sensi = MPU6050_GYRO_SENS_1000 ;
			break;
		case MPU6050_Gyroscope_500s :
			angles->gyro_sensi = MPU6050_GYRO_SENS_500 ;
			break;
		case MPU6050_Gyroscope_250s :
			angles->gyro_sensi = MPU6050_GYRO_SENS_250 ;
			break;
	}

}



