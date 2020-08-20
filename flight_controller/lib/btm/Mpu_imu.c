/*
 * DRONE_mpu6050.c
 *
 *  Created on: 21 mars 2020
 *      Author: Theo
 */

#include "Mpu_imu.h"

#include "systick.h"
#include "stdio.h"





//Fonction valeur absolu pour usage privé
static float absolu(float x);
static float absolu(float x){
	if( x > 0)
		return x ;
	else
		return -x ;
}

//Fonction qui permet d obtenir un angle entre -180 et 180
static float angle_180(float x);
static float angle_180(float x){
	if(x > 180)
		x -= 360;
	else if( x < -180)
		x += 360 ;
	return x ;
}

//corrige l'angle de l acc quand on a la tête à l envers
static float acc_correction(float x);
static float acc_correction(float x){
	x = ( x > 0) ? 180 - x : -180 -x ;
	return x ;
}

//La fonction doit être appelé autant de fois que l'on souhaite d'epoch
//Quand elle a été appelée epoch fois, elle renvoit vrai et met à jour les offset
bool_e Mpu_imu_calibrate(DRONE_mpu6050_t * angles, uint16_t epoch){
	static uint16_t e = 0 ;
	static float sum_ac_x = 0;
	static float sum_ac_y = 0;
	static float sum_ac_z = 0;
	static float sum_gyro_x = 0;
	static float sum_gyro_y = 0;
	static float sum_gyro_z = 0;

	bool_e to_return = 0 ;

	//Quand cette fonction est appelée, le drone est censé être à l'horizontal
	//on sauvegarde donc l'erreur qui servira comme offset plus tard
	sum_ac_x += angles->x_acc + angles->x_acc_offset ;
	sum_ac_y += angles->y_acc + angles->y_acc_offset ;
	sum_ac_z += angles->z_acc - 1 + angles->z_acc_offset ;
	sum_gyro_x += angles->x_gyro + angles->x_gyro_offset ;
	sum_gyro_y += angles->y_gyro + angles->y_gyro_offset ;
	sum_gyro_z += angles->z_gyro + angles->z_gyro_offset ;

	e ++ ;
	if(e == epoch){
		//On met à jour les offsets
		angles->x_acc_offset = sum_ac_x / (float)epoch ;
		angles->y_acc_offset = sum_ac_y / (float)epoch ;
		angles->z_acc_offset = sum_ac_z / (float)epoch ;
		angles->x_gyro_offset = sum_gyro_x / (float)epoch ;
		angles->y_gyro_offset = sum_gyro_y / (float)epoch ;
		angles->z_gyro_offset = sum_gyro_z / (float)epoch ;

		//remet tout à zéro en prévisions d'une réutilisation
		e=0;
		sum_ac_x = 0;
		sum_ac_y = 0;
		sum_ac_z = 0;
		sum_gyro_x = 0;
		sum_gyro_y = 0;
		sum_gyro_z = 0;

		//On renvoit vrai pour dire qu'on a fini
		to_return = 1;


	}
	return to_return ;
}

