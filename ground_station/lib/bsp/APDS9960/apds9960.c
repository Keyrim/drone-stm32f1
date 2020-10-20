/*
 * apds9960.c
 *
 *  Created on: 1 déc. 2016
 *      Author: Cécile Lebrun, portage depuis Arduino (auteur : Shawn Hymel (SparkFun Electronics))
 */

/**
 * Ce pilote permet d'interfacer le capteur de luminosité RGB et de mouvement APDS9960.
 * Ce capteur permet de mesurer la luminosité ambiante, les luminosités de chacune des trois couleurs RGB, mais également de
 * détecter des mouvements tels qu'un "swipe" vers la droite par exemple ou encore de détecter un objet à proximité.
 * Les modes d'uilisation sont les suivants (et peuvent être activés en même temps) :
 *
 ************************************************************************************************************************
 * 	 1.	Mode "Capteur de luminosité" :
 *
 * 	 	    Permet de mesurer la luminosité ambiante et les niveaux de ses trois composantes RGB.
 *		    Il intègre lui même deux modes :
 *
 *			- Mode "Normal" : permet de faire une simple lecture des valeurs.
 *					1. Initialiser le capteur : init()
 *					2. Activer le capteur en mode capteur de luminosité : enableLightSensor(FALSE)
 *					3. Lire les valeurs désirées : readAmbientLight();
 *												   readRedLight();
 *				                                   readBlueLight();
 *				                                   readGreenLight();
 *				    4. Ajouter un délai(250ms par ex) pour s'assurer que les mesures ont le temps de se faire.
 *
 *			- Mode Interruption : quand la luminosité est comprise dans un certain intervalle définit par l'utilisateur,
 *			  une interruption est déclenchée.
 *			  		1. Initialiser le capteur : init()
 *			  		2. Définir les seuils (inférieur et supérieur) d'interruption : setLightIntLowThreshold()
 *			  																		setLightIntHighThreshold()
 *			  		3. Activer le mode capteur de luminosité en autorisant les interruptions : enableLightSensor(TRUE)
 *			  		4. Gérer les interruptions (initialisation de la broche de l'IT, activation des IT, routine d'interruption, etc).
 *			  		5. --!!!-- Après chaque interruption générée par une luminosité comprise dans l'intervalle défini
 *			  		   précédemment, ne pas oublier d'acquitter l'interruption : clearAmbientLightInt() ---!!!---
 *
 *************************************************************************************************************************
 *	 2.	Mode "Capteur de proximité" :
 *
 * 	 	    Permet de détecter un objet à proximité. Le capteur mesure le "niveau de proximité" qui correspond
 * 	 	    à une valeur sur 8 bits. Plus l'objet est proche, plus la valeur est élevée.
 *		    Tout comme le mode "Capteur de luminosité", il présente deux modes :
 *
 *			- Mode "Normal" : permet de faire une simple lecture du niveau de proximité.
 *					1. Initialiser le capteur : init()
 *					2. Activer le capteur en mode capteur de proximité : enableProximitySensor(FALSE)
 *					3. Lire le niveau de proximité : readProximity()
 *					4. Ajouter un délai(250ms par ex) pour s'assurer que les mesures ont le temps de se faire.
 *
 *			- Mode Interruption : quand le niveau de proximité est compris dans un certain intervalle définit par l'utilisateur,
 *			  une interruption est déclenchée.
 *			  		1. Initialiser le capteur : init()
 *			  		2. Définir les seuils (inférieur et supérieur) d'interruption : setProximityIntLowThreshold()
 *			  																		setProximityIntHighThreshold()
 *			  		3. Activer le mode capteur de proximité en autorisant les interruptions : enableProximitySensor(TRUE)
 *			  		4. Gérer les interruptions (initialisation de la broche de l'IT en mode interruption sur front descendant,
 *			  		   activation des IT, routine d'interruption, etc).
 *			  		5. --!!!-- Après chaque interruption générée par un niveau de proximité compris dans l'intervalle défini
 *			  		   précédemment, ne pas oublier d'acquitter l'interruption : clearProximityInt() ---!!!---
 *
 ***************************************************************************************************************************
 *   3.	Mode "Détecteur de mouvement" :
 *
 * 	 	    Permet de détecter certains gestes de l'utilisateur effectués au-dessus du capteur : balayages droite/gauche,
 * 	 	    gauche/droite, bas/haut, haut/bas, éloignement et rapprochement.
 *
 *					1. Initialiser le capteur : init()
 *					2. Initialiser la broche d'interruption en mode interruption sur front descendant.
 *					2. Activer le capteur en mode détecteur de mouvement en autorisant les IT : enableGestureSensor(TRUE)
 *					3. Activer les interruptions.
 *					4. Créer une fonction permettant de traiter un geste détecté. Cette fonction vérifiera tout d'abord
 *					   la disponibilité d'un geste ( isGestureAvailable() ). Si un geste est bien disponible, on lit
 *					   le geste ( readGesture() ) et on fait l'action désirée en fonction du geste lu. A la fin de cette
 *					   fonction, veiller à REACTIVER LES IT qui ont temporairement été désactivées le temps de traiter
 *					   le geste détecté.
 *					5. Définir la routine d'interruption système, dans laquelle, après avoir acquitté l'IT,
 *					   on DESACTIVERA LES IT temporairement, le temps de traiter le mouvement ayant généré l'IT.
 *					6. Quand une IT a été levée, appeler la méthode de traitement de geste créée lors de l'étape 4
 *					   (dans la tâche de fond par exemple).
 *
 *
 *************************************************************************************************************************
 *
 * Par défaut, les broches utilisées sont celles de l'I2C1 : PB8 pour SCL et PB9 pour SDA.
 * Le basculement sur l'I2C2 (PB10 pour SCL et PB11 pour SDA) peut se faire en modifiant la macro APDS9960_I2C dans le .h.
 * La broche d'interruption suggérée (INT) est PC9 (ou n'importe quelle autre broche acceptant les IT).
 * La broche Vl n'est pas branchée.
 * Le capteur est alimenté par sa broche Vcc en 3.3V.
 */

#include "config.h"
#if USE_APDS9960
#include "apds9960.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_gpio.h"
#include "stm32f1_i2c.h"
#include "stm32f1xx_nucleo.h"

/* Members */
gesture_data_type gesture_data_;
int gesture_ud_delta_;
int gesture_lr_delta_;
int gesture_ud_count_;
int gesture_lr_count_;
int gesture_near_count_;
int gesture_far_count_;
int gesture_state_;
int gesture_motion_;


/* Gesture processing */
void APDS9960_resetGestureParameters();
bool_e APDS9960_processGestureData();
bool_e APDS9960_decodeGesture();

