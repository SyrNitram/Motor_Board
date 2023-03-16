/*
 * can_lib_TX.c
 *
 *  Created on: Oct 15, 2021
 *      Author: Sebastien / Mael
 */


// ==================================================
//     INCLUDE
// ==================================================

#include "com_can.h"

// ==================================================
//     GLOBAL VARIABLES
// ==================================================
CAN_TxHeaderTypeDef pTxHeader; //declare a specific header for message transmittions
uint8_t tx[TOTAL_CAN_MSG_SIZE]; //declare byte to be transmitted //declare a receive byte
CAN_MSG_t msg_can;


typedef enum
{
	TPDO1 = 0x180,
	TPDO2 = 0x280,
	RESPONSE = 0x600,

}MSG_TYPE_t;

// ==================================================
//     LOCAL Functions
// ==================================================

/*
 * Function that set CAN msg header
 * in: uint16_t can ID
 *
 */
HAL_StatusTypeDef
Set_TxHeader( CAN_TxHeaderTypeDef * inst, MSG_TYPE_t id )
{
	HAL_StatusTypeDef status = HAL_ERROR;

	if( (id == TPDO1 || id == TPDO2 || id == RESPONSE) && (inst != NULL) )
	{
		inst->DLC=DLC_DATA_LENGTH; // Précise le nombre d'octets de données que la trame transporte ( De 0 à 8 ). ici 8 byte
		inst->IDE=CAN_ID_STD; //set identifier to standard (11bits ID)
		inst->RTR=CAN_RTR_DATA; //set data type to remote transmission request?
		inst->StdId = id + board.number;// Détermine l'adresse du périphérique au quel la trame est destiné.

		status = HAL_OK;
	}


	return status;
}

/*
 * function that copy data into the payload
 */
HAL_StatusTypeDef
Set_Payload_TPDO1 ( Global_Struct_t * inst, uint8_t * tab )
{
	HAL_StatusTypeDef status = HAL_ERROR;

	if( tab != NULL && inst != NULL)
	{
		//====================Swap des données====================
		//swap de l'index
		int16_t	speed_torque = ( int16_t)inst->motor.speed.rpm;
		memcpy(tab,  &speed_torque , sizeof(speed_torque));
		tab+=sizeof(speed_torque);

		//sub-index
		speed_torque = inst->motor.torque;
		memcpy(tab,  &speed_torque , sizeof(speed_torque));
		tab+=sizeof(speed_torque);

		//swap de la data
		int32_t pos = inst->motor.pos.position;
		memcpy(tab,  &pos , sizeof(pos));

		status = HAL_OK;
	}

	return status;
}

/*
 * function that copy data into the payload
 */
HAL_StatusTypeDef
Set_Payload_TPDO2 ( Global_Struct_t * inst, uint8_t * tab )
{
	HAL_StatusTypeDef status = HAL_ERROR;

	if( tab != NULL && inst != NULL)
	{
		int16_t dataX;
		int16_t dataZ;

		if(inst->flags.status_flag.single.calib_mode) {
			dataX = (int16_t)inst->sensor.fx.adc.moy * 100.0;
			dataZ = (int16_t)inst->sensor.fz.adc.moy * 100.0;
		}else{
			dataX = (int16_t)inst->sensor.fx.force.f * 100.0;
			dataZ = (int16_t)inst->sensor.fz.force.f * 100.0;
		}

		//====================Swap des données====================
		//swap de l'index
		memcpy(tab , &dataX , sizeof(dataX));
		tab += sizeof(dataX);

		//sub-index
		memcpy(tab , &dataZ , sizeof(dataZ));
		tab += sizeof(dataZ);

		//swap de la data
		uint8_t d8 = inst->control.ctrl_type;
		memcpy(tab,  &d8 , sizeof(d8));

		tab++;

		d8 = inst->temperature;
		memcpy(tab,  &d8 , sizeof(d8));

		tab++;

		d8 = inst->flags.error_flag.all;
		memcpy(tab,  &d8 , sizeof(d8));

		tab++;

		d8 = inst->flags.status_flag.all;
		memcpy(tab, &d8 , sizeof(d8));

		status = HAL_OK;
	}

	return status;
}

// ==================================================
//     GLOBAL Functions
// ==================================================

int16_t
SWAP_16BITS( int16_t x)
{
	return  ( ( ( x << 8 ) | ( ( x >> 8 ) & 0xFF ) ) & 0xffff );
}

int32_t
SWAP_32BITS( int32_t x)
{
	return ( ( ( ( x >> 24 ) & 0xff       ) | \
            ( ( x <<  8 ) & 0xff0000   ) | \
            ( ( x >>  8 ) & 0xff00     ) | \
            ( ( x << 24 ) & 0xff000000 ) ) & 0xffffffff );
}


