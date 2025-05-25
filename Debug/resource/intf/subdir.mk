################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../resource/intf/intf_at.c 

OBJS += \
./resource/intf/intf_at.o 

C_DEPS += \
./resource/intf/intf_at.d 


# Each subdirectory must supply rules for building sources it contributes
resource/intf/%.o resource/intf/%.su resource/intf/%.cyclo: ../resource/intf/%.c resource/intf/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"E:/st_workspace/vts_fw_gen1/resource/config" -I"E:/st_workspace/vts_fw_gen1/resource/app" -I"E:/st_workspace/vts_fw_gen1/resource/intf" -I"E:/st_workspace/vts_fw_gen1/resource/port" -I"E:/st_workspace/vts_fw_gen1/resource/service" -I"E:/st_workspace/vts_fw_gen1/resource/utils" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-resource-2f-intf

clean-resource-2f-intf:
	-$(RM) ./resource/intf/intf_at.cyclo ./resource/intf/intf_at.d ./resource/intf/intf_at.o ./resource/intf/intf_at.su

.PHONY: clean-resource-2f-intf

