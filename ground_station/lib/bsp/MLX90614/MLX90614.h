/*
 * MLX90614.h
 *
 *  Created on: 16 janv. 2016
 *      Author: Nirgal
 */

#ifndef LIB_BSP_MLX90614_MLX90614_H_
#define LIB_BSP_MLX90614_MLX90614_H_

/***************************************************
  This is a library for the MLX90614 Temp Sensor
  Designed specifically to work with the MLX90614 sensors in the
  adafruit shop
  ----> https://www.adafruit.com/products/1748
  ----> https://www.adafruit.com/products/1749
  These sensors use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruied in any redistribution
 ****************************************************/


#include "macro_types.h"
#include "stm32f1xx_hal.h"

#define MLX90614_I2C 	I2C2

#define MLX90614_I2CADDR 0x5A

// RAM
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08
// EEPROM
#define MLX90614_TOMAX 0x20
#define MLX90614_TOMIN 0x21
#define MLX90614_PWMCTRL 0x22
#define MLX90614_TARANGE 0x23
#define MLX90614_EMISS 0x24
#define MLX90614_CONFIG 0x25
#define MLX90614_ADDR 0x0E
#define MLX90614_ID1 0x3C
#define MLX90614_ID2 0x3D
#define MLX90614_ID3 0x3E
#define MLX90614_ID4 0x3F


  uint32_t readID(void);

  double readObjectTempC(void);
  double readAmbientTempC(void);
  double readObjectTempF(void);
  double readAmbientTempF(void);
  void DEMO_MLX90614_process_1ms(void);
  running_e DEMO_MLX90614_statemachine(bool_e ask_for_finish);

  float readTemp(uint8_t reg);




#endif /* LIB_BSP_MLX90614_MLX90614_H_ */
