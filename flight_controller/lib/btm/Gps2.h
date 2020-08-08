/*
 * gps.h
 *
 *  Created on: 1 févr. 2018
 *      Author: Nirgal
 */

#ifndef DRONE_GPS_H_
#define DRONE_GPS_H_

#include "stm32f1_uart.h"
#include "macro_types.h"

typedef struct{
	bool_e is_ok ;
	uint32_t last_time_read_gps ;
	uint16_t 	id;
	uint32_t 	time;		//[HHMMSS]
	uint32_t 	seconds;	//[sec since 0:00:00]
	double 		latitude_rad;		//rad
	double 		longitude_rad;		//rad
	double		latitude_deg;		//deg
	double		longitude_deg;		//deg
	int16_t		lat_degrees;	//Partie entière des degrés
	double		lat_minutes;	//Minutes d'angle, avec décimales
	int16_t		long_degrees;	//Partie entière des degrés
	double		long_minutes;	//Minutes d'angle, avec décimales
	bool_e		north;
	bool_e 		east;
	uint16_t	ground_speed;
	uint32_t 	date32;
	uint8_t checksum;
}gps_datas_t;

typedef enum{
	NO_TRAME_RECEIVED = 0,	//Une trame est en cours de réception
	CHECKSUM_INVALID,
	TRAME_INVALID,
	TRAME_UNKNOW,			//Une trame inconnue a été reçue
	TRAME_GPRMC,			//Une trame GPRMC a été reçue
	TRAME_GPGGA				//Une trame GPGGA a été reçue
}nmea_frame_e;

nmea_frame_e GPS_process_rx(uint8_t c, gps_datas_t * gps_datas);
void GPS_congif(uart_id_e id_uart);

#endif /* GPS_H_ */

