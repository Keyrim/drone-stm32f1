/*
 * MAE.h
 *
 *  Created on: 13 juin 2020
 *      Author: Théo
 */

#ifndef MAE_H_
#define MAE_H_

//High Level State machine
typedef enum{
	WAIT_LOOP,
	PWM_HIGH,
	UPDATE_ANGLES,
	VERIF_SYSTEM,
	PWM_LOW,
	ESCS_SETPOINTS,
	SEND_DATA,
	ONCE_EVERY_10,
	ERROR_HIGH_LEVEL,
	END
}High_Level_SM;

typedef enum{
	TO_SEND_1,
	TO_SEND_2,
	TO_SEND_3,
	TO_SEND_4,
	TO_SEND_5,
	TO_SEND_6,
}DataSend_SM;

//Are we flying atm ?
typedef enum{
	ON_THE_GROUND,
	FLYING
}Flying_SM;

//Flight mode state machine
typedef enum{
	MANUAL,
	PARACHUTE,
	MANUAL_STYLEE,
	POSITION_HOLD,
	ALTITUDE_HOLD
}Flight_Mode_SM;

#endif /* MAE_H_ */
