/*
 * motor.c
 *
 *  Created on: 29 avr. 2022
 *      Author: Martin RYS
 */

//======================================
//				INCLUDE
//======================================
#include "stander.h"
#include "config.h"
#include "motor.h"
#include "console.h"
#include "pid.h"
#include "drv8353.h"
//======================================
//				DEFINE
//======================================
#define HOMING_Duty_Cycle	-12
#define HOMING_TIMEOUT		10000//ms = 10s
//======================================

// ==================================================
//     LOCAL VARIABLES
// ==================================================
static uint32_t t_init = 0;

//======================================
//				FUNCTIONS
//======================================

/*
 * Renvoie la valeur de l'input du EOS
 */
HAL_StatusTypeDef
stander_Update_Eos( Global_Struct_t * inst){
	HAL_StatusTypeDef status = HAL_ERROR;

	if(inst != NULL)
	{
		inst->sensor.eos = HAL_GPIO_ReadPin(EOS_GPIO_Port, EOS_Pin);
		status = HAL_OK;
	}
	return status ;
}

/*
 * Renvoie la valeur brute de l'ADC
 */
uint32_t
Get_Sensors_Value( Global_Struct_t * inst, SENSOR_ADC_CHANNEL_t channel )
{
	uint32_t ret_val = 0;
	if(inst != NULL)
		ret_val = inst->sensor.rx_dma[ channel ];

	return ret_val;
}


HAL_StatusTypeDef
Callback_HomingProcess ( Global_Struct_t * inst )
{
	HAL_StatusTypeDef status = HAL_ERROR;

	if(inst != NULL)
	{
		//Tant que l'obturateur n'est pas détecté
		// ou que le timeout n'est pas atteint
		if( ( ( HAL_GetTick() - t_init ) >= HOMING_TIMEOUT ) )
		{
			inst->flags.status_flag.single.homing_status = Failed;
	//        error = Homing_error;
			status = Turn_Off_Motor( inst );

			CONSOLE("Homing failed\r");
		}

		else if ( inst->sensor.eos )
		{
			inst->motor.pos.position = 0;

			inst->flags.status_flag.single.homing_status = Done;


			status = Set_ControlType( inst , CTRL_POSITION );
			status |= Set_setpoint(inst , 0);


			CONSOLE("Homing done\r");
		}
		else if( flag_stop_homing == true) //Force en Z détectée -> poignée bloquée par quelque chose
		{
			//TODO : Add homing failed
		}
	//    else
		//    	CONSOLE("In progress\r");
	}

	return status;
}



/*
 * Lance la procédure de Homing
 */
HAL_StatusTypeDef
HOMING_PROCESS_monitoring(  Global_Struct_t * inst )
{
	HAL_StatusTypeDef status = HAL_ERROR;
if(inst != NULL){
	//On active le controle moteur s'il n'est pas encore activé 	!! Dangereux en mode calibration
/*	
	if( !inst->control.IsOn )
		status = Turn_On_Motor(inst );
*/
	if (inst->control.IsOn) {
		//On configure le type de contrôle en Open Loop
		status = Set_ControlType( inst  , CTRL_OPEN_LOOP );
		status |= Set_setpoint( inst , HOMING_Duty_Cycle );

		//On signale que la procedure est en cours
		inst->flags.status_flag.single.homing_status = In_progress;
		//CONSOLE("Homing start \r");

		t_init = HAL_GetTick();
	}
}
return status;
}
