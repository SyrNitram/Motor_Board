/*
 * motor.h
 *
 *  Created on: 29 avr. 2022
 *      Author: Martin RYS
 */

#ifndef MOTOR_INC_MOTOR_H_
#define MOTOR_INC_MOTOR_H_

//TODO: entrer nombre de pas encoder et effet hall pour un tour

#include "common.h"

#define ENCODER_STEPS_BY_ROUND_wheel 	5
#define HALL_STEPS_BY_ROUND_wheel 		930
#define HALL_STEPS_BY_ROUND_stander		12
#define STANDER_PITCH					5/1000
#define WHEEL_PERIMETER					20*3.14

typedef enum {

    POSITION_0 = 0 ,
    POSITION_1     ,
    POSITION_2     ,
    POSITION_3     ,
    POSITION_4     ,
    POSITION_5     ,
    POSITION_MAX   ,

} POSITION_t;

extern uint8_t position_to_hall [ POSITION_MAX ] ;

extern uint8_t hall_to_position [ 8 ] ;

extern uint8_t next_position_CW [ POSITION_MAX ] ;

extern uint8_t next_position_CCW [ POSITION_MAX ];

/* Mise  à jour de la structure motor_board
 * Calcul de la vitesse intantanée s'il sagit d'une roue
 * Determine le sens de rotation
 * Met à jour l'angle magnetique de la roue
 */
void motor_Update_Encoder(void);


void
Motor_UpdatePosition();

HAL_StatusTypeDef
Turn_Off_Motor( Global_Struct_t * inst );

HAL_StatusTypeDef
Turn_On_Motor( Global_Struct_t * inst );

HAL_StatusTypeDef
Set_ControlType ( Global_Struct_t * inst, MOTOR_CTRL_TYPE_t type );

HAL_StatusTypeDef
Set_setpoint( Global_Struct_t * inst, int32_t setpoint );

HAL_StatusTypeDef
Motor_ctrl(Global_Struct_t * inst, uint32_t callback_period_us);

int32_t
Get_Target_PWM ( void );

HAL_StatusTypeDef
Set_Target_PWM ( Global_Struct_t * inst, int32_t dc );


#endif /* MOTOR_INC_MOTOR_H_ */
