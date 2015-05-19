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
block_t* free_list = NULL;
size_t pool_size = 0;

block_t* FindFit(size_t size);
void RemoveFromList(block_t** head, block_t* block);
void InsertToList(block_t** head, block_t* block);
int MergeBlocks(block_t* b1, block_t* b2);

int MemInitPool(void* mem, size_t size)
{
    free_list = (block_t*) mem;
    free_list->size = size - BLOCK_HEADER_SIZE;
    free_list->next = NULL;
    pool_size = size;
    return (size);
}

int MemDeInitPool()
{
    assert(MemCheckPool() == pool_size);
    return (0);
}

void* MemAlloc(size_t size)
{
#ifdef MEM_DEBUG
    printf("Allocation %d %d\n", (int)size, (int)ALIGN(size));
#endif
    block_t* block = FindFit(ALIGN(size));

    if (!block)
    {
        printf("Allocation failed\n");
        return (NULL);
    }

#ifdef MEM_DEBUG
    printf("Allocated %d Block Addr: %p Mem Addr: %p\n", (int) block->size & ~0x1, block,
        (CAST_PTR(block) + BLOCK_HEADER_SIZE));
#endif

    return (CAST_PTR(block) + BLOCK_HEADER_SIZE);

}

block_t* FindFit(size_t size)
{
    block_t* block = NULL;
    block_t* block2 = NULL;

    for (block = free_list; block; block = block->next)
    {
        if (block->size >= MIN_BLOCK_SIZE(size) || block->size == size)
        {
            break;
        }
    }

    if (block)
    {
        if (block->size > MIN_BLOCK_SIZE(size))
        {
            block2 = (block_t*) (CAST_PTR(block) + size + BLOCK_HEADER_SIZE);
            block2->size = block->size - size - BLOCK_HEADER_SIZE;
            block2->size &= ~0x1;
            block2->next = block->next;
            block->next = block2;
            block->size = size;
        }

        block->size |= 0x1;
        RemoveFromList(&free_list, block);
    }

    return (block);
}

void MemFree(void* mem)
{
    assert(mem);
    block_t* block = (block_t*) (CAST_PTR(mem) - BLOCK_HEADER_SIZE);
    block->size &= ~0x1;
    InsertToList(&free_list, block);
}

void RemoveFromList(block_t** head, block_t* block)
{
    assert(head);
    assert(block);

    block_t* tmp;

    // block is the first node
    if (*head == block)
    {
        *head = block->next;
        block->next = NULL;
        return;
    }

    for (tmp = *head; tmp; tmp = tmp->next)
    {
        if (tmp->next == block)
        {
            tmp->next = block->next;
            block->next = NULL;
            break;
        }
    }
}

void InsertToList(block_t** head, block_t* block)
{
    assert(head);
    assert(block);
    assert(block != *head);

    block_t* tmp;

    if (block < *head)
    {
        block->next = *head;
        *head = block;

        MergeBlocks(*head, (*head)->next);
        return;
    }

    for (tmp = *head; tmp; tmp = tmp->next)
    {
        if (block < tmp->next && block > tmp)
        {
            block->next = tmp->next;
            tmp->next = block;

            if (MergeBlocks(tmp, tmp->next))
            {
                MergeBlocks(tmp, tmp->next);
            }
            else
            {
                MergeBlocks(block, block->next);
            }

            break;
        }
    }
}

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

void MemPrintStatus()
{
    block_t* temp;
    printf("FreeList\n");
    for (temp = free_list; temp; temp = temp->next)
    {
        printf("\tSize: %d Block Addr: %p Mem Addr: %p Overhead: %d NextBlock %p\n", (int) temp->size & ~0x01, temp,
            (CAST_PTR(temp) + BLOCK_HEADER_SIZE), (int) BLOCK_HEADER_SIZE, temp->next);
    }
}

size_t MemCheckPool()
{
    block_t* temp;
    size_t total_mem = 0;

    for (temp = free_list; temp; temp = temp->next)
    {
        total_mem += temp->size + BLOCK_HEADER_SIZE;
    }

    printf("Free memory: %d Initialized Memory: %d\n", (int) total_mem, (int) pool_size);
    return total_mem;
}

