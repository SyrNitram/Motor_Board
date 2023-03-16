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

/**
 * @addtogroup middleware
 * @{
 */
/**
 * @addtogroup middleware_linkedlist Linked List
 * @{
 */
/**
 * @file
 *
 * @brief Linked List
 */
/** @} */
/** @} */



// ==================================================
//     INCLUDE
// ==================================================

#include "linked_list.h"



// ==================================================
//     LOCAL PROTOTYPES
// ==================================================

/**
 * Free a pointer
 *
 * @param ptr Pointer to free
 */
void
FREE ( void ** ptr );

/**
 * Create a new node
 *
 * @param data Data to put in the new node
 * @return Pointer on the new node
 */
NODE_t *
linked_list_CreateNode ( void * data );

/**
 * Find a node in a list
 *
 * @param list List to search
 * @param data  Data to look for in the list
 * @return Pointer on the searched node
 */
NODE_t *
linked_list_FindNode ( LIST_t * list ,
                       void   * data );



// ==================================================
//     PUBLIC FUNCTIONS
// ==================================================

LIST_t *
LINKED_LIST_CreateEmptyList ( void )
{
    LIST_t * list = malloc( sizeof( LIST_t ) );

    if ( list == NULL )
    {
        return NULL;
    }

    list->head = NULL;

    return list;
}

LINKED_LIST_RESULT_t
LINKED_LIST_AddNode ( LIST_t * list ,
                      void   * data )
{
    if ( ( list == NULL ) || ( data == NULL ) )
    {
        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( linked_list_FindNode( list , data ) != NULL )
    {
        return LINKED_LIST_RESULT_ERROR_NODE_ALREADY_IN_LIST;
    }

    NODE_t * new_node = linked_list_CreateNode( data );

    if ( new_node != NULL )
    {
        if ( list->head == NULL )
        {
            list->head = new_node;
        }
        else
        {
            NODE_t * last_node = list->head;

            while ( last_node->next != NULL )
            {
                last_node = last_node->next;
            }

            last_node->next = new_node;
        }

        return LINKED_LIST_RESULT_SUCCESS;
    }
    else
    {
        return LINKED_LIST_RESULT_ERROR_MEMORY_ALLOCATION;
    }
}

LINKED_LIST_RESULT_t
LINKED_LIST_DeleteNode ( LIST_t * list ,
                         void   * data )
{
    if ( ( list == NULL ) || ( data == NULL ) )
    {
        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }

    if ( linked_list_FindNode( list , data ) != NULL )
    {
        NODE_t * current  = list->head;
        NODE_t * previous = current;

        while ( current != NULL )
        {
            if ( current->data == data )
            {
                previous->next = current->next;

                if ( current == list->head )
                {
                    list->head = current->next;
                }

                FREE( ( void ** )&current );

                return LINKED_LIST_RESULT_SUCCESS;
            }

            previous = current;
            current  = current->next;
        }
    }

    return LINKED_LIST_RESULT_ERROR_NODE_NOT_IN_LIST;
}

NODE_t *
LINKED_LIST_FindNode ( LIST_t * list ,
                       void   * data )
{
    return linked_list_FindNode( list , data );
}

LINKED_LIST_RESULT_t
LINKED_LIST_DisplayList ( LIST_t * list )
{
    if ( list == NULL )
    {
        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( list->head == NULL )
    {
        return LINKED_LIST_RESULT_ERROR_EMPTY_LIST;
    }

    NODE_t * current = list->head;

    while ( current->next != NULL )
    {
        current = current->next;
    }

    return LINKED_LIST_RESULT_SUCCESS;
}

LINKED_LIST_RESULT_t
LINKED_LIST_ReverseList ( LIST_t * list )
{
    if ( list == NULL )
    {
        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }

    NODE_t * reversed = NULL;
    NODE_t * current  = list->head;
    NODE_t * temp     = NULL;

    while ( current != NULL )
    {
        temp       = current;
        current    = current->next;
        temp->next = reversed;
        reversed   = temp;
    }

    list->head = reversed;

    return LINKED_LIST_RESULT_SUCCESS;
}

LINKED_LIST_RESULT_t
LINKED_LIST_DestroyList ( LIST_t * list )
{
    if ( list == NULL )
    {
        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }

    if ( list->head != NULL )
    {
        NODE_t * current = list->head;
        NODE_t * next    = current;

        while ( current != NULL )
        {
            next = current->next;

            FREE( ( void ** )&current );

            current = next;
        }
    }

    FREE( ( void ** )&list );

    return LINKED_LIST_RESULT_SUCCESS;
}



// ==================================================
//     LOCAL FUNCTIONS
// ==================================================

void
FREE ( void ** ptr )
{
    free( *ptr );

    *ptr = NULL;
}

NODE_t *
linked_list_CreateNode ( void * data )
{
    NODE_t * node = malloc( sizeof( NODE_t ) );

    if ( node == NULL )
    {
        return NULL;
    }

    node->data = data;
    node->next = NULL;

    return node;
}

NODE_t *
linked_list_FindNode ( LIST_t * list ,
                       void   * data )
{
    NODE_t * node = list->head;

    while ( node != NULL )
    {
        if ( node->data == data )
        {
            return node;
        }

        node = node->next;
    }

    return NULL;
}



// End of file
