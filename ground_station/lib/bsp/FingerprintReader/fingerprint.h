/**
  ******************************************************************************
  * @file    fingerprint.h
  * @author  Calvin Ageneau
  * @version V1.0
  * @date    26-January-2017
  * @brief   Fingerprint header
  ******************************************************************************
*/

#ifndef FINGERPRINT_H_
#define FINGERPRINT_H_

#if USE_FINGERPRINT

#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "macro_types.h"

#define ADRESSE_CAPTEUR 0xFFFFFFFF
#define MDP_ACCES_CAPTEUR 0

#define DEFAULTTIMEOUT 5000  // milliseconds
	/**
	 * @brief Initialise la communication avec le capteur
	 * @param baud La vitesse de communication en baud
	 * @retval none
	 */
  void fingerprint_Init(unsigned short baud);

    /**
  	 * @brief Verifie l'empreinte recue en fonction de celles enregistrees dans la base de donnee
  	 * @retval L'ID de l'empreinte si elle est trouvee, -1 sinon
  	 */
  int getFingerprintIDez(void);


  	/**
  	 * @brief Verifie le mot de passe du capteur par rapport a celui configure
  	 * @retval true si le mot de passe correspond, false sinon
  	 */
  bool_e verifyPassword(void);

    /**
  	 * @brief Recupere l'empreinte du doigt pose sur le capteur
  	 * @retval La chaine de caracteres correspondant au resultat de l'operation, -1 si une erreur est trouvee
  	 */
  unsigned char getImage(void);

    /**
  	 * @brief Convertis l'image pour le capteur
  	 * @param slot Slot ou sera enregistre l'image
  	 * @retval La chaine de caracteres correspondant au resultat de l'operation, -1 si une erreur est trouvee
  	 */
  unsigned char image2Tz(unsigned char slot);

    /**
	 * @brief Recherche la correspondance avec l'empreinte renseignee par rapport a la base de donnee
	 * @retval La chaine de caracteres correspondant au resultat de l'operation, -1 si une erreur est trouvee
	 */
  unsigned char fingerFastSearch(void);

    /**
	 * @brief Envois un packet vers le capteur biometrique
	 * @param addr Adresse du capteur
	 * @param packettype Type de packet envoye
	 * @param len Taille du packet a envoyer
	 * @param packet Adresse du packet a envoyer
	 * @retval none
	 */
  void writePacket(unsigned int addr, unsigned char packettype, unsigned short len, unsigned char *packet);

    /**
	 * @brief Recupere la reponse du capteur
	 * @param packet Adresse du packet recupere
	 * @retval La taille du packet, la chaine de caracteres correspondant a l'erreur si une erreur est trouvee
	 */
  unsigned char getReply(unsigned char packet[]);

  /**
   * L'ID d'une empreinte
   */
  unsigned short fingerID;

  /**
   * Valeur de confiance utilisee pour la comparaison entre deux enmpreintes
   */
  unsigned short confidence;

  /**
   * Mot de passe du capteur biometrique (modifiable dans le \c \#define)
   */
  unsigned int thePassword;

  /**
   * Adresse du capteur biometrique (modifiable dans le \c \#define)
   */
  unsigned int theAddress;

  #endif
  
#endif /* FINGERPRINT_H_ */
