################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../resource/app/app.c 

OBJS += \
./resource/app/app.o 

C_DEPS += \
./resource/app/app.d 


# Each subdirectory must supply rules for building sources it contributes
resource/app/%.o resource/app/%.su resource/app/%.cyclo: ../resource/app/%.c resource/app/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG '-DWEAK =__attribute__((weak))' -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"E:/st_workspace/vts_fw_gen1/resource" -I"E:/st_workspace/vts_fw_gen1/resource/app" -I"E:/st_workspace/vts_fw_gen1/resource/atc" -I"E:/st_workspace/vts_fw_gen1/resource/port" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-resource-2f-app

clean-resource-2f-app:
	-$(RM) ./resource/app/app.cyclo ./resource/app/app.d ./resource/app/app.o ./resource/app/app.su

.PHONY: clean-resource-2f-app