/* Proximity Interrupt Threshold */
uint8_t APDS9960_getProxIntLowThresh();
void APDS9960_setProxIntLowThresh(uint8_t threshold);
uint8_t APDS9960_getProxIntHighThresh();
void APDS9960_setProxIntHighThresh(uint8_t threshold);

/* LED Boost Control */
uint8_t APDS9960_getLEDBoost();
void APDS9960_setLEDBoost(uint8_t boost);

/* Proximity photodiode select */
uint8_t APDS9960_getProxGainCompEnable();
void APDS9960_setProxGainCompEnable(uint8_t enable);
uint8_t APDS9960_getProxPhotoMask();
void APDS9960_setProxPhotoMask(uint8_t mask);

/* Gesture threshold control */
uint8_t APDS9960_getGestureEnterThresh();
void APDS9960_setGestureEnterThresh(uint8_t threshold);
uint8_t APDS9960_getGestureExitThresh();
void APDS9960_setGestureExitThresh(uint8_t threshold);

/* Gesture LED, gain, and time control */
uint8_t APDS9960_getGestureWaitTime();
void APDS9960_setGestureWaitTime(uint8_t time);

/* Gesture mode */
uint8_t APDS9960_getGestureMode();
void APDS9960_setGestureMode(uint8_t mode);

/* Raw I2C Commands */
void wireWriteByte(uint8_t val);
void wireWriteDataByte(uint8_t reg, uint8_t val);
void wireWriteDataBlock(uint8_t reg, uint8_t *val, unsigned int len);
void wireReadDataByte(uint8_t reg, uint8_t *val);
void wireReadDataBlock(uint8_t reg, uint8_t *val, unsigned int len);



/*
 * Cette demo initialise le capteur APDS9960 et effectue périodiquement une lecture des lumières Rouge, Verte et Bleu.
 * @post	Attention, cette fonction de démonstration est blocante !
 */
void APDS9960_demo_RGB(void)
{
	APDS9960_init();
	APDS9960_enableLightSensor(FALSE);
	//APDS9960_setLEDDrive(1);
	while(1)
	{
		uint16_t ambiant, red, blue, green;
		APDS9960_readAmbientLight(&ambiant);
		APDS9960_readRedLight(&red);
		APDS9960_readBlueLight(&blue);
		APDS9960_readGreenLight(&green);


		if(ambiant)
		{
			blue = (uint16_t)(((uint32_t)blue*100)/ambiant);
			red = (uint16_t)(((uint32_t)red*100)/ambiant);
			green = (uint16_t)(((uint32_t)green*100)/ambiant);
		}
			printf("A:%d\tR:%d\tG:%d\tB:%d\n",ambiant, red, green, blue);
		HAL_Delay(500);
	}
}



bool_e APDS9960_init() {
    uint8_t id;

    gesture_ud_delta_ = 0;
	gesture_lr_delta_ = 0;

	gesture_ud_count_ = 0;
	gesture_lr_count_ = 0;

	gesture_near_count_ = 0;
	gesture_far_count_ = 0;

	gesture_state_ = 0;
	gesture_motion_ = DIR_NONE;

    /* Initialize I2C */
    I2C_Init(APDS9960_I2C, 100000);

    /* Read ID register and check against known values for APDS-9960 */
    wireReadDataByte(APDS9960_ID, &id);

    if( !(id == APDS9960_ID_1 || id == APDS9960_ID_2) ) {
        return FALSE;
    }

    /* Set ENABLE register to 0 (disable all features) */
   APDS9960_setMode(ALL, OFF);

    /* Set default values for ambient light and proximity registers */
    wireWriteDataByte(APDS9960_ATIME, DEFAULT_ATIME);
    wireWriteDataByte(APDS9960_WTIME, DEFAULT_WTIME);
    wireWriteDataByte(APDS9960_PPULSE, DEFAULT_PROX_PPULSE);
    wireWriteDataByte(APDS9960_POFFSET_UR, DEFAULT_POFFSET_UR);
    wireWriteDataByte(APDS9960_POFFSET_DL, DEFAULT_POFFSET_DL);
    wireWriteDataByte(APDS9960_CONFIG1, DEFAULT_CONFIG1);
    APDS9960_setLEDDrive(DEFAULT_LDRIVE);
    APDS9960_setProximityGain(DEFAULT_PGAIN);
    APDS9960_setAmbientLightGain(DEFAULT_AGAIN);
    APDS9960_setProxIntLowThresh(DEFAULT_PILT);
    APDS9960_setProxIntHighThresh(DEFAULT_PIHT);
    APDS9960_setLightIntLowThreshold(DEFAULT_AILT);
    APDS9960_setLightIntHighThreshold(DEFAULT_AIHT);
    wireWriteDataByte(APDS9960_PERS, DEFAULT_PERS);
    wireWriteDataByte(APDS9960_CONFIG2, DEFAULT_CONFIG2);
    wireWriteDataByte(APDS9960_CONFIG3, DEFAULT_CONFIG3);

    /* Set default values for gesture sense registers */
    APDS9960_setGestureEnterThresh(DEFAULT_GPENTH);
    APDS9960_setGestureExitThresh(DEFAULT_GEXTH);
    wireWriteDataByte(APDS9960_GCONF1, DEFAULT_GCONF1);
    APDS9960_setGestureGain(DEFAULT_GGAIN);
    APDS9960_setGestureLEDDrive(DEFAULT_GLDRIVE);
    APDS9960_setGestureWaitTime(DEFAULT_GWTIME);
    wireWriteDataByte(APDS9960_GOFFSET_U, DEFAULT_GOFFSET);
    wireWriteDataByte(APDS9960_GOFFSET_D, DEFAULT_GOFFSET);
    wireWriteDataByte(APDS9960_GOFFSET_L, DEFAULT_GOFFSET);
    wireWriteDataByte(APDS9960_GOFFSET_R, DEFAULT_GOFFSET);
    wireWriteDataByte(APDS9960_GPULSE, DEFAULT_GPULSE);
    wireWriteDataByte(APDS9960_GCONF3, DEFAULT_GCONF3);
    APDS9960_setGestureIntEnable(DEFAULT_GIEN);


    /* Gesture config register dump */
    uint8_t reg;
    uint8_t val;

    for(reg = 0x80; reg <= 0xAF; reg++) {
        if( (reg != 0x82) && \
            (reg != 0x8A) && \
            (reg != 0x91) && \
            (reg != 0xA8) && \
            (reg != 0xAC) && \
            (reg != 0xAD) )
        {
            wireReadDataByte(reg, &val);
//            Serial.print(reg, HEX);
//            Serial.print(": 0x");
//            Serial.println(val, HEX);
        }
    }

    for(reg = 0xE4; reg <= 0xE7; reg++) {
        wireReadDataByte(reg, &val);
//        Serial.print(reg, HEX);
//        Serial.print(": 0x");
//        Serial.println(val, HEX);
    }
    return TRUE;
}


