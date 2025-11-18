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

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Hardware Configuration ----------------------------------------------------*/
/* CAN Configuration */
#define CAN_TX_Pin                  /* GPIO pin for CAN TX - to be defined */
#define CAN_RX_Pin                  /* GPIO pin for CAN RX - to be defined */

/* I2C Configuration (Sensor Bus) */
#define I2C_SCL_Pin                 /* GPIO pin for I2C SCL - to be defined */
#define I2C_SDA_Pin                 /* GPIO pin for I2C SDA - to be defined */

/* SPI Configuration (Magnetometer/Gyroscope) */
#define SPI_SCK_Pin                 /* GPIO pin for SPI SCK - to be defined */
#define SPI_MISO_Pin                /* GPIO pin for SPI MISO - to be defined */
#define SPI_MOSI_Pin                /* GPIO pin for SPI MOSI - to be defined */
#define SPI_CS_MAG_Pin              /* GPIO pin for Magnetometer CS - to be defined */
#define SPI_CS_GYRO_Pin             /* GPIO pin for Gyroscope CS - to be defined */

/* Magnetorquer Control Pins */
#define COIL_0_EN_Pin               /* GPIO pin for Coil 0 Enable - to be defined */
#define COIL_1_EN_Pin               /* GPIO pin for Coil 1 Enable - to be defined */
#define COIL_2_EN_Pin               /* GPIO pin for Coil 2 Enable - to be defined */
#define COIL_0_PWM_Pin              /* GPIO pin for Coil 0 PWM - to be defined */
#define COIL_1_PWM_Pin              /* GPIO pin for Coil 1 PWM - to be defined */
#define COIL_2_PWM_Pin              /* GPIO pin for Coil 2 PWM - to be defined */

/* Watchdog Configuration */
#define WDI_EXT_Pin                 /* GPIO pin for External Watchdog - to be defined */

/* General Purpose GPIO */
#define GPIO_0_Pin                  /* GPIO 0 - to be defined */
#define GPIO_1_Pin                  /* GPIO 1 - to be defined */
#define GPIO_2_Pin                  /* GPIO 2 - to be defined */

/* I2C Device Addresses */
#define INA226_ADDR_COIL0           0x40  /* Current sensor for coil 0 */
#define INA226_ADDR_COIL1           0x41  /* Current sensor for coil 1 */
#define INA226_ADDR_COIL2           0x42  /* Current sensor for coil 2 */
#define INA226_ADDR_COIL3           0x43  /* Current sensor for coil 3 */

#define TMP100_ADDR_COIL0           0x48  /* Temperature sensor for coil 0 */
#define TMP100_ADDR_COIL1           0x49  /* Temperature sensor for coil 1 */
#define TMP100_ADDR_COIL2           0x4A  /* Temperature sensor for coil 2 */
#define TMP100_ADDR_COIL3           0x4B  /* Temperature sensor for coil 3 */

#define MMC5983MA_ADDR              0x30  /* 3-axis magnetometer */
#define I3G4250D_ADDR               0x68  /* 3-axis gyroscope (I2C mode) */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
