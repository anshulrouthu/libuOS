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
block_t* heap = NULL; // allocate the requested memory from heap
size_t pool_size = 0; // track the total size of the memory initialized
block_t* free_list = NULL; // for tracking freed blocks
block_t* alloc_list = NULL; // for tracking allocated blocks

// local functions for managing memory pool
block_t* FindFit(size_t size);
int RemoveFromList(block_t** head, block_t* block);
int InsertToList(block_t** head, block_t* block);
int MergeBlocks(block_t* b1, block_t* b2);
block_t* MemCoalesceBlocks(block_t** head);

/**
 * Initialize the memory manager with the specified size
 */
int MemInitPool(void* mem, size_t size)
{
    heap = (block_t*) mem;
    heap->size = size - BLOCK_HEADER_SIZE;
    heap->next = NULL;
    pool_size = size;
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
    block_t* tmp = NULL;
    MemCoalesceBlocks(&free_list);

    while (free_list)
    {
        tmp = free_list;
        RemoveFromList(&free_list, tmp);
        InsertToList(&heap, tmp);
    }
    assert(MemGetTotalMemFree() == pool_size);
#endif
    free_list = NULL;
    alloc_list = NULL;
    heap = NULL;
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
    block_t* block = FindFit(ALIGN(size));

    if (!block)
    {
#ifdef MEM_DEBUG
        printf("Allocation failed\n");
#endif
        return (NULL);
    }

#ifdef MEM_DEBUG
    printf("Allocated %d Block Addr: %p Mem Addr: %p\n", (int) block->size & ~0x1, block,
        (CAST_PTR(block) + BLOCK_HEADER_SIZE));
#endif

    InsertToList(&alloc_list, block);
    return (CAST_PTR(block) + BLOCK_HEADER_SIZE);

}

/**
 * Find a block that will fit the size, if there is no memory left in the heap
 * this function will try to coalesce the free_list and insert the largest block after coalesce in
 * to the heap and find the fit from the heap
 * @param size size_t type size of the block requested
 * @return block block_t* type a block that is found as fit
 */
block_t* FindFit(size_t size)
{
    block_t* block = NULL;
    block_t* block2 = NULL;
    block_t* tmp_heap;

    if (!heap || heap->size < size)
    {
        // ran out of heap space, get some from free_list is there is some
        tmp_heap = MemCoalesceBlocks(&free_list);
        if (tmp_heap)
        {
            RemoveFromList(&free_list, tmp_heap);
            InsertToList(&heap, tmp_heap);
        }
    }

    for (block = heap; block; block = block->next)
    {
        if (block->size >= MIN_BLOCK_SIZE(size) || block->size == size)
        {
            break;
        }
    }

    if (block)
    {
        // we do not want to leave the size of 16 bytes in the heap which would be taken up by the
        // block header and the size of this block would be 0, return the entire block with remaining size at this point
        if (block->size > MIN_BLOCK_SIZE(size) + BLOCK_HEADER_SIZE)
        {
            block2 = (block_t*) (CAST_PTR(block) + size + BLOCK_HEADER_SIZE);
            block2->size = block->size - size - BLOCK_HEADER_SIZE;
            block2->size &= ~0x1;
            block2->next = block->next;
            block->next = block2;
            block->size = size;
        }

        block->size |= 0x1;

        RemoveFromList(&heap, block);
    }

    return (block);
}

/**
 * Free the memory
 * @param mem void* type the memory address to be freed
 */
void MemFree(void* mem)
{
#ifdef MEM_DEBUG
    assert(mem);
#endif

    block_t* block = (block_t*) (CAST_PTR(mem) - BLOCK_HEADER_SIZE);
    assert(block);

    block->size &= ~0x1;
    RemoveFromList(&alloc_list, block);
    InsertToList(&free_list, block);
    assert(heap != heap->next);
}

/**
 * Remove a block of memory in to a list
 * @param head block_t** type address for the head of the list
 * @param block block_t* type block to be removed
 * @return 1 if successful or 0
 */
int RemoveFromList(block_t** head, block_t* block)
{
#ifdef MEM_DEBUG
    assert(head);
    assert(block);
#endif

    block_t* tmp = NULL;

    // block is the head node
    if (*head == block)
    {
        *head = block->next;
        block->next = NULL;
        return (1);
    }

    for (tmp = *head; tmp; tmp = tmp->next)
    {
        if (tmp->next == block)
        {
            tmp->next = block->next;
            block->next = NULL;
            return (1);
        }
    }

    return (0);
}

/**
 * Insert a block of memory in to a list
 * @param head block_t** type address for the head of the list
 * @param block block_t* type block to be inserted
 * @return 1 if successful or 0
 */
