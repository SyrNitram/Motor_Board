/*
 * The MIT License
 *
 * Copyright (c) 2019
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

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__



// ==================================================
//     CPP GUARD [INIT]
// ==================================================

#ifdef __cplusplus
extern "C" {
#endif



// ==================================================
//     INCLUDE
// ==================================================

#include <stdint.h>



// ==================================================
//     TYPE DEFINITIONS
// ==================================================

typedef void ( * SCHEDULER_TASK_CB_t )( void * arg );

typedef enum {

    SCHEDULER_RESULT_SUCCESS = 0                   ,
    SCHEDULER_RESULT_ERROR_GENERIC_FAIL            ,
    SCHEDULER_RESULT_ERROR_SCHEDULER_NOT_INIT      ,
    SCHEDULER_RESULT_ERROR_NULL_PARAM              ,
    SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK       ,
    SCHEDULER_RESULT_ERROR_TASK_ALREADY_REGISTERED ,
    SCHEDULER_RESULT_ERROR_MEMORY_ALLOCATION       ,
    SCHEDULER_RESULT_ERROR_TASK_ALREADY_STARTED    ,
    SCHEDULER_RESULT_ERROR_TASK_ALREADY_STOPPED    ,

} SCHEDULER_RESULT_t;

typedef enum {

    SCHEDULER_TASK_TYPE_ONE_SHOT = 0 ,
    SCHEDULER_TASK_TYPE_PERIODIC     ,

} SCHEDULER_TASK_TYPE_t;

typedef enum {

    SCHEDULER_TASK_STATUS_UNKNOWN = 0 ,
    SCHEDULER_TASK_STATUS_STARTED     ,
    SCHEDULER_TASK_STATUS_STOPPED     ,

} SCHEDULER_TASK_STATUS_t;

typedef struct {

    SCHEDULER_TASK_CB_t       callback;
    void                    * arg;
    SCHEDULER_TASK_TYPE_t     type;
    uint32_t                  timeout_us;

    SCHEDULER_TASK_STATUS_t   status;
    uint32_t                  count;

} SCHEDULER_TASK_t;



// ==================================================
//     PUBLIC PROTOTYPES
// ==================================================

/**
 * Initialize the scheduler
 *
 * @param period_us Scheduler period
 * @return status of the operation
 */
SCHEDULER_RESULT_t
SCHEDULER_Init ( uint32_t period_us );

/**
 * Delete the scheduler
 *
 * @return status of the operation
 */
SCHEDULER_RESULT_t
SCHEDULER_Deinit ( void );

/**
 * Verify the loop coherency
 *
 * @return status of the operation
 */
SCHEDULER_RESULT_t
SCHEDULER_MonitoringLoop ( void );

/**
 * Add a new task to the scheduler
 *
 * @param task Scheduler task
 * @return status of the operation
 */
SCHEDULER_RESULT_t
SCHEDULER_Task_Register ( SCHEDULER_TASK_t * task );

/**
 * Remove a task from the scheduler
 *
 * @param task Scheduler task
 * @return status of the operation
 */
SCHEDULER_RESULT_t
SCHEDULER_Task_Unregister ( SCHEDULER_TASK_t * task );

/**
 * Start a task
 *
 * @param task Scheduler task
 * @return status of the operation
 */
SCHEDULER_RESULT_t
SCHEDULER_Task_Start ( SCHEDULER_TASK_t * task );

/**
 * Stop a task
 *
 * @param task Scheduler task
 * @return status of the operation
 */
SCHEDULER_RESULT_t
SCHEDULER_Task_Stop ( SCHEDULER_TASK_t * task );

/**
 * Reset and stop a task
 *
 * @param task Scheduler task
 * @return status of the operation
 */
SCHEDULER_RESULT_t
SCHEDULER_Task_Reinit ( SCHEDULER_TASK_t * task );

/**
 * Reset and start a task
 *
 * @param task Scheduler task
 * @return status of the operation
 */
SCHEDULER_RESULT_t
SCHEDULER_Task_Restart ( SCHEDULER_TASK_t * task );

/**
 * Get the state of a task
 *
 * @param task Scheduler task
 * @return status of the operation
 */
SCHEDULER_TASK_STATUS_t
SCHEDULER_Task_GetStatus ( SCHEDULER_TASK_t * task );



// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __SCHEDULER_H__ */
