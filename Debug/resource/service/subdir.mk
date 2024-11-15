################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../resource/service/service_queue.c 

OBJS += \
./resource/service/service_queue.o 

C_DEPS += \
./resource/service/service_queue.d 


# Each subdirectory must supply rules for building sources it contributes
resource/service/%.o resource/service/%.su resource/service/%.cyclo: ../resource/service/%.c resource/service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG '-DWEAK =__attribute__((weak))' -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"E:/st_workspace/vts_fw_gen1/resource" -I"E:/st_workspace/vts_fw_gen1/resource/app" -I"E:/st_workspace/vts_fw_gen1/resource/atc" -I"E:/st_workspace/vts_fw_gen1/resource/port" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"E:/st_workspace/vts_fw_gen1/resource/service" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-resource-2f-service

clean-resource-2f-service:
	-$(RM) ./resource/service/service_queue.cyclo ./resource/service/service_queue.d ./resource/service/service_queue.o ./resource/service/service_queue.su

.PHONY: clean-resource-2f-service