uint8_t APDS9960_getMode() {
    uint8_t enable_value;

    /* Read current ENABLE register */
    wireReadDataByte(APDS9960_ENABLE, &enable_value);

    return enable_value;
}

/**
 * @brief Enables or disables a feature in the APDS-9960
 *
 * @param[in] mode which feature to enable
 * @param[in] enable ON (1) or OFF (0)
 */
void APDS9960_setMode(uint8_t mode, uint8_t enable) {
    uint8_t reg_val;

    /* Read current ENABLE register */
    reg_val = APDS9960_getMode();

    /* Change bit(s) in ENABLE register */
    enable = enable & 0x01;
    if( mode >= 0 && mode <= 6 ) {
        if (enable) {
            reg_val |= (1 << mode);
        } else {
            reg_val &= ~(1 << mode);
        }
    } else if( mode == ALL ) {
        if (enable) {
            reg_val = 0x7F;
        } else {
            reg_val = 0x00;
        }
    }

    /* Write value back to ENABLE register */
    wireWriteDataByte(APDS9960_ENABLE, reg_val);
}

/**
 * @brief Turn the APDS-9960 on
 */
void APDS9960_enablePower() {
	APDS9960_setMode(POWER, 1);
}

/**
 * @brief Turn the APDS-9960 off
 */
void APDS9960_disablePower() {
	APDS9960_setMode(POWER, 0);
}

/**
 * @brief Starts the light (R/G/B/Ambient) sensor on the APDS-9960
 *
 * @param[in] interrupts true to enable hardware interrupt on high or low light
 */
void APDS9960_enableLightSensor(bool_e interrupts) {
    /* Set default gain, interrupts, enable power, and enable sensor */
    APDS9960_setAmbientLightGain(DEFAULT_AGAIN);
    if(interrupts) {
        APDS9960_setAmbientLightIntEnable(1);
    } else {
        APDS9960_setAmbientLightIntEnable(0);
    }
    APDS9960_enablePower();
    APDS9960_setMode(AMBIENT_LIGHT, 1);
}

/**
 * @brief Ends the light sensor on the APDS-9960
 */
void APDS9960_disableLightSensor() {
	APDS9960_setAmbientLightIntEnable(0);
	APDS9960_setMode(AMBIENT_LIGHT, 0);
}

/**
 * @brief Starts the proximity sensor on the APDS-9960
 *
 * @param[in] interrupts true to enable hardware external interrupt on proximity
 */
void APDS9960_enableProximitySensor(bool_e interrupts) {
    /* Set default gain, LED, interrupts, enable power, and enable sensor */
    APDS9960_setProximityGain(DEFAULT_PGAIN);
    APDS9960_setLEDDrive(DEFAULT_LDRIVE);
    if( interrupts ) {
        APDS9960_setProximityIntEnable(1);
    } else {
        APDS9960_setProximityIntEnable(0);
    }
    APDS9960_enablePower();
    APDS9960_setMode(PROXIMITY, 1);
}

/**
 * @brief Ends the proximity sensor on the APDS-9960
 */
void APDS9960_disableProximitySensor() {
	APDS9960_setProximityIntEnable(0);
	APDS9960_setMode(PROXIMITY, 0);
}

/**
 * @brief Starts the gesture recognition engine on the APDS-9960
 *
 * @param[in] interrupts true to enable hardware external interrupt on gesture
 */
void APDS9960_enableGestureSensor(bool_e interrupts) {
	/* Enable gesture mode
	   Set ENABLE to 0 (power off)
	   Set WTIME to 0xFF
	   Set AUX to LED_BOOST_300
	   Enable PON, WEN, PEN, GEN in ENABLE
	*/
	APDS9960_resetGestureParameters();
	wireWriteDataByte(APDS9960_WTIME, 0xFF);
	wireWriteDataByte(APDS9960_PPULSE, DEFAULT_GESTURE_PPULSE);
	APDS9960_setLEDBoost(LED_BOOST_300);
	if( interrupts ) {
		APDS9960_setGestureIntEnable(1);
	} else {
		APDS9960_setGestureIntEnable(0);
	}
	APDS9960_setGestureMode(1);
	APDS9960_enablePower();
	APDS9960_setMode(WAIT, 1);
	APDS9960_setMode(PROXIMITY, 1);
	APDS9960_setMode(GESTURE, 1);
}

/**
 * @brief Ends the gesture recognition engine on the APDS-9960
 */
void APDS9960_disableGestureSensor() {
    APDS9960_resetGestureParameters();
    APDS9960_setGestureIntEnable(0);
    APDS9960_setGestureMode(0);
    APDS9960_setMode(GESTURE, 0);
}


/**
 * @brief Reads the ambient (clear) light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 */
void APDS9960_readAmbientLight(uint16_t *val) {
    uint8_t val_byte;
    *val = 0;

    /* Read value from clear channel, low byte register */
    wireReadDataByte(APDS9960_CDATAL, &val_byte);
    *val = val_byte;

    /* Read value from clear channel, high byte register */
    wireReadDataByte(APDS9960_CDATAH, &val_byte);
    *val = *val + ((uint16_t)val_byte << 8);
}

/**
 * @brief Reads the red light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 */
void APDS9960_readRedLight(uint16_t *val) {
    uint8_t val_byte;
    *val = 0;

    /* Read value from clear channel, low byte register */
    wireReadDataByte(APDS9960_RDATAL, &val_byte);
    *val = val_byte;

    /* Read value from clear channel, high byte register */
    wireReadDataByte(APDS9960_RDATAH, &val_byte);
    *val = *val + ((uint16_t)val_byte << 8);
}

/**
 * @brief Reads the green light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 */
void APDS9960_readGreenLight(uint16_t *val) {
    uint8_t val_byte;
    *val = 0;

    /* Read value from clear channel, low byte register */
    wireReadDataByte(APDS9960_GDATAL, &val_byte);
    *val = val_byte;

    /* Read value from clear channel, high byte register */
    wireReadDataByte(APDS9960_GDATAH, &val_byte);
    *val = *val + ((uint16_t)val_byte << 8);
}

/**
 * @brief Reads the red light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 */
void APDS9960_readBlueLight(uint16_t *val) {
    uint8_t val_byte;
    *val = 0;

    /* Read value from clear channel, low byte register */
    wireReadDataByte(APDS9960_BDATAL, &val_byte);
    *val = val_byte;

    /* Read value from clear channel, high byte register */
    wireReadDataByte(APDS9960_BDATAH, &val_byte);
    *val = *val + ((uint16_t)val_byte << 8);
}

