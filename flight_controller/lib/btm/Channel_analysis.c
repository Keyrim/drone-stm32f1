/*
 * channel_analysis.c
 *
 *  Created on: 5 août 2020
 *      Author: Theo
 */

#include "Channel_annalysis.h"
#include "stdlib.h"

#define JOYCTICK_DEAD_BAND 30


bool_e channel_analysis_init(channel_analysis_t * channels, int32_t nb_channel, int32_t * channels_array){

	if(nb_channel > NB_CHANNEL_MAX)
		//Si la struct à des tableaux trop petit on laisse tomber
		return 1 ;
	channels->nb_channels = nb_channel ;
	channels->channels = channels_array ;

	//Définition du mode d'analyse pour chaque channel
	channels->analysis_mode[0] = ANALYSIS_STICK_DEAD_BAND ;
	channels->analysis_mode[1] = ANALYSIS_STICK_DEAD_BAND ;
	channels->analysis_mode[2] = ANALYSIS_STICK_LVL ;
	channels->analysis_mode[3] = ANALYSIS_STICK_DEAD_BAND ;
	channels->analysis_mode[4] = ANALYSIS_SWITCH_MODE ;
	channels->analysis_mode[5] = ANALYSIS_SWITCH_MODE ;
	channels->analysis_mode[6] = ANALYSIS_SWITCH_MODE ;
	channels->analysis_mode[7] = ANALYSIS_SWITCH_MODE ;
	channels->analysis_mode[8] = ANALYSIS_BUTTON_PUSH ;
	channels->analysis_mode[9] = ANALYSIS_BUTTON_ON_OFF ;

	channels->is_init = TRUE ;
	return 0 ;
}

//Analyse de chaque channel en fonction du mode d'analyse attribué
void channel_analysis_process(channel_analysis_t * channels){
	//Analyse des channels
	bool_e button_state ;
	for(int8_t ch = 0; ch < channels->nb_channels; ch++ ){
		switch (channels->analysis_mode[ch]) {
			case ANALYSIS_NONE:
				//On fait R
				break;

			case ANALYSIS_STICK_DEAD_BAND :
				if(abs(channels->channels[ch] - 1500) < JOYCTICK_DEAD_BAND)
					channels->channels[ch] = 1500 ;
				break;

			case ANALYSIS_STICK_LVL :
				if(channels->channels[ch] > 1000 && channels->channels[ch] <= 1070)
					channels->throttle_lvl[ch] = THROTTLE_NULL ;
				else if(channels->channels[ch] > 1070 && channels->channels[ch] <= 1120)
					channels->throttle_lvl[ch] = THROTTLE_LOW ;
				else if(channels->channels[ch] > 1120 && channels->channels[ch] < 2000)
					channels->throttle_lvl[ch] = THROTTLE_HIGH ;
				break;

			case ANALYSIS_BUTTON_ON_OFF :
				//Buton state
				button_state = channels->channels[ch] > 1500 ;
				//If different from previous state
				if(button_state != channels->button_state[ch]){
					channels->button_on_off[ch] = 1 - channels->button_on_off[ch] ;
				}
				channels->button_state[ch] = button_state ;
				break;

			case ANALYSIS_BUTTON_PUSH :
				//Buton state
				button_state = channels->channels[ch] > 1500 ;
				//If different from previous state and previous was low
				if(button_state != channels->button_state[ch] && channels->button_state[ch] == 0){
					channels->button_on_off[ch] = 1 ;
				}
				channels->button_state[ch] = button_state ;
				break;

			case ANALYSIS_SEQUENCE:
				//todo analyse de séquences sur les switchs
				break;

			case ANALYSIS_SWITCH_MODE:
				if(channels->channels[ch] < 1300)
					channels->switch_pos[ch] = SWITCH_POS_1 ;
				else if(channels->channels[ch] > 1300 && channels->channels[ch] < 1600)
					channels->switch_pos[ch] = SWITCH_POS_2 ;
				else if(channels->channels[ch] > 1600)
					channels->switch_pos[ch] = SWITCH_POS_3 ;
				else
					channels->switch_pos[ch] = SWITCH_POS_ERROR ;
				break;
			default:
				break;
		}
	}
}























