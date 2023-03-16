/*
 * The MIT License
 *
 * Copyright (c)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// ==================================================
//     INCLUDE
// ==================================================

#include "circular_buffer.h"



// ==================================================
//     LOCAL PROTOTYPES
// ==================================================

uint32_t
circular_buffer_ComputeNextIndex ( CIRCULAR_BUFFER_t * circular_buffer ,
                                   uint32_t            current_index   );

void *
circular_buffer_ComputeOffset ( CIRCULAR_BUFFER_t * circular_buffer ,
                                uint32_t            item_index      );



// ==================================================
//     PUBLIC FUNCTIONS
// ==================================================

CIRCULAR_BUFFER_STATUS_t
CIRCULAR_BUFFER_InitCircularBuffer ( CIRCULAR_BUFFER_t * circular_buffer )
{
    CIRCULAR_BUFFER_STATUS_t status;

    if ( ( circular_buffer != NULL ) && ( circular_buffer->buffer != NULL ) )
    {
        memset( circular_buffer->buffer , 0x00 , ( circular_buffer->item.size * circular_buffer->item.count ) );

        circular_buffer->head       = 0;
        circular_buffer->tail       = 0;
        circular_buffer->data_count = 0;
        status                      = CIRCULAR_BUFFER_STATUS_SUCCESS;
    }
    else
    {
        status = CIRCULAR_BUFFER_STATUS_NULL_POINTER;
    }

    return status;
}

CIRCULAR_BUFFER_STATUS_t
CIRCULAR_BUFFER_Read ( CIRCULAR_BUFFER_t * circular_buffer ,
                       void              * data_handler    )
{
    CIRCULAR_BUFFER_STATUS_t status = CIRCULAR_BUFFER_STATUS_FAIL;

    if ( ( circular_buffer != NULL ) && ( circular_buffer->buffer != NULL ) && ( data_handler != NULL ) )
    {
        if ( circular_buffer->data_count > 0 )
        {
            void * offset = circular_buffer_ComputeOffset( circular_buffer , circular_buffer->tail );

            memcpy( data_handler , offset , circular_buffer->item.size );

            circular_buffer->data_count--;
            circular_buffer->tail = circular_buffer_ComputeNextIndex( circular_buffer , circular_buffer->tail );
            status                = CIRCULAR_BUFFER_STATUS_SUCCESS;
        }
        else
        {
            status = CIRCULAR_BUFFER_STATUS_NO_DATA_AVAILABLE;
        }
    }
    else
    {
        status = CIRCULAR_BUFFER_STATUS_NULL_POINTER;
    }

    return status;
}

CIRCULAR_BUFFER_STATUS_t
CIRCULAR_BUFFER_Write ( CIRCULAR_BUFFER_t * circular_buffer ,
                        void              * data_handler    )
{
    CIRCULAR_BUFFER_STATUS_t status = CIRCULAR_BUFFER_STATUS_FAIL;

    if ( ( circular_buffer != NULL ) && ( circular_buffer->buffer != NULL ) && ( data_handler != NULL ) )
    {
        bool write_allowed = false;

        if ( circular_buffer->data_count < circular_buffer->item.count )
        {
            circular_buffer->data_count++;
            write_allowed = true;
        }
        else
        {
            if ( circular_buffer->overwrite_old_data == true )
            {
                circular_buffer->tail = circular_buffer_ComputeNextIndex( circular_buffer , circular_buffer->tail );
                write_allowed         = true;
            }
            else
            {
                status = CIRCULAR_BUFFER_STATUS_FULL_BUFFER;
            }
        }

        if ( write_allowed == true )
        {
            void * offset = circular_buffer_ComputeOffset( circular_buffer , circular_buffer->head );

            memcpy( offset , data_handler , circular_buffer->item.size );

            circular_buffer->head = circular_buffer_ComputeNextIndex( circular_buffer , circular_buffer->head );
            status                = CIRCULAR_BUFFER_STATUS_SUCCESS;
        }
    }
    else
    {
        status = CIRCULAR_BUFFER_STATUS_NULL_POINTER;
    }

    return status;
}



// ==================================================
//     LOCAL FUNCTIONS
// ==================================================

uint32_t
circular_buffer_ComputeNextIndex ( CIRCULAR_BUFFER_t * circular_buffer ,
                                   uint32_t            current_index   )
{
    uint32_t next_index;

    if ( ( current_index + 1 ) >= circular_buffer->item.count )
    {
        next_index = 0;
    }
    else
    {
        next_index = current_index + 1;
    }

    return next_index;
}

void *
circular_buffer_ComputeOffset ( CIRCULAR_BUFFER_t * circular_buffer ,
                                uint32_t            item_index      )
{
    uint8_t * offset = NULL;  // 'uint8_t*' because of "pointer of type ‘void *’ used in arithmetic" warning

    if ( item_index < circular_buffer->item.count )
    {
        offset = ( uint8_t * )circular_buffer->buffer + ( item_index * circular_buffer->item.size );
    }

    return ( void * )offset;
}



// End of file
