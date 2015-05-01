/*
 * Copyright (C)  Anshul Routhu <anshul.m67@gmail.com>
 * All rights reserved.
 */

#include "memmgr.h"
#include <stdio.h>
#include <assert.h>

#define POOL_SIZE (1024*1024)
char pool[POOL_SIZE];

int main(int argc, char* argv[])
{
    char* ptr[1024];
    printf("Memory Manager\n");
    InitMemPool(pool, POOL_SIZE);
    PrintStatus(pool);

    for (int i = 0; i < 5; ++i)
    {
        ptr[i] = (char*) Mem_malloc(1024);
        assert(ptr[i]);
        PrintStatus(pool);
    }

    printf("Freeing\n");
    for (int i = 0; i < 5; ++i)
    {
        Mem_free(ptr[i]);
        PrintStatus(pool);
    }

    printf("Second iteration \n");

    for (int i = 0; i < 5; ++i)
    {
        ptr[i] = (char*) Mem_malloc(1024);
        assert(ptr[i]);
        PrintStatus(pool);
    }

    printf("Freeing\n");
    for (int i = 0; i < 5; ++i)
    {
        Mem_free(ptr[i]);
        PrintStatus(pool);
    }

    for (int i = 63; i >= 0; --i)
    {
        ptr[i] = (char*) Mem_malloc(i + 1);
        assert(ptr[i]);
        PrintStatus(pool);
    }

    for (int i = 63; i >= 0; --i)
    {
        Mem_free(ptr[i]);
        PrintStatus(pool);
    }

    for (int i = 0; i < 100; ++i)
    {
        ptr[i] = (char*) Mem_malloc(rand() % 100 + 1);
        assert(ptr[i]);
    }

    for (int i = 0; i < 100; ++i)
    {
        Mem_free(ptr[i]);
    }

    PrintStatus(pool);
    CheckPool();
    return 0;
}

