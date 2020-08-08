/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "config.h"
#if USE_VL53L0

#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#include "stm32f1_uart.h"
#include "macro_types.h"
#include "stm32f1_gpio.h"
#include "vl53l0x_api.h"
#include "X-NUCLEO-53L0A1.h"
#include "systick.h"

/*
 * Cette machine d'état présente un exemple d'utilisation de la carte X-NUCLEO-53L0A1
 *
 *  Les données acquises sont affichées sur l'afficheur 7-segments, et envoyées avec plus de détails sur la liaison série (via printf)
 *	Plusieurs modes successifs peuvent être parcourus (le bouton bleu permet de naviguer entre les modes)
 *		FAsT : mode de lecture rapide du capteur central
 *		LonG : mode de lecture du capteur central en LongRange (permet de voir des distances plus lointaines)
 *		AccU : mode de lecture précis (Accuracy) du capteur central
 *		bAr : bargraph utilisant les 3 capteurs en mode rapide
 */
void VL53_DEMO_state_machine(void);



static bool_e button_press_event(void)
{
	static bool_e previous_button = FALSE;
	bool_e current_button = FALSE;
	bool_e ret = FALSE;
	current_button = !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
	ret = (current_button && !previous_button)?TRUE:FALSE;
	previous_button = current_button;
	return ret;
}


typedef enum {
	LONG_RANGE 		= 0, /*!< Long range mode */
	HIGH_SPEED 		= 1, /*!< High speed mode */
	HIGH_ACCURACY	= 2, /*!< High accuracy mode */
} RangingConfig_e;


static VL53L0X_RangingMeasurementData_t RangingMeasurementData;


/** leaky factor for filtered range
 *
 * r(n) = averaged_r(n-1)*leaky +r(n)(1-leaky)
 *
 * */
static int LeakyFactorFix8 = (int)( 0.6 *256);
/** How many device detect set by @a DetectSensors()*/

/** bit is index in VL53L0XDevs that is not necessary the dev id of the BSP */
static int nDevMask;


static VL53L0X_Dev_t VL53L0XDevs[]={
        {.Id=XNUCLEO53L0A1_DEV_LEFT, .DevLetter='l', .I2cHandle=&XNUCLEO53L0A1_hi2c, .I2cDevAddr=0x52},
        {.Id=XNUCLEO53L0A1_DEV_CENTER, .DevLetter='c', .I2cHandle=&XNUCLEO53L0A1_hi2c, .I2cDevAddr=0x52},
        {.Id=XNUCLEO53L0A1_DEV_RIGHT, .DevLetter='r', .I2cHandle=&XNUCLEO53L0A1_hi2c, .I2cDevAddr=0x52},
};

/** range low (and high) in @a RangeToLetter()
 *
 * used for displaying  multiple sensor as bar graph
 */
int RangeLow=100;

/** range medium in @a RangeToLetter()
 *
 * used for displaying  multiple sensor as bar graph
 */
int RangeMedium=300;




/**
 * Handle Error
 *
 * Set err on display and loop forever
 * @param err Error case code
 */
void HandleError(int err){
    char msg[16];
    sprintf(msg,"Er%d", err);
    XNUCLEO53L0A1_SetDisplayString(msg);
    while(1){};
}

/**
 * Reset all sensor then do presence detection
 *
 * All present devices are data initiated and assigned to their final I2C address
 * @return
 */
