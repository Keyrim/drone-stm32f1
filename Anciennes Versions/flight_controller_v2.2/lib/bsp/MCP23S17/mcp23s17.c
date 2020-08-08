/* MCP23S17 - drive the Microchip MCP23S17 16-bit Port Extender using SPI
 * Copyright (c) 2010 Romilly Cocking
 * Released under the MIT License: http://mbed.org/license/mit
 *
 * version 0.4
 */
#include "config.h"
#if USE_MCP23S17
#include "mcp23s17.h"

#ifdef COX_PORT
#include "xhw_types.h"
#include "xhw_memmap.h"
#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "xspi.h"
#else
#include "stm32f1xx_hal.h"
#include "stm32f1_spi.h"
#include "macro_types.h"
#include "stm32f1_gpio.h"

#endif

#define WRITEOPCODE 0x00

#define _readOpcode  (WRITEOPCODE  )
#define _writeOpcode (WRITEOPCODE|1)

//============================================================================
//                          Low layer functions
//                                 Begin
//  If you want to port OLED Driver to other platform, please rewrite follow
//  functions
//
//  1. __InitialComm -->  to initialize hardware port/pin.
//  2. __CSPinSet    -->  contorl Power pin, input parameter can be 0/1
//  3. __SPIWrite    -->  to write a byte to SPI bus
//
//============================================================================
static void MCP23S17_InitialComm (void)
{
#ifdef COX_PORT
    //
    // Configure system clock frequency to 72MHz
    // 8MHz external crystal ocsillator
    // Note: You can comment this code if you have initialize the system clock
    //       in the other place.
    // xSysCtlClockSet(72000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);

    //--------------------------------------------------------------------------------
    // Initialize GPIO port
    xSysCtlPeripheralEnable( xSYSCTL_PERIPH_GPIOD );

    //
    // Set GPIO port D pin 3 , 4 output mode.
    // PD3 is Power pin
    // PD4 is Data pin
    //
    xGPIODirModeSet( xGPIO_PORTD_BASE, xGPIO_PIN_3, xGPIO_DIR_MODE_OUT );
    xGPIODirModeSet( xGPIO_PORTD_BASE, xGPIO_PIN_4, xGPIO_DIR_MODE_OUT );

    //--------------------------------------------------------------------------------
    // Initialize SPI port
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOA);
    //
    // Enable Peripheral SPI1++
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SPI1);

    //
    // Configure Some GPIO pins as SPI Mode
    //
    xSPinTypeSPI(SPI1CLK(1)  , PA5);
    xSPinTypeSPI(SPI1MOSI(1) , PA7);
    xSPinTypeSPI(SPI1MISO(1) , PA6);
    xSPinTypeSPI(SPI1CS(1)   , PA4);

    xSPIConfigSet(SPI1_BASE, 200000, xSPI_MOTO_FORMAT_MODE_0 | xSPI_DATA_WIDTH8 |
            xSPI_MSB_FIRST | xSPI_MODE_SLAVE);
#else
	/* Init CS pin */
	BSP_GPIO_PinCfg(MCP23S17_CS_PORT,MCP23S17_CS_PIN,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_MEDIUM);
	/* Init SPI */
	SPI_Init(MCP23S17_SPI);
#endif

}

static void MCP23S17_CSPinSet    ( int PinValue )
{
#ifdef COX_PORT
    // Note: CS is controled by SPI hardware
    // So, we do nothing here

    // Avoid compiler warning
    (void) PinValue;
#else
    HAL_GPIO_WritePin(MCP23S17_CS_PORT, MCP23S17_CS_PIN, PinValue);
#endif
}

static int  MCP23S17_SPIWrite ( int value )
{
#ifdef COX_PORT
    return (int) xSPISingleDataReadWrite(xSPI1_BASE, value);
#else
    SPI_WriteNoRegister(MCP23S17_SPI, value);
    return SPI_ReadNoRegister(MCP23S17_SPI);
#endif
    return 0;
}

static char MCP23S17_read_1(char address)
{
    char result = 0;

    //_ncs = 0;
    MCP23S17_CSPinSet(0);
    MCP23S17_SPIWrite(_readOpcode);
    MCP23S17_SPIWrite(address);
    result = MCP23S17_SPIWrite(0);
    //_ncs = 1;
    MCP23S17_CSPinSet(1);

    return result;
}

