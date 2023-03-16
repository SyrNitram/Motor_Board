/*
 * drv8353.c
 *
 *  Created on: Apr 28, 2022
 *      Author: Martin RYS
 */
/*
#include <old_drv8353.h>
#include "main.h"

//----------------------------------------
//				DEFINE
//----------------------------------------

#define DRV_GONFIG_MASK_FREE_WHEEL_OFF			0b1111111111111011 	//Apply with & to REG2
#define DRV_GONFIG_MASK_FREE_WHEEL_ON			0b0000000000000100	//Apply with | to REG2

#define DRV_GONFIG_PWM_MODE_SIX_STEP			0b0001000001000000 	//For REG 2
#define DRV_GONFIG_PWM_MODE_SVM					0b0001000000100000	//For REG 2

#define DRV_GONFIG_SPI_LOCK_OFF 				0b0001101100000000	//For REG 3
#define DRV_GONFIG_SPI_LOCK_ON 					0b0001111000000000	//For REG 3


//----------------------------------------
//				PUBLIC VARIABLES
//----------------------------------------
typedef struct
{
	union{
		struct
		{
			uint16_t	vds_lc:1;
			uint16_t	vds_hc:1;
			uint16_t	vds_lb:1;
			uint16_t	vds_hb:1;
			uint16_t	vds_la:1;
			uint16_t	vds_ha:1;
			uint16_t	otsd:1;
			uint16_t	uvlo:1;
			uint16_t	gdf:1;
			uint16_t	vds_ocp:1;
			uint16_t	fault:1;
		}b;

		uint16_t reg;
	}R1;


	union{
		struct
		{
			uint16_t	vgs_lc:1;
			uint16_t	vgs_hc:1;
			uint16_t	vgs_lb:1;
			uint16_t	vgs_hb:1;
			uint16_t	vgs_la:1;
			uint16_t	vgs_ha:1;
			uint16_t	gduv:1;
			uint16_t	otw:1;
			uint16_t	sc_oc:1;
			uint16_t	sb_oc:1;
			uint16_t	sa_oc:1;
		}b;

		uint16_t reg;
	}R2;
}_DRV8353_FAULT ;
_DRV8353_FAULT drv8353_fault;
//----------------------------------------
//			PRIVATE DEFINITIONS
//----------------------------------------
uint16_t DRV_REGISTER_CONFIG[]={
//		      |A9876543210
		0b1000000000000000		, 	//REG 0h	READ ONLY DO NOT CHANGE
		0b1001000000000000		,	//REG 1h	READ ONLY DO NOT CHANGE
		0b0001000001000001		,	//REG 2h	DEFINED LATER WITH PWM_MODE (default : 6 step)
		0b0001100000010001		,	//REG 3h	NO LOCK 	IDRIVEP_HS 	50mA 	IDRIVEN_HS 	100mA
		0b0010010000010001		,	//REG 4h	CBC on 		TDRIVE 		500ns 	IDRIVEP_LS	50mA	IDRIVEN_LS	100mA
		0b0010101101111110		,	//REG 5h
		0b0011000001000011		,	//REG 6h	CSA_GAIN 10V/V
		0b0011100000000000 			//REG 7h 	RESEVED DO NOT CHANGE
};


//----------------------------------------
//			PUBLIC FUNCTIONS
//----------------------------------------
void
DRV8353_Enable( void )
{
	HAL_GPIO_WritePin(DRV_ENABLE_GPIO_Port, DRV_ENABLE_Pin, DRV_ENABLE_ON);
	board.control.IsOn = true;
	board.driver.enable = true;

}


void
DRV8353_Disable( void )
{
	HAL_GPIO_WritePin(DRV_ENABLE_GPIO_Port, DRV_ENABLE_Pin, DRV_ENABLE_OFF);
	board.control.IsOn = false;
	board.driver.enable = false;
}


/*
 * Disable driver (ENABLE pin = low)
 * Set all input to low
 * Enable driver (ENABLE pin = high)
 * Check for fault
 * Apply SPI configuration
 * Check for fault
 * Set hall to Stop mode (free wheel)
 * return HAL_OK
 */
