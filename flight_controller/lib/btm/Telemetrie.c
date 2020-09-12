/*
 * telemetrie.c
 *
 *  Created on: 4 avr. 2020
 *      Author: Theo
 */

#include "Telemetrie.h"
#include "telem_2.h"

//


// ----------------- fonctions hors sub telemetrie -----------------------------
void TELEMETRIE_send_consigne_base(uint8_t consigne,  uart_id_e uart){
	uint8_t bytes[2] ;
	bytes[0] = ID_BASE_CONSIGNE_BASE ;
	bytes[1] = consigne ;
	//UART_puts_it(uart, bytes, 2);
	TELEM2_send_data(ID_BASE_CONSIGNE_BASE, &bytes[1], 1);
}

void TELEMETRIE_send_high_lvl_transi(uint8_t transi, uart_id_e uart){
	uint8_t bytes[2] ;
	bytes[0] = ID_PC_HIGH_LVL_TRANSITION ;
	bytes[1] = transi ;
	//UART_puts_it(uart, bytes, 2);
	TELEM2_send_data(ID_PC_HIGH_LVL_TRANSITION, &bytes[1], 1);
}

//Double
void TELEMETRIE_send_double(float value, uint8_t id,uart_id_e uart){
	uint8_t bytes[5] = {0};
	int32_t int_value = (int32_t)( value * (float) 1000000);
	bytes[0] = id ;
	bytes[1] = (uint8_t)((int_value >> 24) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_value >> 16) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_value >> 8) & 0b11111111) ;
	bytes[4] = (uint8_t)((int_value ) & 0b11111111) ;
	TELEM2_send_data(id, &bytes[1], 4);
	//UART_puts_it(uart, bytes, 5);
}
void TELEMETRIE_send_double_16b(float value, uint8_t id, uart_id_e uart){
	uint8_t  bytes[3] = {0} ;
	int16_t int_value =  (int16_t)( value * (double) 100);
	bytes[0] = id ;
	bytes[1] = (uint8_t)(int_value >> 8);
	bytes[2] = (uint8_t)(int_value & 0b11111111);
	TELEM2_send_data(id, &bytes[1], 2);
	//UART_puts_it(uart, bytes, 3);
}

// ------------------------- Fonctions sub telemetrie


//Altitude
void TELEMETRIE_send_altitude(State_drone_t * drone){
	TELEMETRIE_send_double((float)drone->capteurs.ms5611.altitude, ID_PC_ALTITUDE, drone->communication.uart_telem);
}
//Accélération
void TELEMETRIE_send_acc_z(State_drone_t * drone){
	TELEMETRIE_send_double(drone->capteurs.mpu.z_acc, ID_PC_ACC_Z, drone->communication.uart_telem);
}
//Données gps
void TELEMETRIE_send_lat(State_drone_t * drone){
	TELEMETRIE_send_double((float)drone->capteurs.gps.lat_degrees, ID_PC_LATTITUDE, drone->communication.uart_telem);
}
void TELEMETRIE_send_long(State_drone_t * drone){
	TELEMETRIE_send_double((float)drone->capteurs.gps.long_degrees, ID_PC_LONGITUDE, drone->communication.uart_telem);
}



