/*
 * telemetrie.h
 *
 *  Created on: 4 avr. 2020
 *      Author: Theo
 */

#ifndef TELEMETRIE_H_
#define TELEMETRIE_H_

#include "stdio.h"
#include "../../appli/IDs.h"

#include "Uart_lib.h"
//voir le fichier adressage com drone pour les détails
//Ainsi que IDs.h




//Consigne base
void TELEMETRIE_send_consigne_base(uint8_t consigne,  uart_struct_e * uart);

//High transition state
void TELEMETRIE_send_high_lvl_transi(uint8_t transi, uart_struct_e * uart);

//Envoi moteurs
void TELEMETRIE_send_moteur_all(int32_t m1, int32_t m2, int32_t m3, int32_t m4, uart_struct_e * uart);	//On envoit tous les moteurs en une fois sur 1 octect chacuns

//Envoi channels
void TELEMETRIE_send_channel_all_1_4(int32_t ch1, int32_t ch2, int32_t ch3, int32_t ch4, uart_struct_e * uart);	//On envoit tous les channels en une fois sur 1 octect chacuns
void TELEMETRIE_send_channel_all_5_8(int32_t ch1, int32_t ch2, int32_t ch3, int32_t ch4, uart_struct_e * uart);	//On envoit tous les channels en une fois sur 1 octect chacuns

//Envoi angle
void TELEMETRIE_send_angle_x_y_as_int(double x, double y, uart_struct_e * uart);
void TELEMETRIE_send_angle_z_as_int(double z, uart_struct_e * uart);
//Envoi angle de l acc
void TELEMETRIE_send_angle_x_y_acc_as_int(double x, double y, uart_struct_e * uart);

//Envoi acc
void TELEMETRIE_send_acc_z(double acc_z, uart_struct_e * uart);

//Envoit double
void TELEMETRIE_send_double(double value, uint8_t id, uart_struct_e * uart);
//Envoi state flying
void TELEMETRIE_send_state_flying(uint8_t state, uart_struct_e * uart);

//Envoi v_bat
void TELEMETRIE_send_v_bat(double v_bat, uart_struct_e * uart);


//ENVOI gps
void TELEMETRIE_send_lat(double latitude, uart_struct_e * uart);
void TELEMETRIE_send_long(double longitude, uart_struct_e * uart);

//Envoi every_is_ok
void TELEMETRIE_send_every_is_ok(uint8_t every_is_ok, uart_struct_e * uart);


#endif /* TELEMETRIE_H_ */
