/*
 * Copyright (C) 2015 Anshul Routhu <anshul.m67@gmail.com>
 * All rights reserved.
 */

#include "memmgr.h"
#include <assert.h>

// alignment boundry 4 or 8
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & (~(ALIGNMENT - 0x1)))

#define BLOCK_HEADER_SIZE ALIGN(sizeof(block_t))
#define CAST_PTR(ptr) ((char*)ptr)
#define MIN_BLOCK_SIZE(size) (size + BLOCK_HEADER_SIZE)

typedef struct block_header_t block_t;

struct block_header_t
{
    block_t* next;
    size_t size;
};

//global variables
block_t* heap = NULL; // allocate the requested memory from this pointer
size_t pool_size = 0; // track the total size of the memory initialized
block_t* mem_pool = NULL; // head pointing to the very begining of the memory pool

// local functions for managing memory pool
int MergeBlocks(block_t* b1, block_t* b2);
block_t* MemCoalesceBlocks(block_t* head);
block_t* GetFreeBlock(block_t** head, size_t size);

/**
 * Initialize the memory manager with the specified size
 */
int MemInitPool(void* mem, size_t size)
{
    mem_pool = (block_t*) mem;
    pool_size = size;

    heap = mem_pool;
    heap->size = size - BLOCK_HEADER_SIZE;
    heap->next = NULL;
    return (size);
}

/**
 * De-initialized the memory manager, resets all the pointers
 */
int MemDeInitPool()
{
#ifdef MEM_DEBUG
    // remove all the blocks from free_list and insert into heap
    // this may not be required, can be avoided for performance reasons
    heap = MemCoalesceBlocks(mem_pool);

    if (heap->size + BLOCK_HEADER_SIZE != pool_size)
    {
        printf("Memory not freed correctly\n");
    }

    assert(heap->size + BLOCK_HEADER_SIZE == pool_size);
#endif

    heap = NULL;
    mem_pool = NULL;
    return (0);
}

/**
 * Allocate the memory of requested size
 * @param size size_t type size of requested memory
 * @return block void* type a block of memory
 */
void* MemAlloc(size_t size)
{
#ifdef MEM_DEBUG
    printf("Allocation %d %d\n", (int)size, (int)ALIGN(size));
#endif

    // always align the size to its closest possible allocation size
    size = ALIGN(size);
    block_t* block = GetFreeBlock(&heap, size);

    // unable to find a fit block, coalesce the heap and try again
    if (!block)
    {
        heap = MemCoalesceBlocks(mem_pool);
        block = GetFreeBlock(&heap, size);
    }

    if (block)
    {
#ifdef MEM_DEBUG
    printf("Allocated %d Block Addr: %p Mem Addr: %p\n", (int) block->size & ~0x1, block,
        (CAST_PTR(block) + BLOCK_HEADER_SIZE));
#endif
        return (CAST_PTR(block) + BLOCK_HEADER_SIZE);
    }

#ifdef MEM_DEBUG
        printf("Allocation failed\n");
#endif

    return NULL;
}

/**
 * Get a free block from the provided head of size size
 * @param block block_t** type the block to be partitioned
 * @param size sizt_t type size of the block to be partitioned
 * @return block block_t* type the partitioned block
 */
block_t* GetFreeBlock(block_t** head, size_t size)
{
    block_t* block = NULL;
    if ((*head)->size > size + BLOCK_HEADER_SIZE)
    {
        block = (*head);
        (*head) = (block_t*)(CAST_PTR(block) + BLOCK_HEADER_SIZE + size);
        (*head)->next = block->next;
        block->next = (*head);
        (*head)->size = block->size - size - BLOCK_HEADER_SIZE;
        block->size = size;
        block->size |= 0x1;
    }

    return (block);
}

/**
 * Free the memory
 * @param mem void* type the memory address to be freed
 */
void MemFree(void* mem)
{
    block_t* block = (block_t*) (CAST_PTR(mem) - BLOCK_HEADER_SIZE);
    assert(block);

    block->size &= ~0x1;
}

/**
 * Coalesce all the blocks in the free_list and return the new best heap to be used
 * the new heap is the first largest block found after coalescing the free_list
 * @param head block_t** type address of head of the list to be coalesced.
 * @return tmp_heap largest contiguous block found after coalesce
 */
block_t* MemCoalesceBlocks(block_t* head)
{
    block_t* tmp_heap = head;
    block_t* tmp;

    for (tmp = head; tmp && tmp->next; tmp = tmp->next)
    {
        // if tmp->next is already in use then we need not iterate
        // to tmp = tmp->next, we can jump right away to tmp = tmp->next->next
        if (tmp->next->size & 0x01)
        {
            tmp = tmp->next;
            continue;
        }

        // merge as many blocks as possible
        while(MergeBlocks(tmp, tmp->next));

        if (tmp->size > tmp_heap->size)
        {
            tmp_heap = tmp;
        }
    }
    return tmp_heap;
}

/**
 * Merge any two blocks if they are contiguous
 * @param b1 block_t* type first block
 * @param b2 block_t* type second block
 * return 1 if merged of 0
 */
int MergeBlocks(block_t* b1, block_t* b2)
{
    if ((b2) && !(b2->size & 0x01) && (b1) && !(b1->size & 0x01))
    {
        if (CAST_PTR(b1) + BLOCK_HEADER_SIZE + b1->size == CAST_PTR(b2))
        {
            b1->size += b2->size + BLOCK_HEADER_SIZE;
            b1->next = b2->next;
            return (1);
        }
    }
    return (0);
}

/**
 * Print the current status of the memory pool
 * Prints free_list, allocated_list and heap list
 */
void MemPrintStatus()
{
    block_t* temp;

#ifdef MEM_DEBUG
    printf("Heap\n");
#endif
    for (temp = mem_pool; temp; temp = temp->next)
    {
#ifdef MEM_DEBUG
        printf("\tSize: %d Block Addr: %p NextBlock %p InUse: %d\n", (int) temp->size & ~0x01, temp,
            temp->next, (temp->size & 0x01));
#endif
    }
}

/**
 * Get total memory that is in the free list
 * @return total_mem size_t type total memory that is in the free list, but no coalesced yet
 */
size_t MemGetTotalMemFree()
{
    return (heap->size);
}
