/*
 * file        : filter.h
 * description : <insert here>
 * copyright   : (c) 2020 GEMA
 */

#ifndef __FILTER_H__
#define __FILTER_H__



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



// ==================================================
//     MACRO
// ==================================================

#define FILTER_GET_VALUE( instance )        ( ( instance )->val )



// ==================================================
//     TYPE DEFINITIONS
// ==================================================

typedef struct {

    int32_t order;
    int32_t val;
    int32_t init_val;

} FILTER_SLIDING_AVERAGE_INT_t;

typedef struct {

    float order;
    float val;
    float init_val;

} FILTER_SLIDING_AVERAGE_FLOAT_t;

typedef struct {

    float alpha;
    float val;
    float prev_val;
    float init_val;

} FILTER_1ST_ORDER_t;



// ==================================================
//     PUBLIC PROTOTYPES
// ==================================================

HAL_StatusTypeDef
FILTER_SlidingAverageInt_Init ( FILTER_SLIDING_AVERAGE_INT_t * instance );

HAL_StatusTypeDef
FILTER_SlidingAverageFloat_Init ( FILTER_SLIDING_AVERAGE_FLOAT_t * instance );

HAL_StatusTypeDef
FILTER_1stOrder_Init ( FILTER_1ST_ORDER_t * instance );

HAL_StatusTypeDef
FILTER_SlidingAverageInt_Compute ( FILTER_SLIDING_AVERAGE_INT_t * instance ,
                                   int32_t                        input    );

HAL_StatusTypeDef
FILTER_SlidingAverageFloat_Compute ( FILTER_SLIDING_AVERAGE_FLOAT_t * instance ,
                                     float                            input    );

HAL_StatusTypeDef
FILTER_1stOrder_Compute ( FILTER_1ST_ORDER_t * instance ,
                          float                input    );



// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __FILTER_H__ */
