################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../modules/Scheduler/linked_list.c \
../modules/Scheduler/scheduler.c 

OBJS += \
./modules/Scheduler/linked_list.o \
./modules/Scheduler/scheduler.o 

C_DEPS += \
./modules/Scheduler/linked_list.d \
./modules/Scheduler/scheduler.d 


# Each subdirectory must supply rules for building sources it contributes
modules/Scheduler/%.o modules/Scheduler/%.su: ../modules/Scheduler/%.c modules/Scheduler/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F098xx -c -I"D:/ST/Workspace_ST/Programme/MOTOR_BOARD_V2/modules/stander/Inc" -I../modules/Communication -I../modules/Scheduler -I../modules/Common -I../modules/Console -I../modules/platform/Inc -I"D:/ST/Workspace_ST/Programme/MOTOR_BOARD_V2/modules/motor/Inc" -I"D:/ST/Workspace_ST/Programme/MOTOR_BOARD_V2/modules/drv8353/Inc" -I"D:/ST/Workspace_ST/Programme/MOTOR_BOARD_V2/modules/setup/Inc" -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-modules-2f-Scheduler

clean-modules-2f-Scheduler:
	-$(RM) ./modules/Scheduler/linked_list.d ./modules/Scheduler/linked_list.o ./modules/Scheduler/linked_list.su ./modules/Scheduler/scheduler.d ./modules/Scheduler/scheduler.o ./modules/Scheduler/scheduler.su

.PHONY: clean-modules-2f-Scheduler

