/*
 * filters.h
 *
 *  Created on: 20 août 2020
 *      Author: Théo
 */

#ifndef BTM_FILTERS_H_
#define BTM_FILTERS_H_

#include "macro_types.h"

//Second order filter struct
typedef struct{
	float settings[3];
	float values[3];
}Filter_second_order_t;

void FILTER_second_order_init(Filter_second_order_t * filter, float settings[3]);
float FILTER_first_order_process(Filter_second_order_t * filter, float new_value);
float FILTER_second_order_process(Filter_second_order_t * filter, float new_value);


#endif /* BTM_FILTERS_H_ */
