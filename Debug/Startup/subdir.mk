################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Startup/startup_stm32g030f6px.s 

OBJS += \
./Startup/startup_stm32g030f6px.o 

S_DEPS += \
./Startup/startup_stm32g030f6px.d 


# Each subdirectory must supply rules for building sources it contributes
Startup/%.o: ../Startup/%.s Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m0plus -g3 -DDEBUG -c -I"C:/Users/Stanley/Documents/ptr_tracker_C/Drivers/CMSIS/Device/ST/STM32G0xx/Include" -I"C:/Users/Stanley/Documents/ptr_tracker_C/Drivers/CMSIS/Include" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Startup

clean-Startup:
	-$(RM) ./Startup/startup_stm32g030f6px.d ./Startup/startup_stm32g030f6px.o

.PHONY: clean-Startup

