/**
 ******************************************************************************
 * @file    x_nucleo_iks01a1.c
 * @author  MEMS Application Team
 * @version V2.1.0
 * @date    4-April-2016
 * @brief   This file provides X_NUCLEO_IKS01A1 MEMS shield board specific functions
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#if USE_SENSOR_HTS221 ||  USE_SENSOR_LPS25HB || USE_SENSOR_LSM6DS0 || USE_SENSOR_LIS3MDL ||  USE_SENSOR_LSM6DS3 ||  USE_SENSOR_LPS22HB
#include "x_nucleo_iks01a1.h"
#include "x_nucleo_iks01a1_accelero.h"
#include "x_nucleo_iks01a1_humidity.h"
#include "x_nucleo_iks01a1_gyro.h"
#include "x_nucleo_iks01a1_magneto.h"
#include "x_nucleo_iks01a1_pressure.h"
#include "x_nucleo_iks01a1_temperature.h"

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup X_NUCLEO_IKS01A1 X_NUCLEO_IKS01A1
 * @{
 */

/** @addtogroup X_NUCLEO_IKS01A1_IO IO
 * @{
 */

/** @addtogroup X_NUCLEO_IKS01A1_IO_Private_Variables Private variables
 * @{
 */

static uint32_t I2C_EXPBD_Timeout = NUCLEO_I2C_EXPBD_TIMEOUT_MAX;    /*<! Value of Timeout when I2C communication fails */
static I2C_HandleTypeDef I2C_EXPBD_Handle;

/**
 * @}
 */

/* Link function for sensor peripheral */
uint8_t Sensor_IO_Write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite );
uint8_t Sensor_IO_Read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead );

static void I2C_EXPBD_MspInit( void );
static void I2C_EXPBD_Error( uint8_t Addr );
static uint8_t I2C_EXPBD_ReadData( uint8_t Addr, uint8_t Reg, uint8_t* pBuffer, uint16_t Size );
static uint8_t I2C_EXPBD_WriteData( uint8_t Addr, uint8_t Reg, uint8_t* pBuffer, uint16_t Size );
static uint8_t I2C_EXPBD_Init( void );
#if USE_SENSOR_LPS22HB | USE_SENSOR_LPS25HB | USE_SENSOR_HTS221
static void floatToInt(float in, int32_t *out_int, int32_t *out_dec, int32_t dec_prec);
#include <math.h>
#endif
/** @addtogroup X_NUCLEO_IKS01A1_IO_Public_Functions Public functions
 * @{
 */


