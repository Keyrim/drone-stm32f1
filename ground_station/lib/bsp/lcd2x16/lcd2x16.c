/**
 * @addtogroup lcd2x16
 *
 * @{
 */

/**
 * @file lcd2x16_c18.c
 *
 *  Driver d'afficheur LCD 2 lignes de 16 caractères.
 *
 *  @version 1.3
 *  @date 24/08/2015
 */
#include "config.h"
#if USE_SCREEN_LCD2X16

#include "lcd2x16.h"
#include "macro_types.h"
#include <stdarg.h>
#include <stdio.h>

//Portage...
#define GPIO_SET_OUTPUT(port, pin)	HAL_GPIO_Init(port, (GPIO_InitTypeDef*)&(const GPIO_InitTypeDef){pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH})
#define GPIO_SET_INPUT(port, pin)	HAL_GPIO_Init(port, (GPIO_InitTypeDef*)&(const GPIO_InitTypeDef){pin, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH})
#define GPIO_WRITE					HAL_GPIO_WritePin
#define GPIO_READ					HAL_GPIO_ReadPin
#define DELAY_MS(x)					HAL_Delay(x)


static void LCD2X16_delay_us(uint16_t us);
static uint8_t LCD2X16_getByte(uint8_t rs);
static void LCD2X16_sendNibble(unsigned char n);
static running_e LCD2X16_sendByte( unsigned char rs, unsigned char n);

/**
 * Définition du type d'écran LCD.
 * 
 * Valeurs autorisées :
 *    - 0 = 5x7
 *    - 1 = 5x10
 *    - 2 = 2 lignes
 */
#define LCD2X16_TYPE 2

/**
 * Adresse de la deuxième ligne de l'écran LCD.
 *
 * Adresse dans la RAM interne de l'écran LCD du premier caractère de la deuxième ligne.
 */
#define LCD2X16_LINE_TWO_AD 0x40

/**
 * Chaîne d'initialisation de l'écran LCD.
 *
 * Ces 4 octets doivent être envoyés à l'écran LCD lors de la mise sous tension.
 * L'initialisation est effectuée par la fonction LCD16X2_init().
 */
const unsigned char LCD2X16_INIT_STRING[4] = {0x20 | (LCD2X16_TYPE << 2), 0xc, 1, 6};

/*=================================================================================================*/
//

/*
 * Cette fonction blocante a pour but de vous aider à appréhender les fonctionnalités de ce module logiciel.
 */
void LCD2X16_demo(void)
{
	LCD2X16_init();
	LCD2X16_printf("LCD2x16 - demo");
	uint16_t i = 0;
	while(1)
	{
		LCD2X16_setCursor(1,2);
		LCD2X16_printf("%05d", i);
		i++;
		HAL_Delay(100);
	}
}


/**
 * Fonction d'initialisation du LCD.
 *
 * Cette fonction doit être appelée préalablement à toute utilisation de l'écran LCD.
 */
void LCD2X16_init(void)
{
	unsigned char i;
	LCD2X16_HAL_CLOCK_ENABLE();

	// Configuration des ports de commande et de données en écriture
	GPIO_SET_OUTPUT(PORT_RW, PIN_RW);
	GPIO_SET_OUTPUT(PORT_RS, PIN_RS);
	GPIO_SET_OUTPUT(PORT_E, PIN_E);

	GPIO_WRITE(PORT_DATA_0, PIN_DATA_0, 0);
	GPIO_WRITE(PORT_DATA_1, PIN_DATA_1, 0);
	GPIO_WRITE(PORT_DATA_2, PIN_DATA_2, 0);
	GPIO_WRITE(PORT_DATA_3, PIN_DATA_3, 0);
	GPIO_WRITE(PORT_E, PIN_E, 0);
	GPIO_WRITE(PORT_RS, PIN_RS, 0);
	GPIO_WRITE(PORT_RW, PIN_RW, 0);

	/* Emission d'une séquence de données d'initialisation */
	DELAY_MS(40);

	for(i=0;i<3;i++)
	{
		LCD2X16_sendNibble(3);
		DELAY_MS(5);
	}

	LCD2X16_sendNibble(2);

	for(i=0;i<4;i++)
		LCD2X16_sendByte(0,LCD2X16_INIT_STRING[i]);
	DELAY_MS(5);	// > 1.5ms
}



