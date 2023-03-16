/*
 * file        : console.h
 * description : <insert here>
 * copyright   : (c) 2020 GEMA
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__



// ==================================================
//     CPP GUARD [INIT]
// ==================================================

#ifdef __cplusplus
extern "C" {
#endif



// ==================================================
//     INCLUDE
// ==================================================

#include "main.h"
#include "circular_buffer.h"



// ==================================================
//     DEFINITIONS
// ==================================================

#define CONSOLE_TX_SIZE     1024
#define CONSOLE_RX_SIZE     1024
#define EOL                 "\r\n"



// ==================================================
//     MACRO
// ==================================================

#if FUSE__CONSOLE__ERROR == CONFIG__OPTION__ENABLED
#define CONSOLE_ERROR( fmt , ... )      CONSOLE_PrintLog( LEVEL_ERROR , fmt , ##__VA_ARGS__ )
#else
#define CONSOLE_ERROR( fmt , ... )
#endif

#if FUSE__CONSOLE__WARNING == CONFIG__OPTION__ENABLED
#define CONSOLE_WARNING( fmt , ... )        CONSOLE_PrintLog( LEVEL_WARNING , fmt , ##__VA_ARGS__ )
#else
#define CONSOLE_WARNING( fmt , ... )
#endif

#if FUSE__CONSOLE__RUNTIME == CONFIG__OPTION__ENABLED
#define CONSOLE_RUNTIME( fmt , ... )        CONSOLE_PrintLog( LEVEL_RUNTIME , fmt , ##__VA_ARGS__ )
#else
#define CONSOLE_RUNTIME( fmt , ... )
#endif

#if FUSE__CONSOLE__DEBUG == CONFIG__OPTION__ENABLED
#define CONSOLE_DEBUG( fmt , ... )      CONSOLE_PrintLog( LEVEL_DEBUG , fmt , ##__VA_ARGS__ )
#else
#define CONSOLE_DEBUG( fmt , ... )
#endif



// ==================================================
//     TYPE DEFINITIONS
// ==================================================

typedef enum {

    LEVEL_ERROR = 0 ,
    LEVEL_WARNING   ,
    LEVEL_RUNTIME   ,
    LEVEL_DEBUG     ,

} CONSOLE_LEVEL_t;



// ==================================================
//     EXTERN VARIABLES
// ==================================================

extern CIRCULAR_BUFFER_t console_RX_buffer;



// ==================================================
//     PUBLIC PROTOTYPES
// ==================================================

/**
 * Initialize the Console
 *
 * @param instance UART handler
 * @return HAL_OK if initialization was successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef
CONSOLE_Init ( UART_HandleTypeDef * instance );

/**
 * Print data to the console port
 *
 * @param fmt Printf format string
 */
void
CONSOLE ( const char * fmt ,
          ...              );

/**
 * Print a log message to the console port
 *
 * @param level Level of the log message
 * @param fmt Printf format string
 */
void
CONSOLE_PrintLog ( CONSOLE_LEVEL_t   level ,
                   const char      * fmt   ,
                   ...                     );

/**
 * Ckeck if console UART transmission is busy
 */
bool
CONSOLE_IsBusy ( void );

/**
 * Flush the console UART transmission queue
 */
void
CONSOLE_Flush ( void );

/**
 * Console UART transmission interrupt
 */
void
CONSOLE_UART_TxCpltCallback ( void );

/**
 * Console UART reception interrupt
 */
void
CONSOLE_UART_RxCpltCallback ( void );



// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __CONSOLE_H__ */
