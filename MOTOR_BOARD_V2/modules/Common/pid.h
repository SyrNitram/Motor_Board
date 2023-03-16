/*
 * file        : pid.h
 * description : <insert here>
 * copyright   : (c) 2020 GEMA
 */

#ifndef __PID_H__
#define __PID_H__



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
#include "filter.h"
#include "circular_buffer.h"



// ==================================================
//     DEFINITIONS
// ==================================================

#define PID_BYPASS_THRESHOLD        0.0  /* min < max --> Enable the threshold */
#define PID_BYPASS_FILTER           1.0  /* Sliding average filter: order = 1.0 --> Bypass filter */
#define PID_BYPASS_DELAY            0    /* Depth = 0 --> Bypass delay */



// ==================================================
//     MACRO
// ==================================================

#define PID_GET_OUTPUT( instance )      ( ( instance )->output )



// ==================================================
//     TYPE DEFINITIONS
// ==================================================





// ==================================================
//     PUBLIC PROTOTYPES
// ==================================================

HAL_StatusTypeDef
PID_Init ( PID_t * instance );

HAL_StatusTypeDef
PID_ResetAll ( PID_t * instance );

HAL_StatusTypeDef
PID_SetCoefs ( PID_t * instance ,
               float   kp       ,
               float   ki       ,
               float   kd       );

HAL_StatusTypeDef
PID_SetFilterOrder ( PID_t * instance ,
                     float   order    );

HAL_StatusTypeDef
PID_SetDelayDepth ( PID_t    * instance ,
                    uint32_t   depth    );

HAL_StatusTypeDef
PID_Compute ( PID_t * instance       ,
              int32_t   current_output ,
              float   elapsed_time   );



// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __PID_H__ */
