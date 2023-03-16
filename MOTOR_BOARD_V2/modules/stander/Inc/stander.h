/*
 * motor.h
 *
 *  Created on: 29 avr. 2022
 *      Author: Martin RYS
 */

#ifndef STANDER_INC_STANDER_H_
#define STANDER_INC_STANDER_H_



#include "common.h"


HAL_StatusTypeDef
stander_Update_Eos(Global_Struct_t * inst);

uint32_t
Get_Sensors_Value( Global_Struct_t * inst, SENSOR_ADC_CHANNEL_t channel );

HAL_StatusTypeDef
HOMING_PROCESS_monitoring(  Global_Struct_t * inst );

HAL_StatusTypeDef
Callback_HomingProcess( Global_Struct_t * inst );

#endif /* MOTOR_INC_MOTOR_H_ */
