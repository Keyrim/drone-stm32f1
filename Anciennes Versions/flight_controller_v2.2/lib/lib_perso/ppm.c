/*
 * ppm.c
 *
 *  Created on: 22 mars 2020
 *      Author: Theo
 */

#include "ppm.h"
#include "settings.h"

#define MEAN_OVER 50


static uint8_t actual_channel = 0;
static uint16_t* channels_array ;
static uint16_t channels_average_array [NB_CHANNEL][MEAN_OVER];
static uint16_t compteur = 0 ;
static uint32_t* time_last_read ;
static uint32_t previous_time_rising = 0 ;
//Pin info
static uint8_t gpio_pin_number;
static uint16_t gpio_pin ;
static GPIO_TypeDef* GPIO ;
static IRQn_Type irqn ;

void PPM_it();

void PPM_it(){
	uint32_t time = SYSTICK_get_time_us();
	uint32_t delta = time - previous_time_rising ;
	if(delta > 3000){
		actual_channel = 0 ;
	}
	else{
		if(actual_channel == NB_CHANNEL)
			actual_channel = 0 ;
		//Parfois on a une mesure qui est pouris donc ça permet de l'éviter
		//Raison inconnu à voir à l'oscillo plus tard mais avec le test ça fonctionne bien
		if(delta < 1000)
			delta += 1000 ;
		if(delta > 2010)
			delta -= 1000 ;
		//On fait update de la valeur de la channel sur laquelle on est
		channels_array[actual_channel] = (uint16_t)delta ;

		//Parti pour la moyenne glissante
		if(compteur == MEAN_OVER)
			compteur = 0 ;
		channels_average_array[actual_channel][compteur] = (uint16_t)delta ;

		if(actual_channel == 0)
			compteur ++ ;

		//on passe à la channel suivante
		actual_channel ++ ;
	}
	previous_time_rising = time ;
	*time_last_read = time ;
}

uint16_t PPM_get_average(uint8_t channel){
	volatile uint32_t summ = 0 ;
	for (int8_t c = 0; c < MEAN_OVER; c++)
		summ += channels_average_array[channel][compteur] ;
	volatile uint16_t to_return = (uint16_t)(summ / 50) ;
	summ ++ ;
	return to_return ;
}

void PPM_init(uint16_t* channels_array_, uint8_t pin_number, GPIO_TypeDef* gpio_, uint16_t gpio_pin_, bool_e enable_it, uint32_t *time_last_read_){
	time_last_read = time_last_read_ ;
	channels_array = channels_array_ ;
	gpio_pin_number = pin_number ;

	//Firstly, config the pin used for ppm signal as a rising_it
	gpio_pin = gpio_pin_ ;
	GPIO = gpio_ ;
	BSP_GPIO_PinCfg(GPIO, gpio_pin, GPIO_MODE_IT_RISING, GPIO_PULLDOWN,GPIO_SPEED_FREQ_HIGH);

	//Then we store the irqn
	switch (gpio_pin_number){
		case 0 :	irqn = EXTI0_IRQn ; break ;
		case 1 :	irqn = EXTI1_IRQn ; break ;
		case 2 :	irqn = EXTI2_IRQn ; break ;
		case 3 :	irqn = EXTI3_IRQn ; break ;
		case 4 :	irqn = EXTI4_IRQn ; break ;
		case 5 :	irqn = EXTI9_5_IRQn ; break ;
		case 6 :	irqn = EXTI9_5_IRQn ; break ;
		case 7 :	irqn = EXTI9_5_IRQn ; break ;
		case 8 :	irqn = EXTI9_5_IRQn ; break ;
		case 9 :	irqn = EXTI9_5_IRQn ; break ;
		case 10 :	irqn = EXTI15_10_IRQn ; break ;
		case 11 :	irqn = EXTI15_10_IRQn ; break ;
		case 12 :	irqn = EXTI15_10_IRQn ; break ;
		case 13 :	irqn = EXTI15_10_IRQn ; break ;
		case 14 :	irqn = EXTI15_10_IRQn ; break ;
		case 15 :	irqn = EXTI15_10_IRQn ; break ;
		default : 	irqn = EXTI0_IRQn ; break ;
	}

	if (enable_it)
		NVIC_EnableIRQ(irqn);
}

void enable_it(void){
	NVIC_EnableIRQ(irqn);
}


void disable_it(void){
	NVIC_DisableIRQ(irqn);
}

void EXTI15_10_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(gpio_pin);
	PPM_it();

}
void EXTI9_5_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(gpio_pin);
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


