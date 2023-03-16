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

#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__



// ==================================================
//     CPP GUARD [INIT]
// ==================================================

#ifdef __cplusplus
extern "C" {
#endif



// ==================================================
//     INCLUDE
// ==================================================

#include <stdint.h>
#include <stdbool.h>
#include <string.h>



// ==================================================
//     TYPE DEFINITIONS
// ==================================================

typedef enum {

    CIRCULAR_BUFFER_STATUS_SUCCESS = 0       ,
    CIRCULAR_BUFFER_STATUS_FAIL              ,
    CIRCULAR_BUFFER_STATUS_NULL_POINTER      ,
    CIRCULAR_BUFFER_STATUS_NO_DATA_AVAILABLE ,
    CIRCULAR_BUFFER_STATUS_FULL_BUFFER       ,

} CIRCULAR_BUFFER_STATUS_t;

typedef struct {

    void         * buffer;

    struct {

        uint32_t   size;
        uint32_t   count;

    } item;

    bool           overwrite_old_data;

    uint32_t       head;
    uint32_t       tail;
    uint32_t       data_count;

} CIRCULAR_BUFFER_t;



// ==================================================
//     PUBLIC PROTOTYPES
// ==================================================

CIRCULAR_BUFFER_STATUS_t
CIRCULAR_BUFFER_InitCircularBuffer ( CIRCULAR_BUFFER_t * circular_buffer );

CIRCULAR_BUFFER_STATUS_t
CIRCULAR_BUFFER_Read ( CIRCULAR_BUFFER_t * circular_buffer ,
                       void              * data_handler    );

CIRCULAR_BUFFER_STATUS_t
CIRCULAR_BUFFER_Write ( CIRCULAR_BUFFER_t * circular_buffer ,
                        void              * data_handler    );



// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __CIRCULAR_BUFFER_H__ */
