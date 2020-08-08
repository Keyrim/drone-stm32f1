/*
 * bh1750fvi.h
 *
 *  Created on: 24 nov. 2016
 *      Author: Cécile Lebrun
 */

#ifndef LIB_BSP_BH1750FVI_BH1750FVI_H_
#define LIB_BSP_BH1750FVI_BH1750FVI_H_

#include "macro_types.h"
#include "stm32f1xx_hal.h"

#define BH1750FVI_I2C 	I2C1

#define BH1750FVI_ON		0x00
#define BH1750FVI_OFF		0x01
#define BH1750FVI_RESET		0x07

//Modes de mesure
#define BH1750FVI_CON_H1	0x10	//Mesure continue, haute résolution (1 lx)
#define BH1750FVI_CON_H2	0x11	//Mesure continue, haute résolution (0.5 lx)
#define BH1750FVI_CON_L		0x13	//Mesure continue, basse résolution (4 lx)
#define BH1750FVI_OT_H1		0x20
#define BH1750FVI_OT_H2		0x21
#define BH1750FVI_OT_L		0x23	//Une seule mesure, basse résolution (4 lx)

//Adresses possible des capteurs
#define BH1750FVI_ADDR_L	0x23 //Si ADDR est reliée à la masse.
#define BH1750FVI_ADDR_H	0x5C //Si ADDR est reliée à 3,3V.

void BH1750FVI_demo(void);

void BH1750FVI_init(void);
void BH1750FVI_powerOn(void);
void BH1750FVI_powerDown(void);
void BH1750FVI_reset(void);
void BH1750FVI_measureMode(uint8_t mode);
uint16_t BH1750FVI_readLuminosity();
uint16_t BH1750FVI_read();




#endif /* LIB_BSP_BH1750FVI_BH1750FVI_H_ */
