################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/cbmbasic/cbm_runtime.c \
../Sources/cbmbasic/cbmbasic.c \
../Sources/cbmbasic/console.c \
../Sources/cbmbasic/plugin.c \
../Sources/cbmbasic/runtime_k70.c 

OBJS += \
./Sources/cbmbasic/cbm_runtime.o \
./Sources/cbmbasic/cbmbasic.o \
./Sources/cbmbasic/console.o \
./Sources/cbmbasic/plugin.o \
./Sources/cbmbasic/runtime_k70.o 

C_DEPS += \
./Sources/cbmbasic/cbm_runtime.d \
./Sources/cbmbasic/cbmbasic.d \
./Sources/cbmbasic/console.d \
./Sources/cbmbasic/plugin.d \
./Sources/cbmbasic/runtime_k70.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/cbmbasic/%.o: ../Sources/cbmbasic/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"../Sources" -I"../Includes" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


