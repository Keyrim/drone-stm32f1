/** ----------------  Defines possibles  --------------------
 *	USE_MCP23017				: Activation de le TCA9548A
 *	MCP23017_NB_IC				: Nombre maximum de capteur de type TCA9548A
 */

#ifndef MCP23017_H_
	#define MCP23017_H_

#include "config.h"
#include "macro_types.h"
#include "stm32f1_i2c.h"
	#if USE_MCP23017

		#ifndef MCP23017_NB_IC
			#define MCP23017_NB_IC	2
		#endif

		typedef uint8_t MCP23017_address_t;

		#define MCP23017_ID_ERROR		0xFF
		typedef uint8_t MCP23017_id_t;

		typedef enum{
			MCP23017_POLARITY_ACTIVE_LOW,
			MCP23017_POLARITY_ACTIVE_HIGH
		}MCP23017_polarity_e;

		typedef enum{
			MCP23017_PORT_A,
			MCP23017_PORT_B
		}MCP23017_port_e;

		typedef enum{
			MCP23017_PIN_0	= 0b00000001,
			MCP23017_PIN_1	= 0b00000010,
			MCP23017_PIN_2	= 0b00000100,
			MCP23017_PIN_3	= 0b00001000,
			MCP23017_PIN_4	= 0b00010000,
			MCP23017_PIN_5	= 0b00100000,
			MCP23017_PIN_6	= 0b01000000,
			MCP23017_PIN_7	= 0b10000000
		}MCP23017_pin_e;

		typedef enum{
			MCP23017_PIN_STATE_LOW,
			MCP23017_PIN_STATE_HIGH
		}MCP23017_pinState_e;

		typedef enum{
			MCP23017_PULL_UP_STATE_LOW,
			MCP23017_PULL_UP_STATE_HIGH
		}MCP23017_pullUpState_e;

		typedef enum{
			MCP23017_DIRECTION_INPUT,
			MCP23017_DIRECTION_OUTPUT
		}MCP23017_direction_e;

		bool_e MCP23017_init();

		MCP23017_id_t MCP23017_add(I2C_TypeDef* I2Cx, MCP23017_address_t address);

		bool_e MCP23017_setIO(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_direction_e direction);
		bool_e MCP23017_getIO(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_direction_e * direction);

		bool_e MCP23017_setGPIO(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_pinState_e state);
		bool_e MCP23017_getGPIO(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_pinState_e * state);

		bool_e MCP23017_setPullUp(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_pullUpState_e state);
		bool_e MCP23017_getPullUp(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_pullUpState_e * state);

	#endif

#endif /* MCP23017_H_ */
