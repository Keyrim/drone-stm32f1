/*
 * regulation_filtrage.h
 *
 *  Created on: 20 oct. 2020
 *      Author: Theo
 */

#ifndef REGULATION_FILTRAGE_REGULATION_FILTRAGE_H_
#define REGULATION_FILTRAGE_REGULATION_FILTRAGE_H_

#include "system_d.h"

void REGU_FILTRAGE_pids_orientation_init(State_drone_t * drone);
void REGU_FILTRAGE_filters_imu_config(State_drone_t * drone);

#endif /* REGULATION_FILTRAGE_REGULATION_FILTRAGE_H_ */
