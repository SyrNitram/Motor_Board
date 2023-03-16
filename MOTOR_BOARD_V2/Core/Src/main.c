
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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

#include "drv8353.h"
#include "main.h"
#include "setup.h"
#include "motor.h"
#include "stander.h"
#include "sub_main.h"

ADC_HandleTypeDef 	hadc;
DMA_HandleTypeDef 	hdma_adc;
CAN_HandleTypeDef   hcan;
RTC_HandleTypeDef 	hrtc;
I2C_HandleTypeDef   hi2c1;
SPI_HandleTypeDef	hspi1;
TIM_HandleTypeDef 	htim1;
TIM_HandleTypeDef   htim2;
TIM_HandleTypeDef   htim3;
TIM_HandleTypeDef 	htim7;
UART_HandleTypeDef 	huart1;
Global_Struct_t		board;


/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	// MCU Configuration
	MCU_SETUP();

	//Configuration du driver automatique à chaque reveil
	Turn_Off_Motor( &board ); //Mise en sommeil tant que non utilisé

	stander_Update_Eos( &board );
	SUB_MAIN();

	/* Infinite loop */
	while (1) {
	}
}

//Interuption déclenché par les capteurs à effet hall,l'encoder et le capteur fin de course
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if( (GPIO_Pin == DRV_NFAULT_Pin) ){
			DRV_FaultResolution();
	}

#ifdef ENCODER_ON
	if((GPIO_Pin == ENC_A_Pin) || (GPIO_Pin == ENC_B_Pin)){
		motor_Update_Encoder();   //inutil pour le moment
	}
#endif



	if((GPIO_Pin == HALL_A_Pin) || (GPIO_Pin == HALL_B_Pin) || (GPIO_Pin == HALL_C_Pin)){
		DRV_UpdateHalls();
		Motor_UpdatePosition();
	}

}


/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{

	}
}

