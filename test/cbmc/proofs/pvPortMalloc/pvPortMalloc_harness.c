// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

/*
 * Insert copyright notice
 */
#include <stddef.h>

#include "FreeRTOS.h"

/**
 * @file pvPortMalloc_harness.c
 * @brief Implements the proof harness for pvPortMalloc function.
 */

/*
 * Insert project header files that
 *   - include the declaration of the function
 *   - include the types needed to declare function arguments
 */
void * pvPortMalloc( size_t xWantedSize );

void harness()
{
    uint8_t app_heap[ configTOTAL_HEAP_SIZE ];
    void *pv1, *pv2, *pv3, *pv4;
    size_t xWantedSize1, xWantedSize2, xWantedSize3, xWantedSize4;

    HeapRegion_t xHeapRegions[] =
    {
        { ( unsigned char * ) app_heap, sizeof( app_heap ) },
        { NULL,                         0                  }
    };
    vPortDefineHeapRegions( xHeapRegions );

    __CPROVER_assume( xWantedSize1 < configTOTAL_HEAP_SIZE );
    __CPROVER_assume( xWantedSize2 < configTOTAL_HEAP_SIZE );
    __CPROVER_assume( xWantedSize3 < configTOTAL_HEAP_SIZE );
    __CPROVER_assume( xWantedSize4 < configTOTAL_HEAP_SIZE );

    pv1 = pvPortMalloc( xWantedSize1 );
    pv2 = pvPortMalloc( xWantedSize2 );
    vPortFree( pv1 );
    pv3 = pvPortMalloc( xWantedSize3 );
    vPortFree( pv2 );
    pv4 = pvPortMalloc( xWantedSize4 );
}
