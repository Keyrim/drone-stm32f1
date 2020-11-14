################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../lib/bsp/WS2812S_asm.s 

C_SRCS += \
../lib/bsp/WS2812S.c \
../lib/bsp/stm32f1_adc.c \
../lib/bsp/stm32f1_extit.c \
../lib/bsp/stm32f1_flash.c \
../lib/bsp/stm32f1_gpio.c \
../lib/bsp/stm32f1_i2c.c \
../lib/bsp/stm32f1_motorDC.c \
../lib/bsp/stm32f1_pwm.c \
../lib/bsp/stm32f1_rtc.c \
../lib/bsp/stm32f1_sd.c \
../lib/bsp/stm32f1_spi.c \
../lib/bsp/stm32f1_sys.c \
../lib/bsp/stm32f1_timer.c \
../lib/bsp/stm32f1_uart.c \
../lib/bsp/stm32f1_weak_it.c \
../lib/bsp/stm32f1xx_nucleo.c \
../lib/bsp/system_stm32f1xx.c \
../lib/bsp/systick.c 

OBJS += \
./lib/bsp/WS2812S.o \
./lib/bsp/WS2812S_asm.o \
./lib/bsp/stm32f1_adc.o \
./lib/bsp/stm32f1_extit.o \
./lib/bsp/stm32f1_flash.o \
./lib/bsp/stm32f1_gpio.o \
./lib/bsp/stm32f1_i2c.o \
./lib/bsp/stm32f1_motorDC.o \
./lib/bsp/stm32f1_pwm.o \
./lib/bsp/stm32f1_rtc.o \
./lib/bsp/stm32f1_sd.o \
./lib/bsp/stm32f1_spi.o \
./lib/bsp/stm32f1_sys.o \
./lib/bsp/stm32f1_timer.o \
./lib/bsp/stm32f1_uart.o \
./lib/bsp/stm32f1_weak_it.o \
./lib/bsp/stm32f1xx_nucleo.o \
./lib/bsp/system_stm32f1xx.o \
./lib/bsp/systick.o 

C_DEPS += \
./lib/bsp/WS2812S.d \
./lib/bsp/stm32f1_adc.d \
./lib/bsp/stm32f1_extit.d \
./lib/bsp/stm32f1_flash.d \
./lib/bsp/stm32f1_gpio.d \
./lib/bsp/stm32f1_i2c.d \
./lib/bsp/stm32f1_motorDC.d \
./lib/bsp/stm32f1_pwm.d \
./lib/bsp/stm32f1_rtc.d \
./lib/bsp/stm32f1_sd.d \
./lib/bsp/stm32f1_spi.d \
./lib/bsp/stm32f1_sys.d \
./lib/bsp/stm32f1_timer.d \
./lib/bsp/stm32f1_uart.d \
./lib/bsp/stm32f1_weak_it.d \
./lib/bsp/stm32f1xx_nucleo.d \
./lib/bsp/system_stm32f1xx.d \
./lib/bsp/systick.d 


# Each subdirectory must supply rules for building sources it contributes
lib/bsp/%.o: ../lib/bsp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32F1 -DNUCLEO_F103RB -DSTM32F103RBTx -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/VL53L0X" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MPU6050" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/Common" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/hts221" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lis3mdl" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lps22hb" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lps25hb" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lsm6ds0" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lsm6ds3" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/tft_ili9341" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/tft_pcd8544" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MCP23S17" -I"D:/Git Hub/Flight-Controller/ground_station/lib/hal/inc" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/Common" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/lcd2x16" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MLX90614" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MatrixKeyboard" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MatrixLed" -I"D:/Git Hub/Flight-Controller/ground_station/lib/CMSIS/core" -I"D:/Git Hub/Flight-Controller/ground_station/lib/CMSIS/device" -I"D:/Git Hub/Flight-Controller/ground_station/lib/middleware/FatFs" -I"D:/Git Hub/Flight-Controller/ground_station/lib/middleware/FatFs/src" -I"D:/Git Hub/Flight-Controller/ground_station/lib/middleware/FatFs/src/drivers" -I"D:/Git Hub/Flight-Controller/ground_station/appli"  -Og -g3 -Wall -Wextra -Wconversion -fmessage-length=0 -ffunction-sections -fdata-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

lib/bsp/%.o: ../lib/bsp/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


