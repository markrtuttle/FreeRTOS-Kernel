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

// copied from portable.h
//#define portBYTE_ALIGNMENT (8)
//#define portBYTE_ALIGNMENT_MASK ( 0x0007 )



/****************************************************************/

#define HEAP_SIZE 1024
#define HEAP_BLOCKS 2

typedef struct BLOCK_DESC
{
  size_t offset;
  size_t size;
} BlockDesc_t;

typedef BlockDesc_t HeapDesc_t[HEAP_BLOCKS+1];

void allocate_heap(uint8_t *heap, size_t heap_size) {

  BlockDesc_t block_desc[HEAP_BLOCKS+1];

  // Assume valid heap block descriptions
  size_t max = 0;
  for (int i = 0; i < HEAP_BLOCKS+1; i++) {
    __CPROVER_assume((block_desc[i].offset & portBYTE_ALIGNMENT_MASK) == 0);
    //__CPROVER_assume((block_desc[i].size & heapBLOCK_ALLOCATED_BITMASK) == 0);
    __CPROVER_assume(max <= block_desc[i].offset);
    __CPROVER_assume(block_desc[i].offset <= heap_size);
    __CPROVER_assume(block_desc[i].size >= sizeof(BlockLink_t));
    __CPROVER_assume(block_desc[i].size <= heap_size - block_desc[i].offset);
    max = block_desc[i].offset + block_desc[i].size;
  }

  // Allocate heap with valid heap block descriptions

  xStart.pxNextFreeBlock = (BlockLink_t *) (heap + block_desc[0].offset);
  xStart.xBlockSize = 0;

  pxEnd = (BlockLink_t *) (heap + block_desc[HEAP_BLOCKS].offset);
  pxEnd->pxNextFreeBlock = 0;
  pxEnd->xBlockSize = 0;

  xFreeBytesRemaining = 0;
  for (int i = 0; i < HEAP_BLOCKS; i++) {
    BlockLink_t *header = (BlockLink_t *)(heap + block_desc[i].offset);
    header->pxNextFreeBlock = (BlockLink_t *)(heap + block_desc[i+1].offset);
    header->xBlockSize = block_desc[i].size;
    xFreeBytesRemaining += block_desc[i].size;
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
