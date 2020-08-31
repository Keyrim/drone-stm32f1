/*
 * DRONE_conisgne.h
 *
 *  Created on: 2 juil. 2020
 *      Author: Théo
 */

#ifndef LIB_PERSO_DRONE_CONSIGNE_H_
#define LIB_PERSO_DRONE_CONSIGNE_H_

typedef struct{
	//levelled mode
	float roll;
	float pitch;
	float yaw;
	//Accro mode
	float roll_rate;
	float pitch_rate ;
	float yaw_rate ;
	//Throttle
	float throttle;
}DRONE_consigne_t;

#endif /* LIB_PERSO_DRONE_CONISGNE_H_ */
