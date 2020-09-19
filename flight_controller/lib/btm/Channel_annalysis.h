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
	ANALYSIS_NONE,
	ANALYSIS_STICK_DEAD_BAND,
	ANALYSIS_STICK_LVL,
	ANALYSIS_SEQUENCE,
	ANALYSIS_SWITCH_MODE
}analysis_mode_t;


typedef enum{
	THROTTLE_NULL,
	THROTTLE_LOW,
	THROTTLE_HIGH
}throttle_lvl_t;

typedef enum{
	SWITCH_POS_1,
	SWITCH_POS_2,
	SWITCH_POS_3,
	SWITCH_POS_ERROR
}switch_pos_t;


typedef struct{
	int32_t * channels ;
	int32_t nb_channels ;
	bool_e is_init ;
	switch_pos_t switch_pos [NB_CHANNEL_MAX] ; //For buttons etc (0 = 1000, 1 = 1500, 2= 2000)
	throttle_lvl_t throttle_lvl [NB_CHANNEL_MAX] ;
	analysis_mode_t analysis_mode [NB_CHANNEL_MAX];

}channel_analysis_t;

bool_e channel_analysis_init(channel_analysis_t * channels, int32_t nb_channel, int32_t * channels_array);
void channel_analysis_process(channel_analysis_t * channels);

#endif /* BTM_CHANNEL_ANNALYSIS_H_ */
