################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/hal/src/stm32f1xx_hal.c \
../lib/hal/src/stm32f1xx_hal_adc.c \
../lib/hal/src/stm32f1xx_hal_adc_ex.c \
../lib/hal/src/stm32f1xx_hal_can.c \
../lib/hal/src/stm32f1xx_hal_cec.c \
../lib/hal/src/stm32f1xx_hal_cortex.c \
../lib/hal/src/stm32f1xx_hal_crc.c \
../lib/hal/src/stm32f1xx_hal_dac.c \
../lib/hal/src/stm32f1xx_hal_dac_ex.c \
../lib/hal/src/stm32f1xx_hal_dma.c \
../lib/hal/src/stm32f1xx_hal_eth.c \
../lib/hal/src/stm32f1xx_hal_flash.c \
../lib/hal/src/stm32f1xx_hal_flash_ex.c \
../lib/hal/src/stm32f1xx_hal_gpio.c \
../lib/hal/src/stm32f1xx_hal_gpio_ex.c \
../lib/hal/src/stm32f1xx_hal_hcd.c \
../lib/hal/src/stm32f1xx_hal_i2c.c \
../lib/hal/src/stm32f1xx_hal_i2s.c \
../lib/hal/src/stm32f1xx_hal_irda.c \
../lib/hal/src/stm32f1xx_hal_iwdg.c \
../lib/hal/src/stm32f1xx_hal_mmc.c \
../lib/hal/src/stm32f1xx_hal_msp_template.c \
../lib/hal/src/stm32f1xx_hal_nand.c \
../lib/hal/src/stm32f1xx_hal_nor.c \
../lib/hal/src/stm32f1xx_hal_pccard.c \
../lib/hal/src/stm32f1xx_hal_pcd.c \
../lib/hal/src/stm32f1xx_hal_pcd_ex.c \
../lib/hal/src/stm32f1xx_hal_pwr.c \
../lib/hal/src/stm32f1xx_hal_rcc.c \
../lib/hal/src/stm32f1xx_hal_rcc_ex.c \
../lib/hal/src/stm32f1xx_hal_rtc.c \
../lib/hal/src/stm32f1xx_hal_rtc_ex.c \
../lib/hal/src/stm32f1xx_hal_sd.c \
../lib/hal/src/stm32f1xx_hal_smartcard.c \
../lib/hal/src/stm32f1xx_hal_spi.c \
../lib/hal/src/stm32f1xx_hal_spi_ex.c \
../lib/hal/src/stm32f1xx_hal_sram.c \
../lib/hal/src/stm32f1xx_hal_tim.c \
../lib/hal/src/stm32f1xx_hal_tim_ex.c \
../lib/hal/src/stm32f1xx_hal_uart.c \
../lib/hal/src/stm32f1xx_hal_usart.c \
../lib/hal/src/stm32f1xx_hal_wwdg.c \
../lib/hal/src/stm32f1xx_ll_adc.c \
../lib/hal/src/stm32f1xx_ll_crc.c \
../lib/hal/src/stm32f1xx_ll_dac.c \
../lib/hal/src/stm32f1xx_ll_dma.c \
../lib/hal/src/stm32f1xx_ll_exti.c \
../lib/hal/src/stm32f1xx_ll_fsmc.c \
../lib/hal/src/stm32f1xx_ll_gpio.c \
../lib/hal/src/stm32f1xx_ll_i2c.c \
../lib/hal/src/stm32f1xx_ll_pwr.c \
../lib/hal/src/stm32f1xx_ll_rcc.c \
../lib/hal/src/stm32f1xx_ll_rtc.c \
../lib/hal/src/stm32f1xx_ll_sdmmc.c \
../lib/hal/src/stm32f1xx_ll_spi.c \
../lib/hal/src/stm32f1xx_ll_tim.c \
../lib/hal/src/stm32f1xx_ll_usart.c \
../lib/hal/src/stm32f1xx_ll_usb.c \
../lib/hal/src/stm32f1xx_ll_utils.c 