/** Fonction de positionnement du curseur sur le LCD
 *
 *    @param column indice de colonne (1 à 16)
 *    @param line indice de ligne (1 ou 2)
 *    @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel de LCD2X16_init()
 */
void LCD2X16_setCursor( unsigned char column, unsigned char line)
{
   unsigned char address;
   
   // Calcul de l'adresse correspondant aux coordonnées
   if(line!=1)
     address=LCD2X16_LINE_TWO_AD;
   else
     address=0;
   
   address+=column-1;
   
   // Envoi de la commande de positionnement au LCD
   LCD2X16_sendByte(0,0x80|address);
   LCD2X16_delay_us(200);	//  > 38us
}

/** Fonction d'écriture d'un caractère sur le LCD. Cette fonction affiche un caractère ascii sur l'afficheur	
 *  Il existe des caractères de contrôle :																		
 *     - \f  efface l'écran																					
 *     - \n  saut au début de la seconde ligne																	
 *     - \b  retour arrière d'un caractère
 *
 *     @param c le caractêre à afficher
 *     @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel à LCD2X16_init().
 *     @post    les bits du port de données sont configurés en écriture.
 *  */
void LCD2X16_putChar(char c)
{
   switch (c)
   {
      case '\f':  // Effacer l'écran
         LCD2X16_sendByte(0,1);
         DELAY_MS(10);
         break;
         
      case '\n':  // Passer à la ligne suivante
         LCD2X16_setCursor(1,2);
         break;
         
     case '\b':   // Retour au caractère précédent
         LCD2X16_sendByte(0,0x10);
         break;
         
     default:     // Caractère affichable
         LCD2X16_sendByte(1,c);
   }
}


/** Fonction de lecture d'un caractère sur le LCD.
 *
 *    @param x indice de colonne du caractère à lire (1 à 16)
 *    @param y indice de ligne du caractère à lire (1 ou 2)
 *    @return  le caractère lu en (x,y)
 *    @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel de LCD2X16_init()
 */
char LCD2X16_getChar( unsigned char x, unsigned char y)
{
   LCD2X16_setCursor(x,y);
   return LCD2X16_getByte(1);
}


/** Fonction d'affichage d'une chaine formatée (cf printf)
 * 	@param  premier paramètre = chaine de format
 * 	@param  paramètres suivants = selon les balises %x indiquées dans la chaîne de format
 * 	@pre 	La chaine fabriquée ne DOIT PAS excéder 20 caractères (et en pratique 16 caractères pour un écran de 16 caractères par lignes). 
 *  @pre     l'usage de cette fonction doit être précédé au moins une fois de l'appel de LCD2X16_init()
 * 	@post	Si la chaîne fabriquée excède 20 caractères, un message est affiché via printf.
 */
int	LCD2X16_printf(const char *__restrict string, ...)
{
	char buffer[20];
	uint8_t i;
	uint8_t size;
	va_list args_list;
	va_start(args_list, string);
	size = vsnprintf(buffer, 20, string, args_list);
	if(size>21)
		printf("ATTENTION, chaine LCD trop grande !\n");
	va_end(args_list);
	for(i=0;i<size;i++)
		LCD2X16_putChar(buffer[i]);
	return size;
}


/**
 * Cette fonction produit une attente très grossière de l'ordre de grandeur de 'us' microsecondes...
 *   tests effectués pour un STM32F103 cadencé à 64MHz et avec un programme compilé en -O0...
 *
 *    @param  us   durée en us
 */
static void LCD2X16_delay_us(uint16_t us)
{
	volatile int i;	//L'argument volatile permet d'éviter la simplification de la boucle for 'inutile', lors d'une compilation avec optimisation...
	for(i=0;i<2*us;i++);
}


/**
 * Fonction de lecture d'un octet sur le LCD
 *
 *    @param  rs   numéro du registre du LCD (0 ou 1)
 *    @return l'octet lu
 *    @post   les bits du port de données sont configurés en écriture.
 */
