################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include generate/src/subdir.mk
-include board/subdir.mk
-include RTD/src/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include CDD/CDD_STM32Temp/src/subdir.mk
-include CDD/CDD_INA219/src/subdir.mk
-include Application/Bms_Nvm/src/subdir.mk
-include Application/BmsSoc/src/subdir.mk
-include Application/BmsScheduler/src/subdir.mk
-include Application/BmsFault/src/subdir.mk
-include Application/BmsApp/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
CAPSTONE.siz \


# All Target
all: CAPSTONE.elf secondary-outputs

# Tool invocations
CAPSTONE.elf: $(OBJS) D:/workspaceS32ds3.6.7/CAPSTONE_BMS_ECU1/Project_Settings/Linker_Files/linker_flash_s32k144.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "CAPSTONE.elf" "@CAPSTONE.args"  $(USER_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

CAPSTONE.siz: CAPSTONE.elf
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley CAPSTONE.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
