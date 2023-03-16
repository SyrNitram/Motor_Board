################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../modules/motor/Src/motor.c 

OBJS += \
./modules/motor/Src/motor.o 

C_DEPS += \
./modules/motor/Src/motor.d 


# Each subdirectory must supply rules for building sources it contributes
modules/motor/Src/%.o modules/motor/Src/%.su: ../modules/motor/Src/%.c modules/motor/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F098xx -c -I"D:/ST/Workspace_ST/Programme/MOTOR_BOARD_V2/modules/stander/Inc" -I../modules/Communication -I../modules/Scheduler -I../modules/Common -I../modules/Console -I../modules/platform/Inc -I"D:/ST/Workspace_ST/Programme/MOTOR_BOARD_V2/modules/motor/Inc" -I"D:/ST/Workspace_ST/Programme/MOTOR_BOARD_V2/modules/drv8353/Inc" -I"D:/ST/Workspace_ST/Programme/MOTOR_BOARD_V2/modules/setup/Inc" -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-modules-2f-motor-2f-Src

clean-modules-2f-motor-2f-Src:
	-$(RM) ./modules/motor/Src/motor.d ./modules/motor/Src/motor.o ./modules/motor/Src/motor.su

.PHONY: clean-modules-2f-motor-2f-Src

