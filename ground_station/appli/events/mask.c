/*
 * mask.c
 *
 *  Created on: 16 sept. 2020
 *      Author: Theo
 */

#include "mask.h"

//Renvoit un mask vide
Mask_t MASK_get_empty_mask(){
	Mask_t mask_return ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		mask_return.array[m] = 0 ;
	return mask_return ;
}
//Clean tous les flags d un mask
void MASK_clean_every_flags(Mask_t * mask){
	for(int m = 0; m < NB_ARRAY_MASK; m++)
		mask->array[m] = 0 ;
}

//Fonction qui compare si tous les bits du mask de test sont présent dans le mask de ref
bool_e Mask_test_and(Mask_t mask_test, Mask_t mask_ref){
	//On cherche à savoir si les bits du mask sont présent dans le mask de ref
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		if((mask_test.array[m] & mask_ref.array[m]) != mask_test.array[m]){
			//Si il ne y a pas correspondance sur une des entiers on renvoit faux
			return FALSE ;
		}
	return TRUE ;
}

//Fonction qui compare si au moins des bits du mask de test est présent dans le mask de ref
bool_e Mask_test_or(Mask_t mask_test, Mask_t mask_ref){
	//On cherche à savoir si au moins des bits du mask ref est présent dans le mask de ref
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		if(mask_test.array[m] & mask_ref.array[m])
			//Si un des bits est présent on renvoit vrai
			return TRUE ;
	return FALSE ;
}

//Passe le flag séléctionner à l'état haut, renvoit faux si il l'était déjà, vrai sinon
bool_e MASK_set_flag(Mask_t * mask, Flags_t flag){
	uint32_t array_nb = flag / 32 ;
	uint32_t flag_pos = flag % 32 ;
	uint32_t flag_mask = (uint32_t)(1 << flag_pos) ;
	//On vérifi si le flag est pas déjà haut
	if((mask->array[array_nb] & flag_mask) == flag_mask)
		return FALSE ;
	//Et on passe à létat haut sinon
	mask->array[array_nb] |= flag_mask ;
	return TRUE ;
}

//Clean un flag, renvoit si il état déjà clean, vrai sinon
bool_e MASK_clean_flag(Mask_t * mask, Flags_t flag){
	uint32_t array_nb = flag / 32 ;
	uint32_t flag_pos = flag % 32 ;
	uint32_t flag_mask = (uint32_t)(1 << flag_pos) ;
	//On  vérifi si le flag est bien levé
	if((mask->array[array_nb] & flag_mask ) == flag_mask){
		mask->array[array_nb] -= flag_mask ;
		return TRUE ;
	}
	return FALSE ;
}


//Opérations sur les mask
Mask_t MASK_and(Mask_t mask1, Mask_t mask2){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		mask_return.array[m] = mask1.array[m] & mask2.array[m] ;

	return mask_return ;
}
Mask_t MASK_or(Mask_t mask1, Mask_t mask2){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		mask_return.array[m] = mask1.array[m] | mask2.array[m] ;

	return mask_return ;
}

Mask_t MASK_not(Mask_t mask){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		mask_return.array[m] = ~mask.array[m] ;

	return mask_return ;
}



//Créer un mask à partir d'un tableau de flag
Mask_t MASK_create(Flags_t * flag_array, int32_t len){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t f = 0; f < len; f++)
		MASK_set_flag(&mask_return, flag_array[f]);

	return mask_return ;
}

Mask_t MASK_create_single(Flags_t flag){
	Mask_t mask_return = MASK_get_empty_mask() ;
	MASK_set_flag(&mask_return, flag);
	return mask_return ;
}


