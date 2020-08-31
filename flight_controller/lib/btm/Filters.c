/*
 * Filter.c
 *
 *  Created on: 20 août 2020
 *      Author: Théo
 */
#include "Filters.h"

void FILTER_second_order_init(Filter_second_order_t * filter, float settings[3]){
	for(int32_t s = 0; s < 3; s++)
		filter->settings[s] = settings[s] ;
}

float FILTER_first_order_process(Filter_second_order_t * filter, float new_value){
	filter->values[0] = new_value * filter->settings[0] + filter->values[1] * filter->values[0] ;
	return filter->values[0];
};

float FILTER_second_order_process(Filter_second_order_t * filter, float new_value){
	filter->values[0] = new_value * filter->settings[0] + filter->values[1] * filter->settings[1] + filter->values[2] * filter->settings[2];
	filter->values[2] = filter->values[1];
	filter->values[1] = filter->values[0];
	return filter->values[0];
}
