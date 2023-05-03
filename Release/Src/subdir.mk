################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/gps.c \
../Src/lora.c \
../Src/main.c \
../Src/sx126x.c \
../Src/sx126x_hal.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/trackerHw.c 

OBJS += \
./Src/gps.o \
./Src/lora.o \
./Src/main.o \
./Src/sx126x.o \
./Src/sx126x_hal.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/trackerHw.o 

C_DEPS += \
./Src/gps.d \
./Src/lora.d \
./Src/main.d \
./Src/sx126x.d \
./Src/sx126x_hal.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/trackerHw.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -DSTM32G030F6Px -DSTM32 -DSTM32G0 -DSTM32G030xx -c -I../Inc -I"C:/Users/Stanley/Documents/ptr_tracker_C/Drivers/CMSIS/Device/ST/STM32G0xx/Include" -I"C:/Users/Stanley/Documents/ptr_tracker_C/Drivers/CMSIS/Include" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/gps.cyclo ./Src/gps.d ./Src/gps.o ./Src/gps.su ./Src/lora.cyclo ./Src/lora.d ./Src/lora.o ./Src/lora.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/sx126x.cyclo ./Src/sx126x.d ./Src/sx126x.o ./Src/sx126x.su ./Src/sx126x_hal.cyclo ./Src/sx126x_hal.d ./Src/sx126x_hal.o ./Src/sx126x_hal.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/trackerHw.cyclo ./Src/trackerHw.d ./Src/trackerHw.o ./Src/trackerHw.su

.PHONY: clean-Src

