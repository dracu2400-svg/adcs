/**
  ******************************************************************************
  * @file           : pin_config.h
  * @brief          : Pin configuration definitions for ADCS STM32L476RG
  * @version        : 1.0
  * @date           : 2025-11-18
  ******************************************************************************
  * @attention
  *
  * Pin definitions for the ADCS (Attitude Determination and Control System)
  * running on STM32L476RG microcontroller.
  *
  * IMPORTANT: Verify all pin assignments against actual hardware PCB before use!
  * See firmware/doc/PIN_MAPPING.md for complete documentation.
  *
  ******************************************************************************
  */

#ifndef __PIN_CONFIG_H
#define __PIN_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Uncomment when HAL is available */
/* #include "stm32l4xx_hal.h" */

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/*============================================================================*/
/* CAN BUS INTERFACE (Primary Communication)                                 */
/*============================================================================*/
#define CAN1_TX_Pin                     GPIO_PIN_12
#define CAN1_TX_GPIO_Port               GPIOA
#define CAN1_TX_AF                      GPIO_AF9_CAN1

#define CAN1_RX_Pin                     GPIO_PIN_11
#define CAN1_RX_GPIO_Port               GPIOA
#define CAN1_RX_AF                      GPIO_AF9_CAN1

/*============================================================================*/
/* I2C BUS (Sensor Interface)                                                */
/*============================================================================*/
#define I2C1_SCL_Pin                    GPIO_PIN_6
#define I2C1_SCL_GPIO_Port              GPIOB
#define I2C1_SCL_AF                     GPIO_AF4_I2C1

#define I2C1_SDA_Pin                    GPIO_PIN_7
#define I2C1_SDA_GPIO_Port              GPIOB
#define I2C1_SDA_AF                     GPIO_AF4_I2C1

/* I2C Device Addresses */
#define INA226_ADDR_COIL0               0x40  /* Current sensor coil 0 */
#define INA226_ADDR_COIL1               0x41  /* Current sensor coil 1 */
#define INA226_ADDR_COIL2               0x42  /* Current sensor coil 2 */
#define INA226_ADDR_COIL3               0x43  /* Current sensor coil 3 */

#define TMP100_ADDR_COIL0               0x48  /* Temperature sensor coil 0 */
#define TMP100_ADDR_COIL1               0x49  /* Temperature sensor coil 1 */
#define TMP100_ADDR_COIL2               0x4A  /* Temperature sensor coil 2 */
#define TMP100_ADDR_COIL3               0x4B  /* Temperature sensor coil 3 */

#define MMC5983MA_ADDR                  0x30  /* 3-axis magnetometer */
#define I3G4250D_ADDR                   0x68  /* 3-axis gyroscope */

/*============================================================================*/
/* SPI BUS (Alternative Sensor Interface)                                    */
/*============================================================================*/
#define SPI1_SCK_Pin                    GPIO_PIN_13
#define SPI1_SCK_GPIO_Port              GPIOB
#define SPI1_SCK_AF                     GPIO_AF5_SPI1

#define SPI1_MISO_Pin                   GPIO_PIN_14
#define SPI1_MISO_GPIO_Port             GPIOB
#define SPI1_MISO_AF                    GPIO_AF5_SPI1

#define SPI1_MOSI_Pin                   GPIO_PIN_15
#define SPI1_MOSI_GPIO_Port             GPIOB
#define SPI1_MOSI_AF                    GPIO_AF5_SPI1

#define SPI1_CS_MAG_Pin                 GPIO_PIN_12
#define SPI1_CS_MAG_GPIO_Port           GPIOB

#define SPI1_CS_GYRO_Pin                GPIO_PIN_6
#define SPI1_CS_GYRO_GPIO_Port          GPIOC

/*============================================================================*/
/* PWM OUTPUTS (Magnetorquer Control)                                        */
/*============================================================================*/
/* Coil 0 (X-axis) - TIM3 CH3 */
#define COIL_0_PWM_Pin                  GPIO_PIN_0
#define COIL_0_PWM_GPIO_Port            GPIOB
#define COIL_0_PWM_AF                   GPIO_AF2_TIM3
#define COIL_0_PWM_TIMER                TIM3
#define COIL_0_PWM_CHANNEL              TIM_CHANNEL_3

