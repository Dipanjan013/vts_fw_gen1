################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../resource/utils/utils.c \
../resource/utils/utils_gps.c 

OBJS += \
./resource/utils/utils.o \
./resource/utils/utils_gps.o 

C_DEPS += \
./resource/utils/utils.d \
./resource/utils/utils_gps.d 


# Each subdirectory must supply rules for building sources it contributes
resource/utils/%.o resource/utils/%.su resource/utils/%.cyclo: ../resource/utils/%.c resource/utils/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DUSE_HAL_DRIVER -DSTM32F411xE -DDEBUG -c -I../Core/Inc -I"E:/st_workspace/vts_fw_gen1/resource/config" -I"E:/st_workspace/vts_fw_gen1/resource/app" -I"E:/st_workspace/vts_fw_gen1/resource/intf" -I"E:/st_workspace/vts_fw_gen1/resource/port" -I"E:/st_workspace/vts_fw_gen1/resource/service" -I"E:/st_workspace/vts_fw_gen1/resource/utils" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-resource-2f-utils

clean-resource-2f-utils:
	-$(RM) ./resource/utils/utils.cyclo ./resource/utils/utils.d ./resource/utils/utils.o ./resource/utils/utils.su ./resource/utils/utils_gps.cyclo ./resource/utils/utils_gps.d ./resource/utils/utils_gps.o ./resource/utils/utils_gps.su

.PHONY: clean-resource-2f-utils

