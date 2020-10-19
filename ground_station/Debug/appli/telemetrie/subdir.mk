################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../appli/telemetrie/telemetrie.c 

OBJS += \
./appli/telemetrie/telemetrie.o 

C_DEPS += \
./appli/telemetrie/telemetrie.d 


# Each subdirectory must supply rules for building sources it contributes
appli/telemetrie/%.o: ../appli/telemetrie/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32F1 -DNUCLEO_F103RB -DSTM32F103RBTx -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/VL53L0X" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MPU6050" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/Common" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/hts221" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lis3mdl" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lps22hb" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lps25hb" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lsm6ds0" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lsm6ds3" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/tft_ili9341" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/tft_pcd8544" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MCP23S17" -I"D:/Git Hub/Flight-Controller/ground_station/lib/hal/inc" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/Common" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/lcd2x16" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MLX90614" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MatrixKeyboard" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MatrixLed" -I"D:/Git Hub/Flight-Controller/ground_station/lib/CMSIS/core" -I"D:/Git Hub/Flight-Controller/ground_station/lib/CMSIS/device" -I"D:/Git Hub/Flight-Controller/ground_station/lib/middleware/FatFs" -I"D:/Git Hub/Flight-Controller/ground_station/lib/middleware/FatFs/src" -I"D:/Git Hub/Flight-Controller/ground_station/lib/middleware/FatFs/src/drivers" -I"D:/Git Hub/Flight-Controller/ground_station/appli"  -Og -g3 -Wall -Wextra -Wconversion -fmessage-length=0 -ffunction-sections -fdata-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


