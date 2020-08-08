/*
 * IDs.h
 *
 *  Created on: 15 juin 2020
 *      Author: Théo
 */

#ifndef IDS_H_
#define IDS_H_

//Def de tous ids pour la télémétrie

#define ID_NOMBRE_ID 							13

//Id pour le drone
#define ID_DRONE_CONSIGNE 						0

#define ID_DRONE_PITCH_ROLL_BASE				20

//Id pour le pc
#define ID_PC_BATTERIE 							100
#define ID_PC_STATE_FLYING 						101
#define ID_PC_EVERY_IS_OK						102

#define ID_PC_ANGLE_X_Y							120
#define ID_PC_X_Y_BASE							121

#define	ID_PC_MOTEUR_ALL 						160
#define	ID_PC_CHAN_1_4							161
#define	ID_PC_CHAN_5_8							162
#define	ID_PC_LONGITUDE							163
#define	ID_PC_LATTITUDE							164
#define	ID_PC_ACC_Z								165

//Ids  pour la base
#define ID_BASE_CONSIGNE_BASE 					200


//sub ids drone consigne
#define SUB_ID_DRONE_CONSIGNE_STOP_MOTEUR 			0

//sub ids base consigne
#define SUB_ID_BASE_CONSIGNE_START_SENDING_ANGLES 	0
#define	SUB_ID_CONSIGNE_BASE_STOP_SENDING_ANGLES	1

//uint8_t liste_id[] = {0, 20, 100, 101, 102, 120, 160, 161, 162, 163, 164, 165, 200};
#endif /* IDS_H_ */