int InsertToList(block_t** head, block_t* block)
{
#ifdef MEM_DEBUG
    assert(block);
    assert(block != *head);
#endif

    block_t* tmp = NULL;

    if (!(*head))
    {
        *head = block;
        return (1);
    }

    if (block < *head)
    {
        block->next = *head;
        *head = block;
        return (1);
    }

    /* TODO: this code could be further optimized for performance
     * Rather that checking where the current block fits, we can used another pointer block_t* cnext
     * which will actually represent the next contiguous block, which is always sorted by address and no need to
     * check the sorting order here, instead we can simple insert the block to the beginning of the list using
     * next pointer at the head of this list
     * The down side could be this will add another 8 bytes to the header size
     */
    for (tmp = *head; tmp; tmp = tmp->next)
    {
        if ((block < tmp->next && block > tmp) || (block > tmp && !tmp->next))
        {
            block->next = tmp->next;
            tmp->next = block;
            return (1);
        }
    }

    return (0);
}

/**
 * Coalesce all the blocks in the free_list and return the new best heap to be used
 * the new heap is the first largest block found after coalescing the free_list
 * @param head block_t** type address of head of the list to be coalesced.
 * @return tmp_heap largest contiguous block found after coalesce
 */
block_t* MemCoalesceBlocks(block_t** head)
{
    block_t* tmp_heap = *head;
    block_t* tmp;

    for (tmp = *head; tmp && tmp->next; tmp = tmp->next)
    {
        // check if they are not being used
        assert(!(tmp->size & 0x01) && !(tmp->next->size & 0x01) );

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
    if (CAST_PTR(b1) + BLOCK_HEADER_SIZE + b1->size == CAST_PTR(b2))
    {
        b1->size += b2->size + BLOCK_HEADER_SIZE;
        b1->next = b2->next;
        return (1);
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
    printf("FreeList\n");
#endif
    for (temp = free_list; temp; temp = temp->next)
    {
#ifdef MEM_DEBUG
        printf("\tSize: %d Block Addr: %p Mem Addr: %p Overhead: %d NextBlock %p\n", (int) temp->size & ~0x01, temp,
            (CAST_PTR(temp) + BLOCK_HEADER_SIZE), (int) BLOCK_HEADER_SIZE, temp->next);
#endif
    }

#ifdef MEM_DEBUG
    printf("AllocList\n");
#endif
    for (temp = alloc_list; temp; temp = temp->next)
    {
#ifdef MEM_DEBUG
        printf("\tSize: %d Block Addr: %p Mem Addr: %p Overhead: %d NextBlock %p\n", (int) temp->size & ~0x01, temp,
            (CAST_PTR(temp) + BLOCK_HEADER_SIZE), (int) BLOCK_HEADER_SIZE, temp->next);
#endif
    }

#ifdef MEM_DEBUG
    printf("Heap\n");
#endif
    for (temp = heap; temp; temp = temp->next)
    {
#ifdef MEM_DEBUG
        printf("\tSize: %d Block Addr: %p Mem Addr: %p Overhead: %d NextBlock %p\n", (int) temp->size & ~0x01, temp,
            (CAST_PTR(temp) + BLOCK_HEADER_SIZE), (int) BLOCK_HEADER_SIZE, temp->next);
#endif
    }
}

/**
 * Get the total memory in the alloc_list
 * @return total_mem size_t type total memory that is being allocated
 */
size_t MemGetTotalMemAlloc()
{
    block_t* temp;
    size_t total_mem = 0;

    for (temp = alloc_list; temp; temp = temp->next)
    {
        total_mem += temp->size + BLOCK_HEADER_SIZE;
    }

    return total_mem;
}

/**
 * Get total memory that is in the free list
 * @return total_mem size_t type total memory that is in the free list, but no coalesced yet
 */
size_t MemGetTotalMemFree()
{
    block_t* temp;
    size_t total_mem = 0;

    for (temp = free_list; temp; temp = temp->next)
    {
        total_mem += temp->size + BLOCK_HEADER_SIZE;
    }

    for (temp = heap; temp; temp = temp->next)
    {
        total_mem += temp->size + BLOCK_HEADER_SIZE;
    }

#ifdef MEM_DEBUG
    printf("Free memory: %d Initialized Memory: %d\n", (int) total_mem, (int) pool_size);
#endif
    return total_mem;
}

/**
 * Get total number of freed blocks since the last coalesce
 * @return count number of blocks in the free list
 */
size_t MemGetTotalFreeCount()
{
    block_t* temp;
    size_t count = 0;

    for (temp = free_list; temp; temp = temp->next)
    {
        count++;
    }

    return count;
}

/**
 * Get total number of allocated blocks since the last coalesce
 * @return count number of blocks in the free list
 */
size_t MemGetTotalAllocCount()
{
    block_t* temp;
    size_t count = 0;

    for (temp = alloc_list; temp; temp = temp->next)
    {
        count++;
    }

    return count;
}
