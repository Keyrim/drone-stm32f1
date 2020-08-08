//?????#include "hal.h"
#include "config.h"
#if USE_VL53L0
#include "vl53l0x_platform.h"
#include "vl53l0x_api.h"

#include "stm32f1xx_hal.h"
#include "stm32f1_i2c.h"
#include <string.h>

#define VL53L0X_OsDelay(...) HAL_Delay(2)


#ifndef HAL_I2C_MODULE_ENABLED
#warning "HAL I2C module must be enable "
#endif
//extern I2C_HandleTypeDef hi2c1;
//#define VL53L0X_pI2cHandle    (&hi2c1)

/* when not customized by application define dummy one */
#ifndef VL53L0X_GetI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define VL53L0X_GetI2cBus(...) (void)0
#endif

#ifndef VL53L0X_PutI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define VL53L0X_PutI2cBus(...) (void)0
#endif

#ifndef VL53L0X_OsDelay
#   define  VL53L0X_OsDelay(...) (void)0
#endif


uint8_t _I2CBuffer[64];

int _I2CWrite(VL53L0X_DEV Dev, uint8_t *pdata, uint32_t count) {
    HAL_StatusTypeDef status;
    status = I2C_WriteMultiNoRegister(I2C1, Dev->I2cDevAddr, pdata, (uint16_t)count);
    if (status) {
        printf("I2C error 0x%x %d len\n", Dev->I2cDevAddr, (uint16_t)count);
    }
    return (int)status;
}

int _I2CRead(VL53L0X_DEV Dev, uint8_t *pdata, uint32_t count) {
	HAL_StatusTypeDef status;
    status = I2C_ReadMultiNoRegister(I2C1, Dev->I2cDevAddr|1, pdata, (uint16_t)count);
    if (status) {
    	printf("I2C error 0x%x %d len\n", Dev->I2cDevAddr, (uint16_t)count);
    }
    return (int)status;
}

// the ranging_sensor_comms.dll will take care of the page selection
VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    if (count > sizeof(_I2CBuffer) - 1) {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }
    HAL_StatusTypeDef status;
    status = I2C_WriteMulti(I2C1, Dev->I2cDevAddr, index, pdata, (uint16_t)count);
    if (status != 0)
       	Status = VL53L0X_ERROR_CONTROL_INTERFACE;

    VL53L0X_PutI2cBus();
    return Status;
}

// the ranging_sensor_comms.dll will take care of the page selection
VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    VL53L0X_GetI2cBus();
    HAL_StatusTypeDef status;
    status = I2C_ReadMulti(I2C1, Dev->I2cDevAddr, index, pdata, (uint16_t)count);

    if (status != 0)
    	Status = VL53L0X_ERROR_CONTROL_INTERFACE;

    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    if(I2C_WriteMulti(I2C1, Dev->I2cDevAddr, index, &data, 1))
    	Status = VL53L0X_ERROR_CONTROL_INTERFACE;

    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    _I2CBuffer[0] = index;
    _I2CBuffer[1] = (uint8_t)(data >> 8);
    _I2CBuffer[2] = (uint8_t)(data & 0x00FF);

    VL53L0X_GetI2cBus();

    if(I2C_WriteMulti(I2C1, Dev->I2cDevAddr, index, &_I2CBuffer[1], 2))
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;

    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    _I2CBuffer[0] = index;
    _I2CBuffer[1] = (uint8_t)((data >> 24) & 0xFF);
    _I2CBuffer[2] = (uint8_t)((data >> 16) & 0xFF);
    _I2CBuffer[3] = (uint8_t)((data >> 8)  & 0xFF);
    _I2CBuffer[4] = (uint8_t)((data >> 0 ) & 0xFF);
    VL53L0X_GetI2cBus();

    if(I2C_WriteMulti(I2C1, Dev->I2cDevAddr, index, &_I2CBuffer[1], 4))
          Status = VL53L0X_ERROR_CONTROL_INTERFACE;


    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t data;

    Status = VL53L0X_RdByte(Dev, index, &data);
    if (Status) {
        goto done;
    }
    data = (data & AndData) | OrData;
    Status = VL53L0X_WrByte(Dev, index, data);
done:
    return Status;
}

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    VL53L0X_GetI2cBus();

    if(I2C_ReadMulti(I2C1, Dev->I2cDevAddr, index, data, 1))
          Status = VL53L0X_ERROR_CONTROL_INTERFACE;

    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    VL53L0X_GetI2cBus();

    if(I2C_ReadMulti(I2C1, Dev->I2cDevAddr, index, _I2CBuffer, 2))
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    uint32_t u32;
    u32 = _I2CBuffer[0];
    u32 <<= 8;
    u32 += _I2CBuffer[1];
    *data = (uint16_t)u32;

    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    VL53L0X_GetI2cBus();

    if(I2C_ReadMulti(I2C1, Dev->I2cDevAddr, index, _I2CBuffer, 4))
           Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    *data = ((uint32_t)_I2CBuffer[0]<<24) + ((uint32_t)_I2CBuffer[1]<<16) + ((uint32_t)_I2CBuffer[2]<<8) + (uint32_t)_I2CBuffer[3];

    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev) {
    VL53L0X_Error status = VL53L0X_ERROR_NONE;
    UNUSED(Dev);
    // do nothing
    VL53L0X_OsDelay();
    return status;
}

//end of file
#endif //USE_VL53L0