/**
 * @brief Reads the proximity level as an 8-bit value
 *
 * @param[out] val value of the proximity sensor.
 */
void APDS9960_readProximity(uint8_t *val) {
    *val = 0;

    /* Read value from proximity data register */
    wireReadDataByte(APDS9960_PDATA, val);
}

/**
 * @brief Resets all the parameters in the gesture data member
 */
void APDS9960_resetGestureParameters() {
    gesture_data_.index = 0;
    gesture_data_.total_gestures = 0;

    gesture_ud_delta_ = 0;
    gesture_lr_delta_ = 0;

    gesture_ud_count_ = 0;
    gesture_lr_count_ = 0;

    gesture_near_count_ = 0;
    gesture_far_count_ = 0;

    gesture_state_ = 0;
    gesture_motion_ = DIR_NONE;
}

/**
 * @brief Processes the raw gesture data to determine swipe direction
 *
 * @return True if near or far state seen. False otherwise.
 */
bool_e APDS9960_processGestureData() {
    uint8_t u_first = 0;
    uint8_t d_first = 0;
    uint8_t l_first = 0;
    uint8_t r_first = 0;
    uint8_t u_last = 0;
    uint8_t d_last = 0;
    uint8_t l_last = 0;
    uint8_t r_last = 0;
    int ud_ratio_first;
    int lr_ratio_first;
    int ud_ratio_last;
    int lr_ratio_last;
    int ud_delta;
    int lr_delta;
    int i;

    /* If we have less than 4 total gestures, that's not enough */
    if( gesture_data_.total_gestures <= 4 ) {
        return FALSE;
    }

    /* Check to make sure our data isn't out of bounds */
    if( (gesture_data_.total_gestures <= 32) && (gesture_data_.total_gestures > 0) ) {

        /* Find the first value in U/D/L/R above the threshold */
        for( i = 0; i < gesture_data_.total_gestures; i++ ) {
            if( (gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT) ) {

                u_first = gesture_data_.u_data[i];
                d_first = gesture_data_.d_data[i];
                l_first = gesture_data_.l_data[i];
                r_first = gesture_data_.r_data[i];
                break;
            }
        }

        /* If one of the _first values is 0, then there is no good data */
        if( (u_first == 0) || (d_first == 0) || (l_first == 0) || (r_first == 0) ) {

            return FALSE;
        }
        /* Find the last value in U/D/L/R above the threshold */
        for( i = gesture_data_.total_gestures - 1; i >= 0; i-- ) {
#if DEBUG_APDS
            printf("Finding last:");
            printf(" U:%d", gesture_data_.u_data[i]);
            printf(" D:%d", gesture_data_.d_data[i]);
            printf(" L:%d", gesture_data_.l_data[i]);
            printf(" R:%d\n", gesture_data_.r_data[i]);
#endif
            if( (gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT) ) {

                u_last = gesture_data_.u_data[i];
                d_last = gesture_data_.d_data[i];
                l_last = gesture_data_.l_data[i];
                r_last = gesture_data_.r_data[i];
                break;
            }
        }
    }

    /* Calculate the first vs. last ratio of up/down and left/right */
    ud_ratio_first = ((u_first - d_first) * 100) / (u_first + d_first);
    lr_ratio_first = ((l_first - r_first) * 100) / (l_first + r_first);
    ud_ratio_last = ((u_last - d_last) * 100) / (u_last + d_last);
    lr_ratio_last = ((l_last - r_last) * 100) / (l_last + r_last);

#if DEBUG_APDS
    printf("Last Values:");
    printf(" U:%d", u_last);
    printf("Last Values:");
    printf(" D:%d", d_last);
    printf("Last Values:");
    printf(" L:%d", l_last);
    printf("Last Values:");
    printf(" R:%d\n", r_last);

    printf("Ratios:");
    printf(" UD Fi: %d", ud_ratio_first);
    printf(" UD La: %d", ud_ratio_last);
    printf(" LR Fi: %d", lr_ratio_first);
    printf(" LR La: %d\n", lr_ratio_last);
#endif

    /* Determine the difference between the first and last ratios */
    ud_delta = ud_ratio_last - ud_ratio_first;
    lr_delta = lr_ratio_last - lr_ratio_first;

#if DEBUG_APDS
    printf("Deltas:");
    printf(" UD: %d", ud_delta);
    printf(" LR: %d\n", lr_delta);
#endif

    /* Accumulate the UD and LR delta values */
    gesture_ud_delta_ += ud_delta;
    gesture_lr_delta_ += lr_delta;

#if DEBUG_APDS
    printf("Accumulations:");
    printf(" UD: %d", gesture_ud_delta_);
    printf(" LR: %d\n", gesture_lr_delta_);
#endif

    /* Determine U/D gesture */
    if( gesture_ud_delta_ >= GESTURE_SENSITIVITY_1 ) {
        gesture_ud_count_ = 1;
    } else if( gesture_ud_delta_ <= -GESTURE_SENSITIVITY_1 ) {
        gesture_ud_count_ = -1;
    } else {
        gesture_ud_count_ = 0;
    }

    /* Determine L/R gesture */
    if( gesture_lr_delta_ >= GESTURE_SENSITIVITY_1 ) {
        gesture_lr_count_ = 1;
    } else if( gesture_lr_delta_ <= -GESTURE_SENSITIVITY_1 ) {
        gesture_lr_count_ = -1;
    } else {
        gesture_lr_count_ = 0;
    }

    /* Determine Near/Far gesture */
    if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == 0) ) {
        if( (abs(ud_delta) < GESTURE_SENSITIVITY_2) && \
            (abs(lr_delta) < GESTURE_SENSITIVITY_2) ) {

            if( (ud_delta == 0) && (lr_delta == 0) ) {
                gesture_near_count_++;
            } else if( (ud_delta != 0) || (lr_delta != 0) ) {
                gesture_far_count_++;
            }

            if( (gesture_near_count_ >= 10) && (gesture_far_count_ >= 2) ) {
                if( (ud_delta == 0) && (lr_delta == 0) ) {
                    gesture_state_ = NEAR_STATE;
                } else if( (ud_delta != 0) && (lr_delta != 0) ) {
                    gesture_state_ = FAR_STATE;
                }
                return TRUE;
            }
        }
    } else {
        if( (abs(ud_delta) < GESTURE_SENSITIVITY_2) && \
            (abs(lr_delta) < GESTURE_SENSITIVITY_2) ) {

            if( (ud_delta == 0) && (lr_delta == 0) ) {
                gesture_near_count_++;
            }

            if( gesture_near_count_ >= 10 ) {
                gesture_ud_count_ = 0;
                gesture_lr_count_ = 0;
                gesture_ud_delta_ = 0;
                gesture_lr_delta_ = 0;
            }
        }
    }

