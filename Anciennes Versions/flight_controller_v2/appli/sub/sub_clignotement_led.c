/*
 * sub_clignotement_led.c
 *
 *  Created on: 20 avr. 2020
 *      Author: Theo
 */


#include "sub_action.h"
#include "../WS2812S.h"
#include "../couleur.h"


void sub_clignotement_led(LedMode_e mode){
	switch(mode){
		case LED_MODE_1 :
			LED_MATRIX_display_full(COLOR_AQUA, 6);
			break;

		case LED_MODE_ETEINT:
			LED_MATRIX_display_full(COLOR_BLACK, 6);
			break;
	}
}
