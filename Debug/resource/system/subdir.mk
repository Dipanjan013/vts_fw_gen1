################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../resource/system/system_stm32.c 

OBJS += \
./resource/system/system_stm32.o 

C_DEPS += \
./resource/system/system_stm32.d 


# Each subdirectory must supply rules for building sources it contributes
resource/system/%.o resource/system/%.su resource/system/%.cyclo: ../resource/system/%.c resource/system/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DUSE_HAL_DRIVER -DSTM32F411xE -DDEBUG -c -I../Core/Inc -I"E:/st_workspace/vts_fw_gen1/resource/app" -I"E:/st_workspace/vts_fw_gen1/resource/config" -I"E:/st_workspace/vts_fw_gen1/resource/driver" -I"E:/st_workspace/vts_fw_gen1/resource/intf" -I"E:/st_workspace/vts_fw_gen1/resource/port" -I"E:/st_workspace/vts_fw_gen1/resource/service" -I"E:/st_workspace/vts_fw_gen1/resource/utils" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-resource-2f-system

clean-resource-2f-system:
	-$(RM) ./resource/system/system_stm32.cyclo ./resource/system/system_stm32.d ./resource/system/system_stm32.o ./resource/system/system_stm32.su

.PHONY: clean-resource-2f-system

