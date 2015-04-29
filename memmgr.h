/*
 * memmgr.h
 *
 *  Created on: Apr 27, 2015
 *      Author: routhu
 */

#ifndef MEMMGR_H_
#define MEMMGR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// alignment 4 or 8
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & (~0x7))

#define BLOCK_HEADER_SIZE ALIGN(sizeof(block_t))
#define CAST_PTR(ptr) ((char*)ptr)

typedef char* pool_t;
typedef struct mem_header_t mempool_t;
typedef struct block_header_t block_t;

struct block_header_t
{
	block_t* next;
	block_t* prev;
	size_t size;
};

struct mem_header_t
{
	size_t size;
	block_t tail;
	block_t head;

};


int InitMemPool(void* mem, size_t size);
int DeInitMemPool();
void* Mem_malloc(size_t size);
void Mem_free(void* mem);
block_t* FindFit(size_t size);
void PrintStatus(void* pool);

#ifdef __cplusplus
}
#endif

#endif /* MEMMGR_H_ */
