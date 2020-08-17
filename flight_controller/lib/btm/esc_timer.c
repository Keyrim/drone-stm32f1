/*
 * esc_timer.c
 *
 *  Created on: 16 août 2020
 *      Author: Théo
 */

#include "esc_timer.h"

ESCs_t * escs ;


void ESCS_init(ESCs_t * escs_, Escs_output_t output_mode){
	escs = escs_ ;
	escs->output_mode = output_mode ;

	//On active le tim1 présent sur l apb2
	__HAL_RCC_TIM1_CLK_ENABLE();

	//Les moteurs sont branchés sur les 4 canaux du timer1 avec les pins par défault soit :
	escs->GPIO_escs = GPIOA ;
	escs->PIN_escs[ESC_1_CHANNEL] = GPIO_PIN_8 ;
	escs->PIN_escs[ESC_2_CHANNEL] = GPIO_PIN_9 ;
	escs->PIN_escs[ESC_3_CHANNEL] = GPIO_PIN_10 ;
	escs->PIN_escs[ESC_4_CHANNEL] = GPIO_PIN_11;

	//On configure les pin de sortie pour accepter le pwm
	for(uint8_t e = 0; e <4; e++)
		BSP_GPIO_PinCfg(GPIOA, escs->PIN_escs[e], GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);

	//Configuration du timer1
	TIM1->ARR = 4000 ;		//4000 de période permet une résolution de 1000 sur le signal de sortie (qui varie de 1000 à 2000)

	switch(output_mode){
	case ESC_OUTPUT_PWM :
		TIM1->PSC = 63 ;		// tic toutes les micros secondes
		break;
	case ESC_OUTPUT_ONE_SHOT_125 :
		TIM1->PSC = 7 ;			// tic tous les 8eme de micros secondes
		break;

	default :
		TIM1->PSC = 63;			//Par défault à 250 Hz
	}


	//Configuration des 4 canaux du timer 1 en mode PWM1 et activation du preload register
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE ;
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE ;
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE ;
	TIM1->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE ;

	//Configuration des 4 registre de comparaison pour les 4 canaux
	TIM1->CCR1 = 1000;
	TIM1->CCR2 = 1000;
	TIM1->CCR3 = 1000;
	TIM1->CCR4 = 1000;

	//On active les 4 sorties du timer 1
	TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E ;

	//Réinitialise le compteur (CNT)
	TIM1->EGR |= TIM_EGR_UG ;

	//Les sorties des canaux prennent effets sur les pins
	TIM1->BDTR |= TIM_BDTR_MOE ;

	//On lance le compteur et activation de l'auto-reload preload
	TIM1->CR1 |= TIM_CR1_CEN  | TIM_CR1_ARPE ;

}

void ESCS_set_period(uint32_t period_esc_1, uint32_t period_esc_2, uint32_t period_esc_3, uint32_t period_esc_4){
	escs->Duty[ESC_1_CHANNEL] = (uint16_t)period_esc_1 ;
	escs->Duty[ESC_2_CHANNEL] = (uint16_t)period_esc_2 ;
	escs->Duty[ESC_3_CHANNEL] = (uint16_t)period_esc_3 ;
	escs->Duty[ESC_4_CHANNEL] = (uint16_t)period_esc_4 ;

	for(uint8_t ch = 0; ch < 4; ch++){
		escs->Duty[ch] = MAX(1000, escs->Duty[ch]);
		escs->Duty[ch] = MIN(2000, escs->Duty[ch]);
	}

	TIM1->CCR1 = escs->Duty[0];
	TIM1->CCR2 = escs->Duty[1];
	TIM1->CCR3 = escs->Duty[2];
	TIM1->CCR4 = escs->Duty[3];

}
