void IKS01A1_demo(void)
{
	#if USE_SENSOR_LSM6DS0 | USE_SENSOR_LSM6DS3
		static DrvContextTypeDef *ACCELERO_handle = NULL;
		static DrvContextTypeDef *GYRO_handle = NULL;
		static SensorAxes_t ACC_Value;
		static SensorAxes_t GYR_Value;                  /*!< Gyroscope Value */
	#endif
	#if USE_SENSOR_LIS3MDL
		static DrvContextTypeDef *MAGNETO_handle = NULL;
		static SensorAxes_t MAG_Value;                  /*!< Magnetometer Value */
	#endif
	#if USE_SENSOR_HTS221
		static DrvContextTypeDef *HUMIDITY_handle = NULL;
		static DrvContextTypeDef *TEMPERATURE_handle = NULL;
		static float HUMIDITY_Value;           /*!< Humidity Value */
		static float TEMPERATURE_Value;        /*!< Temperature Value */
	#endif
	#if USE_SENSOR_LPS22HB | USE_SENSOR_LPS25HB
		static float PRESSURE_Value;           /*!< Pressure Value */
		static DrvContextTypeDef *PRESSURE_handle = NULL;
	#endif
	uint8_t status;
	#if USE_SENSOR_LPS22HB | USE_SENSOR_LPS25HB | USE_SENSOR_HTS221
		int32_t d1, d2;
	#endif

	#if USE_SENSOR_LSM6DS0 | USE_SENSOR_LSM6DS3		//Accéléromètre et Gyroscope
		// Try to use LSM6DS3 DIL24 if present, otherwise use LSM6DS0 on board
		BSP_ACCELERO_Init( ACCELERO_SENSORS_AUTO, (void**)&ACCELERO_handle );
		BSP_GYRO_Init( GYRO_SENSORS_AUTO, (void**)&GYRO_handle );
		BSP_ACCELERO_Sensor_Enable( ACCELERO_handle );
		BSP_GYRO_Sensor_Enable( GYRO_handle );
	#endif
	#if USE_SENSOR_LIS3MDL
		// Force to use LIS3MDL
		BSP_MAGNETO_Init( LIS3MDL_0, (void**)&MAGNETO_handle );
		BSP_MAGNETO_Sensor_Enable( MAGNETO_handle );
	#endif
	#if USE_SENSOR_HTS221
		// Force to use HTS221
		BSP_HUMIDITY_Init( HTS221_H_0, (void**)&HUMIDITY_handle );
		BSP_HUMIDITY_Sensor_Enable( HUMIDITY_handle );
		// Force to use HTS221
		BSP_TEMPERATURE_Init( HTS221_T_0, (void**)&TEMPERATURE_handle );
		BSP_TEMPERATURE_Sensor_Enable( TEMPERATURE_handle );
	#endif
	#if USE_SENSOR_LPS22HB | USE_SENSOR_LPS25HB
		// Try to use LPS25HB DIL24 if present, otherwise use LPS25HB on board
		BSP_PRESSURE_Init( PRESSURE_SENSORS_AUTO, (void**)&PRESSURE_handle );
		BSP_PRESSURE_Sensor_Enable( PRESSURE_handle );
	#endif
	while(1)
	{
		#if USE_SENSOR_LSM6DS0 |  USE_SENSOR_LSM6DS3		//Accéléromètre et Gyroscope
			if(BSP_ACCELERO_IsInitialized(ACCELERO_handle, &status) == COMPONENT_OK && status == 1)
			{

				BSP_ACCELERO_Get_Axes(ACCELERO_handle, &ACC_Value);
				printf("ACC_X: %d, ACC_Y: %d, ACC_Z: %d\n",(int)ACC_Value.AXIS_X, (int)ACC_Value.AXIS_Y, (int)ACC_Value.AXIS_Z);
			}
			if(BSP_GYRO_IsInitialized(GYRO_handle, &status) == COMPONENT_OK && status == 1)
			{
				BSP_GYRO_Get_Axes(GYRO_handle, &GYR_Value);
				printf("GYR_X: %d, GYR_Y: %d, GYR_Z: %d\n", (int)GYR_Value.AXIS_X, (int)GYR_Value.AXIS_Y, (int)GYR_Value.AXIS_Z);
			}
		#endif
		#if USE_SENSOR_LIS3MDL
			if(BSP_MAGNETO_IsInitialized(MAGNETO_handle, &status) == COMPONENT_OK && status == 1)
			{
				BSP_MAGNETO_Get_Axes(MAGNETO_handle, &MAG_Value);
				printf("MAG_X: %d, MAG_Y: %d, MAG_Z: %d\n", (int)MAG_Value.AXIS_X, (int)MAG_Value.AXIS_Y, (int)MAG_Value.AXIS_Z);
			}
		#endif
		#if USE_SENSOR_HTS221
			if(BSP_HUMIDITY_IsInitialized(HUMIDITY_handle, &status) == COMPONENT_OK && status == 1)
			{
				BSP_HUMIDITY_Get_Hum(HUMIDITY_handle, &HUMIDITY_Value);
				floatToInt(HUMIDITY_Value, &d1, &d2, 2);
				printf("HUM: %d.%02d\n", (int)d1, (int)d2);
			}
			if(BSP_TEMPERATURE_IsInitialized(TEMPERATURE_handle, &status) == COMPONENT_OK && status == 1)
			{
				BSP_TEMPERATURE_Get_Temp(TEMPERATURE_handle, &TEMPERATURE_Value);
				floatToInt(TEMPERATURE_Value, &d1, &d2, 2);
				printf("TEMP: %d.%02d\n", (int)d1, (int)d2);
			}
		#endif
		#if USE_SENSOR_LPS22HB | USE_SENSOR_LPS25HB
			if(BSP_PRESSURE_IsInitialized(PRESSURE_handle, &status) == COMPONENT_OK && status == 1)
			{
				BSP_PRESSURE_Get_Press(PRESSURE_handle, &PRESSURE_Value);
				floatToInt(PRESSURE_Value, &d1, &d2, 2);
				printf("PRESS: %d.%02d\n", (int)d1, (int)d2);
			}
		#endif
	}
}


#if USE_SENSOR_LPS22HB | USE_SENSOR_LPS25HB | USE_SENSOR_HTS221
/**
 * @brief  Splits a float into two integer values.
 * @param  in the float value as input
 * @param  out_int the pointer to the integer part as output
 * @param  out_dec the pointer to the decimal part as output
 * @param  dec_prec the decimal precision to be used
 * @retval None
 */
static void floatToInt(float in, int32_t *out_int, int32_t *out_dec, int32_t dec_prec)
{
  *out_int = (int32_t)in;
  if(in >= 0.0f)
  {
    in = in - (float)(*out_int);
  }
  else
  {
    in = (float)(*out_int) - in;
  }
  *out_dec = (int32_t)trunc(in * pow(10, dec_prec));
}
#endif

