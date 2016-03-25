/*
 * Copyright (C) 2015 Anshul Routhu <anshul.m67@gmail.com>
 * All rights reserved.
 */

#ifndef MEMMGR_H_
#define MEMMGR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

/**
 * Initialize the memory manager with the specified size
 */
int MemInitPool(void* mem, size_t size);

/**
 * De-initialized the memory manager, resets all the pointers
 */
int MemDeInitPool();

/**
 * Allocate the memory of requested size
 * @param size size_t type size of requested memory
 * @return block void* type a block of memory
 */
void* MemAlloc(size_t size);

/**
 * Free the memory
 * @param mem void* type the memory address to be freed
 */
void MemFree(void* mem);

/**
 * Print the current status of the memory pool
 * Prints free_list, allocated_list and heap list
 */
void MemPrintStatus();

/**
 * Get total number of allocated blocks since the last coalesce
 * @return count number of blocks in the free list
 */
size_t MemGetTotalAllocCount();

/**
 * Get total number of freed blocks since the last coalesce
 * @return count number of blocks in the free list
 */
size_t MemGetTotalFreeCount();

/**
 * Get total memory that is in the free list
 * @return total_mem size_t type total memory that is in the free list, but no coalesced yet
 */
size_t MemGetTotalMemFree();

/**
 * Get the total memory in the alloc_list
 * @return total_mem size_t type total memory that is being allocated
 */
size_t MemGetTotalMemAlloc();

#ifdef __cplusplus
}
#endif

#endif /* MEMMGR_H_ */
