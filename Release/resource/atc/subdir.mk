################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../resource/atc/atc..c 

OBJS += \
./resource/atc/atc..o 

C_DEPS += \
./resource/atc/atc..d 


# Each subdirectory must supply rules for building sources it contributes
resource/atc/%.o resource/atc/%.su resource/atc/%.cyclo: ../resource/atc/%.c resource/atc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-resource-2f-atc

clean-resource-2f-atc:
	-$(RM) ./resource/atc/atc..cyclo ./resource/atc/atc..d ./resource/atc/atc..o ./resource/atc/atc..su

.PHONY: clean-resource-2f-atc

