/*
 * test_transition.c
 *
 *  Created on: 19 juil. 2020
 *      Author: Théo
 */

#include "Test_transition.h"

//On save la fonction
void TRANSITION_init_test(test_t * test, int32_t (*test_function)(State_drone_t * drone, int32_t test)){
	test->nb_test_succesfull = 0 ;
	test->test_function = test_function ;
}

//On appelle la fonction de test qu'on a pour la structure de test, et on renvoit true si on atteint le nombre de test positif consécutif recquis
bool_e TRANSITION_test(test_t * test_struct, State_drone_t * drone, bool_e test, int32_t nb_recquiered){
	//Quand on call une fonction de test elle renvoit, oui, non ou j'ai rien à dire

	int32_t test_result = test_struct->test_function(drone, test);

	if(!test_struct->nb_test_succesfull){
		//Si on est à zéro on prend la valeur anyway, qu'elle soit vielle ou pas
		if(test_result == 1 || test_result == 3)
			test_struct->nb_test_succesfull ++ ;
	}
	else{
		//Si on est pas à zéro on ajoute qqchose au compteur que si on a une nouvelle valeur qui vaut 1
		if(test_result == 0)
			test_struct->nb_test_succesfull = 0 ;
		if(test_result == 1)
			test_struct->nb_test_succesfull ++ ;
	}


	if(test_struct->nb_test_succesfull == nb_recquiered){
		test_struct->nb_test_succesfull = 0 ;
		return TRUE ;
	}
	return FALSE ;
}
