
#include "mcp23017.h"
#include "stm32f1_i2c.h"

#if USE_MCP23017

	#define MCP23017_MAX_NB_ERROR	3

	#define DEFAULT_CLOCK_SPEED		400000

	typedef enum{	// Système de registre en mode IOCON.BANK = 0
		MPC23017_REGISTER_IODIR_A		= 0x00,
		MPC23017_REGISTER_IODIR_B		= 0x01,

		MPC23017_REGISTER_IPOL_A		= 0x02,
		MPC23017_REGISTER_IPOL_B		= 0x03,

		MPC23017_REGISTER_GPINTEN_A		= 0x04,
		MPC23017_REGISTER_GPINTEN_B		= 0x05,

		MPC23017_REGISTER_DEFVAL_A		= 0x06,
		MPC23017_REGISTER_DEFVAL_B		= 0x07,

		MPC23017_REGISTER_INTCON_A		= 0x08,
		MPC23017_REGISTER_INTCON_B		= 0x09,

		MPC23017_REGISTER_IOCON_A		= 0x0A,
		MPC23017_REGISTER_IOCON_B		= 0x0B,

		MPC23017_REGISTER_GPPU_A		= 0x0C,
		MPC23017_REGISTER_GPPU_B		= 0x0D,

		MPC23017_REGISTER_INTF_A		= 0x0E,
		MPC23017_REGISTER_INTF_B		= 0x0F,

		MPC23017_REGISTER_INTCAP_A		= 0x10,
		MPC23017_REGISTER_INTCAP_B		= 0x11,

		MPC23017_REGISTER_GPIO_A		= 0x12,
		MPC23017_REGISTER_GPIO_B		= 0x13,

		MPC23017_REGISTER_OLAT_A		= 0x14,
		MPC23017_REGISTER_OLAT_B		= 0x15

	}MCP23017_register_e;

	typedef enum{
		MCP23017_REG_IODIR_OUTPUT	= 0,
		MCP23017_REG_IODIR_INPUT	= 1
	}MCP23017_reg_iodir_e;

	typedef enum{
		MCP23017_REG_IPOL_SAME_POLARITY		= 0,
		MCP23017_REG_IPOL_OPPOSITE_POLARITY	= 1
	}MCP23017_reg_ipol_e;

	typedef enum{
		MCP23017_REG_GPITEN_DISABLE	= 0,
		MCP23017_REG_GPITEN_ENABLE	= 1
	}MCP23017_reg_gpiten_e;

	/*typedef enum{
		MCP23017_REG_DEFVAL_DISABLE	= 0,
		MCP23017_REG_DEFVAL_ENABLE	= 1
	}MCP23017_reg_defval_e;*/

	typedef enum{
		MCP23017_REG_INTCON_COMP_WITH_PREVIOUS	= 0,
		MCP23017_REG_INTCON_COMP_WITH_DEVFAL	= 1
	}MCP23017_reg_intcon_e;

	typedef union{
		struct{

			enum{
				BANK_PORT_SAME					= 0,
				BANK_PORT_SEPARATED				= 1
			}bank	:1;

			enum{
				INT_PIN_NOT_CONNECTED			= 0,
				INT_PIN_INTERNALLY_CONNECTED	= 1
			}mirror	:1;

			enum{
				SEQUENTIAL_OPERATION_ENABLED	= 0,
				SEQUENTIAL_OPERATION_DISABLED	= 1
			}seqop	:1;

			enum{
				SLEW_RATE_ENABLED				= 0,
				SLEW_RATE_DISABLED				= 1
			}disslw	:1;

			enum{
				ADDRESS_PIN_DISABLE				= 0,
				ADDRESS_PIN_ENABLE				= 1
			}haen	:1;

			enum{
				OUTPUT_ACTIVE_DRIVER			= 0,
				OUTPUT_OPEN_DRAIN				= 1,
			}odr	:1;

			enum{
				POLARITY_INT_PIN_LOW			= 0,
				POLARITY_INT_PIN_HIGH			= 1
			}intpol	:1;

			uint8_t:1;
		};

		uint8_t rawData;

	}MCP23017_reg_iocon_u;

	typedef enum{
		MCP23017_REG_GPPU_PULL_UP_DISABLED	= 0,
		MCP23017_REG_GPPU_PULL_UP_ENABLED	= 1
	}MCP23017_reg_gppu_e;

	typedef enum{
		MCP23017_REG_INTF_INTERRUPT_DISABLED	= 0,
		MCP23017_REG_INTF_INTERRUPT_ENABLED		= 1
	}MCP23017_reg_intf_e;

	typedef enum{
		MCP23017_REG_INTCAP_LOGIC_LOW	= 0,
		MCP23017_REG_INTCAP_LOGIC_HIGH	= 1
	}MCP23017_reg_intcap_e;

	typedef enum{
		MCP23017_REG_GPIO_LOGIC_LOW		= 0,
		MCP23017_REG_GPIO_LOGIC_HIGH	= 1
	}MCP23017_reg_gpio_e;

	typedef enum{
		MCP23017_REG_OLAT_LOGIC_LOW		= 0,
		MCP23017_REG_OLAT_LOGIC_HIGH	= 1
	}MCP23017_reg_olat_e;

	typedef struct{
		bool_e used;

		MCP23017_address_t address;
		I2C_TypeDef* I2Cx;
	}MCP23017_ic_t;

	static volatile MCP23017_ic_t MCP23017_ic[MCP23017_NB_IC];

	static HAL_StatusTypeDef MCP23017_initIc(MCP23017_id_t id, I2C_TypeDef* I2Cx, MCP23017_address_t address);

	bool_e MCP23017_init(){

		uint8_t i;
		for(i=0; i<MCP23017_NB_IC; i++){
			MCP23017_ic[i].used = FALSE;
		}

		return TRUE;
	}

	/*address : 0b000 to 0b111 */
	MCP23017_id_t MCP23017_add(I2C_TypeDef* I2Cx, MCP23017_address_t address){

		uint8_t i;
		for(i=0; i<MCP23017_NB_IC; i++){
			if(MCP23017_ic[i].used == FALSE){
				if(MCP23017_initIc(i, I2Cx, address) == HAL_OK){
					printf("MCP23017_add : initialisation du capteur (address : %d | id : %d) réussi\n", MCP23017_ic[i].address, i);
				}else{
					printf("MCP23017_add : initialisation du capteur (address : %d) echoué\n", MCP23017_ic[i].address);
					return MCP23017_ID_ERROR;
				}
				return i;
			}


		}

		printf("MCP23017_add : Erreur nombre de capteur trop elevé (address : %d)\n", MCP23017_ic[i].address);

		return MCP23017_ID_ERROR;
	}



	bool_e MCP23017_setIO(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_direction_e direction){

		if(id >= MCP23017_NB_IC){
			printf("MCP23017_setIO : Erreur identifiant du capteur inconnue (%d)\n", id);
			return FALSE;
		}

		if(MCP23017_ic[id].used == FALSE){
			printf("MCP23017_setIO : Erreur capteur non initialisé (%d)\n", id);
			return FALSE;
		}

		MCP23017_register_e reg;

		if(port == MCP23017_PORT_A){
			reg = MPC23017_REGISTER_IODIR_A;
		}else{
			reg = MPC23017_REGISTER_IODIR_B;
		}

		uint8_t value;

		if(I2C_Read(MCP23017_ic[id].I2Cx, MCP23017_ic[id].address, reg, &value) != HAL_OK){
			printf("MCP23017_setIO : Erreur lecture du registre IODIR (%d) (address chip : %d)\n", reg, MCP23017_ic[id].address);
			return FALSE;
		}

		if(direction == MCP23017_DIRECTION_OUTPUT){
			value = value & (~pin);
		}else{
			value = value | pin;
		}

		if(I2C_Write(MCP23017_ic[id].I2Cx, MCP23017_ic[id].address, reg, value) != HAL_OK){
			printf("MCP23017_setIO : Erreur GPIO Expander (%d) Configuration direction\n", id);
			return FALSE;
		}

		return TRUE;
	}

	bool_e MCP23017_getIO(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_direction_e * direction){

		if(id >= MCP23017_NB_IC){
			printf("MCP23017_getGPIO : Erreur identifiant du capteur inconnue (%d)\n", id);
			return FALSE;
		}

		if(MCP23017_ic[id].used == FALSE){
			printf("MCP23017_getGPIO : Erreur capteur non initialisé (%d)\n", id);
			return FALSE;
		}

		uint8_t value;
		MCP23017_register_e reg;

		if(port == MCP23017_PORT_A){
			reg = MPC23017_REGISTER_IODIR_A;
		}else{
			reg = MPC23017_REGISTER_IODIR_B;
		}
		if(I2C_Read(MCP23017_ic[id].I2Cx, MCP23017_ic[id].address, reg, &value) != HAL_OK){
			printf("MCP23017_setIO : Erreur lecture du registre IODIR (%d) (address chip : %d)\n", reg, MCP23017_ic[id].address);
			return FALSE;
		}
		*direction = (value&pin)?TRUE:FALSE;

		return TRUE;
	}


	bool_e MCP23017_setGPIO(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_pinState_e state){

		if(id >= MCP23017_NB_IC){
			printf("MCP23017_setGPIO : Erreur identifiant du capteur inconnue (%d)\n", id);
			return FALSE;
		}

		if(MCP23017_ic[id].used == FALSE){
			printf("MCP23017_setGPIO : Erreur capteur non initialisé (%d)\n", id);
			return FALSE;
		}

		MCP23017_register_e reg;

		if(port == MCP23017_PORT_A){
			reg = MPC23017_REGISTER_OLAT_A;
		}else{
			reg = MPC23017_REGISTER_OLAT_B;
		}

		uint8_t value;

		if(I2C_Read(MCP23017_ic[id].I2Cx, MCP23017_ic[id].address, reg, &value) != HAL_OK){
			printf("MCP23017_setGPIO : Erreur lecture du registre OLAT(%d) (address chip : %d)\n", reg, MCP23017_ic[id].address);
			return FALSE;
		}

		if(state == MCP23017_PIN_STATE_LOW){
			value = value & (~pin);
		}else{
			value = value | pin;
		}

		if(I2C_Write(MCP23017_ic[id].I2Cx, MCP23017_ic[id].address, reg, value) != HAL_OK){
			printf("MCP23017_setGPIO : Erreur écriture du registre OLAT(%d) (address chip : %d)\n", reg, MCP23017_ic[id].address);
			return FALSE;
		}

		return TRUE;
	}

	bool_e MCP23017_getGPIO(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_pinState_e * state){

		if(id >= MCP23017_NB_IC){
			printf("MCP23017_getGPIO : Erreur identifiant du capteur inconnue (%d)\n", id);
			return FALSE;
		}

		if(MCP23017_ic[id].used == FALSE){
			printf("MCP23017_getGPIO : Erreur capteur non initialisé (%d)\n", id);
			return FALSE;
		}

		MCP23017_register_e reg;

		if(port == MCP23017_PORT_A){
			reg = MPC23017_REGISTER_GPIO_A;
		}else{
			reg = MPC23017_REGISTER_GPIO_B;
		}

		uint8_t value = 0;

		if(I2C_Read(MCP23017_ic[id].I2Cx, MCP23017_ic[id].address, reg, &value) != HAL_OK){
			printf("MCP23017_getGPIO : Erreur lecture du registre GPIO(%d) (address chip : %d)\n", reg, MCP23017_ic[id].address);
			return FALSE;
		}

		*state = (value&pin)?TRUE:FALSE;

		return TRUE;
	}

	bool_e MCP23017_setPullUp(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_pullUpState_e state){

		if(id >= MCP23017_NB_IC){
			printf("MCP23017_setPullUp : Erreur identifiant du capteur inconnue (%d)\n", id);
			return FALSE;
		}

		if(MCP23017_ic[id].used == FALSE){
			printf("MCP23017_setPullUp : Erreur capteur non initialisé (%d)\n", id);
			return FALSE;
		}

		MCP23017_register_e reg;

		if(port == MCP23017_PORT_A){
			reg = MPC23017_REGISTER_GPPU_A;
		}else{
			reg = MPC23017_REGISTER_GPPU_B;
		}

		uint8_t value;

		if(I2C_Read(MCP23017_ic[id].I2Cx, MCP23017_ic[id].address, reg, &value) != HAL_OK){
			printf("MCP23017_setPullUp : Erreur lecture du registre GPPU(%d) (address chip : %d)\n", reg, MCP23017_ic[id].address);
			return FALSE;
		}

		if(state == MCP23017_PULL_UP_STATE_LOW){
			value = value & (~pin);
		}else{
			value = value | pin;
		}

		if(I2C_Write(MCP23017_ic[id].I2Cx, MCP23017_ic[id].address, reg, value) != HAL_OK){
			printf("MCP23017_setPullUp : Erreur écriture du registre GPPU(%d) (address chip : %d)\n", reg, MCP23017_ic[id].address);
			return FALSE;
		}


		return TRUE;
	}

	bool_e MCP23017_getPullUp(MCP23017_id_t id, MCP23017_port_e port, MCP23017_pin_e pin, MCP23017_pullUpState_e * state){

		if(id >= MCP23017_NB_IC){
			printf("MCP23017_getPullUp : Erreur identifiant du capteur inconnue (%d)\n", id);
			return FALSE;
		}

		if(MCP23017_ic[id].used == FALSE){
			printf("MCP23017_getPullUp : Erreur capteur non initialisé (%d)\n", id);
			return FALSE;
		}

		MCP23017_register_e reg;

		if(port == MCP23017_PORT_A){
			reg = MPC23017_REGISTER_GPPU_A;
		}else{
			reg = MPC23017_REGISTER_GPPU_B;
		}

		uint8_t value = 0;

		if(I2C_Read(MCP23017_ic[id].I2Cx, MCP23017_ic[id].address, reg, &value) != HAL_OK){
			printf("MCP23017_getPullUp : Erreur lecture du registre GPPU(%d) (address chip : %d)\n", reg, MCP23017_ic[id].address);
			return FALSE;
		}

		*state = (value&pin)?TRUE:FALSE;

		return TRUE;
	}

	/*address : 0b000 to 0b111 */
	static HAL_StatusTypeDef MCP23017_initIc(MCP23017_id_t id, I2C_TypeDef* I2Cx, MCP23017_address_t address){
		if(id >= MCP23017_NB_IC){
			printf("MCP23017_initIc : Erreur id (%d) non conforme\n", id);
			return FALSE;
		}

		MCP23017_ic[id].address = ((address & 0x07)<<1) | 0x40;
		MCP23017_ic[id].I2Cx = I2Cx;
		MCP23017_ic[id].used = TRUE;

		return I2C_Init(I2Cx, DEFAULT_CLOCK_SPEED);

		/*if(MCP23017_setIO(id, MCP23017_PORT_A, 0xFF, MCP23017_DIRECTION_INPUT) == FALSE){
			MCP23017_ic[id].used = FALSE;
			printf("MCP23017_initIc : Erreur id (%d) configuration direction port A\n", id);
			return FALSE;
		}

		if(MCP23017_setIO(id, MCP23017_PORT_B, 0xFF, MCP23017_DIRECTION_INPUT) == FALSE){
			MCP23017_ic[id].used = FALSE;
			printf("MCP23017_initIc : Erreur id (%d) configuration direction port B\n", id);
			return FALSE;
		}

		if(MCP23017_setPullUp(id, MCP23017_PORT_A, 0xFF, MCP23017_PULL_UP_STATE_LOW) == FALSE){
			MCP23017_ic[id].used = FALSE;
			printf("MCP23017_initIc : Erreur id (%d) configuration pull up port A\n", id);
			return FALSE;
		}

		if(MCP23017_setPullUp(id, MCP23017_PORT_B, 0xFF, MCP23017_PULL_UP_STATE_LOW) == FALSE){
			MCP23017_ic[id].used = FALSE;
			printf("MCP23017_initIc : Erreur id (%d) configuration pull up port B\n", id);
			return FALSE;
		}*/
	}

	//Démo blocante
	/*
	 * Dans cet exemple ; le PORT_A est configuré comme entrée. le PORT_B est configuré en sortie.
	 * On envoie un compteur binaire sur le PORT_B
	 * On affiche les valeurs lues sur le PORT_A sur l'UART (printf)
	 */
	void MCP23017_demo(void)
	{

		MCP23017_id_t id;
		static uint8_t nb = 0;
		MCP23017_pinState_e state;

		MCP23017_init();
		id = MCP23017_add(I2C1, 0b000);	//adresse : (selon les bits A0, A1, A2 : de 0b000 à 0b111).

		MCP23017_setIO(id,MCP23017_PORT_A, 0xFF, MCP23017_DIRECTION_INPUT);
		MCP23017_setIO(id,MCP23017_PORT_B, 0xFF, MCP23017_DIRECTION_OUTPUT);

		MCP23017_setPullUp(id,MCP23017_PORT_A, 0xFF, MCP23017_PULL_UP_STATE_HIGH);

		MCP23017_setGPIO(id,MCP23017_PORT_B, 0x55, MCP23017_PIN_STATE_HIGH);
		MCP23017_setGPIO(id,MCP23017_PORT_B, 0xAA, MCP23017_PIN_STATE_LOW);
		while(1)
		{
			for(uint8_t i=0; i<8; i++)
			{
				MCP23017_getGPIO(id,MCP23017_PORT_A, 1<<i, &state);
				printf("A%d:%x\t", i, state);
			}
			MCP23017_setGPIO(id,MCP23017_PORT_B, nb, MCP23017_PIN_STATE_HIGH);
			nb++;
			HAL_Delay(100);
		}
	}


#endif
