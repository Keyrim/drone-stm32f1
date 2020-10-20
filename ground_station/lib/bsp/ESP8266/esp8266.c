/**
 * stm32f1_esp8266.c
 *
 *  Created on: 8 Décembre 2016
 *      Author: D. COUET
 */

/*
 *  Fichier source permettant d'utiliser le module wifi ESP8266. Ce fichier a été réalisé sur un ESP8266-201
 *
 *	Exemple de branchement fonctionnel :
 *	STM32F103		-		ESP8266
 *		PB6			-		 RX			//Si utilisation de l'UART1 !
 *		PB7			-		 TX			//Si utilisation de l'UART1 !
 *		3.3V		- 		 3.3V
 *		GND			-		 GND
 *		A9			-		 CHIP_EN
 *		GND			-		 IO15
 *
 *	Documentation utile :
 *	http://www.fais-le-toi-meme.fr/fr/electronique/materiel/esp8266-arduino-wifi-2-euros
 *	http://www.instructables.com/id/Getting-Started-with-the-ESP8266-ESP-12/?ALLSTEPS
 *	http://www.espressif.com/sites/default/files/4a-esp8266_at_instruction_set_en_v1.5.4_0.pdf
 *
 *	/!\ Dans votre terminal UART, pensez à mettre l'option fin de ligne en CR + LF
 *	/!\ Dans vos printf, de même, vous devez finir par \r\n
 *	Terminal UART utilisable : Docklight pour windows (disponible sur les PC de l'école)
 *							   Moserial pour Linux
 */
#include "config.h"
#ifdef USE_ESP8266
#include "esp8266.h"
#include "stm32f1_uart.h"
#include "stm32f1_gpio.h"
#ifndef ESP8266_UART_ID		//Vous pouvez définir cette macro dans config.h si vous souhaitez changer la valeur par défaut.
	#define ESP8266_UART_ID		UART1_ID
#endif


void ESP8266_demo(void)
{
	ESP8266_init();
	ESP8266_exemple("IoT","!IotEseo49");
	while(1)
	{
		HAL_Delay(1000);
		UART_puts(ESP8266_UART_ID,"GET /update?api_key=QNI517W61UOC40KF&field1=3530&field2=3530&field3=3530\r\n",0);
	}
}

/*
 * Cette fonction met la broche CHIP_EN à 1 et initialise l'UART nécessaire à l'utilisation du module ESP8266.
 * ESP8266_UART_ID : 115200 bauds/secondes (92kbits/s) (Si l'UART1 est choisi : PB6 : Tx  | PB7 : Rx)
 */
void ESP8266_init(void)
{
	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_9,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);
	UART_init(ESP8266_UART_ID,115200);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,GPIO_PIN_SET);
}


/*
 * Fonction d'exemple très sommaire et ne respecant aucune bonne pratique de programmation.
 * Cette fonction envoi des commandes directement depuis le code.
 * Les Delay sont là pour laisser le temps à la carte d'effectuer ses actions.
 * 	  Ce qui convenons le... n'est pas très propre.
 * 	  Il faudrait analyser les réponses du module ESP8266 et traiter chaque étape dans une machine à états !
 * Cet exemple provient du site : http://www.instructables.com/id/Getting-Started-with-the-ESP8266-ESP-12/?ALLSTEPS
 * Il vous permet d'envoyer des données sur un serveur et de pouvoir les visualiser ensuite.
 */
void ESP8266_exemple(char* nomWifi, char* password)
{
	char buf[100];
	HAL_Delay(2000);
	UART_puts(ESP8266_UART_ID,(uint8_t*)"AT\r\n",0);
	HAL_Delay(2000);
	UART_puts(ESP8266_UART_ID,(uint8_t*)"AT+CWMODE=3\r\n",0);
	HAL_Delay(2000);
	UART_puts(ESP8266_UART_ID,(uint8_t*)"AT+CWLAP\r\n",0);
	HAL_Delay(2000);
	sprintf(buf,"AT+CWJAP=\"%s\",\"%s\"\r\n",nomWifi,password);
	UART_puts(ESP8266_UART_ID,(uint8_t*)buf,0);
	HAL_Delay(10000);
	UART_puts(ESP8266_UART_ID,(uint8_t*)"AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n",0);
	HAL_Delay(2000);
	// indique la taille de la chaîne qui va Ãªtre envoyé ensuite
	UART_puts(ESP8266_UART_ID,(uint8_t*)"AT+CIPSEND=76\r\n",0);
	HAL_Delay(2000);
	// Envoie des valeurs pour field1, field2 et field3 que vous pourrez ensuite visualiser sur le site.
	// Vous pouvez changer les valeurs, mais attention à bien changer la taille de chaîne en fonction si besoin.
	UART_puts(ESP8266_UART_ID,(uint8_t*)"GET /update?api_key=QNI517W61UOC40KF&field1=3530&field2=3530&field3=3530\r\n",0);
	HAL_Delay(2000);
	UART_puts(ESP8266_UART_ID,(uint8_t*)"\r\n",0);
	HAL_Delay(10000);
}


/*
 * TODO : machine à état pour accélérer et fiabiliser la phase d'init
 * TODO : autres modes de fonctionnement, pour héberger une page web...
 * TODO : ...
 *
 *
 */
#endif