/*
HAL_StatusTypeDef
DRV8353_InitPlatform() {

	HAL_StatusTypeDef status = HAL_ERROR;

	//DISABLE DRV
	DRV8353_Disable();

	//SET all INHx and INLx to low before switching mode
	TIM1->CCR1 = 0;
	status = HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(DRV_INLA_GPIO_Port, DRV_INLA_Pin, HALL_LOW); 	//HALL_A
	HAL_GPIO_WritePin(DRV_INHB_GPIO_Port, DRV_INHB_Pin, HALL_LOW); 	//HALL_B
	HAL_GPIO_WritePin(DRV_INLB_GPIO_Port, DRV_INLB_Pin, HALL_LOW); 	//HALL_C
	HAL_GPIO_WritePin(DRV_INHC_GPIO_Port, DRV_INHC_Pin, HALL_LOW);	//DIR
	HAL_GPIO_WritePin(DRV_INLC_GPIO_Port, DRV_INLC_Pin, HALL_LOW);	//BRAKE

	HAL_Delay(2); //Wait at least T_SLEEP = 1ms for driver to go in sleep mode

	//ENABLE DRV
	DRV8353_Enable();

	HAL_Delay(2); //Wait at least T_WAKE = 1ms for driver to go in operating mode

	//CHECK nFAULT PIN
	if (HAL_GPIO_ReadPin(DRV_NFAULT_GPIO_Port, DRV_NFAULT_Pin)	== GPIO_PIN_RESET)
		return HAL_ERROR;

	//SELECT PWM MODE
	//DRV_REGISTER_CONFIG[2] = DRV_GONFIG_PWM_MODE_SIX_STEP;

	//APPLY SPI CONFIG
	for (int i = 2; i < 7; i++) {
		HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
		HAL_Delay(1);
		status |= HAL_SPI_Transmit(&hspi1, (uint8_t*) &DRV_REGISTER_CONFIG[i],
				1, 300);
		HAL_Delay(1);
		HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);
		HAL_Delay(1);
	}

	//CHECK SPI CONFIG
	for (int16_t i = 2; i < 7; i++) {
		uint16_t DRV_REGISTER_RECEIVE;
		uint16_t DRV_REGISTER_READ = 0b1000000000000000 | (i << 11);

		HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
		HAL_Delay(1);
		status |= HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) &DRV_REGISTER_READ,
				(uint8_t*) &DRV_REGISTER_RECEIVE, 1, 300);
		HAL_Delay(1);
		HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);
		HAL_Delay(1);

		//Don't check bit0 on REG 2h
		if ((DRV_REGISTER_RECEIVE & 0b0000011111111111) != (i == 2) ?(DRV_REGISTER_CONFIG[i] & 0b0000011111111110) : (DRV_REGISTER_CONFIG[i] & 0b0000011111111111)) //5 MSB are dont care bits
			return HAL_ERROR;
	}

	//CHECK nFAULT PIN
	if (HAL_GPIO_ReadPin(DRV_NFAULT_GPIO_Port, DRV_NFAULT_Pin)	== GPIO_PIN_RESET)
		return HAL_ERROR;

	//SET PWM 0
	//status |= DRV8353_Motor_Set_DutyCycle( &bldc_motor , 0.0 );
	TIM1->CCR1 = 0;
	status = HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	//SET DIRECTION CW
	HAL_GPIO_WritePin(DRV_INHC_GPIO_Port, DRV_INHC_Pin, DRV_ROTATION_CW);
	//SET BRAKE OFF
	HAL_GPIO_WritePin(DRV_INLC_GPIO_Port, DRV_INLC_Pin, DRV_BRAKE_OFF);
	//SET MOTOR STOP (all HALL to 0)
	HAL_GPIO_WritePin(DRV_INLA_GPIO_Port, DRV_INLA_Pin, HALL_LOW); //HALL_A
	HAL_GPIO_WritePin(DRV_INHB_GPIO_Port, DRV_INHB_Pin, HALL_LOW); //HALL_B
	HAL_GPIO_WritePin(DRV_INLB_GPIO_Port, DRV_INLB_Pin, HALL_LOW); //HALL_C

	Set_ControlType(CTRL_FREE_WHEEL);

	return HAL_OK;
}


void
DRV8353_Check_Faults( void )
{
	//DISABLE DRV
	DRV8353_Disable();


	for(int16_t i = 0; i<2 ; i++){
		uint16_t DRV_REGISTER_RECEIVE;
		uint16_t DRV_REGISTER_READ = 0b1000000000000000 | (i<<11);
		HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
		CONSOLE("\r");
	//	HAL_Delay(1);
		HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&DRV_REGISTER_READ, (uint8_t *)&DRV_REGISTER_RECEIVE, 1, 300);
		CONSOLE("\r");
	//	HAL_Delay(1);
		HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);
		CONSOLE("\r");
	//	HAL_Delay(1);

		if(i==0)
			drv8353_fault.R1.reg=DRV_REGISTER_RECEIVE;
		else
			drv8353_fault.R2.reg=DRV_REGISTER_RECEIVE;
	}

/*
    //Get Fault Register 1
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
	CONSOLE("\r");
//	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi1, &reg[0], (uint8_t *)&drv8353_fault.R1.reg, 1, 300);
	CONSOLE("\r");
//	HAL_Delay(1);
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);
	CONSOLE("\r");
//	HAL_Delay(1);


    //Get Fault Register 2
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
	CONSOLE("\r");
//	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi1, &reg[1], (uint8_t *)&drv8353_fault.R2.reg, 1, 300);
	CONSOLE("\r");
//	HAL_Delay(1);
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);
	CONSOLE("\r");
//	HAL_Delay(1);
*/
/*
	if(drv8353_fault.R1.reg != 0)//Le bit 10 sera toujours à 1 s'il y une faute donc on fait un masque pour ne pas le prendre en compte
	{
		CONSOLE("R1 : %d\r",drv8353_fault.R1.reg);
		if(drv8353_fault.R1.b.vds_ocp)
			CONSOLE("VDS_OCP\r");
		if(drv8353_fault.R1.b.gdf)
			CONSOLE("GDF\r");
		if(drv8353_fault.R1.b.uvlo)
			CONSOLE("UVLO\r");
		if(drv8353_fault.R1.b.otsd)
			CONSOLE("OTSD\r");
		if(drv8353_fault.R1.b.vds_ha)
			CONSOLE("VDS_HA\r");
		if(drv8353_fault.R1.b.vds_la)
			CONSOLE("VDS_LA\r");
		if(drv8353_fault.R1.b.vds_hb)
			CONSOLE("VDS_HB\r");
		if(drv8353_fault.R1.b.vds_lb)
			CONSOLE("VDS_LB\r");
		if(drv8353_fault.R1.b.vds_hc)
			CONSOLE("VDS_HC\r");
		if(drv8353_fault.R1.b.vds_lc)
			CONSOLE("VDS_LC\r");
	}
	if(drv8353_fault.R2.reg != 0)
	{
		CONSOLE("R2 : %d\r",drv8353_fault.R2.reg);
		if(drv8353_fault.R2.b.sa_oc)
			CONSOLE("SA_OC\r");
		if(drv8353_fault.R2.b.sb_oc)
			CONSOLE("SB_OC\r");
		if(drv8353_fault.R2.b.sc_oc)
			CONSOLE("SC_OC\r");
		if(drv8353_fault.R2.b.otw)
			CONSOLE("OTW\r");
		if(drv8353_fault.R2.b.gduv)
			CONSOLE("GDUV\r");
		if(drv8353_fault.R2.b.vgs_ha)
			CONSOLE("VGS_HA\r");
		if(drv8353_fault.R2.b.vgs_la)
			CONSOLE("VGS_LA\r");
		if(drv8353_fault.R2.b.vgs_hb)
			CONSOLE("VGS_HB\r");
		if(drv8353_fault.R2.b.vgs_lb)
			CONSOLE("VGS_LB\r");
		if(drv8353_fault.R2.b.vgs_hc)
			CONSOLE("VGS_HC\r");
		if(drv8353_fault.R2.b.vgs_lc)
			CONSOLE("VGS_LC\r");
	}


}
*/
