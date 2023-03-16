/*
 * drv8353.c
 *
 *  Created on: 19 août 2022
 *      Author: Martin RYS
 *
 *
 *      API pour driver de moteur BLDC DRV8353 TI
 *      Prevue pour une utilisation en mode 1xPWM
 *
 *      Avant utilisation :
 *      - Definir les pins dans private defines
 *      - Entrer la configuration du driver dans DRV_REGISTER_CONFIG
 *      - Completer les fonctions drv_SPI_TransmitReceive et drv_SPI_Transmit
 *      - Si utillisation sur µcontrôleur autre que ST, remplacer les fonctions d'ecriture sur sortie
 *
 *
 *      /!\ Mise en garde /!\
 *      Pour le moteur roue :
 *      - Rapport cyclique de 5% pour amorcer la rotation
 *      - Rapport cyclique de 15% deja rapide
 *      - Faire varier le rapport cyclique de +-10 MAXIMUM pour ne pas faire tomber le banc de test
 *
 *      Pour le moteur verticalisateur monté sur verticalisateur:
 *      - Rapport cyclique de 5% pour amorcer la rotation (à vide)
 *      - Rapport cyclique de 15% deja rapide car pas de limite haute, il ne faut pas entrer en buté
 *
 */



#include <drv8353.h>
#include <stdlib.h>

//----------------------------------------
//			PRIVATE DEFINES
//----------------------------------------

#define DRV_PWM_TIMER_HANDLER		&htim1
#define DRV_PWM_CHANNEL				TIM_CHANNEL_1
#define DRV_PWM_DUTY_CYCLE			TIM1->CCR1

#define DRV_HALL_A_Pin				DRV_INLA_Pin
#define DRV_HALL_A_GPIO_Port		DRV_INLA_GPIO_Port

#define DRV_HALL_B_Pin				DRV_INHB_Pin
#define DRV_HALL_B_GPIO_Port		DRV_INHB_GPIO_Port

#define DRV_HALL_C_Pin				DRV_INLB_Pin
#define DRV_HALL_C_GPIO_Port		DRV_INLB_GPIO_Port

#define DRV_DIR_Pin					DRV_INHC_Pin
#define DRV_DIR_GPIO_Port			DRV_INHC_GPIO_Port

#define DRV_NBRAKE_Pin				DRV_INLC_Pin
#define DRV_NBRAKE_GPIO_Port		DRV_INLC_GPIO_Port

#define DRV_EN_Pin					DRV_ENABLE_Pin
#define DRV_EN_GPIO_Port			DRV_ENABLE_GPIO_Port

#define DRV_SPI_HANDLER				&hspi1
#define DRV_SPI_NSS_Pin				SPI1_NSS_Pin
#define DRV_SPI_NSS_GPIO_Port		SPI1_NSS_GPIO_Port

#define MTR_HALL_A_Pin				HALL_A_Pin
#define MTR_HALL_A_GPIO_Port		HALL_A_GPIO_Port

#define MTR_HALL_B_Pin				HALL_B_Pin
#define MTR_HALL_B_GPIO_Port		HALL_B_GPIO_Port

#define MTR_HALL_C_Pin				HALL_C_Pin
#define MTR_HALL_C_GPIO_Port		HALL_C_GPIO_Port

#define Max_DC	97
#define Min_DC	-97

//----------------------------------------
//			PRIVATE PROTOTYPES
//----------------------------------------
void drv_sleep();
HAL_StatusTypeDef drv_wake();
HAL_StatusTypeDef drv_configuration();
HAL_StatusTypeDef drv_SPI_Transmit(uint8_t *pData);
HAL_StatusTypeDef drv_SPI_TransmitReceive(uint8_t *pTxData, uint8_t *pRxData);
void drv_setAllLow();

//----------------------------------------
//			PRIVATE DEFINITIONS
//----------------------------------------
uint16_t DRV_REGISTER_CONFIG[]={
//		      |A9876543210
		0b1000000000000000		, 	//REG 0h	READ ONLY DO NOT CHANGE
		0b1001000000000000		,	//REG 1h	READ ONLY DO NOT CHANGE
		0b0001000001000000		,	//REG 2h	DEFINED LATER WITH PWM_MODE (default : 6 step)
		0b0001101100010001		,	//REG 3h	NO LOCK 	IDRIVEP_HS 	50mA 	IDRIVEN_HS 	100mA
		0b0010010000010001		,	//REG 4h	CBC on 		TDRIVE 		500ns 	IDRIVEP_LS	50mA	IDRIVEN_LS	100mA
		0b0010101101111110		,	//REG 5h
		0b0011000011000011		,	//REG 6h	Unidirectional Current Sense 	CSA_GAIN 40V/V
		0b0011100000000000 			//REG 7h 	RESEVED DO NOT CHANGE
};

