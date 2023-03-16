/*
 * motor.c
 *
 *  Created on: 29 avr. 2022
 *      Author: Martin RYS
 */

#include "drv8353.h"
#include "common.h"
#include "motor.h"
#include "pid.h"
/*
#define Max_DC	97
#define Min_DC	-97
*/

uint8_t position_to_hall [ POSITION_MAX ] =
{
    [ POSITION_0 ] = 0b110 ,
    [ POSITION_1 ] = 0b100 ,
    [ POSITION_2 ] = 0b101 ,
    [ POSITION_3 ] = 0b001 ,
    [ POSITION_4 ] = 0b011 ,
    [ POSITION_5 ] = 0b010 ,
};

uint8_t hall_to_position [ 8 ] =
{
    [ 0b000 ] = POSITION_MAX ,
    [ 0b001 ] = POSITION_3   ,
    [ 0b010 ] = POSITION_5   ,
    [ 0b011 ] = POSITION_4   ,
    [ 0b100 ] = POSITION_1   ,
    [ 0b101 ] = POSITION_2   ,
    [ 0b110 ] = POSITION_0   ,
    [ 0b111 ] = POSITION_MAX ,
};

uint8_t next_position_CW [ POSITION_MAX ] =
{
    [ POSITION_0 ] = POSITION_1 ,
    [ POSITION_1 ] = POSITION_2 ,
    [ POSITION_2 ] = POSITION_3 ,
    [ POSITION_3 ] = POSITION_4 ,
    [ POSITION_4 ] = POSITION_5 ,
    [ POSITION_5 ] = POSITION_0 ,
};

uint8_t next_position_CCW [ POSITION_MAX ] =
{
    [ POSITION_0 ] = POSITION_5 ,
    [ POSITION_1 ] = POSITION_0 ,
    [ POSITION_2 ] = POSITION_1 ,
    [ POSITION_3 ] = POSITION_2 ,
    [ POSITION_4 ] = POSITION_3 ,
    [ POSITION_5 ] = POSITION_4 ,
};

HAL_StatusTypeDef reset_all_PID();


uint8_t HALL_Value_to_State(uint8_t hall_value){

	uint8_t ret_val = 0;
	uint8_t state[8];
	//0bABC
	state[0b000] = 0; //STOP		-- mode impossible
	state[0b110] = 1;
	state[0b100] = 2;
	state[0b101] = 3;
	state[0b001] = 4;
	state[0b011] = 5;
	state[0b010] = 6;
	state[0b111] = 7;	//ALIGN 	-- mode impossible

	if( hall_value <= 7 )
		ret_val = state[hall_value];	//retourne l'etat correspondant à la valeur
	return ret_val;
}

uint8_t HALL_State_to_Value(uint8_t hall_state){

	uint8_t ret_val = 0;
	uint8_t value[8];
	//0bABC
	value[0] = 0b000;	//STOP	-- mode impossible
	value[1] = 0b110;
	value[2] = 0b100;
	value[3] = 0b101;
	value[4] = 0b001;
	value[5] = 0b011;
	value[6] = 0b010;
	value[7] = 0b111;	//ALIGN	-- mode impossible

	if( hall_state <= 7 )
		ret_val = value[hall_state]; 	//retourne la valeur correspondant à l'etat

	return ret_val;
}


/*
 * Update motor position
 */
void
Motor_UpdatePosition() {

	//Lecture des entrées HALL
	GPIO_PinState hall_A = HAL_GPIO_ReadPin(HALL_B_GPIO_Port, HALL_B_Pin);
	GPIO_PinState hall_B = HAL_GPIO_ReadPin(HALL_A_GPIO_Port, HALL_A_Pin);
	GPIO_PinState hall_C = HAL_GPIO_ReadPin(HALL_C_GPIO_Port, HALL_C_Pin);

	uint8_t halls = (hall_A << 2) | (hall_B << 1) | hall_C;	//On récupère l'état des 3 capteurs
	uint8_t position = hall_to_position[halls];	//On fait correspondre à une position


	if (next_position_CCW[board.motor.hall.state] == position)//Si on tourne clockwise
	{
		if (board.motor.pos.position >0 ) board.motor.pos.position--;
		switch ( position )
		{
			case 0 :
				board.motor.phase_sensored = C;
				break;
			case 1 :
				board.motor.phase_sensored = C;
				break;
			case 2 :
				board.motor.phase_sensored = B;
				break;
			case 3 :
				board.motor.phase_sensored = B;
				break;
			case 4 :
				board.motor.phase_sensored = A;
				break;
			case 5 :
				board.motor.phase_sensored = A;
				break;
			default:
				board.motor.phase_sensored = B;
				break;

		}
	}
	else if (next_position_CW[board.motor.hall.state] == position)
	{
		board.motor.pos.position++;
		switch ( position )
		{
			case 0 :
				board.motor.phase_sensored = B;
				break;
			case 1 :
				board.motor.phase_sensored = A;
				break;
			case 2 :
				board.motor.phase_sensored = A;
				break;
			case 3 :
				board.motor.phase_sensored = C;
				break;
			case 4 :
				board.motor.phase_sensored = C;
				break;
			case 5 :
				board.motor.phase_sensored = B;
				break;
			default:
				board.motor.phase_sensored = B;
				break;

		}
	}

	board.motor.pos.pos_abs++;
	board.motor.hall.state = position;
}

