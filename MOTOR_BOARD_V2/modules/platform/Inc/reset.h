/*
 * reset.h
 *
 *  Created on: 7 avr. 2021
 *      Author: Sébastien Osenat
 */

#ifndef PLATFORM_INC_RESET_H_
#define PLATFORM_INC_RESET_H_


/// @brief  Possible STM32 system reset causes
typedef enum reset_cause_e
{
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_LOW_POWER_RESET,
    RESET_CAUSE_WINDOW_WATCHDOG_RESET,
    RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
    RESET_CAUSE_SOFTWARE_RESET,
    RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
    RESET_CAUSE_EXTERNAL_RESET_PIN_RESET,
} reset_cause_t;


reset_cause_t reset_cause_get(void);

#endif /* PLATFORM_INC_RESET_H_ */
