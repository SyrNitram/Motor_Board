/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "stm32f0xx_it.h"
#include "com_can.h"

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim7;
extern CAN_HandleTypeDef hcan;

CAN_RxHeaderTypeDef pRxHeader; //declare header for message reception
extern CAN_TxHeaderTypeDef pTxHeader;


/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
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
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
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
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 2 and 3 interrupts.
  */
void EXTI2_3_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(DRV_NFAULT_Pin);
}

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(HALL_A_Pin);
  HAL_GPIO_EXTI_IRQHandler(HALL_B_Pin);
  HAL_GPIO_EXTI_IRQHandler(HALL_C_Pin);
#ifdef ENCODER_ON
  HAL_GPIO_EXTI_IRQHandler(ENC_A_Pin);
  HAL_GPIO_EXTI_IRQHandler(ENC_B_Pin);
#endif
}

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Ch1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_adc);
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}


/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim7);
}

/**
* @brief  This function handles CAN1 RX0 interrupt request.
* @param  None
* @retval None
*/
void CEC_CAN_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan);

  uint8_t               RxData[8];

  /* Get RX message */
  HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &pRxHeader, RxData);

  memcpy(&msg_can.RX.bytes[msg_can.RX.idx][0], &RxData[0], 8) ;

  msg_can.RX.idx ++;
  msg_can.RX.last_time = HAL_GetTick();

}