/**
 * @brief  Configures sensor I2C interface.
 * @param  None
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
DrvStatusTypeDef Sensor_IO_Init( void )
{

  if ( I2C_EXPBD_Init() )
  {
    return COMPONENT_ERROR;
  }
  else
  {
    return COMPONENT_OK;
  }
}



/**
 * @brief  Configures sensor interrupts interface for LSM6DS0 sensor).
 * @param  None
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
DrvStatusTypeDef LSM6DS0_Sensor_IO_ITConfig( void )
{

  /*Not implemented yet*/

  return COMPONENT_OK;
}



/**
 * @brief  Configures sensor interrupts interface for LSM6DS3 sensor.
 * @param  None
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
DrvStatusTypeDef LSM6DS3_Sensor_IO_ITConfig( void )
{

  /* At the moment this feature is only implemented for LSM6DS3 */
  GPIO_InitTypeDef GPIO_InitStructureInt1;
  GPIO_InitTypeDef GPIO_InitStructureInt2;
  /* Enable INT1 GPIO clock */
  M_INT1_GPIO_CLK_ENABLE();

  /* Configure GPIO PINs to detect Interrupts */
  GPIO_InitStructureInt1.Pin = M_INT1_PIN;
  GPIO_InitStructureInt1.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStructureInt1.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStructureInt1.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(M_INT1_GPIO_PORT, &GPIO_InitStructureInt1);

  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(M_INT1_EXTI_IRQn, 0x00, 0x00);
  HAL_NVIC_EnableIRQ(M_INT1_EXTI_IRQn);

  /* Enable INT2 GPIO clock */
  M_INT2_GPIO_CLK_ENABLE();

  /* Configure GPIO PINs to detect Interrupts */
  GPIO_InitStructureInt2.Pin = M_INT2_PIN;
  GPIO_InitStructureInt2.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStructureInt2.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStructureInt2.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(M_INT2_GPIO_PORT, &GPIO_InitStructureInt2);

  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(M_INT2_EXTI_IRQn, 0x00, 0x00);
  HAL_NVIC_EnableIRQ(M_INT2_EXTI_IRQn);

  return COMPONENT_OK;
}



/**
 * @brief  Configures sensor interrupts interface for LPS22HB sensor.
 * @param  None
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
DrvStatusTypeDef LPS22HB_Sensor_IO_ITConfig( void )
{

  /* At the moment this feature is only implemented for LPS22HB */
  GPIO_InitTypeDef GPIO_InitStructureInt1;
  /* Enable INT1 GPIO clock */
  M_INT1_GPIO_CLK_ENABLE();

  /* Configure GPIO PINs to detect Interrupts */
  GPIO_InitStructureInt1.Pin = M_INT1_PIN;
  GPIO_InitStructureInt1.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStructureInt1.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructureInt1.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(M_INT1_GPIO_PORT, &GPIO_InitStructureInt1);

  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(M_INT1_EXTI_IRQn, 0x00, 0x00);
  HAL_NVIC_EnableIRQ(M_INT1_EXTI_IRQn);

  return COMPONENT_OK;
}

/**
 * @}
 */


/** @addtogroup X_NUCLEO_IKS01A1_IO_Private_Functions Private functions
 * @{
 */



/******************************* Link functions *******************************/


/**
 * @brief  Writes a buffer to the sensor
 * @param  handle instance handle
 * @param  WriteAddr specifies the internal sensor address register to be written to
 * @param  pBuffer pointer to data buffer
 * @param  nBytesToWrite number of bytes to be written
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
uint8_t Sensor_IO_Write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite )
{
  DrvContextTypeDef *ctx = (DrvContextTypeDef *)handle;

  /* call I2C_EXPBD Read data bus function */
  if ( I2C_EXPBD_WriteData( ctx->address, WriteAddr, pBuffer, nBytesToWrite ) )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}



/**
 * @brief  Reads a from the sensor to buffer
 * @param  handle instance handle
 * @param  ReadAddr specifies the internal sensor address register to be read from
 * @param  pBuffer pointer to data buffer
 * @param  nBytesToRead number of bytes to be read
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
uint8_t Sensor_IO_Read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead )
{
  DrvContextTypeDef *ctx = (DrvContextTypeDef *)handle;

  /* call I2C_EXPBD Read data bus function */
  if ( I2C_EXPBD_ReadData( ctx->address, ReadAddr, pBuffer, nBytesToRead ) )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}



/******************************* I2C Routines *********************************/