/*
 * Enable driver and reset control
 */
HAL_StatusTypeDef
Turn_On_Motor( Global_Struct_t * inst )
{
	HAL_StatusTypeDef status = ERROR;
	if( inst != NULL && inst->flags.status_flag.single.calib_mode == 0) //On ne demarre pas les moteurs tant que le mode calibration est activé
	{

		status = reset_all_PID( inst );
		//Start the driver but stay in free-wheel
		status |= DRV_SetMode(DRV_MODE_STOP);
		inst->control.IsOn = true;
		status |= Set_ControlType(inst, CTRL_FREE_WHEEL);
		status |= Set_setpoint( inst, 0);

	}
	return status;
}


/*
 * Disable driver and reset control
 */
HAL_StatusTypeDef
Turn_Off_Motor( Global_Struct_t * inst )
{
	HAL_StatusTypeDef status = ERROR;
	if( inst != NULL)
	{
		status = reset_all_PID( inst );
		//DISABLE DRV
		status |= DRV_SetMode(DRV_MODE_SLEEP);
		inst->control.IsOn = false;
		status |= Set_ControlType( inst, CTRL_FREE_WHEEL);
		status |= Set_setpoint( inst, 0);
	}
	return status;
}


/*
 * Met à jour le type de controle du moteur
 * Un reset des PID est nécessaire.
 */
HAL_StatusTypeDef
Set_ControlType ( Global_Struct_t * inst, MOTOR_CTRL_TYPE_t type )
{
	HAL_StatusTypeDef status = HAL_ERROR;

	if(inst != NULL)
	{
		//Protection contre mauvaises valeurs
		if(type <= CTRL_ALIGN && type >= CTRL_FREE_WHEEL)
		{
			if ( reset_all_PID( inst) == HAL_OK ) //Reset des PIDs à chaque changement de type de contrôle
			{
				inst->control.duty_cycle = 0;
				inst->control.ctrl_type = type;
				status =  HAL_OK;
			}
		}
	}
	return status;
}

/*
 * Met à jour la consigne de la boucle de control désirée
 * instance = boucle de controle
 * setpoint = consigne
 */
HAL_StatusTypeDef
Set_setpoint( Global_Struct_t * inst, int32_t setpoint )
{
	HAL_StatusTypeDef status = HAL_ERROR;

	if(inst != NULL)
	{
		switch (inst->control.ctrl_type){
		case CTRL_OPEN_LOOP:
			if(setpoint >= -98 || setpoint <= 98)
				inst->control.duty_cycle = setpoint;
			else
				inst->control.duty_cycle = 0;
			break;

		case CTRL_ALIGN:
			inst->control.duty_cycle = setpoint;
			break;

		case CTRL_TORQUE:
			inst->control.torque.setpoint = setpoint;
			break;

		case CTRL_SPEED:
			inst->control.speed.setpoint = setpoint;
			break;

		case CTRL_POSITION:
			if((inst->select.type_IsStander) && (setpoint >= 0 || setpoint <= 480)){
				inst->control.position.setpoint = setpoint;
			}
			else
				inst->control.position.setpoint = setpoint;
			break;

		default:
			inst->control.duty_cycle = 0;
			break;
		}
		status = HAL_OK;
	}

	return status;
}

/*
 * Set the duty cycle that is targetted as the setpoint duty cycle
 */
HAL_StatusTypeDef
Set_Target_PWM ( Global_Struct_t * inst, int32_t dc )
{
	HAL_StatusTypeDef status = HAL_ERROR;
	if(inst != NULL)
	{
		if(dc < -98) dc = -98;
		else if (dc > 98) dc = 98;
		inst->control.target_duty_cycle = dc;
		status = HAL_OK;
	}

	return status;
}

/*
 * Retrieve the target duty cycle
 */
int32_t
Get_Target_PWM ( void )
{
	return board.control.target_duty_cycle;
}

/*
 * Fonction de controle du moteur
 * En fonction du type de contrôle on passe par la boucle adéquate
 */
