/*
 * sub_main_bis.c
 *
 *  Created on: 26 oct. 2022
 *      Author: Sebastien
 */


// ==================================================
//     INCLUDE
// ==================================================
#include "sub_main.h"
#include "drv8353.h"
#include "stander.h"
#include "com_can.h"
#include "pid.h"


// ==================================================
//     DEFINE
// ==================================================
#define WHEEL_CURRENT_REEL_COEFF	0.715f
#define STD_CURRENT_REEL_COEFF		0.659f


// ==================================================
//     DECLARATION
// ==================================================

bool flag_stop_homing;
bool start_auto_calib = false;
Global_Struct_t _motor;
///// Tasks
//La periode du timer du sequenceur est de 100us
SCHEDULER_TASK_t task_MotorCtrl =
{
    .callback   = TaskCallback_MotorCtrl		  	,
    .arg        = NULL                            	,
    .type       = SCHEDULER_TASK_TYPE_PERIODIC    	,
    .timeout_us = 1000	                          	,// = 1 ms
	.count		= 400								 	,
};

///// Tasks
SCHEDULER_TASK_t task_CurrentCalculation =
{
    .callback   = TaskCallback_Current			  	,
    .arg        = NULL                            	,
    .type       = SCHEDULER_TASK_TYPE_PERIODIC    	,
    .timeout_us = 200	                          	,// = 100 us
	.count		= 100								 	,
};

///// Tasks
SCHEDULER_TASK_t task_Com_mgt =
{
    .callback   = TaskCallback_Com_Mgt   		  	,
    .arg        = NULL                            	,
    .type       = SCHEDULER_TASK_TYPE_PERIODIC    	,
    .timeout_us = 50000                           	,//  = 50 ms
	.count		= 200						 		,
};

SCHEDULER_TASK_t task_ADC =
{
    .callback   = TaskCallback_ADC			 	  	,
    .arg        = NULL                            	,
    .type       = SCHEDULER_TASK_TYPE_PERIODIC    	,
    .timeout_us = 50000                           	,//  = 50 ms
	.count		= 0								 	,
};

SCHEDULER_TASK_t task_AutoCalibration =
{
    .callback   = TaskCallback_Calib		 	  	,
    .arg        = NULL                            	,
    .type       = SCHEDULER_TASK_TYPE_PERIODIC    	,
    .timeout_us = 10000000                        	,//  = 10 s
};



/*
 * Conversion en tension et mise à jour de la moyenne glissante
 */

void
Compute_Average_Force ( _FORCE_t *f, SENSOR_ADC_CHANNEL_t channel)
{
#define SIZE_TAB 5 // TODO : remplacer par une constante liée à la vraie taille du tableau

	const float adcToVolt = 3.3/4096.0/(float)SIZE_TAB; // NE PAS METTRE EN DEFINE sinon temps de calcul plus long

	//On soustrait la valeur la plus ancienne
	f->adc.moy -= f->adc.tab_ADC[f->adc.idx];

	//On remplace l'ancienne valeur par la nouvelle
	f->adc.tab_ADC[f->adc.idx] = (float)(board.sensor.rx_dma[ channel ]) * adcToVolt; // TODO:replace 3.3 by vcc adc channel

	//On ajoute la nouvelle valeur à la moyenne
	f->adc.moy += f->adc.tab_ADC[f->adc.idx];

	//On passe à l'indice suivant pour le prochain tour
	f->adc.idx = (f->adc.idx + 1) % SIZE_TAB ;
}

/*
 * Retourne la valeur de la force calculée
 */
void
Compute_Force ( _FORCE_t *f)
{
		f->force.f = ( f->force.offset - f->adc.moy ) * f->force.pente; //pente en N/v
}




/*
 * Calcul de la moyenne des courants mesurés dans chaque phase
 */
void
Compute_Average_Current ()
{
	static float tab_current[3][10]={{0.0}};
	static uint8_t old_idx=1;
	static float sum_a = 0.0, sum_b =0.0, sum_c =0.0;
	float new_a = 0.0, new_b = 0.0, new_c = 0.0;
	float coeff;
	if(board.select.type_IsStander)
		coeff = STD_CURRENT_REEL_COEFF;
	else
		coeff = WHEEL_CURRENT_REEL_COEFF;

	//Calcul des sommes
	new_a = ( (3.0 - ( ((float)board.sensor.rx_dma[ PHASE_A_CURRENT ] * 3.3)/4095.0 ))
					/ (40.0 * 0.001)) * coeff;
	sum_a += new_a ;
	sum_a -= tab_current[PHASE_A_CURRENT][old_idx];

	new_b = ( (3.0 - ( ((float)board.sensor.rx_dma[ PHASE_B_CURRENT ] * 3.3)/4095.0 ))
						/ (40.0 * 0.001)) * coeff;
	sum_b += new_b ;
	sum_b -= tab_current[PHASE_B_CURRENT][old_idx];

	new_c = ( (3.0 - ( ((float)board.sensor.rx_dma[ PHASE_C_CURRENT ] * 3.3)/4095.0 ))
						/ (40.0 * 0.001)) * coeff;
	sum_c += new_c ;
	sum_c -= tab_current[PHASE_C_CURRENT][old_idx];


	//Calcul des moyennes
	board.sensor.ia = sum_a / 10.0;
	board.sensor.ib = sum_b / 10.0;
	board.sensor.ic = sum_c / 10.0;

	//Sauvegarde de la nouvelle valeur
	tab_current[PHASE_A_CURRENT][old_idx] = new_a ;
	tab_current[PHASE_B_CURRENT][old_idx] = new_b ;
	tab_current[PHASE_C_CURRENT][old_idx] = new_c ;

	//Increment de l'indice
	if(old_idx < 9)old_idx++;
	else old_idx = 0;
}