#if DEBUG_APDS
    printf("UD_CT: %d", gesture_ud_count_);
    printf("LR_CT: %d", gesture_lr_count_);
    printf("NEAR_CT: %d", gesture_near_count_);
    printf("FAR_CT: %d\n", gesture_far_count_);
    printf("----------\n");
#endif

    return FALSE;
}

/**
 * @brief Determines if there is a gesture available for reading
 *
 * @return True if gesture available. False otherwise.
 */
bool_e APDS9960_isGestureAvailable() {
    uint8_t val;

    /* Read value from GSTATUS register */
    wireReadDataByte(APDS9960_GSTATUS, &val);

    /* Shift and mask out GVALID bit */
    val &= APDS9960_GVALID;

    /* Return true/false based on GVALID bit */
    if( val == 1) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @brief Processes a gesture event and returns best guessed gesture
 *
 * @return Number corresponding to gesture. -1 on error.
 */
int APDS9960_readGesture() {
    uint8_t fifo_level = 0;
    uint8_t bytes_read = 0;
    uint8_t fifo_data[128];
    uint8_t gstatus;
    int motion;
    int i;

    /* Make sure that power and gesture is on and data is valid */
    if( !APDS9960_isGestureAvailable() || !(APDS9960_getMode() & 0b01000001) ) {
        return DIR_NONE;
    }

    while(1){

	/* Wait some time to collect next batch of FIFO data */
	HAL_Delay(FIFO_PAUSE_TIME);

	/* Get the contents of the STATUS register. Is data still valid? */
	wireReadDataByte(APDS9960_GSTATUS, &gstatus);

	/* If we have valid data, read in FIFO */
	if( (gstatus & APDS9960_GVALID) == APDS9960_GVALID ) {

		/* Read the current FIFO level */
		wireReadDataByte(APDS9960_GFLVL, &fifo_level);

#if DEBUG_APDS
		printf("FIFO Level: %d\n", fifo_level);
#endif

		/* If there's stuff in the FIFO, read it into our data block */
		if( fifo_level > 0) {
			wireReadDataBlock(APDS9960_GFIFO_U, (uint8_t*)fifo_data, (fifo_level * 4));

			bytes_read = fifo_level*4;

#if DEBUG_APDS
			printf("FIFO Dump: ");
			for ( i = 0; i < bytes_read; i++ ) {
				printf("%d ", fifo_data[i]);
			}
			printf("\n");
#endif

			/* If at least 1 set of data, sort the data into U/D/L/R */
			if( bytes_read >= 4 ) {
				for( i = 0; i < bytes_read; i += 4 ) {
					gesture_data_.u_data[gesture_data_.index] = fifo_data[i + 0];
					gesture_data_.d_data[gesture_data_.index] = fifo_data[i + 1];
					gesture_data_.l_data[gesture_data_.index] = fifo_data[i + 2];
					gesture_data_.r_data[gesture_data_.index] = fifo_data[i + 3];
					gesture_data_.index++;
					gesture_data_.total_gestures++;
				}

#if DEBUG_APDS
			printf("Up Data: ");
			for ( i = 0; i < gesture_data_.total_gestures; i++ ) {
				printf("%d ", gesture_data_.u_data[i]);
			}
			printf("\n");
#endif

				/* Filter and process gesture data. Decode near/far state */
				if( APDS9960_processGestureData() ) {
					if( APDS9960_decodeGesture() ) {
						//***TODO: U-Turn Gestures
#if DEBUG_APDS
						printf("Gesture : %d", gesture_motion_);
#endif
					}
				}

				/* Reset data */
				gesture_data_.index = 0;
				gesture_data_.total_gestures = 0;
			}
		}
	} else {

		/* Determine best guessed gesture and clean up */
		HAL_Delay(FIFO_PAUSE_TIME);
		APDS9960_decodeGesture();
		motion = gesture_motion_;
#if DEBUG_APDS
		printf("END: %d\n", gesture_motion_);
#endif
		APDS9960_resetGestureParameters();
		return motion;
	}
    }
}


/**
 * @brief Determines swipe direction or near/far state
 *
 * @return True if near/far event. False otherwise.
 */
bool_e APDS9960_decodeGesture() {
    /* Return if near or far event is detected */
    if( gesture_state_ == NEAR_STATE ) {
        gesture_motion_ = DIR_NEAR;
        return TRUE;
    } else if ( gesture_state_ == FAR_STATE ) {
        gesture_motion_ = DIR_FAR;
        return TRUE;
    }

    /* Determine swipe direction */
    if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == 0) ) {
        gesture_motion_ = DIR_UP;
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == 0) ) {
        gesture_motion_ = DIR_DOWN;
    } else if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == 1) ) {
        gesture_motion_ = DIR_RIGHT;
    } else if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == -1) ) {
        gesture_motion_ = DIR_LEFT;
    } else if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == 1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_UP;
        } else {
            gesture_motion_ = DIR_RIGHT;
        }
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == -1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_DOWN;
        } else {
            gesture_motion_ = DIR_LEFT;
        }
    } else if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == -1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_UP;
        } else {
            gesture_motion_ = DIR_LEFT;
        }
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == 1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_DOWN;
        } else {
            gesture_motion_ = DIR_RIGHT;
        }
    } else {
        return FALSE;
    }

    return TRUE;
}


/**
 * @brief Returns the lower threshold for proximity detection
 *
 * @return lower threshold
 */
uint8_t APDS9960_getProxIntLowThresh()
{
    uint8_t val;

    /* Read value from PILT register */
    wireReadDataByte(APDS9960_PILT, &val);

    return val;
}

/**
 * @brief Sets the lower threshold for proximity detection
 *
 * @param[in] threshold the lower proximity threshold
 */
void APDS9960_setProxIntLowThresh(uint8_t threshold)
{
    wireWriteDataByte(APDS9960_PILT, threshold);
}

/**
 * @brief Returns the high threshold for proximity detection
 *
 * @return high threshold
 */
uint8_t APDS9960_getProxIntHighThresh()
{
    uint8_t val;

    /* Read value from PIHT register */
    wireReadDataByte(APDS9960_PIHT, &val);

    return val;
}

/**
 * @brief Sets the high threshold for proximity detection
 *
 * @param[in] threshold the high proximity threshold
 */
void APDS9960_setProxIntHighThresh(uint8_t threshold)
{
    wireWriteDataByte(APDS9960_PIHT, threshold);
}

/**
 * @brief Returns LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @return the value of the LED drive strength. 0xFF on failure.
 */
