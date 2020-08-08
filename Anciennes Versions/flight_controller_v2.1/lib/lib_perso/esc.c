/*
 * esc.c
 *
 *  Created on: 23 mars 2020
 *      Author: Theo
 */


#include "esc.h"
#include "stm32f1_gpio.h"
#include "stdio.h"
#include "systick.h"
#include "../../appli/settings.h"
#include "../../appli/global.h"


static uint8_t nb_escs ;
static ESC_e* escs ;

static uint32_t pulsation_start_time = 0 ;


void ESC_Set_pulse(ESC_e* esc_, uint16_t pulsation_){
	pulsation_ = MIN(PULSATION_MAX, pulsation_);
	pulsation_ = MAX(PULSATION_MIN, pulsation_);
	esc_->pulsation = pulsation_ ;
}

void ESC_init(ESC_e* escs_, uint8_t nb_escs_){
	nb_escs = nb_escs_ ;
	escs = escs_ ;
	for(uint8_t e =0; e < nb_escs_; e++){
		BSP_GPIO_PinCfg(escs_[e].gpio, escs_[e].gpio_pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH );
		escs_[e].signal_state = FALSE ;
	}


}

void ESC_Start_pulse(void){
	pulsation_start_time = SYSTICK_get_time_us();
	for(uint8_t e = 0; e < nb_escs; e ++){
		escs[e].gpio->BSRR = escs[e].gpio_pin ;
		escs[e].signal_state = TRUE ;
	}
}


running_e ESC_End_pulse(void){
	static uint32_t pulse ;
	pulse = SYSTICK_get_time_us() ;

	if(pulse >= escs[0].pulsation + pulsation_start_time && escs[0].signal_state){
		escs[0].gpio->BSRR = (uint32_t)(escs[0].gpio_pin) << 16U ;
		escs[0].signal_state = FALSE ;
	}
	if(pulse >= escs[1].pulsation + pulsation_start_time && escs[1].signal_state){
		escs[1].gpio->BSRR = (uint32_t)(escs[1].gpio_pin) << 16U ;
		escs[1].signal_state = FALSE ;
	}
	if(pulse >= escs[2].pulsation + pulsation_start_time && escs[2].signal_state){
		escs[2].gpio->BSRR = (uint32_t)(escs[2].gpio_pin) << 16U ;
		escs[2].signal_state = FALSE ;
	}
	if(pulse >= escs[3].pulsation + pulsation_start_time && escs[3].signal_state){
		escs[3].gpio->BSRR = (uint32_t)(escs[3].gpio_pin) << 16U ;
		escs[3].signal_state = FALSE ;
	}


	bool_e is_not_over = (escs[0].signal_state || escs[1].signal_state || escs[2].signal_state || escs[3].signal_state) ;


	if(is_not_over){
//		TODO free time moteur à test
//		uint32_t free_time = 1000 ;
//		for(uint8_t m = 0; m < nb_escs; m ++){
//			if(escs[m].signal_state)
//				free_time = MIN(free_time, escs[m].pulsation + pulse - pulsation_start_time);
//		}
//		global.free_time = free_time ;
		return IN_PROGRESS ;
	}
	else {
		global.free_time = 0 ;
		return END_OK ;
	}
}


