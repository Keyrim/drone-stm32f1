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

//Opérations sur les mask
Mask_t MASK_and(Mask_t mask1, Mask_t mask2);
Mask_t MASK_or(Mask_t mask1, Mask_t mask2);
Mask_t MASK_not(Mask_t mask);

//Créer un mask à partir d'un tableau de flag
Mask_t MASK_create(Flags_t * flag_array, int32_t len);
Mask_t MASK_create_single(Flags_t flag);

//Renvoit un mask vide
Mask_t MASK_get_empty_mask();
//Clean tous les flags d un mask
Mask_t MASK_clean_every_flags(Mask_t * mask);

#endif /* EVENTS_MASK_H_ */