uint8_t APDS9960_getLEDDrive()
{
    uint8_t val;

    /* Read value from CONTROL register */
    wireReadDataByte(APDS9960_CONTROL, &val);

    /* Shift and mask out LED drive bits */
    val = (val >> 6) & 0b00000011;

    return val;
}

/**
 * @brief Sets the LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @param[in] drive the value (0-3) for the LED drive strength
 */
void APDS9960_setLEDDrive(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    wireReadDataByte(APDS9960_CONTROL, &val);

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 6;
    val &= 0b00111111;
    val |= drive;

    /* Write register value back into CONTROL register */
    wireWriteDataByte(APDS9960_CONTROL, val);
}

/**
 * @brief Returns receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @return the value of the proximity gain. 0xFF on failure.
 */
uint8_t APDS9960_getProximityGain()
{
    uint8_t val;

    /* Read value from CONTROL register */
    wireReadDataByte(APDS9960_CONTROL, &val);

    /* Shift and mask out PDRIVE bits */
    val = (val >> 2) & 0b00000011;

    return val;
}

/**
 * @brief Sets the receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @param[in] drive the value (0-3) for the gain
 */
void APDS9960_setProximityGain(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    wireReadDataByte(APDS9960_CONTROL, &val);

    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 2;
    val &= 0b11110011;
    val |= drive;

    /* Write register value back into CONTROL register */
    wireWriteDataByte(APDS9960_CONTROL, val);
}

/**
 * @brief Returns receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @return the value of the ALS gain. 0xFF on failure.
 */
uint8_t APDS9960_getAmbientLightGain()
{
    uint8_t val;

    /* Read value from CONTROL register */
    wireReadDataByte(APDS9960_CONTROL, &val);

    /* Shift and mask out ADRIVE bits */
    val &= 0b00000011;

    return val;
}

/**
 * @brief Sets the receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @param[in] drive the value (0-3) for the gain
 */
void APDS9960_setAmbientLightGain(uint8_t drive)
{
    uint8_t val;

    /* Read value from CONTROL register */
    wireReadDataByte(APDS9960_CONTROL, &val);

    /* Set bits in register to given value */
    drive &= 0b00000011;
    val &= 0b11111100;
    val |= drive;

    /* Write register value back into CONTROL register */
    wireWriteDataByte(APDS9960_CONTROL, val);
}

/**
 * @brief Get the current LED boost value
 *
 * Value  Boost Current
 *   0        100%
 *   1        150%
 *   2        200%
 *   3        300%
 *
 * @return The LED boost value. 0xFF on failure.
 */
uint8_t APDS9960_getLEDBoost()
{
    uint8_t val;

    /* Read value from CONFIG2 register */
    wireReadDataByte(APDS9960_CONFIG2, &val);

    /* Shift and mask out LED_BOOST bits */
    val = (val >> 4) & 0b00000011;

    return val;
}

/**
 * @brief Sets the LED current boost value
 *
 * Value  Boost Current
 *   0        100%
 *   1        150%
 *   2        200%
 *   3        300%
 *
 * @param[in] drive the value (0-3) for current boost (100-300%)
 */
void APDS9960_setLEDBoost(uint8_t boost)
{
    uint8_t val;

    /* Read value from CONFIG2 register */
    wireReadDataByte(APDS9960_CONFIG2, &val);

    /* Set bits in register to given value */
    boost &= 0b00000011;
    boost = boost << 4;
    val &= 0b11001111;
    val |= boost;

    /* Write register value back into CONFIG2 register */
    wireWriteDataByte(APDS9960_CONFIG2, val);
}

/**
 * @brief Gets proximity gain compensation enable
 *
 * @return 1 if compensation is enabled. 0 if not. 0xFF on error.
 */
uint8_t APDS9960_getProxGainCompEnable()
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    wireReadDataByte(APDS9960_CONFIG3, &val);

    /* Shift and mask out PCMP bits */
    val = (val >> 5) & 0b00000001;

    return val;
}

