/*
 * complementary_filter.c
 *
 *  Created on: 21 mars 2020
 *      Author: Theo
 */

#include "complementary_filter.h"
#include "systick.h"
#include "stdio.h"


static MPU6050_t* raw_data_mpu ;
static COMP_FILTER_angles_e* filtered_angles ;
static bool_e first_read = TRUE ;
double acc_sensi = 1 ;
double gyro_sensi = 1 ;
static double alpha = 1 ;
static uint16_t frequency = 1 ;

//Variables pour la calibration du mpu : acc et gyro
COMP_FILTER_angles_e acc_cal ;
COMP_FILTER_angles_e gyro_cal ;
#define NB_CALIB_MIN 10
static uint32_t nb_calib = 0 ;
static double gyro_x_sum = 0 ;
static double gyro_y_sum = 0 ;
static double gyro_z_sum = 0 ;

static double acc_x_sum = 0 ;
static double acc_y_sum = 0 ;





//Private functions
static double absolu(double x);


static double absolu(double x){
	if( x > 0)
		return x ;
	else
		return -x ;
}

//En cours de dev mais c est peut pas ouf voire de la grosse merde
//Plus simple de poser le drone au sol, de faire des mesures et de noté soit meme le décallage pcq là casse couille en vrai
bool_e COMP_FILTER_calibration(void){
	COMP_FILTER_angles_e acc_angles ;
	acc_angles.x = (double)raw_data_mpu->Accelerometer_X / acc_sensi ;
	acc_angles.y = (double)raw_data_mpu->Accelerometer_Y / acc_sensi ;
	acc_angles.z = (double)raw_data_mpu->Accelerometer_Z / acc_sensi ;
	double acc_total = sqrt((acc_angles.x * acc_angles.x ) + (acc_angles.y * acc_angles.y) + (acc_angles.z * acc_angles.z));
	//Si on peut pas faire de arcos on fait un return
	if(absolu(acc_angles.x) <= absolu(acc_total))
		acc_angles.x = - asin(acc_angles.x / acc_total ) * (double)57.32;
	else
		return FALSE ;
	if(absolu(acc_angles.y) <= absolu(acc_total))
		acc_angles.y = 	 asin(acc_angles.y / acc_total ) * (double)57.32;
	else
		return FALSE ;
	acc_x_sum += acc_angles.x ;
	acc_y_sum += acc_angles.y ;

	gyro_x_sum += raw_data_mpu->Gyroscope_X / (gyro_sensi * frequency) ;
	gyro_y_sum += raw_data_mpu->Gyroscope_Y / (gyro_sensi * frequency) ;
	gyro_z_sum += raw_data_mpu->Gyroscope_Z / (gyro_sensi * frequency) ;

	nb_calib ++ ;
	if(nb_calib > NB_CALIB_MIN){
		gyro_cal.x = gyro_x_sum / nb_calib ;
		gyro_cal.y = gyro_y_sum / nb_calib ;
		gyro_cal.z = gyro_z_sum / nb_calib ;

		acc_cal.x = acc_x_sum / nb_calib ;
		acc_cal.y = acc_x_sum / nb_calib ;

	}
	return TRUE ;
}

