/*
 * system_d.h
 *
 *  Created on: 29 juin 2020
 *      Author: Théo
 */





#ifndef SYSTEM_D_H_
#define SYSTEM_D_H_

#include "settings.h"
#include "macro_types.h"


#include "../lib/btm/Batterie.h"
#include "../lib/btm/Mpu_imu.h"
#include "../lib/btm/Sequence_led.h"
#include "../lib/btm/Filters.h"


#include "telemetrie/telemetrie.h"

//Structure capteurs
typedef struct{
	DRONE_batterie_t batterie ;
	DRONE_mpu6050_t mpu ;
}GROUND_STATION_capteurs_t;



//Structure communication
typedef struct{
	telemetrie_t telemetrie_drone ;
	telemetrie_t telemetrie_pc ;
}GROUND_STATION_communication_t;



//Structure de l'ihm
typedef struct{
	sequence_led_t led_etat ;
}GROUND_STATION_ihm_t;

//Structure principale du drone
typedef struct{

	GROUND_STATION_capteurs_t capteurs ;
	GROUND_STATION_communication_t communication ;
	GROUND_STATION_ihm_t ihm ;

}State_ground_station_t;


#endif /* SYSTEM_D_H_ */
