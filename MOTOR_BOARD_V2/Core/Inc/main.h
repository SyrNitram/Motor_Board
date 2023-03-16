/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "stm32f0xx_hal.h"
#include <stdBool.h>


/* Public defines -----------------------------------------------------------*/
#define DRV_SIA_Pin GPIO_PIN_0
#define DRV_SIA_GPIO_Port GPIOA

#define DRV_SIB_Pin GPIO_PIN_1
#define DRV_SIB_GPIO_Port GPIOA

#define DRV_SIC_Pin GPIO_PIN_2
#define DRV_SIC_GPIO_Port GPIOA

#define DRV_NFAULT_Pin GPIO_PIN_3
#define DRV_NFAULT_GPIO_Port GPIOA

#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA

#define HANDLE_X_Pin GPIO_PIN_0
#define HANDLE_X_GPIO_Port GPIOB

#define HANDLE_Z_Pin GPIO_PIN_1
#define HANDLE_Z_GPIO_Port GPIOB

#define ENC_A_Pin GPIO_PIN_10
#define ENC_A_GPIO_Port GPIOB

#define ENC_B_Pin GPIO_PIN_11
#define ENC_B_GPIO_Port GPIOB

#define HALL_A_Pin GPIO_PIN_6
#define HALL_A_GPIO_Port GPIOC

#define HALL_B_Pin GPIO_PIN_7
#define HALL_B_GPIO_Port GPIOC

#define HALL_C_Pin GPIO_PIN_8
#define HALL_C_GPIO_Port GPIOC

#define DRV_ENABLE_Pin GPIO_PIN_9			//enable
#define DRV_ENABLE_GPIO_Port GPIOC

#define DRV_INHA_Pin GPIO_PIN_8				//pwm
#define DRV_INHA_GPIO_Port GPIOA

#define DRV_INHB_Pin GPIO_PIN_9				//hall b
#define DRV_INHB_GPIO_Port GPIOA

#define DRV_INHC_Pin GPIO_PIN_10			//dir
#define DRV_INHC_GPIO_Port GPIOA

#define DRV_INLA_Pin GPIO_PIN_10			//hall a
#define DRV_INLA_GPIO_Port GPIOC

#define DRV_INLB_Pin GPIO_PIN_11			//hall c
#define DRV_INLB_GPIO_Port GPIOC

#define DRV_INLC_Pin GPIO_PIN_12			//nBrake
#define DRV_INLC_GPIO_Port GPIOC

#define EOS_Pin GPIO_PIN_3
#define EOS_GPIO_Port GPIOB


#define SELECT_Pin GPIO_PIN_5
#define SELECT_GPIO_Port GPIOB

/* PUBLIC STRUCTURE */

/* PUBLIC HANDLERS */
extern ADC_HandleTypeDef 	hadc;
extern DMA_HandleTypeDef 	hdma_adc;
extern CAN_HandleTypeDef    hcan;
extern RTC_HandleTypeDef 	hrtc;
extern I2C_HandleTypeDef 	hi2c1;
extern SPI_HandleTypeDef 	hspi1;
extern TIM_HandleTypeDef 	htim1;
extern TIM_HandleTypeDef 	htim2;
extern TIM_HandleTypeDef 	htim3;
extern TIM_HandleTypeDef 	htim7;
extern UART_HandleTypeDef	huart1;

void Error_Handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
