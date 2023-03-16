/*
 * file        : console.c
 * description : <insert here>
 * copyright   : (c) 2020 GEMA
 */

/**
 * @addtogroup middleware
 * @{
 */
/**
 * @addtogroup middleware_console Debug Console
 * @{
 */
/**
 * @file
 *
 * @brief Debug Console
 */
/** @} */
/** @} */



// ==================================================
//     INCLUDE
// ==================================================

#include "common.h"

#include "console.h"
#include "linked_list.h"




// ==================================================
//     TYPE DEFINITIONS
// ==================================================

typedef struct {

    UART_HandleTypeDef * dev;

    struct {

        char             buffer [ CONSOLE_TX_SIZE ];

    } tx;

    struct {

        char             buffer [ CONSOLE_RX_SIZE ];
        char             item;

    } rx;

    LIST_t             * list;
    volatile bool        is_busy;
    uint32_t             list_size;

} CONSOLE_RUNTIME_t;



// ==================================================
//     LOCAL PROTOTYPES
// ==================================================

void
console_AppendMessageToList ( const char * msg );

void
console_ProcessOutput ( void );




// ==================================================
//     GLOBAL CONSTANTS
// ==================================================

const uint32_t console_list_max_size    = 35;  /* Size limitation to prevent memory overflow */
const char     console_level_mapping [] = "EWRD";




// ==================================================
//     GLOBAL VARIABLES
// ==================================================

static CONSOLE_RUNTIME_t console_runtime;

CIRCULAR_BUFFER_t console_RX_buffer;



// ==================================================
//     PUBLIC FUNCTIONS
// ==================================================

HAL_StatusTypeDef
CONSOLE_Init ( UART_HandleTypeDef * instance )
{

    HAL_StatusTypeDef        status = HAL_ERROR;

    if ( instance != NULL )
    {
        CIRCULAR_BUFFER_STATUS_t circular_buffer_status;
        if ( console_runtime.list != NULL )
        {
            LINKED_LIST_DestroyList( console_runtime.list );
        }

        memset( ( void * )&console_runtime   , 0x00 , sizeof( CONSOLE_RUNTIME_t ) );
        memset( ( void * )&console_RX_buffer , 0x00 , sizeof( CIRCULAR_BUFFER_t ) );

        console_runtime.dev  = instance;
        console_runtime.list = LINKED_LIST_CreateEmptyList();
        status               = ( console_runtime.list != NULL ) ? HAL_OK : HAL_ERROR;

        if ( status != HAL_OK )
        {
            return status;
        }

        console_RX_buffer.buffer             = ( void * )console_runtime.rx.buffer;
        console_RX_buffer.item.size          = sizeof( console_runtime.rx.buffer[ 0 ] );
        console_RX_buffer.item.count         = ARRAY_SIZE( console_runtime.rx.buffer );
        console_RX_buffer.overwrite_old_data = true;

        circular_buffer_status = CIRCULAR_BUFFER_InitCircularBuffer( &console_RX_buffer );
        status                 = ( circular_buffer_status == CIRCULAR_BUFFER_STATUS_SUCCESS ) ? HAL_OK : HAL_ERROR;

        if ( status != HAL_OK )
        {
            return status;
        }

        HAL_UART_Receive_IT( console_runtime.dev , ( uint8_t * )&console_runtime.rx.item , 1 );
    }

    return status;

}

void
CONSOLE ( const char * fmt ,
          ...              )
{

    va_list args;

    ///// Prepare the output buffer
    va_start( args , fmt );
    vsprintf( console_runtime.tx.buffer , fmt , args );
    va_end( args );

    ///// Append message to list
    console_AppendMessageToList( console_runtime.tx.buffer );

    ///// Start the transmission
    console_ProcessOutput();

}

void
CONSOLE_PrintLog ( CONSOLE_LEVEL_t   level ,
                   const char      * fmt   ,
                   ...                     )
{

    va_list args;

    ///// Prepare the output buffer
    sprintf( &console_runtime.tx.buffer[ 0 ] , "[%08lX][%c] " , HAL_GetTick() , console_level_mapping[ level ] );
    va_start( args , fmt );
    vsprintf( &console_runtime.tx.buffer[ strlen( console_runtime.tx.buffer ) ] , fmt , args );
    va_end( args );
    sprintf( &console_runtime.tx.buffer[ strlen( console_runtime.tx.buffer ) ] , EOL );

    ///// Append message to list
    console_AppendMessageToList( console_runtime.tx.buffer );

    ///// Start the transmission
    console_ProcessOutput();

}

bool
CONSOLE_IsBusy ( void )
{
    return console_runtime.is_busy;
}

void
CONSOLE_Flush ( void )
{

    if ( console_runtime.list != NULL )
    {
        if ( console_runtime.is_busy == true )
        {
            HAL_UART_AbortTransmit_IT( console_runtime.dev );

            console_runtime.is_busy = false;
        }

        NODE_t * node;
        void   * data;

        while ( true )
        {
            node = console_runtime.list->head;

            if ( node != NULL )
            {
                data = node->data;

                HAL_UART_Transmit( console_runtime.dev , ( uint8_t * )data , strlen( ( char * )data ) , 10 );

                LINKED_LIST_DeleteNode( console_runtime.list , data );
                free( data );

                console_runtime.list_size--;
            }
            else
            {
                break;
            }
        }
    }

}

void
CONSOLE_UART_TxCpltCallback ( void )
{

    if ( console_runtime.list != NULL )
    {
        NODE_t * node = console_runtime.list->head;

        if ( node != NULL )
        {
            void * data = node->data;

            LINKED_LIST_DeleteNode( console_runtime.list , data );
            free( data );

            console_runtime.list_size--;
        }

        console_runtime.is_busy = false;

        console_ProcessOutput();
    }

}

void
CONSOLE_UART_RxCpltCallback ( void )
{

    CIRCULAR_BUFFER_Write( &console_RX_buffer , ( void * )&console_runtime.rx.item );
    HAL_UART_Receive_IT( console_runtime.dev , ( uint8_t * )&console_runtime.rx.item , 1 );

}



// ==================================================
//     LOCAL FUNCTIONS
// ==================================================


void
console_AppendMessageToList ( const char * msg )
{
    if ( console_runtime.list != NULL )
    {
        if ( console_runtime.list_size >= console_list_max_size )
        {
            return;
        }

        char * tmp_ptr = ( char * )malloc( strlen( msg ) + 1 );

        if ( tmp_ptr != NULL )
        {
            memset( ( void * )tmp_ptr , 0x00 , strlen( msg ) + 1 );
            memcpy( ( void * )tmp_ptr , ( void * )msg , strlen( msg ) );

            LINKED_LIST_AddNode( console_runtime.list , ( void * )tmp_ptr );

            console_runtime.list_size++;
        }
    }
}

void
console_ProcessOutput ( void )
{
    if ( console_runtime.list != NULL )
    {
        if ( console_runtime.is_busy == false )
        {
            NODE_t * node = console_runtime.list->head;

            if ( node != NULL )
            {
                console_runtime.is_busy = true;

                HAL_UART_Transmit_IT( console_runtime.dev , ( uint8_t * )node->data , strlen( ( char * )node->data ) );
            }
        }
    }
}




// End of file
