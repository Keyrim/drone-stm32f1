/*
 * ppm.c
 *
 *  Created on: 22 mars 2020
 *      Author: Theo
 */

#include "ppm.h"


//Pin info (c'est moche mais azy on verra plus tard)
static DRONE_ppm_t * ppm;

void PPM_it();

void PPM_it(){
	uint32_t time = SYSTICK_get_time_us();
	uint32_t delta = time - ppm->previous_time_rising ;
	if(delta > 3000){
		ppm->actual_channel = 0 ;

	}
	else{
		if(ppm->actual_channel == NB_CHANNEL)
			ppm->actual_channel = 0 ;
		//Raison inconnu à voir à l'oscillo plus tard mais avec le test ça fonctionne bien
		if(delta < 1000)
			delta += 1000 ;
		if(delta > 2010)
			delta -= 1000 ;
		//On fait update de la valeur de la channel sur laquelle on est
		ppm->channels[ppm->actual_channel] = (uint16_t)delta ;
		//Si on update la chanel 0 on dit quand on l'a fait
		if(!ppm->actual_channel)
			ppm->last_update = time ;

		//on passe à la channel suivante
		ppm->actual_channel ++ ;
	}
	ppm->previous_time_rising = time ;
}



void DRONE_ppm_init(DRONE_ppm_t * ppm_, uint8_t pin_number, GPIO_TypeDef* gpio, uint16_t gpio_pin){
	//Init des variables
	ppm = ppm_ ;
	ppm->GPIO = gpio ;
	ppm->actual_channel = 0 ;
	ppm->gpio_pin = gpio_pin ;
	ppm->gpio_pin_number = pin_number ;

	//Init du pin pour l'it
	BSP_GPIO_PinCfg(gpio, gpio_pin, GPIO_MODE_IT_RISING, GPIO_PULLDOWN,GPIO_SPEED_FREQ_HIGH);

	//Then we store the irqn
	switch (pin_number){
		case 0 :	ppm->irqn = EXTI0_IRQn ; break ;
		case 1 :	ppm->irqn = EXTI1_IRQn ; break ;
		case 2 :	ppm->irqn = EXTI2_IRQn ; break ;
		case 3 :	ppm->irqn = EXTI3_IRQn ; break ;
		case 4 :	ppm->irqn = EXTI4_IRQn ; break ;
		case 5 :	ppm->irqn = EXTI9_5_IRQn ; break ;
		case 6 :	ppm->irqn = EXTI9_5_IRQn ; break ;
		case 7 :	ppm->irqn = EXTI9_5_IRQn ; break ;
		case 8 :	ppm->irqn = EXTI9_5_IRQn ; break ;
		case 9 :	ppm->irqn = EXTI9_5_IRQn ; break ;
		case 10 :	ppm->irqn = EXTI15_10_IRQn ; break ;
		case 11 :	ppm->irqn = EXTI15_10_IRQn ; break ;
		case 12 :	ppm->irqn = EXTI15_10_IRQn ; break ;
		case 13 :	ppm->irqn = EXTI15_10_IRQn ; break ;
		case 14 :	ppm->irqn = EXTI15_10_IRQn ; break ;
		case 15 :	ppm->irqn = EXTI15_10_IRQn ; break ;
		default : 	ppm->irqn = EXTI0_IRQn ; break ;
	}
	NVIC_EnableIRQ(ppm->irqn);
}

void DRONE_ppm_enable_it(DRONE_ppm_t * ppm){
	NVIC_EnableIRQ(ppm->irqn);
}


void DRONE_ppm_disable_it(DRONE_ppm_t * ppm){
	NVIC_DisableIRQ(ppm->irqn);
}

void EXTI15_10_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(ppm->gpio_pin);
	PPM_it();

}
void EXTI9_5_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(ppm->gpio_pin);
	PPM_it();
}
void EXTI4_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
	PPM_it();
}
void EXTI3_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
	PPM_it();
}
void EXTI2_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
	PPM_it();
}
void EXTI1_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	PPM_it();
}
void EXTI0_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	PPM_it();

}


