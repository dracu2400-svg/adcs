/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * ADCS Firmware for STM32L476RG
  * Attitude Determination and Control System
  *
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Uncomment when HAL is available */
/* #include "stm32l4xx_hal.h" */

#include <stdint.h>
#include <stdbool.h>
#include "pin_config.h"  /* Pin configuration and hardware definitions */

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Note: All pin definitions and hardware configuration are now in pin_config.h */
/* See firmware/doc/PIN_MAPPING.md for complete pin mapping documentation */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
