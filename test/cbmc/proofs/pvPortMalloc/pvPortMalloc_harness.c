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

/****************************************************************/

// copied from heap_5.c
typedef struct A_BLOCK_LINK
{
  struct A_BLOCK_LINK * pxNextFreeBlock;
  size_t xBlockSize;
} BlockLink_t;

// copied from heap_5.c, removed static attribute in heap_5.c
extern BlockLink_t xStart;
extern BlockLink_t *pxEnd;
extern size_t xFreeBytesRemaining;

/****************************************************************/

#define HEAP_SIZE 1024
#define HEAP_BLOCKS 2

void allocate_heap(uint8_t *heap, size_t heap_size) {

  size_t offset[HEAP_BLOCKS+1];
  size_t size[HEAP_BLOCKS+1];

  size_t minimum_offset = 0;
  for (int i = 0; i < HEAP_BLOCKS+1; i++) {
    __CPROVER_assume((offset[i] & portBYTE_ALIGNMENT_MASK) == 0);
    __CPROVER_assume(minimum_offset <= offset[i]);
    __CPROVER_assume(offset[i] <= heap_size);

    __CPROVER_assume(size[i] >= sizeof(BlockLink_t));
    __CPROVER_assume(size[i] <= heap_size - offset[i]);

    minimum_offset = offset[i] + size[i];
  }

  size_t free_bytes = 0;
  for (int i = 0; i < HEAP_BLOCKS; i++) {
    BlockLink_t *header = (BlockLink_t *)(heap + offset[i]);
    header->pxNextFreeBlock = (BlockLink_t *)(heap + offset[i+1]);
    header->xBlockSize = size[i];
    free_bytes += size[i];
  }

  xStart.pxNextFreeBlock = (BlockLink_t *) (heap + offset[0]);
  xStart.xBlockSize = 0;

  pxEnd = (BlockLink_t *) (heap + offset[HEAP_BLOCKS]);
  pxEnd->pxNextFreeBlock = NULL;
  pxEnd->xBlockSize = 0;

  xFreeBytesRemaining = free_bytes;
}

bool valid_heap(BlockLink_t xStart, BlockLink_t *pxEnd, size_t xFreeBytesRemaining) {

  if (xStart.pxNextFreeBlock == NULL) return false;
  if (pxEnd == NULL) return false;

  size_t free_bytes = 0;
  for (BlockLink_t *block = xStart.pxNextFreeBlock; block != pxEnd; block = block->pxNextFreeBlock) {
    if (block > block->pxNextFreeBlock) return false;
    if (block + block->xBlockSize > block->pxNextFreeBlock) return false;
    free_bytes += block->xBlockSize;
  }
  if (xFreeBytesRemaining != free_bytes) return false;
  return true;
}



/****************************************************************/

void harness()
{
    uint8_t heap[HEAP_SIZE];
    size_t wanted_size;

    allocate_heap(&heap[0], HEAP_SIZE);

    //assert(valid_heap(xStart, pxEnd, xFreeBytesRemaining));
    void *pv = pvPortMalloc(wanted_size);
    //assert(valid_heap(xStart, pxEnd, xFreeBytesRemaining));
}