//Pids
#if SET_COEF_ON_RATE_PID
void TELEMETRIE_send_pid_roll(State_drone_t * drone){
	TELEMETRIE_send_double(drone->stabilisation.pid_roll_rate.output, ID_PC_PID_ROLL, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_pitch(State_drone_t * drone){
	TELEMETRIE_send_double(drone->stabilisation.pid_pitch_rate.output, ID_PC_PID_PITCH, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_yaw(State_drone_t * drone){
	TELEMETRIE_send_double(drone->capteurs.mpu.y_gyro, ID_PC_PID_YAW, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_roll_p(State_drone_t * drone){
	TELEMETRIE_send_double(drone->stabilisation.pid_roll_rate.P, ID_PC_PID_P_ROLL, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_roll_d(State_drone_t * drone){
	TELEMETRIE_send_double(drone->stabilisation.pid_roll_rate.D, ID_PC_PID_D_ROLL, drone->communication.uart_telem);
}
#else
void TELEMETRIE_send_pid_roll(State_drone_t * drone){
	TELEMETRIE_send_double(drone->stabilisation.pid_roll.output, ID_PC_PID_ROLL, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_pitch(State_drone_t * drone){
	TELEMETRIE_send_double(drone->stabilisation.pid_pitch.output, ID_PC_PID_PITCH, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_yaw(State_drone_t * drone){
	TELEMETRIE_send_double(drone->stabilisation.pid_yaw.output, ID_PC_PID_YAW, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_roll_p(State_drone_t * drone){
	TELEMETRIE_send_double(drone->stabilisation.pid_roll.P, ID_PC_PID_P_ROLL, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_roll_d(State_drone_t * drone){
	TELEMETRIE_send_double(drone->stabilisation.pid_roll.D, ID_PC_PID_D_ROLL, drone->communication.uart_telem);
}
#endif

#if SET_COEF_ON_RATE_PID
void TELEMETRIE_send_pid_roll_kp(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_roll_rate.settings[PID_KP], ID_PC_ROLL_KP, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_roll_ki(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_roll_rate.settings[PID_KI], ID_PC_ROLL_KI, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_roll_kd(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_roll_rate.settings[PID_KD], ID_PC_ROLL_KD, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_pitch_kp(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_pitch_rate.settings[PID_KP], ID_PC_PITCH_KP, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_pitch_ki(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_pitch_rate.settings[PID_KI], ID_PC_PITCH_KI, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_pitch_kd(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_pitch_rate.settings[PID_KD], ID_PC_PITCH_KD, drone->communication.uart_telem);
}


#else
void TELEMETRIE_send_pid_roll_kp(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_roll.settings[PID_KP], ID_PC_ROLL_KP, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_roll_ki(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_roll.settings[PID_KI], ID_PC_ROLL_KI, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_roll_kd(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_roll.settings[PID_KD], ID_PC_ROLL_KD, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_pitch_kp(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_pitch.settings[PID_KP], ID_PC_PITCH_KP, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_pitch_ki(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_pitch.settings[PID_KI], ID_PC_PITCH_KI, drone->communication.uart_telem);
}
void TELEMETRIE_send_pid_pitch_kd(State_drone_t * drone){
	TELEMETRIE_send_double_16b(drone->stabilisation.pid_pitch.settings[PID_KD], ID_PC_PITCH_KD, drone->communication.uart_telem);
}

#endif

//Période task
void TELEMETRIE_send_periode_task(State_drone_t * drone){
	uint8_t bytes[4] = {0};
	uint32_t int_value = (uint32_t)( TASK_get_task(TASK_GYRO_FILTERING)->real_period_us);
	bytes[0] = ID_PC_TASK_PERIODE ;
	bytes[1] = (uint8_t)((int_value >> 16) & 0b11111111) ;
	bytes[2] = (uint8_t)((int_value >> 8) & 0b11111111) ;
	bytes[3] = (uint8_t)((int_value ) & 0b11111111) ;
	TELEM2_send_data(ID_PC_TASK_PERIODE, &bytes[1], 3);
	//UART_puts_it(drone->communication.uart_telem, bytes, 4);
}

//Cpu used
void TELEMETRIE_send_cpu_pourcentage(State_drone_t * drone){
	uint8_t bytes[2] = {0};
	bytes[0] = ID_PC_CPU_POURCENTAGE ;
	bytes[1] = (uint8_t)get_cpu_load() ;
	TELEM2_send_data(ID_PC_CPU_POURCENTAGE, &bytes[1], 1);
	//UART_puts_it(drone->communication.uart_telem, bytes, 2);
}

// Données des moteurs
void TELEMETRIE_send_moteur_all(State_drone_t * drone){
	uint8_t  bytes[4] = {0} ;
	bytes[0] = ID_PC_MOTEUR_ALL ;
	bytes[1] = (uint8_t)(((drone->stabilisation.escs_timer.Duty[ESC_1_CHANNEL] - 1000) / 4 )  & 0b11111111);
	bytes[2] = (uint8_t)(((drone->stabilisation.escs_timer.Duty[ESC_2_CHANNEL] - 1000) / 4 )  & 0b11111111);
	bytes[3] = (uint8_t)(((drone->stabilisation.escs_timer.Duty[ESC_3_CHANNEL] - 1000) / 4 )  & 0b11111111);
	bytes[4] = (uint8_t)(((drone->stabilisation.escs_timer.Duty[ESC_4_CHANNEL] - 1000) / 4 )  & 0b11111111);
	TELEM2_send_data(ID_PC_MOTEUR_ALL, &bytes[1], 4);
	//UART_puts_it(drone->communication.uart_telem, bytes, 5);
}


//	Données de la técomande
void TELEMETRIE_send_channel_all_1_4(State_drone_t * drone){
	uint8_t  bytes[5] = {0} ;
	bytes[0] = ID_PC_CHAN_1_4 ;
	bytes[1] = (uint8_t)(((drone->communication.ibus.channels[0] - 1000) / 4 )  & 0b11111111);
	bytes[2] = (uint8_t)(((drone->communication.ibus.channels[1] - 1000) / 4 )  & 0b11111111);
	bytes[3] = (uint8_t)(((drone->communication.ibus.channels[2] - 1000) / 4 )  & 0b11111111);
	bytes[4] = (uint8_t)(((drone->communication.ibus.channels[3] - 1000) / 4 )  & 0b11111111);
	TELEM2_send_data(ID_PC_CHAN_1_4, &bytes[1], 4);
	//UART_puts_it(drone->communication.uart_telem, bytes, 5);
}
void TELEMETRIE_send_channel_all_5_8(State_drone_t * drone){
	uint8_t  bytes[5] = {0} ;
	bytes[0] = ID_PC_CHAN_5_8 ;
	bytes[1] = (uint8_t)(((drone->communication.ibus.channels[4] - 1000) / 4 )  & 0b11111111);
	bytes[2] = (uint8_t)(((drone->communication.ibus.channels[5] - 1000) / 4 )  & 0b11111111);
	bytes[3] = (uint8_t)(((drone->communication.ibus.channels[6] - 1000) / 4 )  & 0b11111111);
	bytes[4] = (uint8_t)(((drone->communication.ibus.channels[7] - 1000) / 4 )  & 0b11111111);
	TELEM2_send_data(ID_PC_CHAN_5_8, &bytes[1], 4);
	//UART_puts_it(drone->communication.uart_telem, bytes, 5);
}



void TELEMETRIE_send_angle_x_y_as_int(State_drone_t * drone){
	uint8_t  bytes[3] = {0} ;
	bytes[0] = ID_PC_ANGLE_X_Y ;
	bytes[1] = (uint8_t)((int8_t)drone->capteurs.mpu.x);
	bytes[2] = (uint8_t)((int8_t)drone->capteurs.mpu.y);
	TELEM2_send_data(ID_PC_ANGLE_X_Y, &bytes[1], 2);
	//UART_puts_it(drone->communication.uart_telem, bytes, 3);
}
void TELEMETRIE_send_angle_x_y_z_rate_as_int(State_drone_t * drone){
	uint8_t  bytes[4] = {0} ;
	bytes[0] =  ID_PC_ANGLE_X_Y_Z_RATE ;
	bytes[1] = (uint8_t)((int8_t)drone->capteurs.mpu.x_gyro);
	bytes[2] = (uint8_t)((int8_t)drone->capteurs.mpu.y_gyro);
	bytes[3] = (uint8_t)((int8_t)drone->capteurs.mpu.z_gyro);
	TELEM2_send_data(ID_PC_ANGLE_X_Y_Z_RATE, &bytes[1], 3);
	//UART_puts_it(drone->communication.uart_telem, bytes, 4);
}
void TELEMETRIE_send_angle_z_as_int(State_drone_t * drone){
	uint8_t  bytes[3] = {0} ;
	int16_t yaw = (int16_t)drone->capteurs.mpu.z;
	bytes[0] = ID_PC_ANGLE_Z ;
	bytes[1] = (uint8_t)(yaw >> 8);
	bytes[2] = (uint8_t)(yaw & 0b11111111);
	TELEM2_send_data(ID_PC_ANGLE_Z, &bytes[1], 2);
	//UART_puts_it(drone->communication.uart_telem, bytes, 3);
}

void TELEMETRIE_send_angle_x_y_acc_as_int(State_drone_t * drone){
	uint8_t  bytes[3] = {0} ;
	bytes[0] = ID_PC_ANGLE_X_Y_ACC ;
	bytes[1] = (uint8_t)((int8_t)drone->capteurs.mpu.x_acc_angle);
	bytes[2] = (uint8_t)((int8_t)drone->capteurs.mpu.y_acc_angle);
	TELEM2_send_data(ID_PC_ANGLE_X_Y_ACC, &bytes[1], 2);
	//UART_puts_it(drone->communication.uart_telem, bytes, 3);
}


//Données state
void TELEMETRIE_send_state_flying(State_drone_t * drone){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = ID_PC_STATE_FLYING ;
	bytes[1] = drone->soft.state_flight_mode;
	TELEM2_send_data(ID_PC_STATE_FLYING, &bytes[1], 1);
	//UART_puts_it(drone->communication.uart_telem, bytes, 2);
}



//Donnée v_bat
void TELEMETRIE_send_v_bat(State_drone_t * drone){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = ID_PC_BATTERIE ;
	bytes[1] = (uint8_t)(drone->capteurs.batterie.voltage * 10);
	TELEM2_send_data(ID_PC_BATTERIE, &bytes[1], 1);
	//UART_puts_it(drone->communication.uart_telem, bytes, 2);
}

void TELEMETRIE_send_every_is_ok(State_drone_t * drone){
	uint8_t  bytes[2] = {0} ;
	bytes[0] = ID_PC_EVERY_IS_OK ;
	bytes[1] = (uint8_t)( 2 * drone->communication.ibus.is_ok +  drone->capteurs.gps.is_ok);
	TELEM2_send_data(ID_PC_EVERY_IS_OK, &bytes[1], 1);
	//UART_puts_it(drone->communication.uart_telem, bytes, 2);
}