uint8_t DetectSensors(void)
{
    int i;
    uint16_t Id;
    int status;
    uint8_t FinalAddress;

    /* Reset all */
    static uint8_t nDevPresent=0;
    for (i = 0; i < 3; i++)
        status = XNUCLEO53L0A1_ResetId(i, 0);

    /* detect all sensors (even on-board)*/
    for (i = 0; i < 3; i++) {
        VL53L0X_Dev_t *pDev;
        pDev = &VL53L0XDevs[i];
        pDev->I2cDevAddr = 0x52;
        pDev->Present = 0;
        status = XNUCLEO53L0A1_ResetId( pDev->Id, 1);
        HAL_Delay(2);
        FinalAddress=(uint8_t)(0x52+(i+1)*2);

        do {
        	/* Set I2C standard mode (400 KHz) before doing the first register access */
        	if (status == VL53L0X_ERROR_NONE)
        		status = VL53L0X_WrByte(pDev, 0x88, 0x00);

        	/* Try to read one register using default 0x52 address */
            status = VL53L0X_RdWord(pDev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
            if (status) {
                debug_printf("#%d Read id fail\n", i);
                break;
            }
            if (Id == 0xEEAA) {
				/* Sensor is found => Change its I2C address to final one */
                status = VL53L0X_SetDeviceAddress(pDev,FinalAddress);
                if (status != 0) {
                    debug_printf("#%d VL53L0X_SetDeviceAddress fail\n", i);
                    break;
                }
                pDev->I2cDevAddr = FinalAddress;
                /* Check all is OK with the new I2C address and initialize the sensor */
                status = VL53L0X_RdWord(pDev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
                if (status != 0) {
					debug_printf("#%d VL53L0X_RdWord fail\n", i);
					break;
				}

                status = VL53L0X_DataInit(pDev);
                if( status == 0 ){
                    pDev->Present = 1;
                }
                else{
                    debug_printf("VL53L0X_DataInit %d fail\n", i);
                    break;
                }
                printf("VL53L0X %d Present and initiated to final 0x%x\n", pDev->Id, pDev->I2cDevAddr);
                nDevPresent++;
                nDevMask |= 1 << i;
                pDev->Present = 1;
            }
            else {
                debug_printf("#%d unknown ID %x\n", i, Id);
                status = 1;
            }
        } while (0);
        /* if fail r can't use for any reason then put the  device back to reset */
        if (status) {
            XNUCLEO53L0A1_ResetId(i, 0);
        }
    }

    return nDevPresent;
}

/**
 *  Setup all detected sensors for single shot mode and setup ranging configuration
 */
void SetupSingleShot(RangingConfig_e rangingConfig){
    int i;
    int status;
    uint8_t VhvSettings;
    uint8_t PhaseCal;
    uint32_t refSpadCount;
	uint8_t isApertureSpads;
	FixPoint1616_t signalLimit = (FixPoint1616_t)(0.25*65536);
	FixPoint1616_t sigmaLimit = (FixPoint1616_t)(18*65536);
	uint32_t timingBudget = 33000;
	uint8_t preRangeVcselPeriod = 14;
	uint8_t finalRangeVcselPeriod = 10;

    for( i=0; i<3; i++){
        if( VL53L0XDevs[i].Present){
            status=VL53L0X_StaticInit(&VL53L0XDevs[i]);
            if( status ){
                debug_printf("VL53L0X_StaticInit %d failed\n",i);
            }

            status = VL53L0X_PerformRefCalibration(&VL53L0XDevs[i], &VhvSettings, &PhaseCal);
			if( status ){
			   debug_printf("VL53L0X_PerformRefCalibration failed\n");
			}

			status = VL53L0X_PerformRefSpadManagement(&VL53L0XDevs[i], &refSpadCount, &isApertureSpads);
			if( status ){
			   debug_printf("VL53L0X_PerformRefSpadManagement failed\n");
			}

            status = VL53L0X_SetDeviceMode(&VL53L0XDevs[i], VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
            if( status ){
               debug_printf("VL53L0X_SetDeviceMode failed\n");
            }

            status = VL53L0X_SetLimitCheckEnable(&VL53L0XDevs[i], VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1); // Enable Sigma limit
			if( status ){
			   debug_printf("VL53L0X_SetLimitCheckEnable failed\n");
			}

			status = VL53L0X_SetLimitCheckEnable(&VL53L0XDevs[i], VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1); // Enable Signa limit
			if( status ){
			   debug_printf("VL53L0X_SetLimitCheckEnable failed\n");
			}
			/* Ranging configuration */
            switch(rangingConfig) {
            case LONG_RANGE:
            	signalLimit = (FixPoint1616_t)(0.1*65536);
            	sigmaLimit = (FixPoint1616_t)(60*65536);
            	timingBudget = 33000;
            	preRangeVcselPeriod = 18;
            	finalRangeVcselPeriod = 14;
            	break;
            case HIGH_ACCURACY:
				signalLimit = (FixPoint1616_t)(0.25*65536);
				sigmaLimit = (FixPoint1616_t)(18*65536);
				timingBudget = 200000;
				preRangeVcselPeriod = 14;
				finalRangeVcselPeriod = 10;
				break;
            case HIGH_SPEED:
				signalLimit = (FixPoint1616_t)(0.25*65536);
				sigmaLimit = (FixPoint1616_t)(32*65536);
				timingBudget = 20000;
				preRangeVcselPeriod = 14;
				finalRangeVcselPeriod = 10;
				break;
            default:
            	debug_printf("Not Supported");
            }

            status = VL53L0X_SetLimitCheckValue(&VL53L0XDevs[i],  VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);
			if( status ){
			   debug_printf("VL53L0X_SetLimitCheckValue failed\n");
			}

			status = VL53L0X_SetLimitCheckValue(&VL53L0XDevs[i],  VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
			if( status ){
			   debug_printf("VL53L0X_SetLimitCheckValue failed\n");
			}

            status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&VL53L0XDevs[i],  timingBudget);
            if( status ){
               debug_printf("VL53L0X_SetMeasurementTimingBudgetMicroSeconds failed\n");
            }

            status = VL53L0X_SetVcselPulsePeriod(&VL53L0XDevs[i],  VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
			if( status ){
			   debug_printf("VL53L0X_SetVcselPulsePeriod failed\n");
			}

            status = VL53L0X_SetVcselPulsePeriod(&VL53L0XDevs[i],  VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
			if( status ){
			   debug_printf("VL53L0X_SetVcselPulsePeriod failed\n");
			}

			status = VL53L0X_PerformRefCalibration(&VL53L0XDevs[i], &VhvSettings, &PhaseCal);
			if( status ){
			   debug_printf("VL53L0X_PerformRefCalibration failed\n");
			}

            VL53L0XDevs[i].LeakyFirst=1;
        }
    }
}

char RangeToLetter(VL53L0X_Dev_t *pDev, VL53L0X_RangingMeasurementData_t *pRange){
    char c;
    if( pRange->RangeStatus == 0 ){
        if( pDev->LeakyRange < RangeLow ){
            c='_';
        }
        else if( pDev->LeakyRange < RangeMedium ){
                c='=';
        }
        else {
            c = '~';
        }

    }
    else{
        c='-';
    }
    return c;
}

/* Store new ranging data into the device structure, apply leaky integrator if needed */
void Sensor_SetNewRange(VL53L0X_Dev_t *pDev, VL53L0X_RangingMeasurementData_t *pRange){
    if( pRange->RangeStatus == 0 ){
        if( pDev->LeakyFirst ){
            pDev->LeakyFirst = 0;
            pDev->LeakyRange = pRange->RangeMilliMeter;
        }
        else{
            pDev->LeakyRange = (pDev->LeakyRange*LeakyFactorFix8 + (256-LeakyFactorFix8)*pRange->RangeMilliMeter)>>8;
        }
    }
    else{
        pDev->LeakyFirst = 1;
    }
}

/**
 * Implement the ranging demo with all modes managed through the blue button (short and long press)
 * This function implements a while loop until the blue button is pressed
 * @param UseSensorsMask Mask of any sensors to use if not only one present
 * @param rangingConfig Ranging configuration to be used (same for all sensors)
 */
int RangeDemo(bool_e setup, int UseSensorsMask, RangingConfig_e rangingConfig){
    int status;
    char StrDisplay[5];
    char c;
    int i;
    static int nSensorToUse;
    static int SingleSensorNo=0;

    if(setup)
    {
		/* Setup all sensors in Single Shot mode */
		SetupSingleShot(rangingConfig);

		/* Which sensor to use ? */
		for(i=0, nSensorToUse=0; i<3; i++){
			if(( UseSensorsMask& (1<<i) ) && VL53L0XDevs[i].Present ){
				nSensorToUse++;
				if( nSensorToUse==1 )
					SingleSensorNo=i;
			}
		}
    }
    else
    {
		if( nSensorToUse >1 )
		{
			/* Multiple devices */
			strcpy(StrDisplay, "    ");
			for( i=0; i<3; i++){
				if( ! VL53L0XDevs[i].Present  || (UseSensorsMask & (1<<i))==0 )
					continue;
				/* Call All-In-One blocking API function */
				status = VL53L0X_PerformSingleRangingMeasurement(&VL53L0XDevs[i],&RangingMeasurementData);
				if( status ){
				 //   HandleError(ERR_DEMO_RANGE_MULTI);
				}
				/* Push data logging to UART */
				printf("[%d:%04dmm]", VL53L0XDevs[i].Id, RangingMeasurementData.RangeMilliMeter);
				/* Store new ranging distance */
				Sensor_SetNewRange(&VL53L0XDevs[i],&RangingMeasurementData);
				/* Translate distance in bar graph (multiple device) */
				c = RangeToLetter(&VL53L0XDevs[i],&RangingMeasurementData);
				StrDisplay[i+1]=c;
			}
			printf("\n");
		}
		else if(nSensorToUse == 1)
		{
			/* only one sensor */
			/* Call All-In-One blocking API function */
			status = VL53L0X_PerformSingleRangingMeasurement(&VL53L0XDevs[SingleSensorNo],&RangingMeasurementData);
			if( status ==0 ){
				/* Push data logging to UART */
				printf("[%06ldms][sensor %d - status %d - %04dmm - refl:%ld]\n", HAL_GetTick(), VL53L0XDevs[SingleSensorNo].Id,  RangingMeasurementData.RangeStatus, RangingMeasurementData.RangeMilliMeter, RangingMeasurementData.SignalRateRtnMegaCps);
				Sensor_SetNewRange(&VL53L0XDevs[SingleSensorNo],&RangingMeasurementData);
				/* Display distance in cm */
				if( RangingMeasurementData.RangeStatus == 0 ){
					sprintf(StrDisplay, "%3dc",(int)VL53L0XDevs[SingleSensorNo].LeakyRange/10);
				}
				else{
					sprintf(StrDisplay, "----");
					StrDisplay[0]=VL53L0XDevs[SingleSensorNo].DevLetter;
				}
			}
		}
		XNUCLEO53L0A1_SetDisplayString(StrDisplay);
    }
    return FALSE;
}





static volatile uint32_t t = 0;

void process_ms(void)
{
	if(t)
		t--;
}
void VL53_DEMO_state_machine(void)
{
	typedef enum
	{
		INIT,
		WELCOME,
		DETECT_SENSORS,
		WAIT_BEFORE_RETRY,
		MODE_HIGH_SPEED,
		MODE_LONG_RANGE,
		MODE_HIGH_ACCURACY,
		MODE_BARGRAPH,
		END_OF_MODES
	}state_e;
	static state_e state = INIT;
	static state_e previous_state = INIT;
	static uint8_t nbSensorPresent;
	bool_e entrance;
	entrance = (state != previous_state)?TRUE:FALSE;
	previous_state = state;

	bool_e button;
	button = button_press_event();

	switch(state)
	{
		case INIT:
			Systick_add_callback_function(&process_ms);
			XNUCLEO53L0A1_Init();
			state = WELCOME;
			break;
		case WELCOME:
			if(entrance)
			{
				t = 1000;
				XNUCLEO53L0A1_SetDisplayString("53L0");
			}
			if(!t)
				state = DETECT_SENSORS;
			break;
		case DETECT_SENSORS:
			nbSensorPresent = DetectSensors();
			if(!nbSensorPresent)
				state = WAIT_BEFORE_RETRY;
			else
				state = MODE_HIGH_SPEED;
			break;
		case WAIT_BEFORE_RETRY:
			if(entrance)
				t = 1000;
			if(!t)
				state = DETECT_SENSORS;
			break;


		case MODE_HIGH_SPEED:
			if(entrance)
			{
				t = 1000;
				XNUCLEO53L0A1_SetDisplayString("FAst");
				RangeDemo(TRUE, 1<<XNUCLEO53L0A1_DEV_CENTER, HIGH_SPEED);
			}
			if(!t)
				RangeDemo(FALSE, 1<<XNUCLEO53L0A1_DEV_CENTER, HIGH_SPEED);
			if(button)
				state++;
			break;
		case MODE_LONG_RANGE:
			if(entrance)
			{
				t = 1000;
				XNUCLEO53L0A1_SetDisplayString("Long");
				RangeDemo(TRUE, 1<<XNUCLEO53L0A1_DEV_CENTER, LONG_RANGE);
			}
			if(!t)
				RangeDemo(FALSE, 1<<XNUCLEO53L0A1_DEV_CENTER, LONG_RANGE);
			if(button)
				state++;
			break;
		case MODE_HIGH_ACCURACY:
			if(entrance)
			{
				t = 1000;
				XNUCLEO53L0A1_SetDisplayString("Accu");
				RangeDemo(TRUE, 1<<XNUCLEO53L0A1_DEV_CENTER, HIGH_ACCURACY);
			}
			if(!t)
				RangeDemo(FALSE, 1<<XNUCLEO53L0A1_DEV_CENTER, HIGH_ACCURACY);
			if(button)
				state++;
			break;
		case MODE_BARGRAPH:
			if(entrance)
			{
				t = 1000;
				XNUCLEO53L0A1_SetDisplayString("bAr");
				RangeDemo(TRUE, 0b111, HIGH_SPEED);
			}
			if(!t)
				RangeDemo(FALSE, 0b111, HIGH_SPEED);
			if(button)
				state++;
			break;
		case END_OF_MODES:
			state = MODE_HIGH_SPEED;
			break;
		default:
			break;
	}
}


#endif //USE_VL53L0