HAL_StatusTypeDef
Motor_ctrl(Global_Struct_t * inst, uint32_t callback_period_us) {

	HAL_StatusTypeDef status = HAL_OK;
	static uint8_t t =0;

	switch (inst->control.ctrl_type) {

	case CTRL_FREE_WHEEL:
		status |= DRV_SetMode(DRV_MODE_STOP); // DRV_MODE_SLEEP OR DRV_MODE_STOP (same action on motor)
		inst->control.duty_cycle = 0;
		break;

	case CTRL_OPEN_LOOP:
		status |= DRV_SetMode(DRV_MODE_OPERATING);
		break;

	case CTRL_TORQUE:
		//TODO: Add torque control if required
		inst->control.duty_cycle = 0;
		break;

	case CTRL_SPEED:
		/*
		status |= DRV_SetMode(DRV_MODE_OPERATING);
		status |= PID_Compute(&(inst->control.speed), (int32_t) inst->motor.speed.rpm, (float) callback_period_us / 1000000.0);
		inst->control.duty_cycle = (int32_t) inst->control.speed.output;
		*/
		inst->control.duty_cycle = 0;
		break;

	case CTRL_POSITION:
		status |= DRV_SetMode(DRV_MODE_OPERATING);
		status |= PID_Compute(&(inst->control.position),	(int32_t) inst->motor.pos.position,	(float) callback_period_us / 1000000.0);

		inst->control.duty_cycle = (int32_t) inst->control.position.output;
		break;

	case CTRL_BRAKE:
		status |= DRV_SetMode(DRV_MODE_BRAKE);
		inst->control.duty_cycle = 0;
		break;

	case CTRL_ALIGN:
		status |= DRV_SetMode(DRV_MODE_ALIGN);
		//limit dutycycle to prevent over-heating
		inst->control.duty_cycle = 	(abs(inst->control.duty_cycle) > 10) ? 10 : inst->control.duty_cycle;
		break;
	}

	if(board.select.type_IsStander)
	{
		if((inst->sensor.eos == true) && inst->control.duty_cycle < 0) //limite basse
			inst->control.duty_cycle = 0;

		if((board.motor.pos.position >= 475) && inst->control.duty_cycle > 0) //limite haute
			inst->control.duty_cycle = 0;
	}

	if(board.flags.status_flag.single.calib_mode == 1) //en mode calibration on desactive les moteurs
		inst->control.duty_cycle = 0;

	status |= Set_Target_PWM( inst, board.control.duty_cycle);

	return status;
}


/*
 * 	PRIVATE FUNCTIONS
 */
HAL_StatusTypeDef
reset_all_PID( Global_Struct_t * inst) {

	HAL_StatusTypeDef status = HAL_OK;

	status |= PID_ResetAll(&(inst->control.position));
	status |= PID_ResetAll(&(inst->control.speed));
	status |= PID_ResetAll(&(inst->control.torque));
	inst->control.position.setpoint = inst->motor.pos.position;
	inst->motor.speed.rpm =0;
	inst->motor.torque = 0;

	status |= PID_ResetAll(&(inst->control.voltage));
	inst->motor.actualDC = 0;

	return status;
}


//Fonctions liées à l'encodeur
#ifdef ENCODER_ON
void motor_Update_Encoder(){
/*
	static uint32_t time1 = 0;
	uint32_t time2 = HAL_GetTick();

	ENCODER_t encoder_struct = motor_board.motor.encoder;

	uint8_t encoder_value = (encoder_struct.A <<1) | encoder_struct.B;

	uint8_t encoder_state =  ENCODER_Value_to_State(encoder_value);

	//Si wheel
	if(!(motor_board.type_stander)){
		motor_board.motor.speed_rpm = (1.0/ENCODER_STEPS_BY_ROUND_wheel) 	/ ( (float)(time2-time1) /60000.0); 	//vitesse en tour par minutes
		motor_board.motor.speed_mps = motor_board.motor.speed_rpm * WHEEL_PERIMETER / 60.0; 						// m = tour * perimetre		s = min * 60

		//Verification du sens de rotation
		if( ((encoder_state + 1) % 4) == encoder_struct.state )	//new state = old state -1 -> marche arrière
			motor_board.motor.direction = 1; 	//	1:CCW
		else
			motor_board.motor.direction = 0;	// 	0:CW
	}
	time1 = time2;

	encoder_struct.cnt += motor_board.motor.direction? -1 : 1; //increment de la position
	encoder_struct.angle = ENCODER_STEPS_BY_ROUND_wheel/(3*HALL_STEPS_BY_ROUND_wheel) * 360 * encoder_struct.cnt; //mise à jour de l'angle magnétique
*/
}

uint8_t ENCODER_Value_to_State(uint8_t encoder_value){

	uint8_t ret_val = 0;

	uint8_t state[4];
	//0bAB
	state[0b00] = 0;
	state[0b01] = 1;
	state[0b11] = 2;
	state[0b10] = 3;

	if( encoder_value <= 3 )
		ret_val = state[encoder_value];	//retourne l'etat correspondant à la valeur

	return ret_val;
}

uint8_t ENCODER_State_to_Value(uint8_t encoder_state){
	uint8_t ret_val = 0;
	uint8_t value[4];
	//0bAB
	value[0] = 0b00;
	value[1] = 0b01;
	value[2] = 0b11;
	value[3] = 0b10;

	if(encoder_state <= 3)
		ret_val = value[encoder_state];	//retourne la valeur correspondant à l'etat

	return ret_val;
}

#endif
