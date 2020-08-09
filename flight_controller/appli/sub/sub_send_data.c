/*
 * sub_send_data.c
 *
 *  Created on: 19 avr. 2020
 *      Author: Theo
 */

#include "sub_action.h"

#include "../../lib/btm/Telemetrie.h"

#define NB_OCTECT_MAX 20	//On envoit au plus 20 octects par loop
static bool_e initialialized = FALSE ;



typedef struct{
	int32_t nb_octet;
	uint32_t periode;	//periode d'envoi par cycle ( 4 => un envoit tous les 4 cycles)
	bool_e to_send;
}Data_group;

Data_group angles ;
Data_group angle_z ;
Data_group radio1 ;
Data_group radio2 ;
Data_group state_global ;
Data_group batterie	;
Data_group moteurs  ;
Data_group latitude ;
Data_group longitude ;
Data_group every_is_ok ;
Data_group acc_z ;
Data_group angles_from_acc;
Data_group pid_roll ;
Data_group pid_pitch ;
Data_group pid_yaw ;
Data_group pid_roll_d ;
Data_group pid_roll_p ;
Data_group altitude ;



//Sub qui envoit des données par télémétrie à qui veut l'entendre ^^
void sub_send_data(State_drone_t * drone){
	if(!initialialized){

		//PIDS
		pid_roll.nb_octet = 5 ;
		pid_pitch.nb_octet = 5 ;
		pid_yaw.nb_octet = 50 ;
		pid_roll_d.nb_octet = 5 ;
		pid_roll_p.nb_octet = 5 ;

		pid_roll.periode =300000 ;
		pid_pitch.periode = 300000 ;
		pid_yaw.periode = 50000;
		pid_roll_d.periode = 20000000 ;
		pid_roll_p.periode = 20000000 ;

		//Altitude
		altitude.nb_octet = 5 ;
		altitude.periode =  3000 ;

		//angle
		angles.nb_octet = 3 ;
		angles.periode	= 4 ; //10
		angle_z.nb_octet = 3 ;
		angle_z.periode	= 10000 ; //10
		angles_from_acc.nb_octet = 3 ;
		angles_from_acc.periode = 10000 ;

		//Latitude
		latitude.nb_octet = 5 ;
		latitude.periode = 100000000 ; // 50

		//Longitude
		longitude.nb_octet = 5;
		longitude.periode = 100000000; //50

		//acc
		acc_z.nb_octet = 5 ;
		acc_z.periode = 4 ;

		//radios
		radio1.nb_octet = 5 ;
		radio2.nb_octet = 20;
		radio1.periode  = 5 ; // 50
		radio2.periode	= 50 ; // 50

		//state_global et v_bat
		state_global.nb_octet = 2 ;
		state_global.periode = 30 ; // 250
		batterie.nb_octet	= 2;
		batterie.periode = 500 ;  // 250

		//moteurs
		moteurs.nb_octet 	= 5 ;
		moteurs.periode		= 30 ;

		//Every is ok
		every_is_ok.nb_octet = 2 ;
		every_is_ok.periode = 30 ;


		initialialized = TRUE;
	}

	static uint32_t compteur = 0 ;
	//On place les if dans l'ordre de priorité d'envoit
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


	if(compteur % altitude.periode == 0 || altitude.to_send){
		if(compteur_octet >= altitude.nb_octet){
			TELEMETRIE_send_double(drone->capteurs.ms5611.altitude, ID_PC_ALTITUDE, &drone->communication.uart_telem);
			compteur_octet -= altitude.nb_octet ;
			altitude.to_send = FALSE ;
		}
		else
			altitude.to_send = TRUE ;
	}

	if(compteur % pid_roll.periode == 0 || pid_roll.to_send){
		if(compteur_octet >= pid_roll.nb_octet){
			TELEMETRIE_send_double(drone->stabilisation.pid_roll_rate.output, ID_PC_PID_ROLL, &drone->communication.uart_telem);
			compteur_octet -= pid_roll.nb_octet ;
			pid_roll.to_send = FALSE ;
		}
		else
			pid_roll.to_send = TRUE ;
	}
	if(compteur % pid_pitch.periode == 0 || pid_pitch.to_send){
		if(compteur_octet >= pid_pitch.nb_octet){
			TELEMETRIE_send_double(drone->stabilisation.pid_pitch_rate.output, ID_PC_PID_PITCH, &drone->communication.uart_telem);
			compteur_octet -= pid_pitch.nb_octet ;
			pid_pitch.to_send = FALSE ;
		}
		else
			pid_pitch.to_send = TRUE ;
	}
	if(compteur % pid_roll_d.periode == 0 || pid_roll_d.to_send){
		if(compteur_octet >= pid_roll_d.nb_octet){
			TELEMETRIE_send_double(drone->stabilisation.pid_roll.D_filtered[0], ID_PC_PID_D_ROLL, &drone->communication.uart_telem);
			compteur_octet -= pid_roll_d.nb_octet ;
			pid_roll_d.to_send = FALSE ;
		}
		else
			pid_pitch.to_send = TRUE ;
	}
	if(compteur % pid_roll_p.periode == 0 || pid_roll_p.to_send){
		if(compteur_octet >= pid_roll_p.nb_octet){
			TELEMETRIE_send_double(drone->stabilisation.pid_roll.P, ID_PC_PID_P_ROLL, &drone->communication.uart_telem);
			compteur_octet -= pid_roll_p.nb_octet ;
			pid_roll_p.to_send = FALSE ;
		}
		else
			pid_roll_p.to_send = TRUE ;
	}
	if(compteur % pid_yaw.periode == 0 || pid_yaw.to_send){
		if(compteur_octet >= pid_yaw.nb_octet){
			TELEMETRIE_send_double(drone->stabilisation.pid_yaw.output, ID_PC_PID_YAW, &drone->communication.uart_telem);
			compteur_octet -= pid_yaw.nb_octet ;
			pid_yaw.to_send = FALSE ;
		}
		else
			pid_yaw.to_send = TRUE ;
	}
	if(compteur % angles.periode == 0 || angles.to_send){
			if(compteur_octet >= angles.nb_octet){
				TELEMETRIE_send_angle_x_y_as_int(drone->capteurs.mpu.x, drone->capteurs.mpu.y, &drone->communication.uart_telem);
				compteur_octet -= angles.nb_octet ;
				angles.to_send = FALSE ;
			}
			else
				angles.to_send = TRUE ;
		}
	if(compteur % angle_z.periode == 0 || angle_z.to_send){
			if(compteur_octet >= angle_z.nb_octet){
				TELEMETRIE_send_angle_z_as_int(drone->capteurs.mpu.z, &drone->communication.uart_telem);
				compteur_octet -= angle_z.nb_octet ;
				angle_z.to_send = FALSE ;
			}
			else
				angle_z.to_send = TRUE ;
		}
	if(compteur % angles_from_acc.periode == 0 || angles_from_acc.to_send){
			if(compteur_octet >= angles_from_acc.nb_octet){
				TELEMETRIE_send_angle_x_y_acc_as_int(drone->capteurs.mpu.x_acc_angle, drone->capteurs.mpu.y_acc_angle, &drone->communication.uart_telem);
				compteur_octet -= angles_from_acc.nb_octet ;
				angles_from_acc.to_send = FALSE ;
			}
			else
				angles_from_acc.to_send = TRUE ;
		}
	if(compteur % moteurs.periode == 0 || moteurs.to_send ){
		if(compteur_octet >= moteurs.nb_octet){
			TELEMETRIE_send_moteur_all(drone->stabilisation.escs[0].pulsation, drone->stabilisation.escs[1].pulsation, drone->stabilisation.escs[2].pulsation, drone->stabilisation.escs[3].pulsation, &drone->communication.uart_telem);
			compteur_octet -= moteurs.nb_octet ;
			moteurs.to_send = FALSE ;
		}
		else
			moteurs.to_send = TRUE;
	}
	if(compteur % radio1.periode == 0 || radio1.to_send ){
		if(compteur_octet >= radio1.nb_octet){
			TELEMETRIE_send_channel_all_1_4(drone->communication.ibus.channels[0], drone->communication.ibus.channels[1], drone->communication.ibus.channels[2], drone->communication.ibus.channels[3], &drone->communication.uart_telem);
			compteur_octet -= radio1.nb_octet ;
			radio1.to_send = FALSE;
		}
		else
			radio1.to_send = TRUE ;
	}
	if(compteur % radio2.periode == 0 || radio2.to_send ){
		if(compteur_octet >= radio2.nb_octet){
			TELEMETRIE_send_channel_all_5_8(drone->communication.ibus.channels[4], drone->communication.ibus.channels[5], drone->communication.ibus.channels[6], drone->communication.ibus.channels[7], &drone->communication.uart_telem);
			compteur_octet -= radio2.nb_octet ;
			radio2.to_send = FALSE ;
		}
		else
			radio2.to_send = TRUE;
	}
	if(compteur % batterie.periode == 0 || batterie.to_send ){
		if(compteur_octet >= batterie.nb_octet){
			TELEMETRIE_send_v_bat(drone->capteurs.batterie.voltage, &drone->communication.uart_telem);
			compteur_octet -= batterie.nb_octet ;
			batterie.to_send = FALSE ;
		}
		else
			batterie.to_send = TRUE;
	}
	if(compteur % every_is_ok.periode == 0 || every_is_ok.to_send ){
			if(compteur_octet >= every_is_ok.nb_octet){
				TELEMETRIE_send_every_is_ok(((uint8_t)( 2 * drone->communication.ibus.is_ok +  drone->capteurs.gps.is_ok)), &drone->communication.uart_telem);
				compteur_octet -= every_is_ok.nb_octet ;
				every_is_ok.to_send = FALSE ;
			}
			else
				every_is_ok.to_send = TRUE;
		}
	if(compteur % state_global.periode == 0 || state_global.to_send ){
		if(compteur_octet >= state_global.nb_octet){
			TELEMETRIE_send_state_flying(drone->soft.state_flight_mode, &drone->communication.uart_telem);
			compteur_octet -= state_global.nb_octet ;
			state_global.to_send = FALSE ;
		}
		else
			state_global.to_send = TRUE;
	}

	compteur ++ ;

}

















