/*
 * state_machine.h
 *
 *  Created on: 30 avr. 2020
 *      Author: Theo
 */

#ifndef LIB_PERSO_STATE_MACHINE_H_
#define LIB_PERSO_STATE_MACHINE_H_
#include "macro_types.h"


uint8_t try_action(running_e result, uint8_t in_progress, uint8_t sucess, uint8_t fail);

#endif /* LIB_PERSO_STATE_MACHINE_H_ */
