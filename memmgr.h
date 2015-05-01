/*
 * Copyright (C)  Anshul Routhu <anshul.m67@gmail.com>
 * All rights reserved.
 */

#ifndef MEMMGR_H_
#define MEMMGR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

int InitMemPool(void* mem, size_t size);
int DeInitMemPool();
void* Mem_malloc(size_t size);
void Mem_free(void* mem);
void PrintStatus(void* pool);
void CheckPool();

#ifdef __cplusplus
}
#endif

#endif /* MEMMGR_H_ */
