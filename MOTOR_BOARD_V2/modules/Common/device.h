/*
 * file        : device.h
 * description : <insert here>
 * copyright   : (c) 2020 GEMA
 */

#ifndef __DEVICE_H__
#define __DEVICE_H__



// ==================================================
//     CPP GUARD [INIT]
// ==================================================

#ifdef __cplusplus
extern "C" {
#endif



// ==================================================
//     INCLUDE
// ==================================================

#include "stm32f0xx_hal.h"






// ==================================================
//     PLATFORM
// ==================================================

// ==================================================
//     FUSES
// ==================================================

///// Console
#define FUSE__CONSOLE               CONFIG__OPTION__ENABLED
#define FUSE__CONSOLE__ERROR        CONFIG__OPTION__ENABLED
#define FUSE__CONSOLE__WARNING      CONFIG__OPTION__ENABLED
#define FUSE__CONSOLE__RUNTIME      CONFIG__OPTION__ENABLED
#define FUSE__CONSOLE__DEBUG        CONFIG__OPTION__ENABLED



// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __DEVICE_H__ */
