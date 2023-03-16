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

#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__



// ==================================================
//     CPP GUARD [INIT]
// ==================================================

#ifdef __cplusplus
extern "C" {
#endif



// ==================================================
//     INCLUDE
// ==================================================

#include <stdio.h>
#include <stdlib.h>



// ==================================================
//     TYPE DEFINITIONS
// ==================================================

typedef enum {

    LINKED_LIST_RESULT_SUCCESS = 0                ,
    LINKED_LIST_RESULT_ERROR_NULL_PARAM           ,
    LINKED_LIST_RESULT_ERROR_NODE_ALREADY_IN_LIST ,
    LINKED_LIST_RESULT_ERROR_NODE_NOT_IN_LIST     ,
    LINKED_LIST_RESULT_ERROR_MEMORY_ALLOCATION    ,
    LINKED_LIST_RESULT_ERROR_EMPTY_LIST           ,

} LINKED_LIST_RESULT_t;

typedef struct node {

    struct node * next;
    void        * data;

} NODE_t;

typedef struct list {

    NODE_t * head;

} LIST_t;



// ==================================================
//     PUBLIC PROTOTYPES
// ==================================================

/**
 * Creates the first node of a chained list
 *
 * @return Address of the first node
 */
LIST_t *
LINKED_LIST_CreateEmptyList ( void );

/**
 * Add a node at the end of the list
 *
 * @param list Address of the list
 * @param data Data to put in the new node
 * @return status of the operation
 */
LINKED_LIST_RESULT_t
LINKED_LIST_AddNode ( LIST_t * list ,
                      void   * data );

/**
 * Delete a specific node from the list
 *
 * @param list Address of the list
 * @param data Data to be deleted
 * @return status of the operation
 */
LINKED_LIST_RESULT_t
LINKED_LIST_DeleteNode ( LIST_t * list ,
                         void   * data );

/**
 * Find a node in the list
 *
 * @param list Address of the list
 * @param data Data of the node to be found
 * @return node if found
 */
NODE_t *
LINKED_LIST_FindNode ( LIST_t * list ,
                       void   * data );

/**
 * Browse the list
 *
 * @param list Address of the list
 * @return status of the operation
 */
LINKED_LIST_RESULT_t
LINKED_LIST_DisplayList ( LIST_t * list );


/**
 * Reverse the list direction
 *
 * @param list Address of the list
 * @return status of the operation
 */
LINKED_LIST_RESULT_t
LINKED_LIST_ReverseList ( LIST_t * list );

/**
 * Delete the the list
 *
 * @param list Address of the list
 * @return status of the operation
 */
LINKED_LIST_RESULT_t
LINKED_LIST_DestroyList ( LIST_t * list );



// ==================================================
//     CPP GUARD [END]
// ==================================================

#ifdef __cplusplus
}
#endif



#endif /* __LINKED_LIST_H__ */
