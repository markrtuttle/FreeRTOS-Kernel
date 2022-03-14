// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

/*
 * Insert copyright notice
 */
#include <stddef.h>
#include <stdlib.h>

#include "FreeRTOS.h"

#include <stdbool.h>
#include <assert.h>

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

// copied from heap_5.c
typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK * pxNextFreeBlock; /*<< The next free block in the list. */
    size_t xBlockSize;                     /*<< The size of the free block. */
} BlockLink_t;
extern BlockLink_t xStart;
extern BlockLink_t *pxEnd;
extern size_t xFreeBytesRemaining;

/****************************************************************/

#define HEAP_SIZE 1024
#define HEAP_BLOCKS 2

void allocate_heap(uint8_t *heap, size_t heap_size) {

  size_t offset[HEAP_BLOCKS+1];
  size_t size[HEAP_BLOCKS+1];

  // Assume valid heap block descriptions
  size_t max = 0;
  for (int i = 0; i < HEAP_BLOCKS+1; i++) {
    __CPROVER_assume((offset[i] & portBYTE_ALIGNMENT_MASK) == 0);
    __CPROVER_assume(max <= offset[i]);
    __CPROVER_assume(offset[i] <= heap_size);
    __CPROVER_assume(size[i] >= sizeof(BlockLink_t));
    __CPROVER_assume(size[i] <= heap_size - offset[i]);
    max = offset[i] + size[i];
  }

  // Allocate heap with valid heap block descriptions

  xStart.pxNextFreeBlock = (BlockLink_t *) (heap + offset[0]);
  xStart.xBlockSize = 0;

  pxEnd = (BlockLink_t *) (heap + offset[HEAP_BLOCKS]);
  pxEnd->pxNextFreeBlock = 0;
  pxEnd->xBlockSize = 0;

  xFreeBytesRemaining = 0;
  for (int i = 0; i < HEAP_BLOCKS; i++) {
    BlockLink_t *header = (BlockLink_t *)(heap + offset[i]);
    header->pxNextFreeBlock = (BlockLink_t *)(heap + offset[i+1]);
    header->xBlockSize = size[i];
    xFreeBytesRemaining += size[i];
  }

}




void _allocate_heap(uint8_t *heap, size_t size) {

  pxEnd = (BlockLink_t *) malloc(sizeof(BlockLink_t));
  __CPROVER_assume(pxEnd != NULL);
  pxEnd->pxNextFreeBlock = NULL;
  pxEnd->xBlockSize = 0;

  size_t offset1;
  size_t size1;

  assert(size >= sizeof(BlockLink_t));

  __CPROVER_assume(offset1 <= size - sizeof(BlockLink_t));
  __CPROVER_assume((offset1 & portBYTE_ALIGNMENT_MASK) == 0);
  __CPROVER_assume(size1 >= sizeof(BlockLink_t));
  __CPROVER_assume(size1 <= size - offset1);

  BlockLink_t *block = (BlockLink_t *) (heap + offset1);
  block -> pxNextFreeBlock = pxEnd;
  block -> xBlockSize = size1;

  xStart.pxNextFreeBlock = block;
  xStart.xBlockSize = 0;
  xFreeBytesRemaining = size1;

  return;
}

void harness()
{
    uint8_t app_heap[ HEAP_SIZE ];
    allocate_heap(&app_heap[0], HEAP_SIZE);

    size_t wanted_size;
    void *pv = pvPortMalloc(wanted_size);
}
