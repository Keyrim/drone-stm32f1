/*
 * telemetrie.c
 *
 *  Created on: 4 avr. 2020
 *      Author: Theo
 */

#include "telemetrie.h"

#include "stm32f1_uart.h"
#include "macro_types.h"





// Données des moteurs
void TELEMETRIE_send_moteur_all(int32_t m1, int32_t m2, int32_t m3, int32_t m4){
	uint8_t  bytes[4] = {0} ;
	bytes[0] = MOTOR_ALL ;
	bytes[1] = (uint8_t)(((m1 - 1000) / 4 )  & 0b11111111);
	bytes[2] = (uint8_t)(((m2 - 1000) / 4 )  & 0b11111111);
	bytes[3] = (uint8_t)(((m3 - 1000) / 4 )  & 0b11111111);
	bytes[4] = (uint8_t)(((m4 - 1000) / 4 )  & 0b11111111);
	UART_puts(UART3_ID, bytes, 5);
}
void TELEMETRIE_send_moteur_2b(int32_t int_16, uint8_t id_moteur){
	uint8_t  bytes[3] = {0} ;
	bytes[0] = id_moteur ;
	bytes[1] = (uint8_t)((int_16 >> 8 )  & 0b11111111);
	bytes[2] = (uint8_t)((int_16 	  )  & 0b11111111);
	UART_puts(UART3_ID, bytes, 3);
}
void TELEMETRIE_send_moteur_1b(int32_t int_16, uint8_t id_moteur){
	int_16 -= 1000 ;
	int_16 /= 4 ;
	uint8_t  bytes[2] = {0} ;
	bytes[0] = id_moteur ;
	bytes[1] = (uint8_t)(int_16);
	UART_puts(UART3_ID, bytes, 2);
}

//	Données de la técomande
void TELEMETRIE_send_channel_all_1_4(int32_t ch1, int32_t ch2, int32_t ch3, int32_t ch4){
	uint8_t  bytes[5] = {0} ;
	bytes[0] = CHAN_1_4 ;
	bytes[1] = (uint8_t)(((ch1 - 1000) / 4 )  & 0b11111111);
	bytes[2] = (uint8_t)(((ch2 - 1000) / 4 )  & 0b11111111);
	bytes[3] = (uint8_t)(((ch3 - 1000) / 4 )  & 0b11111111);
	bytes[4] = (uint8_t)(((ch4 - 1000) / 4 )  & 0b11111111);
	UART_puts(UART3_ID, bytes, 5);
}
void TELEMETRIE_send_channel_all_5_8(int32_t ch1, int32_t ch2, int32_t ch3, int32_t ch4){
	uint8_t  bytes[5] = {0} ;
	bytes[0] = CHAN_5_8 ;
	bytes[1] = (uint8_t)(((ch1 - 1000) / 4 )  & 0b11111111);
	bytes[2] = (uint8_t)(((ch2 - 1000) / 4 )  & 0b11111111);
	bytes[3] = (uint8_t)(((ch3 - 1000) / 4 )  & 0b11111111);
	bytes[4] = (uint8_t)(((ch4 - 1000) / 4 )  & 0b11111111);
	UART_puts(UART3_ID, bytes, 5);
}


//Donées angles
void TELEMETRIE_send_angle_as_int(double angle, uint8_t id_angle){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = id_angle ;
	bytes[1] = (int8_t)angle;
	UART_puts(UART3_ID, bytes, 2);
}
void TELEMETRIE_send_angle_x_y_as_int(double x, double y){
	uint8_t  bytes[3] = {0} ;
	bytes[0] = ID_ANGLE_X_Y ;
	bytes[1] = (int8_t)x;
	bytes[2] = (int8_t)y;
	UART_puts(UART3_ID, bytes, 3);
}


//Données state
void TELEMETRIE_send_state_flying(uint8_t state){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = ID_STATE ;
	bytes[1] = state;
	UART_puts(UART3_ID, bytes, 2);
}

//Accélération
void TELEMETRIE_send_acc_z(double acc_z){
	uint8_t bytes[5] = {0};
	int32_t int_ac = (int32_t)( acc_z * (double) 1000000);
	bytes[0] = ID_AcZ ;
	bytes[1] = (uint8_t)((int_ac >> 24) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_ac >> 16) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_ac >> 8) & 0b11111111) ;
	bytes[4] = (uint8_t)((int_ac ) & 0b11111111) ;
	UART_puts(UART3_ID, bytes, 5);
}

//Données gps
void TELEMETRIE_send_lat(double latitude){
	uint8_t bytes[5] = {0};
	int32_t int_lat = (int32_t)( latitude * (double) 1000000);
	bytes[0] = ID_LATITUDE ;
	bytes[1] = (uint8_t)((int_lat >> 24) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_lat >> 16) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_lat >> 8) & 0b11111111) ;
	bytes[4] = (uint8_t)((int_lat ) & 0b11111111) ;
	UART_puts(UART3_ID, bytes, 5);
}

void TELEMETRIE_send_long(double longitude){
	uint8_t bytes[5] = {0};
	int32_t int_long = (int32_t)( longitude * (double) 1000000);
	bytes[0] = ID_LONGITUDE ;
	bytes[1] = (uint8_t)((int_long >> 24) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_long >> 16) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_long >> 8) & 0b11111111) ;
	bytes[4] = (uint8_t)((int_long ) & 0b11111111) ;
	UART_puts(UART3_ID, bytes, 5);
}

//Donnée v_bat
void TELEMETRIE_send_v_bat(double v_bat){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = ID_BAT ;
	bytes[1] = (uint8_t)(v_bat * 10 );
	UART_puts(UART3_ID, bytes, 2);
}

void TELEMETRIE_send_every_is_ok(uint8_t every_is_ok){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = ID_EVERY_IS_OK ;
	bytes[1] = every_is_ok;
	UART_puts(UART3_ID, bytes, 2);
}