/* Coil 1 (Y-axis) - TIM3 CH4 */
#define COIL_1_PWM_Pin                  GPIO_PIN_1
#define COIL_1_PWM_GPIO_Port            GPIOB
#define COIL_1_PWM_AF                   GPIO_AF2_TIM3
#define COIL_1_PWM_TIMER                TIM3
#define COIL_1_PWM_CHANNEL              TIM_CHANNEL_4

/* Coil 2 (Z-axis) - TIM2 CH3 */
#define COIL_2_PWM_Pin                  GPIO_PIN_10
#define COIL_2_PWM_GPIO_Port            GPIOB
#define COIL_2_PWM_AF                   GPIO_AF1_TIM2
#define COIL_2_PWM_TIMER                TIM2
#define COIL_2_PWM_CHANNEL              TIM_CHANNEL_3

/* PWM Configuration Constants */
#define PWM_FREQUENCY_HZ                20000   /* 20 kHz */
#define PWM_TIMER_CLOCK_HZ              80000000 /* 80 MHz */
#define PWM_ARR_VALUE                   ((PWM_TIMER_CLOCK_HZ / PWM_FREQUENCY_HZ) - 1) /* 3999 */
#define PWM_MAX_DUTY_CYCLE              PWM_ARR_VALUE
#define PWM_MIN_DUTY_CYCLE              0

/*============================================================================*/
/* GPIO OUTPUTS (Magnetorquer H-Bridge Control)                              */
/*============================================================================*/
/* Direction Control Pins */
#define COIL_0_DIR_Pin                  GPIO_PIN_3
#define COIL_0_DIR_GPIO_Port            GPIOA

#define COIL_1_DIR_Pin                  GPIO_PIN_4
#define COIL_1_DIR_GPIO_Port            GPIOA

#define COIL_2_DIR_Pin                  GPIO_PIN_5
#define COIL_2_DIR_GPIO_Port            GPIOA

/* Enable Control Pins */
#define COIL_0_EN_Pin                   GPIO_PIN_6
#define COIL_0_EN_GPIO_Port             GPIOA

#define COIL_1_EN_Pin                   GPIO_PIN_7
#define COIL_1_EN_GPIO_Port             GPIOA

#define COIL_2_EN_Pin                   GPIO_PIN_4
#define COIL_2_EN_GPIO_Port             GPIOC

/* H-Bridge Control Macros */
#define COIL_0_ENABLE()                 HAL_GPIO_WritePin(COIL_0_EN_GPIO_Port, COIL_0_EN_Pin, GPIO_PIN_SET)
#define COIL_0_DISABLE()                HAL_GPIO_WritePin(COIL_0_EN_GPIO_Port, COIL_0_EN_Pin, GPIO_PIN_RESET)
#define COIL_0_SET_FORWARD()            HAL_GPIO_WritePin(COIL_0_DIR_GPIO_Port, COIL_0_DIR_Pin, GPIO_PIN_RESET)
#define COIL_0_SET_REVERSE()            HAL_GPIO_WritePin(COIL_0_DIR_GPIO_Port, COIL_0_DIR_Pin, GPIO_PIN_SET)

#define COIL_1_ENABLE()                 HAL_GPIO_WritePin(COIL_1_EN_GPIO_Port, COIL_1_EN_Pin, GPIO_PIN_SET)
#define COIL_1_DISABLE()                HAL_GPIO_WritePin(COIL_1_EN_GPIO_Port, COIL_1_EN_Pin, GPIO_PIN_RESET)
#define COIL_1_SET_FORWARD()            HAL_GPIO_WritePin(COIL_1_DIR_GPIO_Port, COIL_1_DIR_Pin, GPIO_PIN_RESET)
#define COIL_1_SET_REVERSE()            HAL_GPIO_WritePin(COIL_1_DIR_GPIO_Port, COIL_1_DIR_Pin, GPIO_PIN_SET)

