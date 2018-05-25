################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/CANProcess.c \
../Src/PedalBox.c \
../Src/car.c \
../Src/freertos.c \
../Src/main.c \
../Src/motor_controller_functions.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_hal_timebase_TIM.c \
../Src/stm32f4xx_it.c \
../Src/system_stm32f4xx.c 

OBJS += \
./Src/CANProcess.o \
./Src/PedalBox.o \
./Src/car.o \
./Src/freertos.o \
./Src/main.o \
./Src/motor_controller_functions.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_hal_timebase_TIM.o \
./Src/stm32f4xx_it.o \
./Src/system_stm32f4xx.o 

C_DEPS += \
./Src/CANProcess.d \
./Src/PedalBox.d \
./Src/car.d \
./Src/freertos.d \
./Src/main.d \
./Src/motor_controller_functions.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_hal_timebase_TIM.d \
./Src/stm32f4xx_it.d \
./Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F407xx -I"C:/Users/Harrison/Documents/GitHub/MainModule2018F4/Inc" -I"C:/Users/Harrison/Documents/GitHub/MainModule2018F4/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/Harrison/Documents/GitHub/MainModule2018F4/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Harrison/Documents/GitHub/MainModule2018F4/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Users/Harrison/Documents/GitHub/MainModule2018F4/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Harrison/Documents/GitHub/MainModule2018F4/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/Harrison/Documents/GitHub/MainModule2018F4/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/Harrison/Documents/GitHub/MainModule2018F4/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