/**
 * @brief  Configures I2C interface.
 * @param  None
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t I2C_EXPBD_Init( void )
{
  if(HAL_I2C_GetState( &I2C_EXPBD_Handle) == HAL_I2C_STATE_RESET )
  {

    /* I2C_EXPBD peripheral configuration */
	  I2C_EXPBD_Handle.Init.ClockSpeed = NUCLEO_I2C_EXPBD_SPEED;
	  I2C_EXPBD_Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;


    I2C_EXPBD_Handle.Init.OwnAddress1    = 0x33;
    I2C_EXPBD_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2C_EXPBD_Handle.Instance            = NUCLEO_I2C_EXPBD;

    /* Init the I2C */
    I2C_EXPBD_MspInit();
    HAL_I2C_Init( &I2C_EXPBD_Handle );
  }

  if( HAL_I2C_GetState( &I2C_EXPBD_Handle) == HAL_I2C_STATE_READY )
  {
    return 0;
  }
  else
  {
    return 1;
  }
}



/**
 * @brief  Write data to the register of the device through BUS
 * @param  Addr Device address on BUS
 * @param  Reg The target register address to be written
 * @param  pBuffer The data to be written
 * @param  Size Number of bytes to be written
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t I2C_EXPBD_WriteData( uint8_t Addr, uint8_t Reg, uint8_t* pBuffer, uint16_t Size )
{

  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Write( &I2C_EXPBD_Handle, Addr, ( uint16_t )Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Size, I2C_EXPBD_Timeout );

  /* Check the communication status */
  if( status != HAL_OK )
  {

    /* Execute user timeout callback */
    I2C_EXPBD_Error( Addr );
    return 1;
  }
  else
  {
    return 0;
  }
}



/**
 * @brief  Read a register of the device through BUS
 * @param  Addr Device address on BUS
 * @param  Reg The target register address to read
 * @param  pBuffer The data to be read
 * @param  Size Number of bytes to be read
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static uint8_t I2C_EXPBD_ReadData( uint8_t Addr, uint8_t Reg, uint8_t* pBuffer, uint16_t Size )
{

  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read( &I2C_EXPBD_Handle, Addr, ( uint16_t )Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Size, I2C_EXPBD_Timeout );

  /* Check the communication status */
  if( status != HAL_OK )
  {

    /* Execute user timeout callback */
    I2C_EXPBD_Error( Addr );
    return 1;
  }
  else
  {
    return 0;
  }
}



/**
 * @brief  Manages error callback by re-initializing I2C
 * @param  Addr I2C Address
 * @retval None
 */
static void I2C_EXPBD_Error( uint8_t Addr )
{

  /* De-initialize the I2C comunication bus */
  HAL_I2C_DeInit( &I2C_EXPBD_Handle );

  /* Re-Initiaize the I2C comunication bus */
  I2C_EXPBD_Init();
}



/**
 * @brief I2C MSP Initialization
 * @param None
 * @retval None
 */

static void I2C_EXPBD_MspInit( void )
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable I2C GPIO clocks */
  NUCLEO_I2C_EXPBD_SCL_SDA_GPIO_CLK_ENABLE();

  /* I2C_EXPBD SCL and SDA pins configuration -------------------------------------*/
  GPIO_InitStruct.Pin        = NUCLEO_I2C_EXPBD_SCL_PIN | NUCLEO_I2C_EXPBD_SDA_PIN;
  GPIO_InitStruct.Mode       = GPIO_MODE_AF_OD;

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStruct.Pull       = GPIO_NOPULL;
 // GPIO_InitStruct.Alternate  = NUCLEO_I2C_EXPBD_SCL_SDA_AF;
  __HAL_AFIO_REMAP_I2C1_ENABLE();

  HAL_GPIO_Init( NUCLEO_I2C_EXPBD_SCL_SDA_GPIO_PORT, &GPIO_InitStruct );

  /* Enable the I2C_EXPBD peripheral clock */
  NUCLEO_I2C_EXPBD_CLK_ENABLE();

  /* Force the I2C peripheral clock reset */
  NUCLEO_I2C_EXPBD_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  NUCLEO_I2C_EXPBD_RELEASE_RESET();

  /* Enable and set I2C_EXPBD Interrupt to the highest priority */
  HAL_NVIC_SetPriority(NUCLEO_I2C_EXPBD_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(NUCLEO_I2C_EXPBD_EV_IRQn);


  /* Enable and set I2C_EXPBD Interrupt to the highest priority */
  HAL_NVIC_SetPriority(NUCLEO_I2C_EXPBD_ER_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(NUCLEO_I2C_EXPBD_ER_IRQn);


}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
