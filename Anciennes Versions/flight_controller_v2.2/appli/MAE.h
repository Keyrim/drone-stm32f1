/*
 * MAE.h
 *
 *  Created on: 13 juin 2020
 *      Author: Théo
 */

#ifndef MAE_H_
#define MAE_H_
#include "settings.h"
#include "macro_types.h"

//High Level State machine
typedef enum{
	WAIT_LOOP,
	PWM_HIGH,
	UPDATE_ANGLES,
	VERIF_SYSTEM,
	PWM_LOW,
	ESCS_SETPOINTS,
	SEND_DATA,
	ERROR_HIGH_LEVEL
}Low_Level_SM;

//Flight mode state machine
typedef enum{
	ON_THE_GROUND,
	MANUAL,
	PARACHUTE,
	MANUAL_STYLEE,
	POSITION_HOLD,
	ALTITUDE_HOLD
}Flight_Mode_SM;



#endif /* MAE_H_ */
