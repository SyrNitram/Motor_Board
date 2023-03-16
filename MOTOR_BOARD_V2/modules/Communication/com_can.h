/*
 * com_can.h
 *
 *  Created on: Oct 15, 2021
 *      Author: Sebastien / Mael
 */

#ifndef __COM_CAN_H__
#define __COM_CAN_H__


// ==================================================
//     INCLUDE
// ==================================================

#include "device.h"
//#include "main.h"
#include "common.h"

// ==================================================
//     DEFINITIONS
// ==================================================

//Headers
#define HEADER_SDO_REQ			    0x600

#define H_TPDO1_STD_R			    0x180
#define H_TPDO2_STD_R			    0x280
#define H_TPDO1_STD_L			    0x181
#define H_TPDO2_STD_L			    0x281
#define H_TPDO1_WHL_R			    0x182
#define H_TPDO2_WHL_R			    0x282
#define H_TPDO1_WHL_L			    0x183
#define H_TPDO2_WHL_L			    0x283

#define DLC_DATA_LENGTH			    8

//Index
#define IDX_CMD_SET_SETPOINT		0x0020
#define IDX_CMD_SET_CTRL_MODE		0x0021
#define IDX_CMD_SET_MOTOR_ON_OFF	0x0023
#define IDX_CMD_START_HOMING		0x0031
#define IDX_REP_GET_VERSION			0x0110
#define IDX_CMD_SET_KP_POS			0x0041
#define IDX_CMD_SET_KP_SPEED		0x0042
#define IDX_CMD_SET_KP_TORQUE		0x0043
#define IDX_CMD_SET_KI_POS			0x1040
#define IDX_CMD_SET_KI_SPEED		0x2040
#define IDX_CMD_SET_KI_TORQUE		0x3040
#define IDX_CMD_SET_KD_POS			0x0140
#define IDX_CMD_SET_KD_SPEED		0x0240
#define IDX_CMD_SET_KD_TORQUE		0x0340
#define IDX_CMD_AUTO_CALIB			0x0050
#define IDX_CMD_SET_CALIB_ON_OFF	0x0053


//#define IDX_TPDO1                   speed
//#define IDX_TPDO2                   fx

//Sub_index
#define NO_SUB_IDX					0
#define SUB_IDX_MOTOR_ON			0x0011
#define SUB_IDX_MOTOR_OFF			0x0012
#define SUB_IDX_CALIB_ON			0x0011
#define SUB_IDX_CALIB_OFF			0x0012


//Data
//#define DTA_TPDO1                   position

//size in Byte
/*
 * Can data packet size
 */
#define INDEX_SIZE					2 // index : 2 bytes
#define SUBINDEX_SIZE				2 // Sudindex : 2 bytes
#define DATA_SIZE					4 // Payload : 4 bytes
#define TOTAL_CAN_MSG_SIZE			(INDEX_SIZE + SUBINDEX_SIZE + DATA_SIZE) // Payload sur 8 bytes (HEX format)


// ==================================================
//     TYPE DEFINITIONS
// ==================================================

typedef struct {

	struct{
		uint8_t 					TPDO[2][8];

		struct{
			uint16_t 				idx;
			uint16_t 				sub_idx;
			uint8_t					data[4];
		}response;
	}TX;

	struct{
		int8_t						idx;//Sert pour ranger les msg can
		uint8_t						bytes[4][8];
		uint32_t 					last_time;
		bool						received;
	}RX;

	struct {
		union
		{
			struct{
				uint8_t				motor_on_off:1;
				uint8_t				auto_calib_status:2;
				uint8_t				calib_mode:1;
				uint8_t				NA:2;
				uint8_t				homing_status:2;
			}single;

			uint8_t 				all;
		}status;

		union
		{
			struct{
				uint8_t				temp_trop_elevee:1;
				uint8_t				boucle_trop_grande:1;
				uint8_t				moteur_bloque:1;
				uint8_t				init_fail:1;
				uint8_t				NA:2;
				uint8_t				Reset_cause:2;
			}single;

			uint8_t					all;
		}error;

	}flags;


}CAN_MSG_t;

extern CAN_MSG_t msg_can;


// ==================================================
//     PUBLIC PROTOTYPES
// ==================================================


HAL_StatusTypeDef
CAN_Init_periph ( void );

HAL_StatusTypeDef
Can_frame_mgt ( Global_Struct_t * inst );

HAL_StatusTypeDef
CAN_Rep_Version( void );

HAL_StatusTypeDef
CAN_TPDO1_MSG( Global_Struct_t * inst  );

HAL_StatusTypeDef
CAN_TPDO2_MSG( Global_Struct_t * inst  );

int16_t
SWAP_16BITS( int16_t x);

int32_t
SWAP_32BITS( int32_t x);

// ==================================================
//     EXTERN VARIABLES
// ==================================================
extern CAN_MSG_t msg_can;


#endif /* __COM_CAN_H__ */
