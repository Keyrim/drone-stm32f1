/**
  ******************************************************************************
  * @file    x_nucleo_iks01a1.h
  * @author  MEMS Application Team
  * @version V2.1.0
  * @date    4-April-2016
  * @brief   This file contains definitions for the x_nucleo_iks01a1.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __X_NUCLEO_IKS01A1_H
#define __X_NUCLEO_IKS01A1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/* Includes ------------------------------------------------------------------*/

#include "accelerometer.h"
#include "gyroscope.h"
#include "magnetometer.h"
#include "humidity.h"
#include "temperature.h"
#include "pressure.h"

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup X_NUCLEO_IKS01A1 X_NUCLEO_IKS01A1
 * @{
 */

/** @addtogroup X_NUCLEO_IKS01A1_IO IO
 * @{
 */

/** @addtogroup X_NUCLEO_IKS01A1_IO_Public_Constants Public constants
 * @{
 */

/* I2C clock speed configuration (in Hz) */
#define NUCLEO_I2C_EXPBD_SPEED                         400000

/* I2C peripheral configuration defines */
#define NUCLEO_I2C_EXPBD                            I2C1
#define NUCLEO_I2C_EXPBD_CLK_ENABLE()               __HAL_RCC_I2C1_CLK_ENABLE()
#define NUCLEO_I2C_EXPBD_SCL_SDA_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
//#define NUCLEO_I2C_EXPBD_SCL_SDA_AF                 GPIO_AF4_I2C1
#define NUCLEO_I2C_EXPBD_SCL_SDA_GPIO_PORT          GPIOB
#define NUCLEO_I2C_EXPBD_SCL_PIN                    GPIO_PIN_8
#define NUCLEO_I2C_EXPBD_SDA_PIN                    GPIO_PIN_9

#define NUCLEO_I2C_EXPBD_FORCE_RESET()              __HAL_RCC_I2C1_FORCE_RESET()
#define NUCLEO_I2C_EXPBD_RELEASE_RESET()            __HAL_RCC_I2C1_RELEASE_RESET()

/* I2C interrupt requests */

#define NUCLEO_I2C_EXPBD_EV_IRQn                    I2C1_EV_IRQn
#define NUCLEO_I2C_EXPBD_ER_IRQn                    I2C1_ER_IRQn



/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define NUCLEO_I2C_EXPBD_TIMEOUT_MAX    0x1000 /*<! The value of the maximal timeout for BUS waiting loops */

/* Definition for interrupt Pins */
#define HTS221_DRDY_GPIO_PORT           GPIOB
#define HTS221_DRDY_GPIO_CLK_ENABLE()   __GPIOB_CLK_ENABLE()
#define HTS221_DRDY_GPIO_CLK_DISABLE()  __GPIOB_CLK_DISABLE()
#define HTS221_DRDY_PIN                 GPIO_PIN_10

#define HTS221_DRDY_EXTI_IRQn           EXTI15_10_IRQn



#define LSM6DS0_INT1_GPIO_PORT           GPIOB
#define LSM6DS0_INT1_GPIO_CLK_ENABLE()   __GPIOB_CLK_ENABLE()
#define LSM6DS0_INT1_GPIO_CLK_DISABLE()  __GPIOB_CLK_DISABLE()
#define LSM6DS0_INT1_PIN                 GPIO_PIN_5

#define LSM6DS0_INT1_EXTI_IRQn           EXTI9_5_IRQn

#define LIS3MDL_DRDY_GPIO_PORT           GPIOC
#define LIS3MDL_DRDY_GPIO_CLK_ENABLE()   __GPIOC_CLK_ENABLE()
#define LIS3MDL_DRDY_GPIO_CLK_DISABLE()  __GPIOC_CLK_DISABLE()
#define LIS3MDL_DRDY_PIN                 GPIO_PIN_0

#define LIS3MDL_DRDY_EXTI_IRQn           EXTI0_IRQn

#define LIS3MDL_INT1_GPIO_PORT           GPIOC
#define LIS3MDL_INT1_GPIO_CLK_ENABLE()   __GPIOC_CLK_ENABLE()
#define LIS3MDL_INT1_GPIO_CLK_DISABLE()  __GPIOC_CLK_DISABLE()
#define LIS3MDL_INT1_PIN                 GPIO_PIN_1

#define LIS3MDL_INT1_EXTI_IRQn           EXTI1_IRQn

#define LPS25HB_INT1_GPIO_PORT           GPIOB
#define LPS25HB_INT1_GPIO_CLK_ENABLE()   __GPIOB_CLK_ENABLE()
#define LPS25HB_INT1_GPIO_CLK_DISABLE()  __GPIOB_CLK_DISABLE()
#define LPS25HB_INT1_PIN                 GPIO_PIN_4

#define LPS25HB_INT1_EXTI_IRQn           EXTI4_IRQn

// ready for use
#define USER_INT_GPIO_PORT           GPIOA
#define USER_INT_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define USER_INT_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()
#define USER_INT_PIN                 GPIO_PIN_10

#define USER_INT_EXTI_IRQn           EXTI15_10_IRQn

// ready for use
#define M_INT1_GPIO_PORT           GPIOA
#define M_INT1_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define M_INT1_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()
#define M_INT1_PIN                 GPIO_PIN_4

#define M_INT1_EXTI_IRQn           EXTI4_IRQn


// ready for use
#define M_INT2_GPIO_PORT           GPIOB
#define M_INT2_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define M_INT2_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOB_CLK_DISABLE()
#define M_INT2_PIN                 GPIO_PIN_0

#define M_INT2_EXTI_IRQn           EXTI0_IRQn


/**
  * @}
  */

/** @addtogroup X_NUCLEO_IKS01A1_IO_Public_FunctionPrototypes Public function prototypes
 * @{
 */
void IKS01A1_demo(void);
DrvStatusTypeDef Sensor_IO_Init( void );
DrvStatusTypeDef LSM6DS0_Sensor_IO_ITConfig( void );
DrvStatusTypeDef LSM6DS3_Sensor_IO_ITConfig( void );
DrvStatusTypeDef LPS22HB_Sensor_IO_ITConfig( void );

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

#ifdef __cplusplus
}
#endif

#endif /* __X_NUCLEO_IKS01A1_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
