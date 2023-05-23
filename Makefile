##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.17.1] date: [Sun Mar 12 20:58:56 MDT 2023]
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = MTB-data-logger


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Core/Src/main.c \
Core/Src/stm32f4xx_it.c \
Core/Src/stm32f4xx_hal_msp.c \
sources/mtbdl_init.c \
sources/mtbdl_app.c \
sources/mtbdl_dir.c \
sources/int_handlers.c \
sources/screen_msgs.c \
sources/config_files/m8q_config.c \
../STM32F4-driver-library/sources/analog_driver.c \
../STM32F4-driver-library/sources/data_filters.c \
../STM32F4-driver-library/sources/dma_driver.c \
../STM32F4-driver-library/sources/gpio_driver.c \
../STM32F4-driver-library/sources/hc05_driver.c \
../STM32F4-driver-library/sources/hc05_controller.c \
../STM32F4-driver-library/sources/hd44780u_driver.c \
../STM32F4-driver-library/sources/hd44780u_controller.c \
../STM32F4-driver-library/sources/hw125_driver.c \
../STM32F4-driver-library/sources/hw125_controller.c \
../STM32F4-driver-library/sources/i2c_comm.c \
../STM32F4-driver-library/sources/interrupt_driver.c \
../STM32F4-driver-library/sources/linked_list_driver.c \
../STM32F4-driver-library/sources/m8q_driver.c \
../STM32F4-driver-library/sources/m8q_controller.c \
../STM32F4-driver-library/sources/mpu6050_driver.c \
../STM32F4-driver-library/sources/mpu6050_controller.c \
../STM32F4-driver-library/sources/spi_comm.c \
../STM32F4-driver-library/sources/stm32f411xe_custom.c \
../STM32F4-driver-library/sources/switch_debounce.c \
../STM32F4-driver-library/sources/timers.c \
../STM32F4-driver-library/sources/tools.c \
../STM32F4-driver-library/sources/uart_comm.c \
../STM32F4-driver-library/sources/ws2812_driver.c \
../STM32F4-driver-library/stmcode/FATFS/Target/user_diskio.c \
../STM32F4-driver-library/stmcode/FATFS/App/fatfs.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Core/Src/system_stm32f4xx.c \
../STM32F4-driver-library/stmcode/Middlewares/Third_Party/FatFs/src/diskio.c \
../STM32F4-driver-library/stmcode/Middlewares/Third_Party/FatFs/src/ff.c \
../STM32F4-driver-library/stmcode/Middlewares/Third_Party/FatFs/src/ff_gen_drv.c \
../STM32F4-driver-library/stmcode/Middlewares/Third_Party/FatFs/src/option/syscall.c \
../STM32F4-driver-library/stmcode/Middlewares/Third_Party/FatFs/src/option/ccsbcs.c  
# FATFS/Target/user_diskio.c \
# FATFS/App/fatfs.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
# Core/Src/system_stm32f4xx.c \
# Middlewares/Third_Party/FatFs/src/diskio.c \
# Middlewares/Third_Party/FatFs/src/ff.c \
# Middlewares/Third_Party/FatFs/src/ff_gen_drv.c \
# Middlewares/Third_Party/FatFs/src/option/syscall.c \
# Middlewares/Third_Party/FatFs/src/option/ccsbcs.c  

# ASM sources
ASM_SOURCES =  \
startup_stm32f411xe.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F411xE


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-Iheaders \
-Iheaders/config_files \
-I../STM32F4-driver-library/headers \
-I../STM32F4-driver-library/stmcode/FATFS/Target \
-I../STM32F4-driver-library/stmcode/FATFS/App \
-ICore/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-I../STM32F4-driver-library/stmcode/Middlewares/Third_Party/FatFs/src \
-I../STM32F4-driver-library/stmcode/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
-I../STM32F4-driver-library/stmcode/Drivers/CMSIS/Include
# -Iheaders \
# -Iheaders/config_files \
# -I../STM32F4-driver-library/headers \
# -IFATFS/Target \
# -IFATFS/App \
# -ICore/Inc \
# -IDrivers/STM32F4xx_HAL_Driver/Inc \
# -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
# -IMiddlewares/Third_Party/FatFs/src \
# -IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
# -IDrivers/CMSIS/Include


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F411RETx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# openocd
#######################################
flash: all
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
