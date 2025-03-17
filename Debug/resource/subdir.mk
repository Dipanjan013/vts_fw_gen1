################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../resource/utils.c \
../resource/utils_gps.c 

OBJS += \
./resource/utils.o \
./resource/utils_gps.o 

C_DEPS += \
./resource/utils.d \
./resource/utils_gps.d 


# Each subdirectory must supply rules for building sources it contributes
resource/%.o resource/%.su resource/%.cyclo: ../resource/%.c resource/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG '-DWEAK =__attribute__((weak))' -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"E:/st_workspace/vts_fw_gen1/resource/intf" -I"E:/st_workspace/vts_fw_gen1/resource" -I"E:/st_workspace/vts_fw_gen1/resource/app" -I"E:/st_workspace/vts_fw_gen1/resource/port" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"E:/st_workspace/vts_fw_gen1/resource/service" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-resource

clean-resource:
	-$(RM) ./resource/utils.cyclo ./resource/utils.d ./resource/utils.o ./resource/utils.su ./resource/utils_gps.cyclo ./resource/utils_gps.d ./resource/utils_gps.o ./resource/utils_gps.su

.PHONY: clean-resource