static char MCP23S17_read_2(Port port, char address)
{
    return MCP23S17_read_1(address + (char) port);
}

static void MCP23S17_write_2(char address, char data)
{
    //_ncs = 0;
	MCP23S17_CSPinSet(0);
	MCP23S17_SPIWrite(_writeOpcode);
	MCP23S17_SPIWrite(address);
	MCP23S17_SPIWrite(data);
    //_ncs = 1;
	MCP23S17_CSPinSet(1);
}

static void  MCP23S17_write_3(Port port, char address, char data)
{
	MCP23S17_write_2(address + (char) port, data);
}

void MCP23S17_init(void)
{
    //! Init the SPI communication port
	MCP23S17_InitialComm();
    // Hardware addressing on, operations toggle between A and B registers
    MCP23S17_write_2(MCP23S17_IOCON, (IOCON_BYTE_MODE | IOCON_HAEN ));
}

/*
 * Controls the direction of the data I/O.
 * When a bit is set, the corresponding pin becomes an
 * input. When a bit is clear, the corresponding pin
 * becomes an output.
 * (cf Datasheet page 18)
 */
void MCP23S17_direction(Port port, char direction)
{
	MCP23S17_write_3(port, MCP23S17_IODIRA, direction);
}

/*
 * The GPPU register controls the pull-up resistors for the
 * port pins. If a bit is set and the corresponding pin is
 * configured as an input, the corresponding port pin is
 * internally pulled up with a 100 kOhm resistor.
 * (cf Datasheet page 22)
 */
void MCP23S17_configurePullUps(Port port, char offOrOn)
{
	MCP23S17_write_3(port, MCP23S17_GPPUA, offOrOn);
}

/* The GPINTEN register controls the
 * interrupt-on-change feature for each pin.
 * If a bit is set, the corresponding pin is enabled for
 * interrupt-on-change.
 * (cf Datasheet page 19)
 */
void MCP23S17_interruptEnable(Port port, char interruptsEnabledMask)
{
	MCP23S17_write_3(port, MCP23S17_GPINTENA, interruptsEnabledMask);
}

/*
 * 	The MIRROR bit controls how the INTA and INTB pins
 * 	function with respect to each other.
 *	• When MIRROR = 1, the INTn pins are functionally
 *	OR’ed so that an interrupt on either port will cause
 *	both pins to activate.
 *	• When MIRROR = 0, the INT pins are separated.
 *	Interrupt conditions on a port will cause its
 *	respective INT pin to activate
 *(cf Datasheet page 20)
 */
void MCP23S17_mirrorInterrupts(bool mirror)
{
    char iocon = MCP23S17_read_1(MCP23S17_IOCON);
    if (mirror)
    {
        iocon = iocon | INTERRUPT_MIRROR_BIT;
    }
    else
    {
        iocon = iocon & ~INTERRUPT_MIRROR_BIT;
    }
    MCP23S17_write_2(MCP23S17_IOCON, iocon);

}


/*The Interrupt Polarity (INTPOL) sets the polarity of the
 *INT pin. This bit is functional only when the ODR bit is
 *cleared, configuring the INT pin as active push-pull.
 * (cf Datasheet page 21)
 */
void  MCP23S17_interruptPolarity(Polarity polarity)
{
    char iocon = MCP23S17_read_1(MCP23S17_IOCON);
    if (polarity == ACTIVE_LOW)
    {
        iocon = iocon & ~INTERRUPT_POLARITY_BIT;
    }
    else
    {
        iocon = iocon | INTERRUPT_POLARITY_BIT;
    }
    MCP23S17_write_2(MCP23S17_IOCON, iocon);
}

void MCP23S17_defaultValue(Port port, char valuesToCompare)
{
	MCP23S17_write_3(port, MCP23S17_DEFVALA, valuesToCompare);
}

void MCP23S17_interruptControl(Port port, char interruptContolBits)
{
	MCP23S17_write_3(port, MCP23S17_INTCONA, interruptContolBits);
}

void MCP23S17_write(Port port, char byte)
{
	MCP23S17_write_3(port, MCP23S17_OLATA, byte);
}

char MCP23S17_read(Port port)
{
    return MCP23S17_read_2(port, MCP23S17_GPIOA);
}
#endif
