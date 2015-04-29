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
	free_list = (block_t*)mem;
	free_list->size = size - BLOCK_HEADER_SIZE;
	free_list->next = NULL;
	free_list->prev = NULL;
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
//		block = (block_t*) mem_pool;
//		block->size = size;
//		block->next = NULL;
//		block->prev = NULL;
//		// block is being used
//		block->size |= 0x1;
//		mem_pool += size + BLOCK_HEADER_SIZE;
//		pool_size -= size + BLOCK_HEADER_SIZE;
		printf("Allocation failed\n");
	}

	printf("Allocated %d Block Addr: 0x%x Mem Addr: 0x%x\n", block->size & ~0x1, block, CAST_PTR(block) + BLOCK_HEADER_SIZE);

	return (CAST_PTR(block) + BLOCK_HEADER_SIZE);

}

void Mem_free(void* mem)
{
	block_t* temp;
	block_t* block = (block_t*)(CAST_PTR(mem) - BLOCK_HEADER_SIZE);
	block->size &= ~0x1;;

//	if (!free_list)
//	{
//		free_list = block;
//	}
//	else
	{
//		for (temp = free_list; temp->next; temp = temp->next);
		block->next = free_list;
		free_list->prev = block;
		free_list = block;
	}

}

block_t* FindFit(size_t size)
{
	block_t* block = NULL;
	block_t* block2 = NULL;

	for (block = free_list; block; block = block->next)
	{
		if (block->size >= size)
		{
			break;
		}
	}

	if (block && (block->size > size))
	{
		block2 = (block_t*)(CAST_PTR(block) + size + BLOCK_HEADER_SIZE);
		block2->size = block->size - size - BLOCK_HEADER_SIZE;
		if (block2->size > 0)
		{
			block2->size &=~ 0x1;
			block2->next = block->next;
			block2->prev = block;
			block->next = block2;

			if (block2)
			{
				if (block2->prev)
					block2->prev->next = block->next;
				if (block2->next)
					block2->next->prev = block->prev;

			}
		}
	}

	free_list = block->next;
	block->size = size;
	block->size |= 0x1;
	block->next = NULL;
	block->prev = NULL;
	return (block);

}

void PrintStatus(void* pool)
{
	block_t* temp;
	printf("FreeList\n");

//	if (free_list->next == free_list)
//	{
//		printf("FreeList1\n");
//		printf("\tSize: %d Block Addr: 0x%x Mem Addr: 0x%x Overhead: %d\n", free_list->size & ~0x01, free_list, CAST_PTR(free_list) + BLOCK_HEADER_SIZE, BLOCK_HEADER_SIZE);
//	}
//	else
	{
		for (temp = free_list; temp; temp = temp->next)
		{
			printf("\tSize: %d Block Addr: 0x%x Mem Addr: 0x%x Overhead: %d\n", temp->size & ~0x01, temp, CAST_PTR(temp) + BLOCK_HEADER_SIZE, BLOCK_HEADER_SIZE);
		}
	}

}



