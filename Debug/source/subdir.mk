################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/colors.c \
../source/controller.c \
../source/cursor.c \
../source/leds.c \
../source/linetetris.c \
../source/main.c \
../source/msgeq07.c \
../source/pattern.c \
../source/power.c \
../source/random.c \
../source/rtc.c \
../source/scheduler.c \
../source/semihost_hardfault.c \
../source/utils.c 

OBJS += \
./source/colors.o \
./source/controller.o \
./source/cursor.o \
./source/leds.o \
./source/linetetris.o \
./source/main.o \
./source/msgeq07.o \
./source/pattern.o \
./source/power.o \
./source/random.o \
./source/rtc.o \
./source/scheduler.o \
./source/semihost_hardfault.o \
./source/utils.o 

C_DEPS += \
./source/colors.d \
./source/controller.d \
./source/cursor.d \
./source/leds.d \
./source/linetetris.d \
./source/main.d \
./source/msgeq07.d \
./source/pattern.d \
./source/power.d \
./source/random.d \
./source/rtc.d \
./source/scheduler.d \
./source/semihost_hardfault.d \
./source/utils.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../ -I../drivers -I../device -I../CMSIS -I../component/serial_manager -I../component/uart -I../utilities -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


