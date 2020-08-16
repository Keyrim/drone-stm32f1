/*
 * esc_timer.c
 *
 *  Created on: 16 août 2020
 *      Author: Théo
 */

#include "esc_timer.h"

ESCs_t * escs ;


void ESCS_init(ESCs_t * escs_){
	escs = escs_ ;

	//On active le tim1 présent sur l apb2
	__HAL_RCC_TIM1_CLK_ENABLE();

	//Les moteurs sont branchés sur les 4 canaux du timer1 avec les pins par défault soit :
	escs->GPIO_escs = GPIOA ;
	escs->PIN_escs[0] = GPIO_PIN_8 ;
	escs->PIN_escs[1] = GPIO_PIN_9 ;
	escs->PIN_escs[2] = GPIO_PIN_10 ;
	escs->PIN_escs[3] = GPIO_PIN_11;

	//On configure les pin de sortie pour accepter le pwm
	for(uint8_t e = 0; e <4; e++)
		BSP_GPIO_PinCfg(GPIOA, escs->PIN_escs[e], GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);

	//Configuration du timer1
	TIM1->ARR = 4000 ;	//4000 micro s de période soit 250 Hz
	TIM1->PSC = 63 ;	//1 tic toutes les micros secondes car horloge de 64 MHz

	//Configuration des 4 canaux du timer 1 en mode PWM1 et activation du preload register
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE ;
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE ;
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE ;
	TIM1->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE ;

	//Configuration des 4 registre de comparaison pour les 4 canaux
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 1000;
	TIM1->CCR3 = 2000;
	TIM1->CCR4 = 3000;

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
	escs->Duty[0] = period_esc_1 ;
	escs->Duty[1] = period_esc_2 ;
	escs->Duty[2] = period_esc_3 ;
	escs->Duty[3] = period_esc_4 ;
	TIM1->CCR1 = period_esc_1;
	TIM1->CCR2 = period_esc_2;
	TIM1->CCR3 = period_esc_3;
	TIM1->CCR4 = period_esc_4;

}
















