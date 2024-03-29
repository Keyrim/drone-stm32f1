/*
 * sub_send_data.c
 *
 *  Created on: 19 avr. 2020
 *      Author: Theo
 */

#include "sub_action.h"
#include "../lib_perso/telemetrie.h"

#define NB_OCTECT_MAX 20	//On envoit au plus 10 octect par loop
static bool_e first_call = TRUE ;



typedef struct{
	int32_t nb_octet;
	uint32_t periode;	//periode d'envoi par cycle ( 4 => un envoit tous les 4 cycles) et si on veut jamais que noter valeur soit envoy� on peut lui attribu� une valeur de -1
	bool_e to_send;
}Data_group;

Data_group angles ;
Data_group radio1 ;
Data_group radio2 ;
Data_group state_global ;
Data_group batterie	;
Data_group moteurs  ;
Data_group latitude ;
Data_group longitude ;
Data_group every_is_ok ;
Data_group acc_z ;

void sub_send_data(State_drone_t * drone){
	if(first_call){

		//On config l'uart

		//La prem�re fois on init les valeurs de p�riode et de taille pour chaque groupe de don�e

		//angle
		angles.nb_octet = 3 ;
		angles.periode	= 30 ; //10

		//Latitude
		latitude.nb_octet = 5 ;
		latitude.periode = 100000000 ; // 50

		//Longitude
		longitude.nb_octet = 5;
		longitude.periode = 100000000; //50

		//acc
		acc_z.nb_octet = 5 ;
		acc_z.periode = 50 ;

		//radios
		radio1.nb_octet = 5 ;
		radio2.nb_octet = 5;
		radio1.periode  = 50 ; // 50
		radio2.periode	= 50 ; // 50

		//state_global et v_bat
		state_global.nb_octet = 2 ;
		state_global.periode = 100 ; // 250
		batterie.nb_octet	= 2;
		batterie.periode = 250 ;  // 250

		//moteurs
		moteurs.nb_octet 	= 5 ;
		moteurs.periode		= 100 ;

		//Every is ok
		every_is_ok.nb_octet = 2 ;
		every_is_ok.periode = 250 ;


		first_call = FALSE;
	}

	static uint32_t compteur = 0 ;
	//On place les if dans l'ordre de priorit� d'envoit
	int32_t compteur_octet = NB_OCTECT_MAX ;

//	if(compteur % latitude.periode == 0 || latitude.to_send){
//		if(compteur_octet >= latitude.nb_octet){
//			TELEMETRIE_send_lat(drone., uart_telem);
//			compteur_octet -= latitude.nb_octet ;
//			latitude.to_send = FALSE ;
//		}
//		else
//			latitude.to_send = TRUE ;
//	}
//	if(compteur % longitude.periode == 0 || longitude.to_send){
//		if(compteur_octet >= longitude.nb_octet){
//			TELEMETRIE_send_long(longitude_, uart_telem);
//			compteur_octet -= longitude.nb_octet ;
//			longitude.to_send = FALSE ;
//		}
//		else
//			longitude.to_send = TRUE ;
//	}
//	if(compteur % acc_z.periode == 0 || acc_z.to_send){
//		if(compteur_octet >= acc_z.nb_octet){
//			TELEMETRIE_send_acc_z(acc_z_, uart_telem);
//			compteur_octet -= acc_z.nb_octet ;
//			acc_z.to_send = FALSE ;
//		}
//		else
//			acc_z.to_send = TRUE ;
//	}

	if(compteur % angles.periode == 0 || angles.to_send){
		if(compteur_octet >= angles.nb_octet){
			TELEMETRIE_send_angle_x_y_as_int(drone->mpu_angles.x, drone->mpu_angles.y, &drone->uart_telem);
			compteur_octet -= angles.nb_octet ;
			angles.to_send = FALSE ;
		}
		else
			angles.to_send = TRUE ;
	}
	if(compteur % moteurs.periode == 0 || moteurs.to_send ){
		if(compteur_octet >= moteurs.nb_octet){
			TELEMETRIE_send_moteur_all(drone->escs[0].pulsation, drone->escs[1].pulsation, drone->escs[2].pulsation, drone->escs[3].pulsation, &drone->uart_telem);
			compteur_octet -= moteurs.nb_octet ;
			moteurs.to_send = FALSE ;
		}
		else
			moteurs.to_send = TRUE;
	}
	if(compteur % radio1.periode == 0 || radio1.to_send ){
		if(compteur_octet >= radio1.nb_octet){
			TELEMETRIE_send_channel_all_1_4(drone->channels[0], drone->channels[1], drone->channels[2], drone->channels[3], &drone->uart_telem);
			compteur_octet -= radio1.nb_octet ;
			radio1.to_send = FALSE;
		}
		else
			radio1.to_send = TRUE ;
	}
	if(compteur % radio2.periode == 0 || radio2.to_send ){
		if(compteur_octet >= radio2.nb_octet){
			TELEMETRIE_send_channel_all_5_8(drone->channels[4], drone->channels[5], drone->channels[6], drone->channels[7], &drone->uart_telem);
			compteur_octet -= radio2.nb_octet ;
			radio2.to_send = FALSE ;
		}
		else
			radio2.to_send = TRUE;
	}
	if(compteur % batterie.periode == 0 || batterie.to_send ){
		if(compteur_octet >= batterie.nb_octet){
			TELEMETRIE_send_v_bat(drone->v_bat, &drone->uart_telem);
			compteur_octet -= batterie.nb_octet ;
			batterie.to_send = FALSE ;
		}
		else
			batterie.to_send = TRUE;
	}
	if(compteur % every_is_ok.periode == 0 || every_is_ok.to_send ){
			if(compteur_octet >= every_is_ok.nb_octet){
				TELEMETRIE_send_every_is_ok(((uint8_t)( 2 * drone->ppm_is_ok +  drone->gps_is_ok)), &drone->uart_telem);
				compteur_octet -= every_is_ok.nb_octet ;
				every_is_ok.to_send = FALSE ;
			}
			else
				every_is_ok.to_send = TRUE;
		}
	if(compteur % state_global.periode == 0 || state_global.to_send ){
		if(compteur_octet >= state_global.nb_octet){
			TELEMETRIE_send_state_flying(drone->state_flight_mode, &drone->uart_telem);
			compteur_octet -= state_global.nb_octet ;
			state_global.to_send = FALSE ;
		}
		else
			state_global.to_send = TRUE;
	}

	compteur ++ ;

}

















