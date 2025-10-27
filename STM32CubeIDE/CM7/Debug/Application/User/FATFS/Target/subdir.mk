################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
F:/400_Cellular/Projects/H745_Discovery_TouchGFX/CM7/FATFS/Target/user_diskio.c 

C_DEPS += \
./Application/User/FATFS/Target/user_diskio.d 

OBJS += \
./Application/User/FATFS/Target/user_diskio.o 


# Each subdirectory must supply rules for building sources it contributes
Application/User/FATFS/Target/user_diskio.o: F:/400_Cellular/Projects/H745_Discovery_TouchGFX/CM7/FATFS/Target/user_diskio.c Application/User/FATFS/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DZW_CONTROLLER -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H745xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -c -I../../../CM7/Core/Inc -I../../../CM7/Core/Coldfire -I../../../CM7/TouchGFX/App -I../../../CM7/TouchGFX/target/generated -I../../../CM7/TouchGFX/target -I../../../Drivers/BSP/Components/Common -I../../../Drivers/BSP/STM32H745I-DISCO -I../../../Utilities/JPEG -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/CMSIS/Include -I../../CM7/../../CM7/Middlewares/ST/touchgfx/framework/include -I../../CM7/../../CM7/TouchGFX/generated/fonts/include -I../../CM7/../../CM7/TouchGFX/generated/gui_generated/include -I../../CM7/../../CM7/TouchGFX/generated/images/include -I../../CM7/../../CM7/TouchGFX/generated/texts/include -I../../CM7/../../CM7/TouchGFX/generated/videos/include -I../../CM7/../../CM7/TouchGFX/gui/include -I../../../CM7/FATFS/Target -I../../../CM7/FATFS/App -I../../../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-FATFS-2f-Target

clean-Application-2f-User-2f-FATFS-2f-Target:
	-$(RM) ./Application/User/FATFS/Target/user_diskio.cyclo ./Application/User/FATFS/Target/user_diskio.d ./Application/User/FATFS/Target/user_diskio.o ./Application/User/FATFS/Target/user_diskio.su

.PHONY: clean-Application-2f-User-2f-FATFS-2f-Target

