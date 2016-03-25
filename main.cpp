/*
 * Copyright (C) 2015 Anshul Routhu <anshul.m67@gmail.com>
 * All rights reserved.
 */

#include "memmgr.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define POOL_SIZE (1024*80)
char pool[POOL_SIZE];

int main(int argc, char* argv[])
{
    char* ptr[1024];
    char* ptr1[1024];
    char* ptr2[1024];
    char* ptr3[1024];

    printf("Memory Manager\n");
    MemInitPool(pool, POOL_SIZE);
    MemPrintStatus();

    for (int i = 0; i < 5; ++i)
    {
        ptr[i] = (char*) MemAlloc(1024);
        assert(ptr[i]);
        MemFree(ptr[i]);
    }

    for (int i = 0; i < 5; ++i)
    {
        ptr[i] = (char*) MemAlloc(1024);
        assert(ptr[i]);
    }

    for (int i = 0; i < 5; ++i)
    {
        ptr1[i] = (char*) MemAlloc(1024);
        assert(ptr1[i]);
    }

    for (int i = 0; i < 5; ++i)
    {
        MemFree(ptr[i]);
    }

    for (int i = 63; i >= 0; --i)
    {
        ptr2[i] = (char*) MemAlloc(i + 1);
        assert(ptr2[i]);
    }

    for (int i = 0; i < 1000; ++i)
    {
        ptr3[i] = (char*) MemAlloc(rand() % 100 + 1);
        assert(ptr3[i]);
    }

    for (int i = 0; i < 5; ++i)
    {
        MemFree(ptr1[i]);
    }

    for (int i = 63; i >= 0; --i)
    {
        MemFree(ptr2[i]);
    }

    for (int i = 0; i < 1000; ++i)
    {
        MemFree(ptr3[i]);
    }

    MemPrintStatus();
    MemDeInitPool();

    return 0;
}

