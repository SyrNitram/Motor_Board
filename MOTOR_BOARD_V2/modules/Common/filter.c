/*
 * file        : filter.c
 * description : <insert here>
 * copyright   : (c) 2020 GEMA
 */

// ==================================================
//     INCLUDE
// ==================================================

#include "filter.h"



// ==================================================
//     PUBLIC FUNCTIONS
// ==================================================

HAL_StatusTypeDef
FILTER_SlidingAverageInt_Init ( FILTER_SLIDING_AVERAGE_INT_t * instance )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( instance != NULL )
    {
        instance->val = instance->init_val;
        status        = HAL_OK;
    }

    return status;
}

HAL_StatusTypeDef
FILTER_SlidingAverageFloat_Init ( FILTER_SLIDING_AVERAGE_FLOAT_t * instance )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( instance != NULL )
    {
        instance->val = instance->init_val;
        status        = HAL_OK;
    }

    return status;
}

HAL_StatusTypeDef
FILTER_1stOrder_Init ( FILTER_1ST_ORDER_t * instance )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( ( instance != NULL ) && ( instance->alpha >= 0.0 ) && ( instance->alpha <= 1.0 ) )
    {
        instance->val      = instance->init_val;
        instance->prev_val = instance->init_val;
        status             = HAL_OK;
    }

    return status;
}

HAL_StatusTypeDef
FILTER_SlidingAverageInt_Compute ( FILTER_SLIDING_AVERAGE_INT_t * instance ,
                                   int32_t                        input    )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( instance != NULL )
    {
        instance->val += ( input - instance->val ) / instance->order;
        status         = HAL_OK;
    }

    return status;
}

HAL_StatusTypeDef
FILTER_SlidingAverageFloat_Compute ( FILTER_SLIDING_AVERAGE_FLOAT_t * instance ,
                                     float                            input    )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( instance != NULL )
    {
        instance->val += ( input - instance->val ) / instance->order;
        status         = HAL_OK;
    }

    return status;
}

HAL_StatusTypeDef
FILTER_1stOrder_Compute ( FILTER_1ST_ORDER_t * instance ,
                          float                input    )
{
    HAL_StatusTypeDef status = HAL_ERROR;

    if ( instance != NULL )
    {
        instance->val      = ( instance->alpha * input ) + ( ( 1.0 - instance->alpha ) * instance->prev_val );
        instance->prev_val = input;
        status             = HAL_OK;
    }

    return status;
}



// End of file
