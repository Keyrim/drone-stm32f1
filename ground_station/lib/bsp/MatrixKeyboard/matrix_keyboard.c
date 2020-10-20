/*
 * matrix_keyboard.c
 *
 *  Created on: 24 août 2015
 *      Author: S. Poiraud
 */
#include "config.h"
#if USE_MATRIX_KEYBOARD
#include "matrix_keyboard.h"
#include "systick.h"

#ifdef CONFIG_PULL_UP
	#define DEFAULT_STATE 		(TRUE)
	#define CONFIG_PULL	GPIO_PULLUP
#else
	#ifdef CONFIG_PULL_DOWN
		#define DEFAULT_STATE 	(FALSE)
		#define CONFIG_PULL	GPIO_PULLDOWN
	#else
		#error "Vous devez définir CONFIG_PULL_UP ou CONFIG_PULL_DOWN"
	#endif
#endif

//Portage...
#define GPIO_SET_OUTPUT(port, pin)	HAL_GPIO_Init(port, &(GPIO_InitTypeDef){pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH})
#define GPIO_SET_INPUT(port, pin)	HAL_GPIO_Init(port, &(GPIO_InitTypeDef){pin, GPIO_MODE_INPUT, CONFIG_PULL, GPIO_SPEED_FREQ_HIGH})
#define GPIO_WRITE					HAL_GPIO_WritePin
#define GPIO_READ					HAL_GPIO_ReadPin


//Fonctions privées
static char KEYBOARD_touch_to_key(uint32_t touchs_pressed);
static uint32_t KEYBOARD_read_all_touchs(void);
static uint8_t  KEYBOARD_get_inputs(void);
static void KEYBOARD_write_bit_output(uint8_t bit);

//Disposition des touches sur le clavier. (attention, ne correspond pas forcément à la disposition physique dans le bon ordre !)
const char default_keyboard_keys[16] = {
								'D','#','0','*',
								'C','9','8','7',
								'B','6','5','4',
								'A','3','2','1' };

char * keyboard_keys;
static bool_e initialized = FALSE;

/*
@function	Initialise le module keyboard.
@post		Les ports concernés sont configurés en entrée ou en sortie.
*/
void KEYBOARD_init(const char * new_keyboard_keys)
{
	MATRIX_KEYBOARD_HAL_CLOCK_ENABLE();
	GPIO_SET_INPUT(PORT_INPUT_0,PIN_INPUT_0);
	GPIO_SET_INPUT(PORT_INPUT_1,PIN_INPUT_1);
	GPIO_SET_INPUT(PORT_INPUT_2,PIN_INPUT_2);
	GPIO_SET_INPUT(PORT_INPUT_3,PIN_INPUT_3);

	KEYBOARD_write_bit_output(DEFAULT_STATE);
	if(new_keyboard_keys)
		keyboard_keys = (char *)new_keyboard_keys;
	else
		keyboard_keys = (char *)default_keyboard_keys;
	initialized = TRUE;
}





//Les positions des touches sur ce clavier sont probablement fausses...
//	mais elles permettent de montrer l'utilisation de la fonction KEYBOARD_init(const char * new_keyboard_keys);
const char custom_keyboard[16] =  {
										'0','4','8','C',
										'1','5','9','D',
										'2','6','A','E',
										'3','7','B','F' };
const char custom_keyboard_12_touchs[16]  =  {
										'1','2','3','X',
										'4','5','6','X',
										'7','8','9','X',
										'*','0','#','X' };

static volatile uint32_t t = 0;

