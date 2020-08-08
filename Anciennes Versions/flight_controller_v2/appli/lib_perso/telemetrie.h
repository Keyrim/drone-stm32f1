/*
 * telemetrie.h
 *
 *  Created on: 4 avr. 2020
 *      Author: Theo
 */

#ifndef TELEMETRIE_H_
#define TELEMETRIE_H_

#include "stdio.h"

//Pour envoyer des choses
//Premier octet annonce le type de donnée :
//		Les 2 premiers bit précise le type entre : un msg type str / un nombre / un msg d'erreur
//Si on envoit un nombre:
//		Les 6 bit restant annonce le nombre d'octect total envoyés
//		Puis on envoit le nombre dont il est question
//		Et on envoit sur le dernier octet l'id du nombre pour savoir à quoi il correspond
//Si c'est un msg
//		premier octect (char) avec l'id msg
//		On envoit les octects qui forment le string et on finit la chaine par un \n
//Si c'est une erreur
//		Premier octect avec l'id erreur(2 premiers bit°
//		6 octe

//Id des moteurs
#define MOTOR_1_ID	0
#define MOTOR_2_ID	1
#define MOTOR_3_ID	2
#define MOTOR_4_ID	3
#define MOTOR_ALL	12

//Id channels
#define CHAN_1_ID	4
#define CHAN_2_ID	5
#define CHAN_3_ID	6
#define CHAN_4_ID	7
#define CHAN_5_ID	8
#define CHAN_6_ID	9
#define CHAN_7_ID	10
#define CHAN_8_ID	11
#define CHAN_1_4    13
#define CHAN_5_8	14

// Others
#define ID_BAT		15
#define ID_STATE			16

//ANgles
#define ID_ANGLE_X			17
#define ID_ANGLE_Y			18
#define ID_ANGLE_X_Y		19
//Acc
#define ID_AcZ				24

//Gps
#define ID_LATITUDE			20
#define ID_LONGITUDE		21
#define ID_GPS				22

//states
#define ID_EVERY_IS_OK 		23






//Envoi moteurs
void TELEMETRIE_send_moteur_2b(int32_t int_16, uint8_t id_moteur);		//On envoit la valeur du signal moteur sur deux octects pour la conserver tel quelle
void TELEMETRIE_send_moteur_1b(int32_t int_32, uint8_t id_moteur);		//On envoi sur un octect donc par rapport à la valeur d'origine on fait - 1000 / 4 pour avoir un truc entre 0 et 250
void TELEMETRIE_send_moteur_all(int32_t m1, int32_t m2, int32_t m3, int32_t m4);	//On envoit tous les moteurs en une fois sur 1 octect chacuns

//Envoi channels
void TELEMETRIE_send_channel_1b(int32_t int_32, uint8_t id_channel);		//On envoi sur un octect donc par rapport à la valeur d'origine on fait - 1000 / 4 pour avoir un truc entre 0 et 250
void TELEMETRIE_send_channel_2b(int32_t int_32, uint8_t id_channel);		//On envoit sur 2 octects pour préserver la valeur d'origine (si besoin de debug par ex)
void TELEMETRIE_send_channel_all_1_4(int32_t ch1, int32_t ch2, int32_t ch3, int32_t ch4);	//On envoit tous les channels en une fois sur 1 octect chacuns
void TELEMETRIE_send_channel_all_5_8(int32_t ch1, int32_t ch2, int32_t ch3, int32_t ch4);	//On envoit tous les channels en une fois sur 1 octect chacuns

//Envoi angle
void TELEMETRIE_send_angle_as_int(double angle, uint8_t id_angle);
void TELEMETRIE_send_angle_x_y_as_int(double x, double y);

//Envoi acc
void TELEMETRIE_send_acc_z(double acc_z);

//Envoi state flying
void TELEMETRIE_send_state_flying(uint8_t state);

//Envoi v_bat
void TELEMETRIE_send_v_bat(double v_bat);

//ENVOI gps
void TELEMETRIE_send_lat(double latitude);
void TELEMETRIE_send_long(double longitude);
void TELEMETRIE_send_gps(double latitude, double longitude);

//Envoi every_is_ok
void TELEMETRIE_send_every_is_ok(uint8_t every_is_ok);


#endif /* TELEMETRIE_H_ */