OBJS += \
./lib/hal/src/stm32f1xx_hal.o \
./lib/hal/src/stm32f1xx_hal_adc.o \
./lib/hal/src/stm32f1xx_hal_adc_ex.o \
./lib/hal/src/stm32f1xx_hal_can.o \
./lib/hal/src/stm32f1xx_hal_cec.o \
./lib/hal/src/stm32f1xx_hal_cortex.o \
./lib/hal/src/stm32f1xx_hal_crc.o \
./lib/hal/src/stm32f1xx_hal_dac.o \
./lib/hal/src/stm32f1xx_hal_dac_ex.o \
./lib/hal/src/stm32f1xx_hal_dma.o \
./lib/hal/src/stm32f1xx_hal_eth.o \
./lib/hal/src/stm32f1xx_hal_flash.o \
./lib/hal/src/stm32f1xx_hal_flash_ex.o \
./lib/hal/src/stm32f1xx_hal_gpio.o \
./lib/hal/src/stm32f1xx_hal_gpio_ex.o \
./lib/hal/src/stm32f1xx_hal_hcd.o \
./lib/hal/src/stm32f1xx_hal_i2c.o \
./lib/hal/src/stm32f1xx_hal_i2s.o \
./lib/hal/src/stm32f1xx_hal_irda.o \
./lib/hal/src/stm32f1xx_hal_iwdg.o \
./lib/hal/src/stm32f1xx_hal_mmc.o \
./lib/hal/src/stm32f1xx_hal_msp_template.o \
./lib/hal/src/stm32f1xx_hal_nand.o \
./lib/hal/src/stm32f1xx_hal_nor.o \
./lib/hal/src/stm32f1xx_hal_pccard.o \
./lib/hal/src/stm32f1xx_hal_pcd.o \
./lib/hal/src/stm32f1xx_hal_pcd_ex.o \
./lib/hal/src/stm32f1xx_hal_pwr.o \
./lib/hal/src/stm32f1xx_hal_rcc.o \
./lib/hal/src/stm32f1xx_hal_rcc_ex.o \
./lib/hal/src/stm32f1xx_hal_rtc.o \
./lib/hal/src/stm32f1xx_hal_rtc_ex.o \
./lib/hal/src/stm32f1xx_hal_sd.o \
./lib/hal/src/stm32f1xx_hal_smartcard.o \
./lib/hal/src/stm32f1xx_hal_spi.o \
./lib/hal/src/stm32f1xx_hal_spi_ex.o \
./lib/hal/src/stm32f1xx_hal_sram.o \
./lib/hal/src/stm32f1xx_hal_tim.o \
./lib/hal/src/stm32f1xx_hal_tim_ex.o \
./lib/hal/src/stm32f1xx_hal_uart.o \
./lib/hal/src/stm32f1xx_hal_usart.o \
./lib/hal/src/stm32f1xx_hal_wwdg.o \
./lib/hal/src/stm32f1xx_ll_adc.o \
./lib/hal/src/stm32f1xx_ll_crc.o \
./lib/hal/src/stm32f1xx_ll_dac.o \
./lib/hal/src/stm32f1xx_ll_dma.o \
./lib/hal/src/stm32f1xx_ll_exti.o \
./lib/hal/src/stm32f1xx_ll_fsmc.o \
./lib/hal/src/stm32f1xx_ll_gpio.o \
./lib/hal/src/stm32f1xx_ll_i2c.o \
./lib/hal/src/stm32f1xx_ll_pwr.o \
./lib/hal/src/stm32f1xx_ll_rcc.o \
./lib/hal/src/stm32f1xx_ll_rtc.o \
./lib/hal/src/stm32f1xx_ll_sdmmc.o \
./lib/hal/src/stm32f1xx_ll_spi.o \
./lib/hal/src/stm32f1xx_ll_tim.o \
./lib/hal/src/stm32f1xx_ll_usart.o \
./lib/hal/src/stm32f1xx_ll_usb.o \
./lib/hal/src/stm32f1xx_ll_utils.o 

