/**
 * @addtogroup lcd2x16
 *
 * Afficheur LCD 2 lignes de 16 caractères.
 * 
 *
 * L'afficheur est utilisé en mode 4 bits. Les ports d'entrée/sortie se décomposent par conséquent en
 *    - trois bits de commande
 *    - quatre bits de données
 *
 * Les bits de commande sont configurées en sortie tandis que les bits de données peuvent être accédés
 * soit en lecture, soit en écriture.
 *
 * Eléments à modifier pour configurer l'écran LCD :
 *    - pour la gestion des délais : #CLOCK_FREQUENCY
 *    - Configuration des ports de données et de commande
 * @{
 */

/**
 * @file lcd2x16_c18.h
 *
 *  Driver d'afficheur LCD 2 lignes de 16 caractères.
 *
 *  @version 1.3
 *  @date 24/08/2015
 */

#ifndef __LCD2X16_H
#define __LCD2X16_H

#include "stm32f1xx_hal.h"

/**
 * Bits du port de données.
 * Adaptez ces configurations à votre design (ports et pins utilisés).
 */
	#define PORT_DATA_0 GPIOB
	#define PORT_DATA_1 GPIOB
	#define PORT_DATA_2 GPIOB
	#define PORT_DATA_3 GPIOB
	#define	PIN_DATA_0	GPIO_PIN_9
	#define	PIN_DATA_1	GPIO_PIN_10
	#define	PIN_DATA_2	GPIO_PIN_11
	#define	PIN_DATA_3	GPIO_PIN_12
	
	#define PORT_RS		GPIOB
	#define PORT_RW		GPIOB
	#define PORT_E		GPIOB
	#define PIN_RS		GPIO_PIN_8
	#define PIN_RW		GPIO_PIN_4
	#define PIN_E		GPIO_PIN_5

	#define LCD2X16_HAL_CLOCK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE(); 	__HAL_AFIO_REMAP_SWJ_NONJTRST();


void LCD2X16_demo(void);

/**
 * Fonction d'initialisation du LCD.
 *
 * Cette fonction doit être appelée préalablement à toute utilisation de l'écran LCD.
 */
void LCD2X16_init(void);

/** Fonction de test de ce module logiciel. Elle présente un exemple d'utilisation des fonctions.
 *
 */
void LCD2X16_test(void);

/** Fonction d'écriture d'un caractère sur le LCD. Cette fonction affiche un caractère ascii sur l'afficheur	\n
 *  Il existe des caractères de contrôle :																		\n
 *     - \\f  efface l'écran																					\n
 *     - \\n  saut au début de la seconde ligne																	\n
 *     - \\b  retour arrière d'un caractère
 *     
 *     @param c le caractère à afficher
 *     @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel à LCD2X16_init().
 *     @post    les bits du port de données sont configurés en écriture.
 *  */
void LCD2X16_putChar(char c);


/** Fonction de positionnement du curseur sur le LCD
 * 
 *    @param x indice de colonne (1 à 16)
 *    @param y indice de ligne (1 ou 2)
 *    @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel de LCD2X16_init()
 */
void LCD2X16_setCursor( unsigned char column, unsigned char ligne);



/** Fonction de lecture d'un caractère sur le LCD.
 * 
 *    @param x indice de colonne du caractère à lire (1 à 16)
 *    @param y indice de ligne du caractère à lire (1 ou 2)
 *    @return  le caractère lu en (x,y)
 *    @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel de LCD2X16_init()
 */
char LCD2X16_getChar( unsigned char x, unsigned char y);



/** Fonction d'affichage d'une chaine formattée (cf printf)
 * 	@param  premier paramètre = chaine de format
 * 	@param  paramètres suivantes = selon les balises %x indiquées dans la chaîne de format
 * 	@pre 	La chaine fabriquée ne DOIT PAS excéder 20 caractères (et en pratique 16 caractères pour un écran de 16 caractères par lignes). *    @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel de LCD2X16_init()
 * 	@post	Si la chaîne fabriquée excède 20 caractères, un message est affichée via printf.
 */
int	LCD2X16_printf(const char *__restrict, ...) __attribute__ ((__format__ (__printf__, 1, 2)));


/**
 * @}
 */
#endif
