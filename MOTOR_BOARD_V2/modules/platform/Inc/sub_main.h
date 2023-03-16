/*
 * file        : sub_main.h
 * description : <insert here>
 * copyright   : (c) 2019 Kickmaker
 * author      : Luis CRUZ
 */

#ifndef __SUB_MAIN_H__
#define __SUB_MAIN_H__



// ==================================================
//     CPP GUARD [INIT]
// ==================================================

#ifdef __cplusplus
extern "C" {
#endif



// ==================================================
//     INCLUDE
// ==================================================

#include "common.h"
#include "scheduler.h"
#include "motor.h"



// ==================================================
//     TYPE DEFINITIONS
// ==================================================

typedef struct {

    DEV_STATUS_t console;
    DEV_STATUS_t solenoid;
    DEV_STATUS_t drv8323rs;
    DEV_STATUS_t scheduler;
    DEV_STATUS_t motor_ctrl;
    DEV_STATUS_t adc_fx;
    DEV_STATUS_t affichage;
    DEV_STATUS_t sensor_mgr;
    DEV_STATUS_t comm;

} DEVS_t;



// ==================================================
//     EXTERN VARIABLES
// ==================================================

extern DEVS_t devs;

extern SCHEDULER_TASK_t task_MotorCtrl;
extern SCHEDULER_TASK_t task_AutoCalibration;
extern SCHEDULER_TASK_t task_Com_mgt;
extern SCHEDULER_TASK_t task_CurrentCalculation;
extern SCHEDULER_TASK_t task_ADC;


// ==================================================
//     PUBLIC PROTOTYPES
// ==================================================

/**
 * Entry vector of the user application code
 */
void
SUB_MAIN ( void );
/**
 * Motor control processing task
 *
 * @param arg Unused
 */
void
TaskCallback_MotorCtrl ( void * arg );

/**
 * Motor control processing task
 *
 * @param arg Unused
 */
void
TaskCallback_Current ( void * arg );

/**
 * Communication management task
 *
 * @param arg Unused
 */
void
TaskCallback_Com_Mgt ( void * arg );

/**
 * ADC Fx/Fz raw processing task
 */
void
TaskCallback_ADC ( void * arg );

/**
 * Offsets auto-calibration processing task
 */
void
TaskCallback_Calib ( void * arg );


// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __SUB_MAIN_H__ */