typedef struct{
	struct{
		float 	INHA; 	//rapport cyclique en %
		bool 	INLA;	//HALL_A
		bool 	INHB;	//HALL_B
		bool 	INLB;	//HALL_C
		bool 	INHC;	//DIR	0:CW 	1:CCW	(direction de la commande et non du moteur)
		bool 	INLC;	//nBRAKE 0:BRAKE	1:NO BRAKE
		bool 	ENABLE;
//		bool	nFAULT;
	}signal;

	DRV8353_MODE_t mode;

}DRV8353_t;

//----------------------------------------
//			PRIVATE VARIABLES
//----------------------------------------
DRV8353_t DRV8353;

//----------------------------------------
//			PUBLIC FUNCTIONS
//----------------------------------------

/*
 * Sature le rapport cyclique (dc) s'il est trop grand.
 * Applique le rapport cyclique sur la sortie PWM.
 * L'entrée doit etre comprise entre 0 et 600 car la periode du signal
 * PWM est égale à un comptage de 600.
 */
void
DRV_ApplyPWM(uint32_t dc)
{
	const uint32_t max = TIM1->ARR ; 	//1 period is 600 cnt



	if(dc < max)
		DRV_PWM_DUTY_CYCLE = dc * max/100.0;
	else
		DRV_PWM_DUTY_CYCLE = max;

}


/*
 * Permet de définir le rapport cyclique de la PWM envoyée au driver.
 * L’entrée doit être comprise entre -100 et 100 et correspond au rapport cyclique signé.
 */
void
DRV_SetPWM (int32_t dc){

	if(dc>=0)
		HAL_GPIO_WritePin(DRV_DIR_GPIO_Port, DRV_DIR_Pin, GPIO_PIN_SET); 	//DIR
	else
		HAL_GPIO_WritePin(DRV_DIR_GPIO_Port, DRV_DIR_Pin, GPIO_PIN_RESET); 	//DIR

	DRV8353.signal.INHC = HAL_GPIO_ReadPin(DRV_DIR_GPIO_Port,DRV_DIR_Pin);	//update local struct

	dc = abs(dc);

	if(dc > Max_DC) dc = Max_DC;	//Saturation

	DRV_ApplyPWM(dc);

}

/*
 * Permet de définir le mode de fonctionnement du DRV8353.
 * Au changement de mode, la valeur du rapport cyclique est remise à 0.
 * En cas de sortie du mode veille (SLEEP), la fonction drv_wake()
 * est appelée pour renvoyer la configuration au DRV8353 via SPI.
 */
HAL_StatusTypeDef
DRV_SetMode(DRV8353_MODE_t mode) {

	HAL_StatusTypeDef status = HAL_OK;

	if (DRV8353.mode != mode) {

		if (mode < DRV_MODE_SLEEP || mode > DRV_MODE_OPERATING)
			mode = DRV_MODE_SLEEP;

		if (mode == DRV_MODE_SLEEP)
			drv_sleep(); 		//Put the driver in sleep mode if needed

		else if (DRV8353.mode == DRV_MODE_SLEEP) //if mode change from sleep mode
			status |= drv_wake(); 						//wake up and configure

		if (status == HAL_OK)	//if no problem while wakening the driver
			DRV8353.mode = mode; //Apply the new mode

		DRV_ApplyPWM(0); //Set duty cycle to 0 because mode is still not applied and hall interrupt can happen in between

		DRV_UpdateHalls(); //Update Outputs

		//Set the nBrake when mode is not BRAKE
		HAL_GPIO_WritePin(DRV_NBRAKE_GPIO_Port, DRV_NBRAKE_Pin,
				(mode != DRV_MODE_BRAKE));	//nBRAKE

		//Reset DIR pin (defined later when DRV_SetPWM is called)
		HAL_GPIO_WritePin(DRV_DIR_GPIO_Port, DRV_DIR_Pin, GPIO_PIN_RESET);//DIR
	}
	return status;
}


