################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/BmsSoc/src/BMS_SoC.c 

OBJS += \
./Application/BmsSoc/src/BMS_SoC.o 

C_DEPS += \
./Application/BmsSoc/src/BMS_SoC.d 


# Each subdirectory must supply rules for building sources it contributes
Application/BmsSoc/src/%.o: ../Application/BmsSoc/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Application/BmsSoc/src/BMS_SoC.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


