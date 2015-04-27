/*
 * memmgr.c
 *
 *  Created on: Apr 27, 2015
 *      Author: routhu
 */

#include "memmgr.h"


pool_t mem_pool = NULL;
block_t* free_list = NULL;
size_t pool_size;

int InitMemPool(void* mem, size_t size)
{
	mem_pool = mem;
	pool_size = size;
	return (size);
}

int DeInitMemPool()
{

}

void* Mem_malloc(size_t size)
{
	block_t* block = FindFit(size);

	// found an exact fit of size
	if (!block)
	{

		// exact fit is not available, get a block from pool
		block = (block_t*) mem_pool;
		block->size = size;
		block->next = NULL;
		block->prev = NULL;
		// block is being used
		block->size |= 0x1;
		mem_pool += size + BLOCK_HEADER_SIZE;
		pool_size -= size + BLOCK_HEADER_SIZE;
	}

	return (CAST_PTR(block) + BLOCK_HEADER_SIZE);

}

void Mem_free(void* mem)
{
	block_t* temp;
	block_t* block = (block_t*)(CAST_PTR(mem) - BLOCK_HEADER_SIZE);
	block->size &= ~0x1;;

	if (!free_list)
	{
		free_list = block;
	}
	else
	{
		for (temp = free_list; temp->next; temp = temp->next);
		temp->next = block;
		block->prev = temp;
	}

}

block_t* FindFit(size_t size)
{
	return (NULL);

}

void PrintStatus(void* pool)
{
	block_t* temp;
	printf("FreeList\n");
	for (temp = free_list; temp && temp->next; temp = temp->next)
	{
		printf("\tSize: %d Block Addr: 0x%x Mem Addr: 0x%x Overhead: %d\n", temp->size & ~0x01, temp, CAST_PTR(temp) + BLOCK_HEADER_SIZE, BLOCK_HEADER_SIZE);
	}

	printf("\n Pool Size: %d\n", pool_size);

}