//Cette fonction doit être appelée dans la boucle de tâche de fond
running_e DEMO_matrix_keyboard_process_main (bool_e ask_for_finish)
{
	typedef enum
	{
		INIT = 0,
		RUN
	}state_e;
	static state_e state = INIT;
	running_e ret;
	char press_key_event, release_key_event;
	uint32_t all_touch_pressed;

	ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:
			Systick_add_callback_function(DEMO_matrix_keyboard_process_1ms);

			//A modifier en fonction du clavier utilisé : par défaut, personnalisé ou personnalisé 12 touches
			//KEYBOARD_init(NULL);						//Initialisation du clavier avec le clavier par défaut
			//KEYBOARD_init(custom_keyboard);			//Initialisation du clavier avec un clavier personnalisé
			KEYBOARD_init(custom_keyboard_12_touchs);	//Initialisation du clavier avec un clavier personnalisé 12 touches

			printf("To run this demo, you should plug a matrix keyboard on the right ports. See matrix_keyboard.h\n");
			printf("Warning: the defaults ports used by this drivers are not compatible with the presence of SD card or Motors!\n");
			state = RUN;
			break;
		case RUN:
			if(!t)	//A chaque fois que t vaut 0 (toutes les 10ms)...
			{
				t = 10;							//[ms] On recharge le chronomètre t pour 10ms...
				KEYBOARD_press_and_release_events(&press_key_event, &release_key_event, &all_touch_pressed);
				switch(press_key_event)
				{
					case NO_KEY:
						break;
					case MANY_KEYS:
						printf("Many keys pressed : %lx\n", all_touch_pressed);
						break;
					default:
						printf("%c pressed\n", press_key_event);
						break;
				}
				switch(release_key_event)
				{
					case NO_KEY:
						break;
					case MANY_KEYS:
						printf("Many keys released : %lx\n", all_touch_pressed);
						break;
					default:
						printf("%c released\n", release_key_event);
						break;
				}
			}
			if(ask_for_finish)
			{
				state = INIT;
				Systick_remove_callback_function(DEMO_matrix_keyboard_process_1ms);
				ret = END_OK;
			}
			break;
		default:
			break;
	}
	return ret;
}

//Cette fonction doit être appelée toutes les ms.
void DEMO_matrix_keyboard_process_1ms(void)
{
	if(t)		//Si le chronomètre est "chargé", on décompte... (comme un minuteur de cuisine !)
		t--;
}



/*
@function	Vérifie qu'il y a appui sur une touche ou non.
@return		TRUE si une touche est appuyée, FALSE sinon.
*/
bool_e KEYBOARD_is_pressed(void)
{
	uint8_t ret;
	if(!initialized)
		return FALSE;

	KEYBOARD_write_bit_output(!DEFAULT_STATE);

	//Delay

	//Si l'un des ports n'est pas dans l'état par défaut, c'est qu'une touche est pressée.
	ret = (	(GPIO_READ(PORT_INPUT_0,PIN_INPUT_0) != DEFAULT_STATE) ||
			(GPIO_READ(PORT_INPUT_1,PIN_INPUT_1) != DEFAULT_STATE) ||
			(GPIO_READ(PORT_INPUT_2,PIN_INPUT_2) != DEFAULT_STATE) ||
			(GPIO_READ(PORT_INPUT_3,PIN_INPUT_3) != DEFAULT_STATE));

	KEYBOARD_write_bit_output(DEFAULT_STATE);

	return ret;
}


/*
@function	Cette fonction renvoie le caractère de la touche pressée à chaque nouvel appui, ainsi que le caractère relâché à chaque relâchement.
@onte		Il est recommandé d'appeler cette fonction toutes les 10ms. (à des fins d'anti rebond logiciel, et pour ne louper aucun évènement).
@return 	Retourne 0 si aucun évènement d'appui. (soit pour 99,9% des appels...à 0,1% près)
@param		all_touchs_pressed permet de récupérer l'état de l'ensemble du clavier (1 bit par touche)
@pre		KEYBOARD_init() doit avoir été appelée avant.
@post		attention, si deux évènement d'appuis au moins sont simultanés, press_event reçoit l'information MANY_KEYS
@post		attention, si deux évènement de relachement au moins sont simultanés, release_event reçoit l'information MANY_KEYS
*/
void KEYBOARD_press_and_release_events(char * press_event, char * release_event, uint32_t * all_touchs_pressed)
{
	static uint32_t previous_touchs = 0;
	uint32_t current_touchs, up_touchs, down_touchs;

	if(!initialized)
		return;

	current_touchs = KEYBOARD_read_all_touchs();
	down_touchs = ~current_touchs & previous_touchs;
	up_touchs = current_touchs & ~previous_touchs;
	previous_touchs = current_touchs;

	*all_touchs_pressed = current_touchs;
	*press_event = KEYBOARD_touch_to_key(up_touchs);
	*release_event = KEYBOARD_touch_to_key(down_touchs);
}

/*
@function	Renvoi le code ASCII de la touche pressée. En correspondance avec le tableau de codes ASCII.
@post		Cette fonction intègre un anti-rebond
@pre		Il est conseillé d'appeler cette fonction périodiquement (10ms par exemple)
@return		Retourne le caractère ASCII si UNE touche est pressée. Sinon, renvoie 0.
*/
char KEYBOARD_get_key(void)
{
	if(!initialized)
		return FALSE;
	uint32_t touchs_pressed;
	touchs_pressed = KEYBOARD_read_all_touchs();
	return KEYBOARD_touch_to_key(touchs_pressed);
}

