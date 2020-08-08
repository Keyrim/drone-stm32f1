/*
 * bh1750fvi.c
 *
 *  Created on: 24 nov. 2016
 *      Author: Cécile Lebrun
 */

/**
 * Ce pilote permet d'interfacer le capteur de luminosité BH1750FVI.
 * Ce capteur permet de mesurer la luminosité ambiante pour ensuite, par exemple, ajuster la luminosité d'un écran
 * de smartphone ou encore le rétro-éclairage d'un clavier.
 *
 * Comment utiliser ce pilote :
 * 		1. L'initialiser (initialiser son bus I2C) avec la fonction BH1750FVI_init.
 * 		2. L'allumer avec la fonction BH1750FVI_powerOn.
 * 		3. Choisir son mode de mesure. Il est possible de choisir entre une mesure en continu ou une mesure en "one-shot".
 * 		   On peut également choisir la résolution du capteur. Les résolutions disponibles (en lux) sont les suivantes :
 *
 * 		   										Temps de mesure		Resolution
 * 		   			High resolution mode 2			120 ms			0.5 lx
 * 		   			High resolution mode 1			120 ms			1 lx
 * 		   			Low resolution					16 ms			4 lx
 *
 * 		   Le constructeur conseille l'utilisation du high resolution mode 1, celui-ci étant également adapté à la mesure
 * 		   de la luminosité dans l'obsurité (quand la luminosité est inférieure à 10 lx).
 * 		   Cette étape se fait via l'appel de la fonction BH1750FVI_measureMode en lui passant comme argument le mode souhaité
 * 		   (les différents modes possibles sont listés dans le fichier bh1750fvi.h).
 *
 * 		 4. Eteindre le capteur. Dans le cas du mode de mesures en continu, il faut l'éteindre à la main à la fin des mesures
 * 		 	en appelant la fonction BH1750FVI_powerDown. Dans le cas d'une mesure en one-shot, l'extinction se fait automatiquement
 * 		 	après que la mesure ait été effectuée.
 *
 *
 * Par défaut, les broches utilisées sont celles de l'I2C1 : PB8 pour SCL et PB9 pour SDA.
 * Le basculement sur l'I2C2 (PB10 pour SCL et PB11 pour SDA) peut se faire en modifiant la fonction BH1750FI_init().
 * Il est possible de brancher 2 capteurs BH1750FVI sur le même I2C, pour ce faire l'un doit avoir sa broche ADDR reliée
 * à la masse (son adresse sera alors 0x23), l'autre ayant sa broche ADDR alimentée par une tension de 3,3V (son adresse
 * sera alors 0x5C).
 * Le capteur est alimenté sur sa broche Vcc en 3,3V.
 */

#include "config.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#if USE_BH1750FVI
#include "bh1750fvi.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_gpio.h"
#include "stm32f1_i2c.h"
#include "stm32f1xx_nucleo.h"

//Si la pin ADDR n'est pas reliée à la masse mais à 3,3V, changer cette macro en lui donnant la valeur BH1750FVI_ADDR_H
#define BH1750FVI_ADDR BH1750FVI_ADDR_L


/**
 * @brief	Exemple d'utilisation du capteur en mode one-shot, haute résolution. Dans cet exemple,
 * 			toutes les 200ms, le capteur s'allume, prend une mesure, puis on lit la luminosité
 * 			qu'on affiche ensuite sur l'uart. Le capteur s'éteint
 * 			automatiquement après avoir effectué une mesure.
 * @post	Attention, cette fonction de démonstration est blocante !!!
 * @pre		La fonction printf étant appelée, il faut avoir initialisé au préalable une liaison série.
 */
void BH1750FVI_demo()
{
	uint16_t luminosity;
	BH1750FVI_init();
	BH1750FVI_powerOn();
	BH1750FVI_measureMode(BH1750FVI_CON_L);
	while(1)
	{
		HAL_Delay(200);
		luminosity = BH1750FVI_readLuminosity();
		printf("\nLuminosite = %d lx", luminosity);
	}
}



/**
 * @brief	Fonction d'initialisation du bus I2C du capteur.
 */
void BH1750FVI_init()
{
	I2C_Init(BH1750FVI_I2C, 100000);
}

/**
 * @brief	Fonction d'allumage du capteur.
 */
void BH1750FVI_powerOn()
{
	I2C_WriteNoRegister(BH1750FVI_I2C, BH1750FVI_ADDR<<1, BH1750FVI_ON);
}

/**
 * @brief	Fonction d'extinction du capteur.
 */
void BH1750FVI_powerDown()
{
	I2C_WriteNoRegister(BH1750FVI_I2C, BH1750FVI_ADDR<<1, BH1750FVI_OFF);
}

/**
 * @brief	Fonction de reset du capteur.
 */
void BH1750FVI_reset()
{
	I2C_WriteNoRegister(BH1750FVI_I2C, BH1750FVI_ADDR<<1, BH1750FVI_RESET);
}

/**
 * @brief	Fonction permettant de lancer les/la mesure en choisissant un mode de mesure.
 * @param	mode: Mode de mesure souhaité. Le mode conseillé par le constructeur est le mode BH1750FVI_CON_H1
 * 			(ou BH1750FVI_OT_H1 pour une unique mesure), ce mode permettant d'avoir une sensibilité	suffisante pour
 * 			mesurer la luminosité dans l'obscurité. Pour connaître les autres modes disponibles, voir le fichier bh1750fvi.h.
 */
void BH1750FVI_measureMode(uint8_t mode)
{
	I2C_WriteNoRegister(BH1750FVI_I2C, BH1750FVI_ADDR<<1, mode);
}


/**
 * @brief	Fonction permettant de lire la luminosité mesurée par le capteur.
 * @return	Cette fonction retourne la valeur de la luminosité en lux.
 */
uint16_t BH1750FVI_readLuminosity()
{
	uint16_t luminositeNonConvertie = 0;
	uint32_t luminosite = 0;
	luminositeNonConvertie = BH1750FVI_read();
	luminosite = (((uint32_t)(luminositeNonConvertie))*53)/64; //La datasheet du capteur indique qu'il faut diviser par 1.2
	return (uint16_t)luminosite;
}

/**
 * @brief	Fonction permettant de lire un registre du capteur, typiquement les registres contenant les
 * 			mesures effectuées par la capteur.
 * @return 	La valeur du registre (deux octets).
 */
uint16_t BH1750FVI_read()
{
	uint8_t ret[3];
	I2C_ReadMultiNoRegister(BH1750FVI_I2C, BH1750FVI_ADDR<<1, ret, 3);
	return ret[1] | ((uint16_t)(ret[0])) << 8;
}

#endif