static uint8_t LCD2X16_getByte(uint8_t rs)
{
	uint8_t read;

	GPIO_WRITE(PORT_RS, PIN_RS, (rs)?1:0);

	// configuration de l'accès aux données en lecture
	GPIO_SET_INPUT(PORT_DATA_0, PIN_DATA_0);
	GPIO_SET_INPUT(PORT_DATA_1, PIN_DATA_1);
	GPIO_SET_INPUT(PORT_DATA_2, PIN_DATA_2);
	GPIO_SET_INPUT(PORT_DATA_3, PIN_DATA_3);

	GPIO_WRITE(PORT_RW, PIN_RW, 1);
	LCD2X16_delay_us(1);

	GPIO_WRITE(PORT_E, PIN_E, 1);
	LCD2X16_delay_us(10);

	// Lecture du quartet de poids fort
	read  = (GPIO_READ(PORT_DATA_0, PIN_DATA_0)	 << 4) |
			(GPIO_READ(PORT_DATA_1, PIN_DATA_1)  << 5) |
			(GPIO_READ(PORT_DATA_2, PIN_DATA_2)  << 6) |
			(GPIO_READ(PORT_DATA_3, PIN_DATA_3)  << 7);

	GPIO_WRITE(PORT_E, PIN_E, 0);
	LCD2X16_delay_us(1);

	GPIO_WRITE(PORT_E, PIN_E, 1);
	LCD2X16_delay_us(10);

	// Lecture du quartet de poids faible
	read |= (GPIO_READ(PORT_DATA_0, PIN_DATA_0)  << 0) |
			(GPIO_READ(PORT_DATA_1, PIN_DATA_1)  << 1) |
			(GPIO_READ(PORT_DATA_2, PIN_DATA_2)  << 2) |
			(GPIO_READ(PORT_DATA_3, PIN_DATA_3)  << 3);

	GPIO_WRITE(PORT_E, PIN_E, 0);

	// Configuration de l'accès aux données en écriture
	GPIO_SET_OUTPUT(PORT_DATA_0, PIN_DATA_0);
	GPIO_SET_OUTPUT(PORT_DATA_1, PIN_DATA_1);
	GPIO_SET_OUTPUT(PORT_DATA_2, PIN_DATA_2);
	GPIO_SET_OUTPUT(PORT_DATA_3, PIN_DATA_3);

	// Concaténation des deux quartets lus et retour
	return read;
}

/**
 * Fonction d'envoi d'un quartet sur le LCD
 *
 *    @param n le quartet à envoyer (les 4 bits de poids faible de n)
 *    @pre     le port de données doit être configuré en écriture
 */
static void LCD2X16_sendNibble(unsigned char n)
{
	GPIO_WRITE(PORT_DATA_0, PIN_DATA_0, (n   )&0x01);
	GPIO_WRITE(PORT_DATA_1, PIN_DATA_1, (n>>1)&0x01);
	GPIO_WRITE(PORT_DATA_2, PIN_DATA_2, (n>>2)&0x01);
	GPIO_WRITE(PORT_DATA_3, PIN_DATA_3, (n>>3)&0x01);

	GPIO_WRITE(PORT_E, PIN_E, 1);
	LCD2X16_delay_us(2);
	GPIO_WRITE(PORT_E, PIN_E, 0);
	LCD2X16_delay_us(2);
}

/**
 * Fonction d'envoi d'un octet sur le LCD
 *
 *    @param rs   numéro du registre du LCD (0 ou 1)
 *    @param n    octet a envoyer
 *    @pre        le port de données doit être configuré en écriture
 */
static running_e LCD2X16_sendByte( unsigned char rs, unsigned char n)
{
	uint8_t nb_try = 3;
   while((LCD2X16_getByte(0) & 0b10000000) != 0 && nb_try--);

   if(nb_try)
   {
	   GPIO_WRITE(PORT_RS, PIN_RS, (rs)?1:0);
	   GPIO_WRITE(PORT_RW, PIN_RW, 0);
	   LCD2X16_delay_us(1);

	   LCD2X16_sendNibble(n >> 4);
	   LCD2X16_sendNibble(n & 0xf);
	   return END_OK;
   }
   return END_ERROR;
}
	
/**
 * @}
 */
#endif