#define COIL_2_ENABLE()                 HAL_GPIO_WritePin(COIL_2_EN_GPIO_Port, COIL_2_EN_Pin, GPIO_PIN_SET)
#define COIL_2_DISABLE()                HAL_GPIO_WritePin(COIL_2_EN_GPIO_Port, COIL_2_EN_Pin, GPIO_PIN_RESET)
#define COIL_2_SET_FORWARD()            HAL_GPIO_WritePin(COIL_2_DIR_GPIO_Port, COIL_2_DIR_Pin, GPIO_PIN_RESET)
#define COIL_2_SET_REVERSE()            HAL_GPIO_WritePin(COIL_2_DIR_GPIO_Port, COIL_2_DIR_Pin, GPIO_PIN_SET)

/*============================================================================*/
/* ADC INPUTS (Sun Sensors)                                                  */
/*============================================================================*/
#define SUN_SENS_0_Pin                  GPIO_PIN_3
#define SUN_SENS_0_GPIO_Port            GPIOC
#define SUN_SENS_0_ADC_CHANNEL          ADC_CHANNEL_4

#define SUN_SENS_1_Pin                  GPIO_PIN_0
#define SUN_SENS_1_GPIO_Port            GPIOA
#define SUN_SENS_1_ADC_CHANNEL          ADC_CHANNEL_5

#define SUN_SENS_2_Pin                  GPIO_PIN_1
#define SUN_SENS_2_GPIO_Port            GPIOA
#define SUN_SENS_2_ADC_CHANNEL          ADC_CHANNEL_6

#define SUN_SENS_3_Pin                  GPIO_PIN_2
#define SUN_SENS_3_GPIO_Port            GPIOA
#define SUN_SENS_3_ADC_CHANNEL          ADC_CHANNEL_7

/*============================================================================*/
/* WATCHDOG                                                                   */
/*============================================================================*/
#define WDI_EXT_Pin                     GPIO_PIN_5
#define WDI_EXT_GPIO_Port               GPIOC

#define WATCHDOG_TOGGLE()               HAL_GPIO_TogglePin(WDI_EXT_GPIO_Port, WDI_EXT_Pin)
#define WATCHDOG_SET()                  HAL_GPIO_WritePin(WDI_EXT_GPIO_Port, WDI_EXT_Pin, GPIO_PIN_SET)
#define WATCHDOG_RESET()                HAL_GPIO_WritePin(WDI_EXT_GPIO_Port, WDI_EXT_Pin, GPIO_PIN_RESET)

/*============================================================================*/
/* SENSOR INTERRUPTS (Optional)                                              */
/*============================================================================*/
#define MAG_INT_Pin                     GPIO_PIN_7
#define MAG_INT_GPIO_Port               GPIOC
#define MAG_INT_EXTI_IRQn               EXTI9_5_IRQn

#define GYRO_INT_Pin                    GPIO_PIN_8
#define GYRO_INT_GPIO_Port              GPIOC
#define GYRO_INT_EXTI_IRQn              EXTI9_5_IRQn

/*============================================================================*/
/* DEBUG UART (Optional)                                                      */
/*============================================================================*/
#define USART1_TX_Pin                   GPIO_PIN_9
#define USART1_TX_GPIO_Port             GPIOA
#define USART1_TX_AF                    GPIO_AF7_USART1

#define USART1_RX_Pin                   GPIO_PIN_10
#define USART1_RX_GPIO_Port             GPIOA
#define USART1_RX_AF                    GPIO_AF7_USART1

#define DEBUG_UART_BAUDRATE             115200

/*============================================================================*/
/* GENERAL PURPOSE GPIO                                                       */
/*============================================================================*/
#define GPIO_0_Pin                      GPIO_PIN_0
#define GPIO_0_GPIO_Port                GPIOC

#define GPIO_1_Pin                      GPIO_PIN_1
#define GPIO_1_GPIO_Port                GPIOC

#define GPIO_2_Pin                      GPIO_PIN_2
#define GPIO_2_GPIO_Port                GPIOC

