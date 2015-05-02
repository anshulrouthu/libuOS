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

int MemInitPool(void* mem, size_t size);
int MemDeInitPool();
void* MemAlloc(size_t size);
void MemFree(void* mem);
void MemPrintStatus();
size_t MemCheckPool();

#ifdef __cplusplus
}
#endif

#endif /* MEMMGR_H_ */
