/*
 * stp16cp05.h
 *
 *  Created on: 17 janv. 2018
 *  Author: Nirgal
 *
 *      Le driver de LED STP16C05 pilote jusqu'à 16 LED en fournissant un courant régulé commun et réglable (via une résistance de référence).
 *
 */

#ifndef STP16CP05_H_
#define STP16CP05_H_
#include <stdint.h>

void STP16CP05_init(void);
void STP16CP05_test(void);
void STP16CP05_set_leds(uint16_t leds);


#endif /* STP16CP05_H_ */
