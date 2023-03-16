/*
 * The MIT License
 *
 * Copyright (c)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @addtogroup middleware Middleware
 * @{
 */
/**
 * @addtogroup middleware_scheduler Task scheduler
 * @{
 */
/**
 * @file
 *
 * @brief Task scheduler
 */
/** @} */
/** @} */



// ==================================================
//     INCLUDE
// ==================================================

#include "scheduler.h"
#include "linked_list.h"

#include "main.h"

// ==================================================
//     GLOBAL VARIABLES
// ==================================================

static uint32_t   loop_period_us = 0;
static LIST_t   * task_list      = NULL;



// ==================================================
//     PUBLIC FUNCTIONS
// ==================================================

SCHEDULER_RESULT_t
SCHEDULER_Init ( uint32_t period_us )
{
    task_list = LINKED_LIST_CreateEmptyList();

    if ( task_list == NULL )
    {
        return SCHEDULER_RESULT_ERROR_MEMORY_ALLOCATION;
    }

    loop_period_us = period_us;

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Deinit ( void )
{
    if ( task_list == NULL )
    {
        return SCHEDULER_RESULT_ERROR_SCHEDULER_NOT_INIT;
    }

    LINKED_LIST_DestroyList( task_list );

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_MonitoringLoop ( void )
{
    if ( task_list == NULL )
    {
        return SCHEDULER_RESULT_ERROR_SCHEDULER_NOT_INIT;
    }

    NODE_t * task = task_list->head;

    while ( task != NULL )
    {
        if ( task->data != NULL )
        {
            SCHEDULER_TASK_t * data = ( SCHEDULER_TASK_t * )task->data;

            if ( data->status == SCHEDULER_TASK_STATUS_STARTED )
            {
                data->count += loop_period_us;

                if ( data->count >= data->timeout_us )
                {
                	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 1);
                    data->callback( data->arg );
                    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0);

                    if ( data->type == SCHEDULER_TASK_TYPE_PERIODIC )
                    {
                        data->count = 0;
                    }
                    else
                    {
                        data->status = SCHEDULER_TASK_STATUS_STOPPED;
                    }
                }
            }
        }

        task = task->next;
    }

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Register ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }

    if ( LINKED_LIST_AddNode( task_list , ( void * )task ) == LINKED_LIST_RESULT_SUCCESS )
    {
        task->status = SCHEDULER_TASK_STATUS_STOPPED;
        //task->count  = 0;

        return SCHEDULER_RESULT_SUCCESS;
    }

    return SCHEDULER_RESULT_ERROR_TASK_ALREADY_REGISTERED;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Unregister ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }

    if ( LINKED_LIST_DeleteNode( task_list , ( void * )task ) == LINKED_LIST_RESULT_SUCCESS )
    {
        return SCHEDULER_RESULT_SUCCESS;
    }

    return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Start ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( LINKED_LIST_FindNode( task_list , ( void * )task ) == NULL )
    {
        return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
    }
    else
    if ( task->status == SCHEDULER_TASK_STATUS_STARTED )
    {
        return SCHEDULER_RESULT_ERROR_TASK_ALREADY_STARTED;
    }

    task->status = SCHEDULER_TASK_STATUS_STARTED;

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Stop ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( LINKED_LIST_FindNode( task_list , ( void * )task ) == NULL )
    {
        return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
    }
    else
    if ( task->status == SCHEDULER_TASK_STATUS_STOPPED )
    {
        return SCHEDULER_RESULT_ERROR_TASK_ALREADY_STOPPED;
    }

    task->status = SCHEDULER_TASK_STATUS_STOPPED;

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Reinit ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( LINKED_LIST_FindNode( task_list , ( void * )task ) == NULL )
    {
        return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
    }

    task->status = SCHEDULER_TASK_STATUS_STOPPED;
    task->count  = 0;

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Restart ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( LINKED_LIST_FindNode( task_list , ( void * )task ) == NULL )
    {
        return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
    }

    task->status = SCHEDULER_TASK_STATUS_STARTED;
    task->count  = 0;

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_TASK_STATUS_t
SCHEDULER_Task_GetStatus ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        return SCHEDULER_TASK_STATUS_UNKNOWN;
    }

    return task->status;
}



// End of file