/*
 * Permet de mettre à jour la sortie les sorties halls en fonction du
 * mode actuel du driver et des entrées halls.
 * Les sorties sont égales aux entrées uniquement en mode OPERATING.
 * En mode ALIGN elles sont forcées à l’état haut et dans tous les autres modes
 * elles sont forcées à l’état bas.
 */
void
DRV_UpdateHalls() {

/*	///////////  TEST   //////////////////
	GPIO_PinState hall_A = GPIO_PIN_RESET;
	GPIO_PinState hall_B = GPIO_PIN_SET;
	GPIO_PinState hall_C = GPIO_PIN_SET;

	HAL_GPIO_WritePin(DRV_HALL_A_GPIO_Port, DRV_HALL_A_Pin, hall_A); 	//HALL_A
	HAL_GPIO_WritePin(DRV_HALL_B_GPIO_Port, DRV_HALL_B_Pin, hall_B); 	//HALL_B
	HAL_GPIO_WritePin(DRV_HALL_C_GPIO_Port, DRV_HALL_C_Pin, hall_C); 	//HALL_C

	///////// FIN de TEST ////////////////
*/

	if ((DRV8353.mode) == DRV_MODE_OPERATING) {

		GPIO_PinState hall_A = HAL_GPIO_ReadPin(MTR_HALL_B_GPIO_Port, MTR_HALL_B_Pin); 	//HALL_A signal come from B connector
		GPIO_PinState hall_B = HAL_GPIO_ReadPin(MTR_HALL_A_GPIO_Port, MTR_HALL_A_Pin);	//HALL_B signal come from A connector
		GPIO_PinState hall_C = HAL_GPIO_ReadPin(MTR_HALL_C_GPIO_Port, MTR_HALL_C_Pin);	//HALL_C signal come from C connector

		HAL_GPIO_WritePin(DRV_HALL_A_GPIO_Port, DRV_HALL_A_Pin, hall_A); 	//HALL_A
		HAL_GPIO_WritePin(DRV_HALL_B_GPIO_Port, DRV_HALL_B_Pin, hall_B); 	//HALL_B
		HAL_GPIO_WritePin(DRV_HALL_C_GPIO_Port, DRV_HALL_C_Pin, hall_C); 	//HALL_C

	} else {
		//HALLs = 0 if ALIGN and 0 else
		HAL_GPIO_WritePin(DRV_HALL_A_GPIO_Port, DRV_HALL_A_Pin,	(DRV8353.mode == DRV_MODE_ALIGN)); 	//HALL_A
		HAL_GPIO_WritePin(DRV_HALL_B_GPIO_Port, DRV_HALL_B_Pin,	(DRV8353.mode == DRV_MODE_ALIGN)); 	//HALL_B
		HAL_GPIO_WritePin(DRV_HALL_C_GPIO_Port, DRV_HALL_C_Pin,	(DRV8353.mode == DRV_MODE_ALIGN)); 	//HALL_C
	}
/*
	DRV8353.signal.INLA = HAL_GPIO_ReadPin(DRV_HALL_A_GPIO_Port,DRV_HALL_A_Pin);	//HALL_A
	DRV8353.signal.INHB = HAL_GPIO_ReadPin(DRV_HALL_A_GPIO_Port,DRV_HALL_A_Pin);	//HALL_B
	DRV8353.signal.INLB = HAL_GPIO_ReadPin(DRV_HALL_A_GPIO_Port,DRV_HALL_A_Pin);	//HALL_C
	*/
}

void
DRV_FaultResolution(){

	// ｡･ﾟﾟ･(>д<)･ﾟﾟ･｡		(ó﹏ò｡)


}

//----------------------------------------
//			PRIVATE FUNCTIONS
//----------------------------------------

/*
 * Permet de mettre le DRV8353 en veille en abaissant le pin ENABLE à l’état bas
 * dans cet état tous les MOSFET sont éteints, le moteur est donc en roue libre.
 * Tous les registres interne sont effacés, il faudra renvoyer le configuration via
 * SPI après le réveille (déjà inclus dans la fonction drv_Wake() ).
 */
void
drv_sleep(){
	//DISABLE DRV
	HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_RESET);
	DRV8353.signal.ENABLE = 0;
	DRV8353.mode = DRV_MODE_SLEEP;
	drv_setAllLow();
	HAL_Delay(2); //Wait at least T_SLEEP = 1ms for driver to go in sleep mode
}