C_DEPS += \
./lib/hal/src/stm32f1xx_hal.d \
./lib/hal/src/stm32f1xx_hal_adc.d \
./lib/hal/src/stm32f1xx_hal_adc_ex.d \
./lib/hal/src/stm32f1xx_hal_can.d \
./lib/hal/src/stm32f1xx_hal_cec.d \
./lib/hal/src/stm32f1xx_hal_cortex.d \
./lib/hal/src/stm32f1xx_hal_crc.d \
./lib/hal/src/stm32f1xx_hal_dac.d \
./lib/hal/src/stm32f1xx_hal_dac_ex.d \
./lib/hal/src/stm32f1xx_hal_dma.d \
./lib/hal/src/stm32f1xx_hal_eth.d \
./lib/hal/src/stm32f1xx_hal_flash.d \
./lib/hal/src/stm32f1xx_hal_flash_ex.d \
./lib/hal/src/stm32f1xx_hal_gpio.d \
./lib/hal/src/stm32f1xx_hal_gpio_ex.d \
./lib/hal/src/stm32f1xx_hal_hcd.d \
./lib/hal/src/stm32f1xx_hal_i2c.d \
./lib/hal/src/stm32f1xx_hal_i2s.d \
./lib/hal/src/stm32f1xx_hal_irda.d \
./lib/hal/src/stm32f1xx_hal_iwdg.d \
./lib/hal/src/stm32f1xx_hal_mmc.d \
./lib/hal/src/stm32f1xx_hal_msp_template.d \
./lib/hal/src/stm32f1xx_hal_nand.d \
./lib/hal/src/stm32f1xx_hal_nor.d \
./lib/hal/src/stm32f1xx_hal_pccard.d \
./lib/hal/src/stm32f1xx_hal_pcd.d \
./lib/hal/src/stm32f1xx_hal_pcd_ex.d \
./lib/hal/src/stm32f1xx_hal_pwr.d \
./lib/hal/src/stm32f1xx_hal_rcc.d \
./lib/hal/src/stm32f1xx_hal_rcc_ex.d \
./lib/hal/src/stm32f1xx_hal_rtc.d \
./lib/hal/src/stm32f1xx_hal_rtc_ex.d \
./lib/hal/src/stm32f1xx_hal_sd.d \
./lib/hal/src/stm32f1xx_hal_smartcard.d \
./lib/hal/src/stm32f1xx_hal_spi.d \
./lib/hal/src/stm32f1xx_hal_spi_ex.d \
./lib/hal/src/stm32f1xx_hal_sram.d \
./lib/hal/src/stm32f1xx_hal_tim.d \
./lib/hal/src/stm32f1xx_hal_tim_ex.d \
./lib/hal/src/stm32f1xx_hal_uart.d \
./lib/hal/src/stm32f1xx_hal_usart.d \
./lib/hal/src/stm32f1xx_hal_wwdg.d \
./lib/hal/src/stm32f1xx_ll_adc.d \
./lib/hal/src/stm32f1xx_ll_crc.d \
./lib/hal/src/stm32f1xx_ll_dac.d \
./lib/hal/src/stm32f1xx_ll_dma.d \
./lib/hal/src/stm32f1xx_ll_exti.d \
./lib/hal/src/stm32f1xx_ll_fsmc.d \
./lib/hal/src/stm32f1xx_ll_gpio.d \
./lib/hal/src/stm32f1xx_ll_i2c.d \
./lib/hal/src/stm32f1xx_ll_pwr.d \
./lib/hal/src/stm32f1xx_ll_rcc.d \
./lib/hal/src/stm32f1xx_ll_rtc.d \
./lib/hal/src/stm32f1xx_ll_sdmmc.d \
./lib/hal/src/stm32f1xx_ll_spi.d \
./lib/hal/src/stm32f1xx_ll_tim.d \
./lib/hal/src/stm32f1xx_ll_usart.d \
./lib/hal/src/stm32f1xx_ll_usb.d \
./lib/hal/src/stm32f1xx_ll_utils.d 


# Each subdirectory must supply rules for building sources it contributes
lib/hal/src/%.o: ../lib/hal/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32F1 -DNUCLEO_F103RB -DSTM32F103RBTx -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/VL53L0X" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MPU6050" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/Common" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/hts221" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lis3mdl" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lps22hb" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lps25hb" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lsm6ds0" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/iks01a1/lsm6ds3" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/tft_ili9341" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/tft_pcd8544" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MCP23S17" -I"D:/Git Hub/Flight-Controller/ground_station/lib/hal/inc" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/Common" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/lcd2x16" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MLX90614" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MatrixKeyboard" -I"D:/Git Hub/Flight-Controller/ground_station/lib/bsp/MatrixLed" -I"D:/Git Hub/Flight-Controller/ground_station/lib/CMSIS/core" -I"D:/Git Hub/Flight-Controller/ground_station/lib/CMSIS/device" -I"D:/Git Hub/Flight-Controller/ground_station/lib/middleware/FatFs" -I"D:/Git Hub/Flight-Controller/ground_station/lib/middleware/FatFs/src" -I"D:/Git Hub/Flight-Controller/ground_station/lib/middleware/FatFs/src/drivers" -I"D:/Git Hub/Flight-Controller/ground_station/appli"  -Og -g3 -Wall -Wextra -Wconversion -fmessage-length=0 -ffunction-sections -fdata-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


