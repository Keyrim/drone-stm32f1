/*
 * channel_annalysis.h
 *
 *  Created on: 5 août 2020
 *      Author: Theo
 */

#ifndef BTM_CHANNEL_ANNALYSIS_H_
#define BTM_CHANNEL_ANNALYSIS_H_

#include "macro_types.h"

#define NB_CHANNEL_MAX 10

typedef enum{
	NO_ANALYSIS,
	JOYSTICK_DEAD_BAND,
	SEQUENCE_ANALYSIS,
	INSTANT_ANALYSIS
}analysis_mode_t;

typedef enum{
	JOYSTICK,
	SWITCH_2_POS,
	SWITCH_3_POS
}channel_type_t;


typedef struct{
	int32_t * channels ;
	int32_t nb_channels ;
	bool_e is_init ;
	int8_t pos [NB_CHANNEL_MAX] ; //For buttons etc (0 = 1000, 1 = 1500, 2= 2000)
	analysis_mode_t analysis_mode [NB_CHANNEL_MAX];
	channel_type_t channel_type [NB_CHANNEL_MAX];

}channel_analysis_t;

bool_e channel_analysis_init(channel_analysis_t * channels, int32_t nb_channel, int32_t * channels_array);
void channel_analysis_process(channel_analysis_t * channels);

#endif /* BTM_CHANNEL_ANNALYSIS_H_ */
