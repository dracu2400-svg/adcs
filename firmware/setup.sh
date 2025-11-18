#!/bin/bash

##########################################################
# ADCS Firmware Setup Script
# This script downloads and sets up the required libraries:
# - STM32 HAL drivers for STM32L4 series
# - CMSIS files
# - FreeRTOS kernel
##########################################################

set -e

echo "=========================================="
echo "ADCS Firmware Setup for STM32L476RG"
echo "=========================================="

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo ""
echo -e "${YELLOW}This script will download the following libraries:${NC}"
echo "  1. STM32CubeL4 HAL Library (from ST GitHub)"
echo "  2. FreeRTOS Kernel (from FreeRTOS GitHub)"
echo ""

# Check if git is installed
if ! command -v git &> /dev/null; then
    echo -e "${RED}Error: git is not installed. Please install git first.${NC}"
    exit 1
fi

# Function to clone or update a repository
clone_or_update() {
    local repo_url=$1
    local target_dir=$2
    local branch=$3

    if [ -d "$target_dir/.git" ]; then
        echo -e "${YELLOW}Updating existing repository in $target_dir...${NC}"
        cd "$target_dir"
        git pull
        cd "$SCRIPT_DIR"
    else
        echo -e "${YELLOW}Cloning $repo_url...${NC}"
        git clone --depth 1 -b "$branch" "$repo_url" "$target_dir"
    fi
}

# Create necessary directories
mkdir -p Drivers/STM32L4xx_HAL_Driver
mkdir -p Drivers/CMSIS
mkdir -p Middlewares/Third_Party

echo ""
echo -e "${GREEN}[1/2] Downloading STM32CubeL4 HAL Library...${NC}"
echo "This may take a few minutes..."

# Clone STM32CubeL4 (contains HAL drivers and CMSIS)
CUBE_TEMP_DIR="temp_stm32cubel4"
if [ ! -d "$CUBE_TEMP_DIR" ]; then
    git clone --depth 1 https://github.com/STMicroelectronics/STM32CubeL4.git "$CUBE_TEMP_DIR"
else
    echo "STM32CubeL4 already downloaded, skipping..."
fi

# Copy HAL drivers
echo "Copying HAL drivers..."
cp -r "$CUBE_TEMP_DIR/Drivers/STM32L4xx_HAL_Driver/Inc" Drivers/STM32L4xx_HAL_Driver/
cp -r "$CUBE_TEMP_DIR/Drivers/STM32L4xx_HAL_Driver/Src" Drivers/STM32L4xx_HAL_Driver/

# Copy CMSIS files
echo "Copying CMSIS files..."
cp -r "$CUBE_TEMP_DIR/Drivers/CMSIS/Device" Drivers/CMSIS/
cp -r "$CUBE_TEMP_DIR/Drivers/CMSIS/Include" Drivers/CMSIS/

# Create stm32l4xx_hal_conf.h template if it doesn't exist
if [ ! -f "Core/Inc/stm32l4xx_hal_conf.h" ]; then
    echo "Creating HAL configuration header..."
    cp "$CUBE_TEMP_DIR/Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_conf_template.h" \
       "Core/Inc/stm32l4xx_hal_conf.h"
fi

echo ""
echo -e "${GREEN}[2/2] Downloading FreeRTOS Kernel...${NC}"

# Clone FreeRTOS
FREERTOS_TEMP_DIR="temp_freertos"
if [ ! -d "$FREERTOS_TEMP_DIR" ]; then
    git clone --depth 1 -b V10.5.1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git "$FREERTOS_TEMP_DIR"
else
    echo "FreeRTOS already downloaded, skipping..."
fi

# Copy FreeRTOS
echo "Copying FreeRTOS kernel..."
cp -r "$FREERTOS_TEMP_DIR" Middlewares/Third_Party/FreeRTOS

# Create FreeRTOSConfig.h template if it doesn't exist
if [ ! -f "Core/Inc/FreeRTOSConfig.h" ]; then
    echo "Creating FreeRTOS configuration header template..."
    cat > Core/Inc/FreeRTOSConfig.h << 'EOF'
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Ensure definitions are only used by the compiler, and not by the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
  #include <stdint.h>
  extern uint32_t SystemCoreClock;
#endif

#define configUSE_PREEMPTION                     1
#define configSUPPORT_STATIC_ALLOCATION          0
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCPU_CLOCK_HZ                       ( SystemCoreClock )
#define configTICK_RATE_HZ                       ((TickType_t)1000)
#define configMAX_PRIORITIES                     ( 7 )
#define configMINIMAL_STACK_SIZE                 ((uint16_t)128)
#define configTOTAL_HEAP_SIZE                    ((size_t)15360)
#define configMAX_TASK_NAME_LEN                  ( 16 )
#define configUSE_16_BIT_TICKS                   0
#define configUSE_MUTEXES                        1
#define configQUEUE_REGISTRY_SIZE                8
#define configUSE_RECURSIVE_MUTEXES              1
#define configUSE_COUNTING_SEMAPHORES            1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                    0
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )

/* Set the following definitions to 1 to include the API function, or zero to exclude the API function. */
#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
 #define configPRIO_BITS       __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS       4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS standard names. */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
EOF
fi

echo ""
echo -e "${GREEN}Setup complete!${NC}"
echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo "  1. Edit Core/Inc/stm32l4xx_hal_conf.h to enable/disable HAL modules"
echo "  2. Edit Core/Inc/FreeRTOSConfig.h to configure FreeRTOS parameters"
echo "  3. Update Makefile to include HAL and FreeRTOS source files (uncomment the lines)"
echo "  4. Run 'make' to build the firmware"
echo ""
echo -e "${YELLOW}Optional: Clean up temporary directories${NC}"
echo "  rm -rf $CUBE_TEMP_DIR $FREERTOS_TEMP_DIR"
echo ""
echo -e "${GREEN}You can now build the firmware using: make${NC}"
echo "=========================================="
