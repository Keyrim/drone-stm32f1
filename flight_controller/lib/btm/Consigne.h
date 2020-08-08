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
	double roll;
	double pitch;
	double yaw;
	//Accro mode
	double roll_rate;
	double pitch_rate ;
	double yaw_rate ;
	//Throttle
	double throttle;
}DRONE_consigne_t;

#endif /* LIB_PERSO_DRONE_CONISGNE_H_ */