/**
 * @brief Sets the proximity gain compensation enable
 *
 * @param[in] enable 1 to enable compensation. 0 to disable compensation.
 */
 void APDS9960_setProxGainCompEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from CONFIG3 register */
    wireReadDataByte(APDS9960_CONFIG3, &val);

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;

    /* Write register value back into CONFIG3 register */
    wireWriteDataByte(APDS9960_CONFIG3, val);
}


 /**
  * @brief Gets the current mask for enabled/disabled proximity photodiodes
  *
  * 1 = disabled, 0 = enabled
  * Bit    Photodiode
  *  3       UP
  *  2       DOWN
  *  1       LEFT
  *  0       RIGHT
  *
  * @return Current proximity mask for photodiodes. 0xFF on error.
  */
 uint8_t APDS9960_getProxPhotoMask()
 {
     uint8_t val;

     /* Read value from CONFIG3 register */
     wireReadDataByte(APDS9960_CONFIG3, &val);

     /* Mask out photodiode enable mask bits */
     val &= 0b00001111;

     return val;
 }


 /**
  * @brief Sets the mask for enabling/disabling proximity photodiodes
  *
  * 1 = disabled, 0 = enabled
  * Bit    Photodiode
  *  3       UP
  *  2       DOWN
  *  1       LEFT
  *  0       RIGHT
  *
  * @param[in] mask 4-bit mask value
  */
 void APDS9960_setProxPhotoMask(uint8_t mask)
 {
     uint8_t val;

     /* Read value from CONFIG3 register */
     wireReadDataByte(APDS9960_CONFIG3, &val);

     /* Set bits in register to given value */
     mask &= 0b00001111;
     val &= 0b11110000;
     val |= mask;

     /* Write register value back into CONFIG3 register */
     wireWriteDataByte(APDS9960_CONFIG3, val);
 }

 /**
  * @brief Gets the entry proximity threshold for gesture sensing
  *
  * @return Current entry proximity threshold.
  */
 uint8_t APDS9960_getGestureEnterThresh()
 {
     uint8_t val;

     /* Read value from GPENTH register */
     wireReadDataByte(APDS9960_GPENTH, &val);

     return val;
 }

 /**
  * @brief Sets the entry proximity threshold for gesture sensing
  *
  * @param[in] threshold proximity value needed to start gesture mode
  */
 void APDS9960_setGestureEnterThresh(uint8_t threshold)
 {
     wireWriteDataByte(APDS9960_GPENTH, threshold);
 }

 /**
  * @brief Gets the exit proximity threshold for gesture sensing
  *
  * @return Current exit proximity threshold.
  */
 uint8_t APDS9960_getGestureExitThresh()
 {
     uint8_t val;

     /* Read value from GEXTH register */
     wireReadDataByte(APDS9960_GEXTH, &val);

     return val;
 }

 /**
  * @brief Sets the exit proximity threshold for gesture sensing
  *
  * @param[in] threshold proximity value needed to end gesture mode
  */
 void APDS9960_setGestureExitThresh(uint8_t threshold)
 {
     wireWriteDataByte(APDS9960_GEXTH, threshold);
 }

 /**
  * @brief Gets the gain of the photodiode during gesture mode
  *
  * Value    Gain
  *   0       1x
  *   1       2x
  *   2       4x
  *   3       8x
  *
  * @return the current photodiode gain. 0xFF on error.
  */
 uint8_t APDS9960_getGestureGain()
 {
     uint8_t val;

     /* Read value from GCONF2 register */
     wireReadDataByte(APDS9960_GCONF2, &val);

     /* Shift and mask out GGAIN bits */
     val = (val >> 5) & 0b00000011;

     return val;
 }

 /**
  * @brief Sets the gain of the photodiode during gesture mode
  *
  * Value    Gain
  *   0       1x
  *   1       2x
  *   2       4x
  *   3       8x
  *
  * @param[in] gain the value for the photodiode gain
  */
 void APDS9960_setGestureGain(uint8_t gain)
 {
     uint8_t val;

     /* Read value from GCONF2 register */
     wireReadDataByte(APDS9960_GCONF2, &val);

     /* Set bits in register to given value */
     gain &= 0b00000011;
     gain = gain << 5;
     val &= 0b10011111;
     val |= gain;

     /* Write register value back into GCONF2 register */
     wireWriteDataByte(APDS9960_GCONF2, val);
 }

 /**
  * @brief Gets the drive current of the LED during gesture mode
  *
  * Value    LED Current
  *   0        100 mA
  *   1         50 mA
  *   2         25 mA
  *   3         12.5 mA
  *
  * @return the LED drive current value. 0xFF on error.
  */
 uint8_t APDS9960_getGestureLEDDrive()
 {
     uint8_t val;

     /* Read value from GCONF2 register */
     wireReadDataByte(APDS9960_GCONF2, &val);

     /* Shift and mask out GLDRIVE bits */
     val = (val >> 3) & 0b00000011;

     return val;
 }

 /**
  * @brief Sets the LED drive current during gesture mode
  *
  * Value    LED Current
  *   0        100 mA
  *   1         50 mA
  *   2         25 mA
  *   3         12.5 mA
  *
  * @param[in] drive the value for the LED drive current
  */
 void APDS9960_setGestureLEDDrive(uint8_t drive)
 {
     uint8_t val;

     /* Read value from GCONF2 register */
     wireReadDataByte(APDS9960_GCONF2, &val);

     /* Set bits in register to given value */
     drive &= 0b00000011;
     drive = drive << 3;
     val &= 0b11100111;
     val |= drive;

     /* Write register value back into GCONF2 register */
     wireWriteDataByte(APDS9960_GCONF2, val);
 }

 /**
  * @brief Gets the time in low power mode between gesture detections
  *
  * Value    Wait time
  *   0          0 ms
  *   1          2.8 ms
  *   2          5.6 ms
  *   3          8.4 ms
  *   4         14.0 ms
  *   5         22.4 ms
  *   6         30.8 ms
  *   7         39.2 ms
  *
  * @return the current wait time between gestures. 0xFF on error.
  */
 uint8_t APDS9960_getGestureWaitTime()
 {
     uint8_t val;

     /* Read value from GCONF2 register */
     wireReadDataByte(APDS9960_GCONF2, &val);

     /* Mask out GWTIME bits */
     val &= 0b00000111;

     return val;
 }

 /**
  * @brief Sets the time in low power mode between gesture detections
  *
  * Value    Wait time
  *   0          0 ms
  *   1          2.8 ms
  *   2          5.6 ms
  *   3          8.4 ms
  *   4         14.0 ms
  *   5         22.4 ms
  *   6         30.8 ms
  *   7         39.2 ms
  *
  * @param[in] the value for the wait time
  */
 void APDS9960_setGestureWaitTime(uint8_t time)
 {
     uint8_t val;

     /* Read value from GCONF2 register */
     wireReadDataByte(APDS9960_GCONF2, &val);

     /* Set bits in register to given value */
     time &= 0b00000111;
     val &= 0b11111000;
     val |= time;

     /* Write register value back into GCONF2 register */
     wireWriteDataByte(APDS9960_GCONF2, val);
 }

 /**
  * @brief Gets the low threshold for ambient light interrupts
  *
  * @param[out] threshold current low threshold stored on the APDS-9960
   */
 void APDS9960_getLightIntLowThreshold(uint16_t *threshold)
 {
     uint8_t val_byte;
     *threshold = 0;

     /* Read value from ambient light low threshold, low byte register */
     wireReadDataByte(APDS9960_AILTL, &val_byte);
     *threshold = val_byte;

     /* Read value from ambient light low threshold, high byte register */
     wireReadDataByte(APDS9960_AILTH, &val_byte);
     *threshold = *threshold + ((uint16_t)val_byte << 8);
 }

 /**
  * @brief Sets the low threshold for ambient light interrupts
  *
  * @param[in] threshold low threshold value for interrupt to trigger
  */
 void APDS9960_setLightIntLowThreshold(uint16_t threshold)
 {
     uint8_t val_low;
     uint8_t val_high;

     /* Break 16-bit threshold into 2 8-bit values */
     val_low = threshold & 0x00FF;
     val_high = (threshold & 0xFF00) >> 8;

     /* Write low byte */
     wireWriteDataByte(APDS9960_AILTL, val_low);

     /* Write high byte */
     wireWriteDataByte(APDS9960_AILTH, val_high);
 }

 /**
  * @brief Gets the high threshold for ambient light interrupts
  *
  * @param[out] threshold current low threshold stored on the APDS-9960
  */
 void APDS9960_getLightIntHighThreshold(uint16_t *threshold)
 {
     uint8_t val_byte;
     *threshold = 0;

     /* Read value from ambient light high threshold, low byte register */
     wireReadDataByte(APDS9960_AIHTL, &val_byte);
     *threshold = val_byte;

     /* Read value from ambient light high threshold, high byte register */
     wireReadDataByte(APDS9960_AIHTH, &val_byte);
     *threshold = *threshold + ((uint16_t)val_byte << 8);
 }

 /**
  * @brief Sets the high threshold for ambient light interrupts
  *
  * @param[in] threshold high threshold value for interrupt to trigger
  */
