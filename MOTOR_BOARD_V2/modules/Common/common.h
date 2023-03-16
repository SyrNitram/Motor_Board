/*
 * file        : common.h
 * description : <insert here>
 * copyright   : (c) 2020 GEMA
 */

#ifndef __COMMON_H__
#define __COMMON_H__



// ==================================================
//     CPP GUARD [INIT]
// ==================================================

#ifdef __cplusplus
extern "C" {
#endif

// ==================================================
//     INCLUDE
// ==================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

#include "config.h"
#include "console.h"

#include "main.h"



// ==================================================
//     VERSION
// ==================================================

#define MAJOR           0
#define MINOR           2
#define REVISION        6

// ==================================================
//     MACRO
// ==================================================

#define ARRAY_SIZE( x )         ( sizeof( x ) / sizeof( x[ 0 ] ) )
#define NOP()                   asm volatile( "nop\n\t" )
/*#define SWAP_16BITS( x )        ( ( ( x << 8 ) | ( ( x >> 8 ) & 0xFF ) ) & 0xffff )
#define SWAP_32BITS( x )        ( ( ( ( x >> 24 ) & 0xff       ) | \
                                  ( ( x <<  8 ) & 0xff0000   ) | \
                                  ( ( x >>  8 ) & 0xff00     ) | \
                                  ( ( x << 24 ) & 0xff000000 ) ) & 0xffffffff )
*/

// ==================================================
//     TYPE DEFINITIONS
// ==================================================

typedef enum {

    DEV_STATUS_UNKNOWN = 0 ,
    DEV_STATUS_DISABLED    ,
    DEV_STATUS_ENABLED     ,
    DEV_STATUS_MAX         ,

} DEV_STATUS_t;

typedef enum {
	TYPE_STANDER_RIGHT =1,
	TYPE_STANDER_LEFT,
	TYPE_WHEEL_RIGHT,
	TYPE_WHEEL_LEFT
}Board_number_t;

typedef enum {
	Not_started = 0,
	In_progress,
	Done,
	Failed = 3
}HOMING_status_t, Calibration_status_t;

typedef enum
{
	No_error = 0,
	High_temp = 1,
	Loop_error = 2,
	Motor_block = 4,
	Init_fail = 8,
	Reset = 192,

	Max_Item
}Error_t;

typedef enum {

    CTRL_FREE_WHEEL = 0 ,
	CTRL_OPEN_LOOP		,
	CTRL_TORQUE  		,
	CTRL_SPEED 			,
	CTRL_POSITION  		,
	CTRL_BRAKE			,
	CTRL_ALIGN
}MOTOR_CTRL_TYPE_t;

typedef struct{
	bool A;
	bool B;
	uint8_t state;
	int32_t cnt;	//compte le nombre de pas depuis le dernier RAZ
	float angle; //en degree magnétique (1 trio de capteur HALL = 360°)
}MTR_ENCODER_t;	// for struct MOTOR_t

typedef struct{
	bool A;
	bool B;
	bool C;
	uint8_t state;
}MTR_HALL_t;	// for struct MOTOR_t

typedef struct{
	float A;
	float B;
	float C;
}MTR_CURRENT_t;	// for struct MOTOR_t

typedef struct{
	float	rps;	// vitesse en tr/s de la roue ou du verticalisateur
	float	rpm;	// tour/minute
	bool	dir_IsCW;	// 1:CW		0:CCW (direction du moteur et non de la commande)
}MTR_SPEED_t;	// for struct MOTOR_t

typedef enum {
	A = 0,
	B,
	C
}_SENSORED_PHASE;

typedef struct{
	MTR_ENCODER_t 	encoder;
	MTR_HALL_t 		hall;
	MTR_CURRENT_t 	current;
	_SENSORED_PHASE	phase_sensored;
	MTR_SPEED_t		speed;
	struct
	{
		int32_t			position;//nombre de pas moteur
		uint32_t		pos_abs;//nombre de pas total depuis dernier reset
	}pos;
	int16_t			torque;
	float			actualDC;
}BRD_MOTOR_t;	// for struct MOTOR_BOARD_t

typedef struct{
	float 	INHA; 	//rapport cyclique en %
	bool 	INLA;	//HALL_A
	bool 	INHB;	//HALL_B
	bool 	INLB;	//HALL_C
	bool 	INHC;	//DIR	0:CW 	1:CCW	(direction de la commande et non du moteur)
	bool 	INLC;	//nBRAKE 0:BRAKE	1:NO BRAKE
}DRV_CTRL;		// for struct DRIVER_t

typedef struct{
	float SIA; 		//en mV
	float SIB;		//en mV
	float SIC;		//en mV
	bool nFAULT;	//0:ERREUR	1:OK
}DRV_FEEDBACK;	// for struct DRIVER_t

typedef struct{
	bool enable;
	DRV_CTRL ctrl;
	DRV_FEEDBACK fb;
}BRD_DRIVER_t;	// for struct MOTOR_BOARD_t

typedef enum {

    PHASE_A_CURRENT = 0	,
	PHASE_B_CURRENT   	,
	PHASE_C_CURRENT   	,

    HANDLER_FX        	,
    HANDLER_FZ        	,

    ELECTRODE   		,

    TEMPERATURE 		,
    VREF_INT          	,

    ADC_MAX_CHANNELS    ,

} SENSOR_ADC_CHANNEL_t;

typedef struct {
	struct{
		uint8_t idx;
		float tab_ADC[10];
		float moy;
	}adc;

	struct
	{
		float pente;
		float offset;
		float f;
	}force;

}_FORCE_t;

typedef struct
{
	uint32_t	rx_dma[ADC_MAX_CHANNELS];
	_FORCE_t 	fx, fz;

	float ia;
	float ib;
	float ic;

	bool eos;

}sensors_t;

typedef struct
{
	bool type_IsStander;	// 0:ROUE	1:VERTICALISATEUR
	bool side_IsRight;		// 0:GAUCHE	1:DROITE
}BRD_SELECT_t;	// for struct MOTOR_BOARD_t

typedef struct {
	union {
		struct{
			uint8_t		surchauffe:1;
			uint8_t 	loop_error:1;
			uint8_t 	motor_blocked:1;
			uint8_t		init_fail:1;
			uint8_t 	NA:2;
			uint8_t		reboot:2;
		}single;

		Error_t all;
	}error_flag;

	union {
		uint8_t all;
		struct{
			uint8_t		motor_on:1;
			uint8_t		auto_calib_status:2;
			uint8_t		calib_mode:1;
			uint8_t		NA:2;
			uint8_t		homing_status:2;
		}single;
	}status_flag;
}FLAGS_t;

typedef struct {

    int32_t                          setpoint;
    float                            output;

    struct {

    	float                        kp;
    	float                        ki;
    	float                        kd;

    } coef;

    struct {

        float                        proportional;
        float                        integral;
        float                        derivative;
        float                        prev_error;

    } control;

    struct {

        bool                         enabled;
        CIRCULAR_BUFFER_t          * buffer;
        uint32_t                     depth;
        uint32_t                     max_depth;

    } delay;

    struct {

        bool                         enabled;
        float                        min;
        float                        max;

    } threshold;


} PID_t;

typedef struct
{
	bool				IsOn;
	PID_t				position;
	PID_t				speed;
	PID_t				torque;
	PID_t				voltage;
	MOTOR_CTRL_TYPE_t	ctrl_type;
	int32_t				target_duty_cycle;
	int32_t				duty_cycle;

}CONTROL_t;

typedef struct
{
	float				temperature;

	FLAGS_t				flags;
	CONTROL_t			control;
	BRD_SELECT_t 		select;
	BRD_MOTOR_t 		motor;
	BRD_DRIVER_t 		driver;
	sensors_t	 		sensor;
	Board_number_t		number;
}Global_Struct_t;

// ==================================================
//     EXTERN VARIABLES
// ==================================================

extern Global_Struct_t board;

extern bool flag_stop_homing;


// ==================================================
//     PUBLIC PROTOTYPES
// ==================================================

// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __COMMON_H__ */