#define STATUS_LED_Pin                  GPIO_PIN_11
#define STATUS_LED_GPIO_Port            GPIOB

#define LED_ON()                        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET)
#define LED_OFF()                       HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET)
#define LED_TOGGLE()                    HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin)

/*============================================================================*/
/* DEBUG INTERFACE (SWD)                                                      */
/*============================================================================*/
#define SWDIO_Pin                       GPIO_PIN_13
#define SWDIO_GPIO_Port                 GPIOA

#define SWCLK_Pin                       GPIO_PIN_14
#define SWCLK_GPIO_Port                 GPIOA

/* Note: SWD pins are configured automatically by the debug hardware */
/* Do not reconfigure PA13/PA14 unless debugging is not needed */

/*============================================================================*/
/* GPIO PIN COLLECTIONS (for initialization loops)                           */
/*============================================================================*/

/* All GPIO output pins for magnetorquer control */
#define MAGNETORQUER_GPIO_PINS  (COIL_0_EN_Pin | COIL_1_EN_Pin | \
                                 COIL_0_DIR_Pin | COIL_1_DIR_Pin | COIL_2_DIR_Pin)

/* All GPIO Port A pins used */
#define GPIOA_ALL_PINS          (CAN1_RX_Pin | CAN1_TX_Pin | \
                                 SUN_SENS_1_Pin | SUN_SENS_2_Pin | SUN_SENS_3_Pin | \
                                 COIL_0_DIR_Pin | COIL_1_DIR_Pin | COIL_2_DIR_Pin | \
                                 COIL_0_EN_Pin | COIL_1_EN_Pin | \
                                 USART1_TX_Pin | USART1_RX_Pin | \
                                 SWDIO_Pin | SWCLK_Pin)

/* All GPIO Port B pins used */
#define GPIOB_ALL_PINS          (COIL_0_PWM_Pin | COIL_1_PWM_Pin | COIL_2_PWM_Pin | \
                                 SPI1_CS_MAG_Pin | SPI1_SCK_Pin | SPI1_MISO_Pin | SPI1_MOSI_Pin | \
                                 I2C1_SCL_Pin | I2C1_SDA_Pin | STATUS_LED_Pin)

/* All GPIO Port C pins used */
#define GPIOC_ALL_PINS          (GPIO_0_Pin | GPIO_1_Pin | GPIO_2_Pin | \
                                 SUN_SENS_0_Pin | COIL_2_EN_Pin | WDI_EXT_Pin | \
                                 SPI1_CS_GYRO_Pin | MAG_INT_Pin | GYRO_INT_Pin)

/*============================================================================*/
/* COMPILE TIME CONFIGURATION CHECKS                                         */
/*============================================================================*/

/* Verify that PWM configuration is valid */
#if PWM_ARR_VALUE > 65535
    #error "PWM_ARR_VALUE exceeds 16-bit timer limit. Adjust prescaler or frequency."
#endif

#if PWM_ARR_VALUE < 100
    #warning "PWM_ARR_VALUE is very low. Consider lowering PWM frequency for better resolution."
#endif

/* Exported macro ------------------------------------------------------------*/

/**
  * @brief  Helper macro to calculate PWM duty cycle from percentage
  * @param  percent: Duty cycle percentage (0-100)
  * @retval CCR value for timer
  */
#define PWM_DUTY_PERCENT_TO_CCR(percent) \
    ((uint32_t)(((percent) * PWM_ARR_VALUE) / 100))

/**
  * @brief  Helper macro to calculate PWM percentage from CCR value
  * @param  ccr: Timer CCR value
  * @retval Duty cycle percentage (0-100)
  */
#define PWM_CCR_TO_DUTY_PERCENT(ccr) \
    ((uint32_t)(((ccr) * 100) / PWM_ARR_VALUE))

/* Exported functions prototypes ---------------------------------------------*/

/* Pin configuration functions (to be implemented in pin_config.c) */
void MX_GPIO_Init_All(void);
void MX_Magnetorquer_GPIO_Init(void);
void MX_Sensor_INT_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __PIN_CONFIG_H */

/**
  * @}
  */

/**
  * @}
  */