void COMP_FILTER_update_angles(void){

	COMP_FILTER_angles_e acc_angles ;
	filtered_angles->x_acc = (double)raw_data_mpu->Accelerometer_Z / acc_sensi ;
	filtered_angles->y_acc = (double)raw_data_mpu->Accelerometer_Y / acc_sensi ;
	filtered_angles->z_acc = (double)raw_data_mpu->Accelerometer_X / acc_sensi ;
	double acc_total = sqrt((filtered_angles->x_acc * filtered_angles->x_acc ) + (filtered_angles->y_acc * filtered_angles->y_acc) + (filtered_angles->z_acc * filtered_angles->z_acc));
	if(acc_total != 0  ){
		if(absolu(filtered_angles->x_acc) <= absolu(acc_total))
			acc_angles.x =   asin(filtered_angles->x_acc / acc_total ) * (double)57.32;
		if(absolu(filtered_angles->y_acc) <= absolu(acc_total))
			acc_angles.y = - asin(filtered_angles->y_acc / acc_total ) * (double)57.32;
	}

	//If it is the first reading we do, we initialize
	if(first_read){
		filtered_angles->x = acc_angles.x ;
		filtered_angles->y = acc_angles.y ;
		filtered_angles->z = 0 ;
		first_read = FALSE ;
	}
	//Otherwise, we use the gyro
	else{

		filtered_angles->x_gyro = (double)raw_data_mpu->Gyroscope_Z / ((double)frequency * gyro_sensi );
		filtered_angles->y_gyro = (double)raw_data_mpu->Gyroscope_Y / ((double)frequency * gyro_sensi );
		filtered_angles->z_gyro = (double)raw_data_mpu->Gyroscope_X / ((double)frequency * gyro_sensi );

		filtered_angles->x += filtered_angles->x_gyro - gyro_cal.x ;
		filtered_angles->y += filtered_angles->y_gyro - gyro_cal.y ;
		filtered_angles->z = (double)frequency * filtered_angles->z_gyro ;		//On intégre pas car pour le yaw on bosse en deg/sec

		//Pour prendre en compte le transfert d'angle quand je fais une rotation sur le yaw
		filtered_angles->x -= sin(filtered_angles->z_gyro * 0.017) * filtered_angles->y ;
		filtered_angles->y += sin(filtered_angles->z_gyro * 0.017) * filtered_angles->x ;

		//Complementary filter
		//acc_x used with gyY makes sense dw (it really does btw)
		//-2 et +8 car en posant au sol à plat j'ai 2 et -8
		filtered_angles->x = alpha * filtered_angles->x + (filtered_angles->y_acc ) * ((double)1 - alpha);
		filtered_angles->y = alpha * filtered_angles->y + (filtered_angles->x_acc - 2) * ((double)1 - alpha);
		//printf("Gx %f Ax \n", filtered_angles->x);

	}
}

void COMP_FILTER_init(MPU6050_t* mpu_data_struct, COMP_FILTER_angles_e* angles, MPU6050_Accelerometer_t acc, MPU6050_Gyroscope_t gyro, double alpha_, uint16_t frequency_){
	filtered_angles = angles ;
	alpha = alpha_ ;
	frequency = frequency_ ;
	raw_data_mpu = mpu_data_struct ;
	gyro_cal.x = 0;
	gyro_cal.y = 0 ;
	gyro_cal.z = 0 ;

	acc_cal.x = 0 ;
	acc_cal.y = 0 ;
	acc_cal.z = 0 ;
	switch(acc){
		case MPU6050_Accelerometer_16G :
			acc_sensi = MPU6050_ACCE_SENS_16 ;
			break;
		case MPU6050_Accelerometer_8G :
			acc_sensi = MPU6050_ACCE_SENS_8 ;
			break;
		case MPU6050_Accelerometer_4G :
			acc_sensi = MPU6050_ACCE_SENS_4 ;
			break;
		case MPU6050_Accelerometer_2G :
			acc_sensi = MPU6050_ACCE_SENS_2 ;
			break;
	}
	switch(gyro){
		case MPU6050_Gyroscope_2000s :
			gyro_sensi = MPU6050_GYRO_SENS_2000 ;
			break;
		case MPU6050_Gyroscope_1000s :
			gyro_sensi = MPU6050_GYRO_SENS_1000 ;
			break;
		case MPU6050_Gyroscope_500s :
			gyro_sensi = MPU6050_GYRO_SENS_500 ;
			break;
		case MPU6050_Gyroscope_250s :
			gyro_sensi = MPU6050_GYRO_SENS_250 ;
			break;
	}

}