float
Select_phase_current( _SENSORED_PHASE phase )
{
	if( phase == A)
		return board.sensor.ia;
	else if( phase == B)
		return board.sensor.ib;
	else
		return board.sensor.ic;

}


// ==================================================
//     TASK
// ==================================================

void
TaskCallback_MotorCtrl ( void * arg )
{

	if(board.control.IsOn)
	{
		if(board.select.type_IsStander)
		{
			stander_Update_Eos( &board ); //Lecture capteur fin de course

			if( board.flags.status_flag.single.homing_status == In_progress )
				Callback_HomingProcess( &board );
		}

		Motor_ctrl( &board , task_MotorCtrl.timeout_us );
	}

}

/*
 * EQUATION FOURNIE P.43 DATASHEET DRV
 * 		Vref - Vadc
 * I = -------------
 * 		G * Rsense
 *
 * 		Vref = 1.5V
 * 		G = 40V/V cf drv8353.c
 * 		Rsense = 0.001 ohm
 *
 * 	Iréel = I * gain
 * 		gain = 0.715 calculé ampériquement
 */
void
TaskCallback_Current ( void * arg)
{

	if(board.control.IsOn)
	{
/*
		//Calcul de la moyenne du courant dans chaque phase
		//Compute_Average_Current();

		//Selection de la phase à prendre en compte
		//float current = Select_phase_current( board.motor.phase_sensored );

		//Application du correcteur sur le courant
		float pwm = 0.0, error = 0.0;
		const float Kadc = 0.0, Kcorr = 0.0, Tech = 200.0, tau = 2000.0;

		error =  (float)Get_Target_PWM() - current;
*/
		board.control.voltage.setpoint = board.control.target_duty_cycle * 1000.0;

		PID_Compute(&(board.control.voltage),	(int32_t) board.motor.actualDC,	 200.0 / 1000000.0);

		DRV_SetPWM( (int32_t)(board.control.voltage.output / 1000.0) ); //board.control.target_duty_cycle

		static int i = 0;
		static float last_output = 0;

		if (i >= 10000 || ((board.control.voltage.output != last_output) && i >= 50)) {
			i = 0;
//			CONSOLE(EOL"%ld;%.0f;%ld"EOL, board.control.position.setpoint, board.control.position.output, board.control.duty_cycle);
//			CONSOLE("%ld;%ld;%ld"EOL, board.control.voltage.setpoint,(int32_t) (board.motor.actualDC ),	 (int32_t)(board.control.voltage.output ));
			last_output = board.control.voltage.output;
		}

		i++;

		board.motor.actualDC = board.control.voltage.output;

	}
	else{
		DRV_SetPWM( 0 );
		board.motor.actualDC = 0;
		board.control.voltage.setpoint = 0;
	}


}

void
TaskCallback_Com_Mgt ( void * arg)
{

	if(msg_can.RX.idx > 0) //Si au moins 1 message CAN a été reçu
	{
/*		uint8_t i=msg_can.RX.idx;
		CONSOLE("B%d %d\r",
						msg_can.RX.bytes[i-1][5],
						msg_can.RX.bytes[i-1][4]);*/
		//On appelle la fonction de tri des msg can reçus
		Can_frame_mgt( &board );
	}

	//On envoie msg TPDO1 et 2
	CAN_TPDO1_MSG( &board );
	CAN_TPDO2_MSG( &board );
}

/*
 * Tache ADC qui récupère les valeurs
 * brutes ADC et les convertit en V
 *
 */
void
TaskCallback_ADC ( void * arg )
{
	//Calcule des tensions moyennes
	Compute_Average_Force(&board.sensor.fx, HANDLER_FX);
	Compute_Average_Force(&board.sensor.fz, HANDLER_FZ);

	//Calcule des forces
	Compute_Force( &board.sensor.fx );
	Compute_Force( &board.sensor.fz );

	//On détecte que la poignée a buté sur quelque chose
	if( board.sensor.fz.force.f < 0.0)
		flag_stop_homing = true;
}

/*
 * Tache d'auto-calibration
 * Récupère la valeur moyenne lue
 * par l'ADC sur le capteur
 * et on en calcule la moyenne sur
 * plusieurs dizaines de secondes
 *
 */
void
TaskCallback_Calib( void* arg )
{
#define size 5
	static float   t_offset_x[size] = {0.0}, t_offset_z[size] = {0.0};
	static uint8_t idx = 0;

	t_offset_x[idx] = board.sensor.fx.adc.moy ;
	t_offset_z[idx] = board.sensor.fz.adc.moy ;

	if( idx == (size-1) )
	{
		//Calcul moyenne et stockage des offset
		float somme_x = 0.0, somme_z = 0.0;
		for(uint8_t i =0; i < size; i++)
		{
			somme_x += t_offset_x[i];
			somme_z += t_offset_z[i];
		}

		board.sensor.fx.force.offset = somme_x / size;
		board.sensor.fz.force.offset = somme_z / size;

		//Reset pour une potentielle prochaine calibration
		idx = 0;

		//MAJ du flag d'auto-calibration
		board.flags.status_flag.single.auto_calib_status = Done;

	}
	else
		idx++;
}
