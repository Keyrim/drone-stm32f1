/*
 * global.h
 *
 *  Created on: 30 avr. 2020
 *      Author: Theo
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

typedef struct{
	volatile uint32_t free_time ;		//Corespond au temps dispo à la fin d'un passage dans la high lvl
											//Si une fonction modifie cette variable elle doit aussi prendre en charge sa remise à zéro quand elle n'est plus active
}Global_var_e;

Global_var_e global ;

#endif /* GLOBAL_H_ */