static char KEYBOARD_touch_to_key(uint32_t touchs_pressed)
{
	uint32_t index;
	if(touchs_pressed == 0)
		return NO_KEY;
	else if((touchs_pressed & (touchs_pressed-1)))
		return MANY_KEYS;
	else
	{
		//touchs_pressed ne contient qu'un seul bit à 1
		for(index = 0; index < 16; index++)
		{
			if(touchs_pressed == 1u<<index)
				return keyboard_keys[index];
		}
	}
	return MANY_KEYS;	//never happen.
}

static uint32_t KEYBOARD_read_all_touchs(void)
{
	uint32_t ret;
	uint8_t i;
	ret = 0;

	for(i=0;i<4;i++)
	{
		KEYBOARD_write_bit_output(DEFAULT_STATE);
		switch(i)
		{
			case 0:
				GPIO_SET_OUTPUT(PORT_OUTPUT_0,PIN_OUTPUT_0);
				GPIO_WRITE(PORT_OUTPUT_0,PIN_OUTPUT_0,!DEFAULT_STATE);
				break;
			case 1:
				GPIO_SET_OUTPUT(PORT_OUTPUT_1,PIN_OUTPUT_1);
				GPIO_WRITE(PORT_OUTPUT_1,PIN_OUTPUT_1,!DEFAULT_STATE);
				break;
			case 2:
				GPIO_SET_OUTPUT(PORT_OUTPUT_2,PIN_OUTPUT_2);
				GPIO_WRITE(PORT_OUTPUT_2,PIN_OUTPUT_2,!DEFAULT_STATE);
				break;
			case 3:
				GPIO_SET_OUTPUT(PORT_OUTPUT_3,PIN_OUTPUT_3);
				GPIO_WRITE(PORT_OUTPUT_3,PIN_OUTPUT_3,!DEFAULT_STATE);
				break;
			default:
				break;	//Ne doit pas se produire.
		}
		//Acquisition entrées.
		ret |= (uint32_t)(KEYBOARD_get_inputs()) << (4*i);
	}
	//printf("t%lx\n",ret);
	return ret;
}

/*
@function 	Fonction privée. Retourne la lecture des entrées si UNE entrée est à l'état pressée. Sinon, renvoie NO_KEY.
*/
static uint8_t  KEYBOARD_get_inputs(void)
{
	uint32_t inputs;
	inputs = 	((GPIO_READ(PORT_INPUT_0,PIN_INPUT_0) != DEFAULT_STATE)?1u:0)|
				((GPIO_READ(PORT_INPUT_1,PIN_INPUT_1) != DEFAULT_STATE)?2u:0)|
				((GPIO_READ(PORT_INPUT_2,PIN_INPUT_2) != DEFAULT_STATE)?4u:0)|
				((GPIO_READ(PORT_INPUT_3,PIN_INPUT_3) != DEFAULT_STATE)?8u:0);
	return (uint8_t)inputs;
}

static void KEYBOARD_write_bit_output(uint8_t bit)
{
	if(bit == DEFAULT_STATE)
	{
		GPIO_SET_INPUT(PORT_OUTPUT_0,PIN_OUTPUT_0);
		GPIO_SET_INPUT(PORT_OUTPUT_1,PIN_OUTPUT_1);
		GPIO_SET_INPUT(PORT_OUTPUT_2,PIN_OUTPUT_2);
		GPIO_SET_INPUT(PORT_OUTPUT_3,PIN_OUTPUT_3);
	}
	else
	{
		GPIO_WRITE(PORT_OUTPUT_0,PIN_OUTPUT_0,bit);
		GPIO_WRITE(PORT_OUTPUT_1,PIN_OUTPUT_1,bit);
		GPIO_WRITE(PORT_OUTPUT_2,PIN_OUTPUT_2,bit);
		GPIO_WRITE(PORT_OUTPUT_3,PIN_OUTPUT_3,bit);
		GPIO_SET_OUTPUT(PORT_OUTPUT_0,PIN_OUTPUT_0);
		GPIO_SET_OUTPUT(PORT_OUTPUT_1,PIN_OUTPUT_1);
		GPIO_SET_OUTPUT(PORT_OUTPUT_2,PIN_OUTPUT_2);
		GPIO_SET_OUTPUT(PORT_OUTPUT_3,PIN_OUTPUT_3);
	}
}
#endif
