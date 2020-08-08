/*
 * telemetrie.c
 *
 *  Created on: 4 avr. 2020
 *      Author: Theo
 */

#include "Telemetrie.h"

#include "stm32f1_uart.h"
#include "macro_types.h"


void TELEMETRIE_send_consigne_base(uint8_t consigne,  uart_struct_e * uart){
	uint8_t bytes[2] ;
	bytes[0] = ID_BASE_CONSIGNE_BASE ;
	bytes[1] = consigne ;
	uart_add_few(uart, bytes, 2);
}

void TELEMETRIE_send_high_lvl_transi(uint8_t transi, uart_struct_e * uart){
	uint8_t bytes[2] ;
	bytes[0] = ID_PC_HIGH_LVL_TRANSITION ;
	bytes[1] = transi ;
	uart_add_few(uart, bytes, 2);
}


// Données des moteurs
void TELEMETRIE_send_moteur_all(int32_t m1, int32_t m2, int32_t m3, int32_t m4, uart_struct_e * uart){
	uint8_t  bytes[4] = {0} ;
	bytes[0] = ID_PC_MOTEUR_ALL ;
	bytes[1] = (uint8_t)(((m1 - 1000) / 4 )  & 0b11111111);
	bytes[2] = (uint8_t)(((m2 - 1000) / 4 )  & 0b11111111);
	bytes[3] = (uint8_t)(((m3 - 1000) / 4 )  & 0b11111111);
	bytes[4] = (uint8_t)(((m4 - 1000) / 4 )  & 0b11111111);
	uart_add_few(uart, bytes, 5);
}


//	Données de la técomande
void TELEMETRIE_send_channel_all_1_4(int32_t ch1, int32_t ch2, int32_t ch3, int32_t ch4, uart_struct_e * uart){
	uint8_t  bytes[5] = {0} ;
	bytes[0] = ID_PC_CHAN_1_4 ;
	bytes[1] = (uint8_t)(((ch1 - 1000) / 4 )  & 0b11111111);
	bytes[2] = (uint8_t)(((ch2 - 1000) / 4 )  & 0b11111111);
	bytes[3] = (uint8_t)(((ch3 - 1000) / 4 )  & 0b11111111);
	bytes[4] = (uint8_t)(((ch4 - 1000) / 4 )  & 0b11111111);
	uart_add_few(uart, bytes, 5);
}
void TELEMETRIE_send_channel_all_5_8(int32_t ch1, int32_t ch2, int32_t ch3, int32_t ch4, uart_struct_e * uart){
	uint8_t  bytes[5] = {0} ;
	bytes[0] = ID_PC_CHAN_5_8 ;
	bytes[1] = (uint8_t)(((ch1 - 1000) / 4 )  & 0b11111111);
	bytes[2] = (uint8_t)(((ch2 - 1000) / 4 )  & 0b11111111);
	bytes[3] = (uint8_t)(((ch3 - 1000) / 4 )  & 0b11111111);
	bytes[4] = (uint8_t)(((ch4 - 1000) / 4 )  & 0b11111111);
	uart_add_few(uart, bytes, 5);
}



void TELEMETRIE_send_angle_x_y_as_int(double x, double y, uart_struct_e * uart){
	uint8_t  bytes[3] = {0} ;
	bytes[0] = ID_PC_ANGLE_X_Y ;
	bytes[1] = (int8_t)x;
	bytes[2] = (int8_t)y;
	uart_add_few(uart, bytes, 3);
}
void TELEMETRIE_send_angle_z_as_int(double z, uart_struct_e * uart){
	uint8_t  bytes[3] = {0} ;
	int16_t yaw = (int16_t)z;
	bytes[0] = ID_PC_ANGLE_Z ;
	bytes[1] = (uint8_t)(yaw >> 8);
	bytes[2] = (uint8_t)(yaw & 0b11111111);
	uart_add_few(uart, bytes, 3);
}

void TELEMETRIE_send_angle_x_y_acc_as_int(double x, double y, uart_struct_e * uart){
	uint8_t  bytes[3] = {0} ;
	bytes[0] = ID_PC_ANGLE_X_Y_ACC ;
	bytes[1] = (uint8_t)x;
	bytes[2] = (uint8_t)y;
	uart_add_few(uart, bytes, 3);
}


//Données state
void TELEMETRIE_send_state_flying(uint8_t state, uart_struct_e * uart){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = ID_PC_STATE_FLYING ;
	bytes[1] = state;
	uart_add_few(uart, bytes, 2);
}

//Accélération
void TELEMETRIE_send_acc_z(double acc_z, uart_struct_e * uart){
	uint8_t bytes[5] = {0};
	int32_t int_ac = (int32_t)( acc_z * (double) 1000000);
	bytes[0] = ID_PC_ACC_Z ;
	bytes[1] = (uint8_t)((int_ac >> 24) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_ac >> 16) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_ac >> 8) & 0b11111111) ;
	bytes[4] = (uint8_t)((int_ac ) & 0b11111111) ;
	uart_add_few(uart, bytes, 5);
}

//Double
void TELEMETRIE_send_double(double value, uint8_t id, uart_struct_e * uart){
	uint8_t bytes[5] = {0};
	int32_t int_value = (int32_t)( value * (double) 1000000);
	bytes[0] = id ;
	bytes[1] = (uint8_t)((int_value >> 24) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_value >> 16) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_value >> 8) & 0b11111111) ;
	bytes[4] = (uint8_t)((int_value ) & 0b11111111) ;
	uart_add_few(uart, bytes, 5);
}

//Données gps
void TELEMETRIE_send_lat(double latitude, uart_struct_e * uart){
	uint8_t bytes[5] = {0};
	int32_t int_lat = (int32_t)( latitude * (double) 1000000);
	bytes[0] = ID_PC_LATTITUDE ;
	bytes[1] = (uint8_t)((int_lat >> 24) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_lat >> 16) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_lat >> 8) & 0b11111111) ;
	bytes[4] = (uint8_t)((int_lat ) & 0b11111111) ;
	uart_add_few(uart, bytes, 5);
}

void TELEMETRIE_send_long(double longitude, uart_struct_e * uart){
	uint8_t bytes[5] = {0};
	int32_t int_long = (int32_t)( longitude * (double) 1000000);
	bytes[0] = ID_PC_LONGITUDE ;
	bytes[1] = (uint8_t)((int_long >> 24) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_long >> 16) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_long >> 8) & 0b11111111) ;
	bytes[4] = (uint8_t)((int_long ) & 0b11111111) ;
	uart_add_few(uart, bytes, 5);
}

//Donnée v_bat
void TELEMETRIE_send_v_bat(double v_bat, uart_struct_e * uart){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = ID_PC_BATTERIE ;
	bytes[1] = (uint8_t)(v_bat * 10 );
	uart_add_few(uart, bytes, 2);
}

void TELEMETRIE_send_every_is_ok(uint8_t every_is_ok, uart_struct_e * uart){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = ID_PC_EVERY_IS_OK ;
	bytes[1] = every_is_ok;
	uart_add_few(uart, bytes, 2);
}
