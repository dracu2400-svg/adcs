/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */

#include "main.h"
#include "stm32l4xx_it.h"
/* Uncomment when HAL is available */
/* #include "stm32l4xx_hal.h" */

/* External variables --------------------------------------------------------*/
/* Uncomment when peripherals are configured */
/* extern CAN_HandleTypeDef hcan1; */
/* extern I2C_HandleTypeDef hi2c1; */
/* extern SPI_HandleTypeDef hspi1; */
/* extern TIM_HandleTypeDef htim2; */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* HAL_IncTick(); */

  /* Uncomment when FreeRTOS is integrated */
  /* #if (INCLUDE_xTaskGetSchedulerState == 1) */
  /*   if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) */
  /*   { */
  /*     xPortSysTickHandler(); */
  /*   } */
  /* #endif */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles CAN1 TX interrupt.
  */
void CAN1_TX_IRQHandler(void)
{
  /* HAL_CAN_IRQHandler(&hcan1); */
}

/**
  * @brief This function handles CAN1 RX0 interrupt.
  */
void CAN1_RX0_IRQHandler(void)
{
  /* HAL_CAN_IRQHandler(&hcan1); */
}

/**
  * @brief This function handles CAN1 RX1 interrupt.
  */
void CAN1_RX1_IRQHandler(void)
{
  /* HAL_CAN_IRQHandler(&hcan1); */
}

/**
  * @brief This function handles CAN1 SCE interrupt.
  */
void CAN1_SCE_IRQHandler(void)
{
  /* HAL_CAN_IRQHandler(&hcan1); */
}

/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  /* HAL_I2C_EV_IRQHandler(&hi2c1); */
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  /* HAL_I2C_ER_IRQHandler(&hi2c1); */
}

/**
  * @brief This function handles SPI1 global interrupt.
  */
void SPI1_IRQHandler(void)
{
  /* HAL_SPI_IRQHandler(&hspi1); */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* HAL_TIM_IRQHandler(&htim2); */
}