HAL_StatusTypeDef
CAN_TPDO1_MSG( Global_Struct_t * inst )
{
	HAL_StatusTypeDef status = HAL_ERROR;

	if(inst != NULL)
	{
		//Set TPDO Header
		status = Set_TxHeader( &pTxHeader, TPDO1 );

		//Set TPDO Payload
		status |= Set_Payload_TPDO1( inst, &msg_can.TX.TPDO[0][0] );

		uint32_t TxMailbox=0;
		do{

			if (((hcan.Instance->TSR & CAN_TSR_TME0) == 0U) &&
				((hcan.Instance->TSR & CAN_TSR_TME1) == 0U) &&
				((hcan.Instance->TSR & CAN_TSR_TME2) == 0U))
			{
				(void)HAL_Delay(1);
			}
			else
			{
				// Check Tx Mailbox 0 status
				if ((hcan.Instance->TSR & CAN_TSR_TME0) != 0U)
				{
					TxMailbox = CAN_TX_MAILBOX0;
				}
				// Check Tx Mailbox 1 status
				else if ((hcan.Instance->TSR & CAN_TSR_TME1) != 0U)
				{
					TxMailbox = CAN_TX_MAILBOX1;
				}
				// Check Tx Mailbox 2 status
				else if ((hcan.Instance->TSR & CAN_TSR_TME2) != 0U)
				{
					TxMailbox = CAN_TX_MAILBOX2;
				}
			}

		}while(TxMailbox == 0);

		//TODO: Add CAN Tx
		status |= HAL_CAN_AddTxMessage(&hcan, &pTxHeader, &msg_can.TX.TPDO[0][0], &TxMailbox);
	}

	return status ;
}


HAL_StatusTypeDef
CAN_TPDO2_MSG( Global_Struct_t * inst )
{
	HAL_StatusTypeDef status = HAL_ERROR;

	if(inst!= NULL)
	{
		//Set TPDO Header
		status = Set_TxHeader( &pTxHeader, TPDO2 );

		//Set TPDO Payload
		status |= Set_Payload_TPDO2( inst, &msg_can.TX.TPDO[1][0] );

		uint32_t TxMailbox=0;
		do{

			if (((hcan.Instance->TSR & CAN_TSR_TME0) == 0U) && ((hcan.Instance->TSR & CAN_TSR_TME1) == 0U) && ((hcan.Instance->TSR & CAN_TSR_TME2) == 0U))
			{
				HAL_Delay(1);
			}
			else
			{
				// Check Tx Mailbox 0 status
				if ((hcan.Instance->TSR & CAN_TSR_TME0) != 0U)
				{
					TxMailbox = CAN_TX_MAILBOX0;
				}
				// Check Tx Mailbox 1 status
				else if ((hcan.Instance->TSR & CAN_TSR_TME1) != 0U)
				{
					TxMailbox = CAN_TX_MAILBOX1;
				}
				// Check Tx Mailbox 2 status
				else if ((hcan.Instance->TSR & CAN_TSR_TME2) != 0U)
				{
					TxMailbox = CAN_TX_MAILBOX2;
				}
			}

		}while(TxMailbox == 0);

		//TODO: Add CAN Tx
		status |= HAL_CAN_AddTxMessage(&hcan, &pTxHeader, &msg_can.TX.TPDO[1][0], &TxMailbox);
	}

	return status ;
}


/*
 * Response to the version request received from the main board
 */
HAL_StatusTypeDef
CAN_Rep_Version( void )
{
	HAL_StatusTypeDef status = HAL_OK;

	//Set TPDO Header
	status |= Set_TxHeader( &pTxHeader, RESPONSE );

	//====================Swap des données====================
	//TODO: Add index and data
	//swap de l'index
	uint16_t d = SWAP_16BITS( IDX_REP_GET_VERSION );
	memcpy(&msg_can.TX.response.idx , &d, sizeof(uint16_t));

	//sub-index
	d = SWAP_16BITS( MAJOR );
	memcpy(&msg_can.TX.response.sub_idx , &d, sizeof(uint16_t));

	//swap de la data
	uint32_t d32 = (MINOR << 16) | REVISION;
	d32 = SWAP_32BITS( d32 );
	memcpy(&msg_can.TX.response.data[0], &d32, sizeof(d32));


	//put everything together
	//put index first
	memcpy(&tx[0], &msg_can.TX.response.idx, sizeof( msg_can.TX.response.idx));
	//then put
	memcpy(&tx[2], &msg_can.TX.response.sub_idx, sizeof( msg_can.TX.response.sub_idx ));
	memcpy(&tx[4], &msg_can.TX.response.data[0], sizeof( msg_can.TX.response.data[0] ));


	uint32_t TxMailbox=0;
	do{

		if (((hcan.Instance->TSR & CAN_TSR_TME0) == 0U) && ((hcan.Instance->TSR & CAN_TSR_TME1) == 0U) && ((hcan.Instance->TSR & CAN_TSR_TME2) == 0U))
		{
			HAL_Delay(1);
		}
		else
		{
			// Check Tx Mailbox 0 status
			if ((hcan.Instance->TSR & CAN_TSR_TME0) != 0U)
			{
				TxMailbox = CAN_TX_MAILBOX0;
			}
			// Check Tx Mailbox 1 status
			else if ((hcan.Instance->TSR & CAN_TSR_TME1) != 0U)
			{
				TxMailbox = CAN_TX_MAILBOX1;
			}
			// Check Tx Mailbox 2 status
			else if ((hcan.Instance->TSR & CAN_TSR_TME2) != 0U)
			{
				TxMailbox = CAN_TX_MAILBOX2;
			}
		}

	}while(TxMailbox == 0);

	//TODO: Add CAN Tx
	status |= HAL_CAN_AddTxMessage(&hcan, &pTxHeader, &tx[0], &TxMailbox);


	return status ;
}
