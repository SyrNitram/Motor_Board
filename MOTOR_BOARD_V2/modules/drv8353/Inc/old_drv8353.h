/*
 * drv8353.h
 *
 *  Created on: Apr 28, 2022
 *      Author:  Martin RYS
 */

#ifndef DRV8353_INC_OLD_DRV8353_H_
#define DRV8353_INC_OLD_DRV8353_H_

/*
//----------------------------------------
//				INCLUDE
//----------------------------------------

#include "stm32f0xx_hal.h"
#include "pid.h"

//----------------------------------------
//			PUBLIC DEFINEs
//----------------------------------------
#define DRV_ROTATION_CW 	1
#define DRV_ROTATION_CCW 	0

#define DRV_BRAKE_ON 		0
#define DRV_BRAKE_OFF 		1

#define HALL_LOW			0
#define HALL_HIGH			1

#define DRV_ENABLE_ON 		1
#define DRV_ENABLE_OFF 		0

#define DRV8353__SPEED__FILTER__ORDER             4.0

// ==================================================
//     EXTERN VARIABLES
// ==================================================

typedef struct {
	PID_t                 pid;

	struct {

		CIRCULAR_BUFFER_t obj;
		float             buffer [ CONFIG__MOTOR_CTRL__PID__DELAY__MAX_DEPTH ];

	} delay;
}Buffer_PID_t;

typedef struct {

    bool                      active;
    MOTOR_CTRL_TYPE_t         type;
	Board_t					  board_type;

	Buffer_PID_t position_ctrl;

	Buffer_PID_t speed_ctrl;

	Buffer_PID_t Iq_ctrl;

	Buffer_PID_t Id_ctrl;

	struct{
		int16_t 	position_hall;
		int32_t 	position_codeur;
		uint32_t 	pos_abs;
		float		angle;
		int16_t 	speed;
		int16_t 	torque;

		struct{
			float ia;
			float ib;
			float ic;

			float id;
			float iq;
		}current;//courants mesurés

		struct//sur 8 bits car de -100 à 100
		{
			int8_t	u_A;
			int8_t	u_B;
			int8_t	u_C;
		}pwm_to_send;
	}motor_data;

} MOTOR_CTRL_RUNTIME_t;




HAL_StatusTypeDef
DRV8353_InitPlatform(void) ;

void
DRV8353_Check_Faults( void );

void
DRV8353_Enable( void );

void
DRV8353_Disable( void );
*/
#endif /* DRV8353_INC_OLD_DRV8353_H_ */
