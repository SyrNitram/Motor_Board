/*
 * file        : pid.c
 * description : <insert here>
 * copyright   : (c) 2020 GEMA
 */

// ==================================================
//     INCLUDE
// ==================================================

#include "pid.h"



// ==================================================
//     PUBLIC FUNCTIONS
// ==================================================

HAL_StatusTypeDef
PID_Init ( PID_t * instance )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( instance != NULL )
    {
        memset( ( void * )&instance->coef , 0x00 , sizeof( instance->coef ) );

        instance->threshold.enabled = false;
        instance->delay.depth       = PID_BYPASS_DELAY;

        if ( instance->threshold.min < instance->threshold.max )
        {
            instance->threshold.enabled = true;
        }

        status = PID_ResetAll( instance );
    }

    return status;
}

HAL_StatusTypeDef
PID_ResetAll ( PID_t * instance )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( instance != NULL )
    {
        memset( ( void * )&instance->control , 0x00 , sizeof( instance->control ) );

        instance->setpoint = 0;
        instance->delay.enabled = false;

        status = HAL_OK;
    }

    return status;
}

HAL_StatusTypeDef
PID_SetCoefs ( PID_t * instance ,
               float   kp       ,
               float   ki       ,
               float   kd       )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( instance != NULL )
    {
        instance->coef.kp = kp;
        instance->coef.ki = ki;
        instance->coef.kd = kd;

        status = PID_ResetAll( instance );
    }

    return status;
}


HAL_StatusTypeDef
PID_SetDelayDepth ( PID_t    * instance ,
                    uint32_t   depth    )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( ( instance != NULL ) && ( depth <= instance->delay.max_depth ) )
    {
        instance->delay.depth = depth;

        status = PID_ResetAll( instance );
    }

    return status;
}

HAL_StatusTypeDef
PID_Compute ( PID_t * instance       ,
              int32_t current_output ,
              float   elapsed_time   )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( instance != NULL )
    {
    	int16_t process_value =0;
        float  error = 0.0, result = 0.0;

        ///// Process value (current output)
        process_value = current_output;

        ///// Current error
        error = (float)( instance->setpoint - process_value);

        ///// Proportional value
        instance->control.proportional = error;

        ///// Integral value
        instance->control.integral += error ;//* elapsed_time;

        ///// Derivative value
        if( fpclassify(( error - instance->control.prev_error ) / elapsed_time) != FP_NORMAL )//Evite les NaN, Inf and co
        	instance->control.derivative = 0.0;
        else
        	instance->control.derivative = ( error - instance->control.prev_error ) / elapsed_time;


        instance->control.prev_error = error;

        ///// PID
        result = ( instance->coef.kp * instance->control.proportional ) +
                 ( instance->coef.ki * instance->control.integral     ) +
                 ( instance->coef.kd * instance->control.derivative   );

        status = HAL_OK;

        if((instance) == &board.control.position)
        {
			static int i = 0;
			static int16_t last_process_value=0;


			if (i >= 10 || (process_value !=  last_process_value)) {
				i = 0;
/*				CONSOLE(EOL"M%ld %ld %.0f"EOL, instance->setpoint,
											process_value,
										    result);*/

/*				CONSOLE("P%.0f %.0f %.0f %.0f"EOL, error,
							( instance->coef.kp * instance->control.proportional ),
							( instance->coef.ki * instance->control.integral ),
							( instance->coef.kd * instance->control.derivative ));*/
				last_process_value = process_value;
			}

			i++;

			//CONSOLE("%ld ; %ld ; %f\r",instance->setpoint, process_value, result);
        }
        else if( instance == &board.control.voltage)
        {
        	if(result > 100000.0)
				result = 100000.0;
			else if(result < -100000.0)
				result = -100000.0;
        }

        ///// Apply output
        instance->output = result;
    }

    return status;
}



// End of file