/*
 * Permet de sortir le DRV8353 du mode veille, au réveille tous les registres sont
 * effacés la configuration est donc renvoyé via SPI.
 * La valeur de la PWM est remise à 0.
 */
HAL_StatusTypeDef
drv_wake(){

	drv_setAllLow(); //SET all INHx and INLx to low before switching mode

	//ENABLE DRV
	HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_SET);
	DRV8353.signal.ENABLE = 1;

	HAL_Delay(2); //Wait at least T_WAKE = 1ms for driver to wake up

	if(drv_configuration() != HAL_OK) return HAL_ERROR;

	return HAL_OK;
}

/*
 * Permet d’envoyer la configuration au DRV8353 via SPI.
 */
HAL_StatusTypeDef
drv_configuration(){
	HAL_StatusTypeDef status = HAL_OK;
	//APPLY SPI CONFIG
	for (int i = 2; i < 7; i++)
		status |= drv_SPI_Transmit((uint8_t*) &DRV_REGISTER_CONFIG[i]);

	//CHECK SPI CONFIG
	for (int16_t i = 2; i < 7; i++) {
		uint16_t DRV_REGISTER_RECEIVE;
		uint16_t DRV_REGISTER_READ = 0b1000000000000000 | (i << 11);

		status |= drv_SPI_TransmitReceive((uint8_t*) &DRV_REGISTER_READ, (uint8_t*) &DRV_REGISTER_RECEIVE);

		if ((DRV_REGISTER_RECEIVE & 0b0000011111111111)	!= (DRV_REGISTER_CONFIG[i] & 0b0000011111111111)) //5 MSB are dont care bits
			status = HAL_ERROR;
	}

	return status;
}

/*
 * Permet d’envoyer la trame à l’adresse pData en SPI en incluant le contrôle du pin NSS.
 */
HAL_StatusTypeDef
drv_SPI_Transmit(uint8_t *pData){
	HAL_StatusTypeDef status = HAL_OK;
	HAL_GPIO_WritePin(DRV_SPI_NSS_GPIO_Port, DRV_SPI_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	status |= HAL_SPI_Transmit(DRV_SPI_HANDLER, pData,	1, 300);
	HAL_Delay(1);
	HAL_GPIO_WritePin(DRV_SPI_NSS_GPIO_Port, DRV_SPI_NSS_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	return status;
}

/*
 * Permet d’envoyer la trame à l’adresse pTxData et de stocker à
 * l’adresse pTxData la trame reçue en SPI en incluant le contrôle du pin NSS.
 */
HAL_StatusTypeDef
drv_SPI_TransmitReceive(uint8_t *pTxData, uint8_t *pRxData){
	HAL_StatusTypeDef status = HAL_OK;
	HAL_GPIO_WritePin(DRV_SPI_NSS_GPIO_Port, DRV_SPI_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	status |= HAL_SPI_TransmitReceive(DRV_SPI_HANDLER, pTxData, pRxData, 1, 300);
	HAL_Delay(1);
	HAL_GPIO_WritePin(DRV_SPI_NSS_GPIO_Port, DRV_SPI_NSS_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	return status;
}

/*
 * Permet de mettre toute les entrées du driver à 0 (avant de l’activer ou de changer de mode par exemple)
 */
void
drv_setAllLow(){
	//SET all INHx and INLx to low before switching mode
	HAL_GPIO_WritePin(DRV_HALL_A_GPIO_Port, DRV_HALL_A_Pin, GPIO_PIN_RESET); 	//HALL_A
	HAL_GPIO_WritePin(DRV_HALL_B_GPIO_Port, DRV_HALL_B_Pin, GPIO_PIN_RESET); 	//HALL_B
	HAL_GPIO_WritePin(DRV_HALL_C_GPIO_Port, DRV_HALL_C_Pin, GPIO_PIN_RESET); 	//HALL_C
	HAL_GPIO_WritePin(DRV_DIR_GPIO_Port, DRV_DIR_Pin, GPIO_PIN_RESET);			//DIR
	HAL_GPIO_WritePin(DRV_NBRAKE_GPIO_Port, DRV_NBRAKE_Pin, GPIO_PIN_RESET);	//BRAKE

	DRV_ApplyPWM(0);

	DRV8353.signal.INHA = 0;
	DRV8353.signal.INLA = 0;
	DRV8353.signal.INHB = 0;
	DRV8353.signal.INLB = 0;
	DRV8353.signal.INHC = 0;
	DRV8353.signal.INLC = 0;
}