//On lit les données brutes de l'accèléromètre puis on les filtres pour récupèrer l'angle
void Mpu_imu_update_angles(DRONE_mpu6050_t * angles){
	//Mis à jour des donnée brute
	MPU6050_ReadAll(&angles->raw_data_mpu);

	//On déduit l'accélération sur chaques axes en tenant compte de la sensi choisie
	angles->x_acc = ((float)angles->raw_data_mpu.Accelerometer_X * angles->acc_sensi) - angles->x_acc_offset ;
	angles->y_acc = ((float)angles->raw_data_mpu.Accelerometer_Y * angles->acc_sensi) - angles->y_acc_offset ;
	angles->z_acc = ((float)angles->raw_data_mpu.Accelerometer_Z * angles->acc_sensi) - angles->z_acc_offset ;


	//On calcul l'accélération total
	float acc_total = sqrtf((angles->x_acc * angles->x_acc ) + (angles->y_acc * angles->y_acc) + (angles->z_acc * angles->z_acc));

	//On divise par l'accélération plus tard donc pour éviter une divisions par zéro ..
	if(acc_total != 0  ){
		//On veut pas de asin (x) avec x >1
		if(absolu(angles->x_acc) <= absolu(acc_total)){
			angles->x_acc_angle = - asinf(angles->x_acc / acc_total ) * (float)57.32;
		}

		//On veut pas de asin (y) avec y >1
		if(absolu(angles->y_acc) <= absolu(acc_total)){
			angles->y_acc_angle =   asinf(angles->y_acc / acc_total ) * (float)57.32;
		}

		if(angles->z_acc < 0){
			angles->x_acc_angle = acc_correction(angles->x_acc_angle);
			angles->y_acc_angle = acc_correction(angles->y_acc_angle);
		}
	}


	//Pour la première lecture on utilise juste l'accéléromètre pour déterminer l'angle de départ
	if(angles->first_read){
		angles->x = angles->x_acc_angle ;
		angles->y = angles->y_acc_angle ;
		angles->z = 0 ;
		angles->first_read = FALSE ;
	}
	else{
		//On récupère la vitesse angulaire
		angles->x_gyro = ((float)angles->raw_data_mpu.Gyroscope_X * angles->gyro_sensi) - angles->x_gyro_offset;
		angles->y_gyro = ((float)angles->raw_data_mpu.Gyroscope_Y * angles->gyro_sensi) - angles->y_gyro_offset;
		angles->z_gyro = ((float)angles->raw_data_mpu.Gyroscope_Z * angles->gyro_sensi) - angles->z_gyro_offset;

		//On intègre la vitesse sur les angles
		angles->x += angles->x_gyro * angles->periode;
		angles->y += angles->y_gyro * angles->periode;
		angles->z += angles->z_gyro * angles->periode;



		//On prend en compte le transfert d'angle causé par le yaw (axe vertical)
		angles->x += sinf(angles->z_gyro * 0.017f * angles->periode)  * angles->y ;
		angles->y -= sinf(angles->z_gyro * 0.017f * angles->periode) * angles->x ;

		//On vérifie qu'on reste dans l'interval -180, 180
		angles->x = angle_180(angles->x);
		angles->y = angle_180(angles->y);
		angles->z = angle_180(angles->z);

		//Complementary filter
		//acc_x avec gyro_y c'est normal
		angles->x = angles->alpha * angles->x + (angles->y_acc_angle ) * ((float)1 - angles->alpha);
		angles->y = angles->alpha * angles->y + (angles->x_acc_angle ) * ((float)1 - angles->alpha);

	}
}

//
void Mpu_imu_init(DRONE_mpu6050_t * angles, MPU6050_Accelerometer_t acc, MPU6050_Gyroscope_t gyro, float alpha, int32_t frequency){
	//Init du mpu
	angles->mpu_result =  MPU6050_Init(&angles->raw_data_mpu, NULL, GPIO_PIN_12, MPU6050_Device_0,acc, gyro);

	angles->alpha = alpha ;
	angles->frequency = frequency ;
	angles->periode = (float)1 / (float)frequency ;
	angles->x_gyro_offset = -2.77471852f ;
	angles->y_gyro_offset = 0.390320688f ;
	angles->z_gyro_offset = 0.586473227f ;
	angles->x_acc_offset = 0.00647851545f ;
	angles->y_acc_offset = -0.0312011726f ;
	angles->z_acc_offset = -0.0675112307f ;


	switch(acc){
		case MPU6050_Accelerometer_16G :
			angles->acc_sensi = (float)1 / MPU6050_ACCE_SENS_16 ;
			break;
		case MPU6050_Accelerometer_8G :
			angles->acc_sensi = (float)1 / MPU6050_ACCE_SENS_8 ;
			break;
		case MPU6050_Accelerometer_4G :
			angles->acc_sensi = (float)1 / MPU6050_ACCE_SENS_4 ;
			break;
		case MPU6050_Accelerometer_2G :
			angles->acc_sensi = (float)1 / MPU6050_ACCE_SENS_2 ;
			break;
	}
	switch(gyro){
		case MPU6050_Gyroscope_2000s :
			angles->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_2000 ;
			break;
		case MPU6050_Gyroscope_1000s :
			angles->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_1000 ;
			break;
		case MPU6050_Gyroscope_500s :
			angles->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_500 ;
			break;
		case MPU6050_Gyroscope_250s :
			angles->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_250 ;
			break;
	}
}



