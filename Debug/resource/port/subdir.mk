################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../resource/port/port_gpio.c \
../resource/port/port_led.c \
../resource/port/port_timer.c \
../resource/port/port_uart.c 

OBJS += \
./resource/port/port_gpio.o \
./resource/port/port_led.o \
./resource/port/port_timer.o \
./resource/port/port_uart.o 

C_DEPS += \
./resource/port/port_gpio.d \
./resource/port/port_led.d \
./resource/port/port_timer.d \
./resource/port/port_uart.d 


# Each subdirectory must supply rules for building sources it contributes
resource/port/%.o resource/port/%.su resource/port/%.cyclo: ../resource/port/%.c resource/port/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DUSE_HAL_DRIVER -DSTM32F411xE -DDEBUG -c -I../Core/Inc -I"E:/st_workspace/vts_fw_gen1/resource/config" -I"E:/st_workspace/vts_fw_gen1/resource/app" -I"E:/st_workspace/vts_fw_gen1/resource/intf" -I"E:/st_workspace/vts_fw_gen1/resource/port" -I"E:/st_workspace/vts_fw_gen1/resource/service" -I"E:/st_workspace/vts_fw_gen1/resource/utils" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-resource-2f-port

clean-resource-2f-port:
	-$(RM) ./resource/port/port_gpio.cyclo ./resource/port/port_gpio.d ./resource/port/port_gpio.o ./resource/port/port_gpio.su ./resource/port/port_led.cyclo ./resource/port/port_led.d ./resource/port/port_led.o ./resource/port/port_led.su ./resource/port/port_timer.cyclo ./resource/port/port_timer.d ./resource/port/port_timer.o ./resource/port/port_timer.su ./resource/port/port_uart.cyclo ./resource/port/port_uart.d ./resource/port/port_uart.o ./resource/port/port_uart.su

.PHONY: clean-resource-2f-port

