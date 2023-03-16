/*
 * can_lib_RX.c
 *
 *  Created on: Oct 15, 2021
 *      Author:
 */


// ==================================================
//     INCLUDE
// ==================================================

#include <com_can.h>
#include <main.h>
#include <stm32f0xx_it.h>
#include "motor.h"
#include "stander.h"
#include "pid.h"

// ====================================
//		 FUNCTION PROTOTYPE
// ====================================

// ====================================

HAL_StatusTypeDef
Can_frame_mgt ( Global_Struct_t * inst )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if(inst != NULL)
    {
		int16_t valeur_idx_Rx_trame; //on crée une variable dans laquel on rangera l'index du message CAN reçu
		int16_t valeur_sub_idx_RX_trame;//Sous index de la trame

		union
		{
			union {
				struct{
					int16_t msb;
					int16_t lsb;
				};
				int32_t  i32 ;
			}integer;
			float	 d_float;
		}d;

		uint8_t i = 0;
		do
		{

			memcpy(&valeur_idx_Rx_trame, &msg_can.RX.bytes[i][0], 2); //On range dans notre variable "valeur_idx_Rx_trame" la valeur de l'index du message reçu

			memcpy(&d.integer.i32, &msg_can.RX.bytes[i][4], sizeof(d.integer.i32));

			float k;

			switch ( valeur_idx_Rx_trame )
			{

			case IDX_CMD_SET_MOTOR_ON_OFF:
				memcpy(&valeur_sub_idx_RX_trame, &msg_can.RX.bytes[i][2], 2);

				if (valeur_sub_idx_RX_trame == SUB_IDX_MOTOR_ON )
					status = Turn_On_Motor( inst );
				else if (valeur_sub_idx_RX_trame == SUB_IDX_MOTOR_OFF)
					status = Turn_Off_Motor( inst );

				break;

			case IDX_CMD_SET_SETPOINT:
				status = Set_setpoint(inst , d.integer.i32 );

				break;

			case IDX_CMD_SET_CTRL_MODE :

				status = Set_ControlType( inst , d.integer.i32 );
				break;

			case IDX_CMD_START_HOMING:
				status = HOMING_PROCESS_monitoring( inst);

				break;

			case IDX_REP_GET_VERSION :
				status = CAN_Rep_Version();
				break;

			case IDX_CMD_SET_CALIB_ON_OFF:
				memcpy(&valeur_sub_idx_RX_trame, &msg_can.RX.bytes[i][2], 2);

				if (valeur_sub_idx_RX_trame == SUB_IDX_CALIB_ON){
					inst->flags.status_flag.single.calib_mode = 1;
					status = Turn_Off_Motor(inst);
				}
				else if (valeur_sub_idx_RX_trame == SUB_IDX_CALIB_OFF){
					inst->flags.status_flag.single.calib_mode = 0;
					status = Turn_On_Motor(inst);
				}
				break;


	////////////////////////////////////////////////////////
			case IDX_CMD_SET_KP_POS :
				inst->control.duty_cycle = 0;//Pour que le moteur ne tourne pas

				d.integer.i32 = SWAP_32BITS(d.integer.i32);
				k = d.d_float;

				status = PID_SetCoefs(&inst->control.position, k, inst->control.position.coef.ki, inst->control.position.coef.kd);
				board.control.position.setpoint = board.motor.pos.position;
				board.motor.speed.rpm =0;
				board.motor.torque = 0;
				break;

			case IDX_CMD_SET_KP_SPEED :
				board.control.duty_cycle = 0;//Pour que le moteur ne tourne pas

				d.integer.i32 = SWAP_32BITS(d.integer.i32);
				k = d.d_float;

				status = PID_SetCoefs(&inst->control.speed, k, inst->control.speed.coef.ki , inst->control.speed.coef.kd);
				board.control.position.setpoint = board.motor.pos.position;
				board.motor.speed.rpm =0;
				board.motor.torque = 0;
				break;

			case IDX_CMD_SET_KP_TORQUE :
				inst->control.duty_cycle = 0;//Pour que le moteur ne tourne pas

				d.integer.i32 = SWAP_32BITS(d.integer.i32);
				k = d.d_float;

				status = PID_SetCoefs(&inst->control.voltage, k, inst->control.voltage.coef.ki, inst->control.voltage.coef.kd) ;
				board.control.position.setpoint = board.motor.pos.position;
				board.motor.speed.rpm =0;
				board.motor.torque = 0;
				break;


	//////////////////////////////////////////
			case IDX_CMD_SET_KI_POS :
				board.control.duty_cycle = 0;//Pour que le moteur ne tourne pas

				d.integer.i32 = SWAP_32BITS(d.integer.i32);
				k = d.d_float;

				status = PID_SetCoefs(&inst->control.position, inst->control.position.coef.kp, k, inst->control.position.coef.kd);
				board.control.position.setpoint = board.motor.pos.position;
				board.motor.speed.rpm =0;
				board.motor.torque = 0;
				break;

			case IDX_CMD_SET_KI_SPEED :
				inst->control.duty_cycle = 0;//Pour que le moteur ne tourne pas

				d.integer.i32 = SWAP_32BITS(d.integer.i32);
				k = d.d_float;

				status = PID_SetCoefs(&inst->control.speed, inst->control.speed.coef.kp, k, inst->control.speed.coef.kd);
				board.control.position.setpoint = board.motor.pos.position;
				board.motor.speed.rpm =0;
				board.motor.torque = 0;
				break;

			case IDX_CMD_SET_KI_TORQUE :
				inst->control.duty_cycle = 0;//Pour que le moteur ne tourne pas

				d.integer.i32 = SWAP_32BITS(d.integer.i32);
				k = d.d_float;

				status = PID_SetCoefs(&inst->control.voltage, inst->control.voltage.coef.kp, k, inst->control.voltage.coef.kd) ;
				board.control.position.setpoint = board.motor.pos.position;
				board.motor.speed.rpm =0;
				board.motor.torque = 0;
				break;


	///////////////////////////////////////
			case IDX_CMD_SET_KD_POS :
				inst->control.duty_cycle = 0;//Pour que le moteur ne tourne pas

				d.integer.i32 = SWAP_32BITS(d.integer.i32);
				k = d.d_float;

				status = PID_SetCoefs(&inst->control.position, inst->control.position.coef.kp, inst->control.position.coef.ki, k);
				board.control.position.setpoint = board.motor.pos.position;
				board.motor.speed.rpm =0;
				board.motor.torque = 0;
				break;

			case IDX_CMD_SET_KD_SPEED :
				inst->control.duty_cycle = 0;//Pour que le moteur ne tourne pas

				d.integer.i32 = SWAP_32BITS(d.integer.i32);
				k = d.d_float;

				status = PID_SetCoefs(&inst->control.speed, inst->control.speed.coef.kp, inst->control.speed.coef.ki ,k);
				board.control.position.setpoint = board.motor.pos.position;
				board.motor.speed.rpm =0;
				board.motor.torque = 0;
				break;

			case IDX_CMD_SET_KD_TORQUE :
				inst->control.duty_cycle = 0;//Pour que le moteur ne tourne pas

				d.integer.i32 = SWAP_32BITS(d.integer.i32);
				k = d.d_float;

				status = PID_SetCoefs(&inst->control.voltage, inst->control.voltage.coef.kp, inst->control.voltage.coef.ki, k) ;
				board.control.position.setpoint = board.motor.pos.position;
				board.motor.speed.rpm =0;
				board.motor.torque = 0;
				break;

	///////////////////////////////////////////

			case IDX_CMD_AUTO_CALIB :

				inst->flags.status_flag.single.auto_calib_status = In_progress;
				
				inst->sensor.fx.force.pente = 1.0/( (float)d.integer.msb ); 	//On inverse la pente pour multiplier au lieu de diviser
				inst->sensor.fz.force.pente = 1.0/( (float)d.integer.lsb );	//On gagne du temps de calcul lors de la tache ADC
			
				break;

			default:

				status = HAL_ERROR;
				//TODO: Add case
				break;
			}

			//Reset du buffer
			memset(&msg_can.RX.bytes[i][0], 0, sizeof(msg_can.RX.bytes[i]));

			i++;

		}while( i < msg_can.RX.idx);

		msg_can.RX.idx = 0;
    }
	return status;
}
