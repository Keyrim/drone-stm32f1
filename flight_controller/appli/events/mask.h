/*
 * mask.h
 *
 *  Created on: 16 sept. 2020
 *      Author: Theo
 */

#ifndef EVENTS_MASK_H_
#define EVENTS_MASK_H_

#define NB_ARRAY_MASK 2

#include "macro_types.h"
#include "flags.h"

typedef struct{
	uint32_t array[2] ;
}Mask_t;

//Fonctions qui gèrent les bits / les flags sur les masques

bool_e MASK_set_flag(Mask_t * mask, Flags_t flag);
bool_e MASK_clean_flag(Mask_t * mask, Flags_t flag);


//Fonctions de tests avec les masques
bool_e Mask_test_and(Mask_t mask_test, Mask_t mask_ref);
bool_e Mask_test_or(Mask_t mask_test, Mask_t mask_ref);

#endif /* EVENTS_MASK_H_ */