void APDS9960_setLightIntHighThreshold(uint16_t threshold)
 {
     uint8_t val_low;
     uint8_t val_high;

     /* Break 16-bit threshold into 2 8-bit values */
     val_low = threshold & 0x00FF;
     val_high = (threshold & 0xFF00) >> 8;

     /* Write low byte */
     wireWriteDataByte(APDS9960_AIHTL, val_low);

     /* Write high byte */
     wireWriteDataByte(APDS9960_AIHTH, val_high);
 }

/**
 * @brief Gets the low threshold for proximity interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 */
void APDS9960_getProximityIntLowThreshold(uint8_t *threshold)
{
    *threshold = 0;

    /* Read value from proximity low threshold register */
    wireReadDataByte(APDS9960_PILT, threshold);
}

/**
 * @brief Sets the low threshold for proximity interrupts
 *
 * @param[in] threshold low threshold value for interrupt to trigger
 */
void APDS9960_setProximityIntLowThreshold(uint8_t threshold)
{
    /* Write threshold value to register */
    wireWriteDataByte(APDS9960_PILT, threshold);
}

/**
 * @brief Gets the high threshold for proximity interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9960
 */
void APDS9960_getProximityIntHighThreshold(uint8_t *threshold)
{
    *threshold = 0;

    /* Read value from proximity low threshold register */
    wireReadDataByte(APDS9960_PIHT, threshold);
}

/**
 * @brief Sets the high threshold for proximity interrupts
 *
 * @param[in] threshold high threshold value for interrupt to trigger
 */
void APDS9960_setProximityIntHighThreshold(uint8_t threshold)
{

    /* Write threshold value to register */
    wireWriteDataByte(APDS9960_PIHT, threshold);
}

/**
 * @brief Gets if ambient light interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t APDS9960_getAmbientLightIntEnable()
{
    uint8_t val;

    /* Read value from ENABLE register */
    wireReadDataByte(APDS9960_ENABLE, &val);

    /* Shift and mask out AIEN bit */
    val = (val >> 4) & 0b00000001;

    return val;
}

/**
 * @brief Turns ambient light interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 */
void APDS9960_setAmbientLightIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from ENABLE register */
    wireReadDataByte(APDS9960_ENABLE, &val);

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 4;
    val &= 0b11101111;
    val |= enable;

    /* Write register value back into ENABLE register */
    wireWriteDataByte(APDS9960_ENABLE, val);
}

/**
 * @brief Gets if proximity interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t APDS9960_getProximityIntEnable()
{
    uint8_t val;

    /* Read value from ENABLE register */
    wireReadDataByte(APDS9960_ENABLE, &val);

    /* Shift and mask out PIEN bit */
    val = (val >> 5) & 0b00000001;

    return val;
}

/**
 * @brief Turns proximity interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 */
void APDS9960_setProximityIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from ENABLE register */
    wireReadDataByte(APDS9960_ENABLE, &val);

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;

    /* Write register value back into ENABLE register */
    wireWriteDataByte(APDS9960_ENABLE, val);
}

/**
 * @brief Gets if gesture interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t APDS9960_getGestureIntEnable()
{
    uint8_t val;

    /* Read value from GCONF4 register */
    wireReadDataByte(APDS9960_GCONF4, &val);

    /* Shift and mask out GIEN bit */
    val = (val >> 1) & 0b00000001;

    return val;
}

/**
 * @brief Turns gesture-related interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 */
void APDS9960_setGestureIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from GCONF4 register */
    wireReadDataByte(APDS9960_GCONF4, &val);

    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 1;
    val &= 0b11111101;
    val |= enable;

    /* Write register value back into GCONF4 register */
    wireWriteDataByte(APDS9960_GCONF4, val);
}

/**
 * @brief Clears the ambient light interrupt
 */
void APDS9960_clearAmbientLightInt()
{
    uint8_t throwaway;
    wireReadDataByte(APDS9960_AICLEAR, &throwaway);
}

/**
 * @brief Clears the proximity interrupt
 */
void APDS9960_clearProximityInt()
{
    uint8_t throwaway;
    wireReadDataByte(APDS9960_PICLEAR, &throwaway);
}

/**
 * @brief Tells if the gesture state machine is currently running
 *
 * @return 1 if gesture state machine is running, 0 if not. 0xFF on error.
 */
uint8_t APDS9960_getGestureMode()
{
    uint8_t val;

    /* Read value from GCONF4 register */
    wireReadDataByte(APDS9960_GCONF4, &val);

    /* Mask out GMODE bit */
    val &= 0b00000001;

    return val;
}

/**
 * @brief Tells the state machine to either enter or exit gesture state machine
 *
 * @param[in] mode 1 to enter gesture state machine, 0 to exit.
 */
void APDS9960_setGestureMode(uint8_t mode)
{
    uint8_t val;

    /* Read value from GCONF4 register */
    wireReadDataByte(APDS9960_GCONF4, &val);

    /* Set bits in register to given value */
    mode &= 0b00000001;
    val &= 0b11111110;
    val |= mode;

    /* Write register value back into GCONF4 register */
    wireWriteDataByte(APDS9960_GCONF4, val);
}

/**
 * @brief Writes a single byte to the I2C device (no register)
 *
 * @param[in] val the 1-byte value to write to the I2C device
 */
void wireWriteByte(uint8_t val) {
	I2C_WriteNoRegister(APDS9960_I2C, APDS9960_I2C_ADDR<<1, val);
}

/**
 * @brief Writes a single byte to the I2C device and specified register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val the 1-byte value to write to the I2C device
 */
void wireWriteDataByte(uint8_t reg, uint8_t val) {
	I2C_Write(APDS9960_I2C, APDS9960_I2C_ADDR<<1, reg, val);
}

/**
 * @brief Writes a block (array) of bytes to the I2C device and register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val pointer to the beginning of the data byte array
 * @param[in] len the length (in bytes) of the data to write
 */
void wireWriteDataBlock(uint8_t reg, uint8_t *val, unsigned int len) {
	I2C_WriteMulti(APDS9960_I2C, APDS9960_I2C_ADDR<<1, reg, val, len);
}

/**
 * @brief Reads a single byte from the I2C device and specified register
 *
 * @param[in] reg the register to read from
 * @param[out] the value returned from the register
 */
void wireReadDataByte(uint8_t reg, uint8_t *val) {
	I2C_Read(APDS9960_I2C, APDS9960_I2C_ADDR<<1, reg, val);
}

/**
 * @brief Reads a block (array) of bytes from the I2C device and register
 *
 * @param[in] reg the register to read from
 * @param[out] val pointer to the beginning of the data
 * @param[in] len number of bytes to read
 */
void wireReadDataBlock(uint8_t reg, uint8_t *val, unsigned int len) {
	I2C_ReadMulti(APDS9960_I2C, APDS9960_I2C_ADDR<<1, reg, val, len);
}


#endif
