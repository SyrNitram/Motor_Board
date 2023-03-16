/*
 * file        : config.h
 * description : <insert here>
 * copyright   : (c) 2020 GEMA
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__



// ==================================================
//     CPP GUARD [INIT]
// ==================================================

#ifdef __cplusplus
extern "C" {
#endif



// ==================================================
//     DEFINITIONS
// ==================================================

///// UART - Baudrate
#define CONFIG__UART__BAUDRATE__CONSOLE                             230400

///// Motor current - Calibration - Timeout
#define CONFIG__MOTOR_CURRENT__CALIBRATION__TIMEOUT                 1000  /* Unit: ms */

///// Over-current protection (default)
#define OCP__IS_ACTIVE                                              true
#define OCP__THRESHOLD                                              6000//2000  /* Unit: mA */
#define OCP__PID__COEF__KP                                          0.009999999776482582f
#define OCP__PID__COEF__KI                                          500.0f
#define OCP__PID__COEF__KD                                          9.999999747378752e-05f

///// Motor control - PID - Delay - Max depth
#define CONFIG__MOTOR_CTRL__PID__DELAY__MAX_DEPTH                   15

///// Motor control - Stander - PID
#define STANDER__PID__COEF__KP                   					2.3f
#define STANDER__PID__COEF__KI                   					0.05000f
#define STANDER__PID__COEF__KD                   					0.0f

///// Motor control - Wheel - PID
#define WHEEL__PID__COEF__KP                   		  				2.0f
#define WHEEL__PID__COEF__KI                    					0.0f
#define WHEEL__PID__COEF__KD                     					0.0f

///// Motor control - Sequence order
#define CONFIG__MOTOR_CTRL__SEQUENCE_ORDER__STANDER		            0
#define CONFIG__MOTOR_CTRL__SEQUENCE_ORDER__WHEEL		            1


// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __CONFIG_H__ */
