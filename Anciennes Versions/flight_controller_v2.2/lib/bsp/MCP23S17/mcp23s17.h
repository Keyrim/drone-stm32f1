/* MCP23S17 - drive the Microchip MCP23S17 16-bit Port Extender using SPI
* Copyright (c) 2010 Romilly Cocking
* Released under the MIT License: http://mbed.org/license/mit
*
* version 0.4
*/

#ifndef MCP23S17_H
#define MCP23S17_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup MCP23S17_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @brief  This SPI pins are used on STM32F429-Discovery board
 */
#ifndef MCP23S17_SPI
	#define MCP23S17_SPI           	SPI1
	#define MCP23S17_SPI_PORT		GPIOA
	#define MCP23S17_SPI_MISO_PIN	GPIO_PIN_6
	#define MCP23S17_SPI_MOSI_PIN	GPIO_PIN_7
	#define MCP23S17_SPI_SCK_PIN	GPIO_PIN_5
#endif

/**
 * @brief  CS PIN for SPI, used as on STM32F429-Discovery board
 */
#ifndef MCP23S17_CS_PIN
#define MCP23S17_CS_PORT       GPIOA
#define MCP23S17_CS_PIN        GPIO_PIN_12
#endif

#define INTERRUPT_POLARITY_BIT 0x02
#define INTERRUPT_MIRROR_BIT 0x40

// all register addresses assume IOCON.BANK = 0 (POR default)

#define MCP23S17_IODIRA 0x00
#define MCP23S17_GPINTENA 0x04
#define MCP23S17_DEFVALA 0x06
#define MCP23S17_INTCONA 0x08
#define MCP23S17_IOCON 0x0A
#define MCP23S17_GPPUA 0x0C
#define MCP23S17_GPIOA 0x12
#define MCP23S17_OLATA 0x14

// Control settings

#define IOCON_BANK 0x80 // Banked registers
#define IOCON_BYTE_MODE 0x20 // Disables sequential operation. If bank = 0, operations toggle between A and B registers
#define IOCON_HAEN 0x08 // Hardware address enable

typedef enum Polarity { ACTIVE_LOW , ACTIVE_HIGH } Polarity;
typedef enum Port { PORT_A, PORT_B } Port;

extern void MCP23S17_init(void);
extern void MCP23S17_direction(Port port, char direction);
extern void MCP23S17_configurePullUps(Port port, char offOrOn);
extern void MCP23S17_interruptEnable(Port port, char interruptsEnabledMask);
extern void MCP23S17_mirrorInterrupts(bool mirror);
extern void MCP23S17_interruptPolarity(Polarity polarity);
extern void MCP23S17_defaultValue(Port port, char valuesToCompare);
extern void MCP23S17_interruptControl(Port port, char interruptContolBits);
extern void MCP23S17_write(Port port, char byte);
extern char MCP23S17_read(Port port);

#ifdef __cplusplus
}
#endif


#endif
