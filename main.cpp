/*
 * Copyright (C) 2015 Anshul Routhu <anshul.m67@gmail.com>
 * All rights reserved.
 */

#include "memmgr.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>

#define POOL_SIZE (1024*80)
char pool[POOL_SIZE];

void test1()
{
    char* ptr[1024];
    char* ptr1[1024];
    char* ptr2[1024];
    char* ptr3[1024];

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
}

void test2()
{
    char* ptr[1024];
    char* ptr1[1024];
    char* ptr2[1024];
    char* ptr3[1024];

    for (int i = 0; i < 5; ++i)
     {
         ptr[i] = (char*) malloc(1024);
         assert(ptr[i]);
         free(ptr[i]);
     }

     for (int i = 0; i < 5; ++i)
     {
         ptr[i] = (char*) malloc(1024);
         assert(ptr[i]);
     }

     for (int i = 0; i < 5; ++i)
     {
         ptr1[i] = (char*) malloc(1024);
         assert(ptr1[i]);
     }

     for (int i = 0; i < 5; ++i)
     {
         free(ptr[i]);
     }

     for (int i = 63; i >= 0; --i)
     {
         ptr2[i] = (char*) malloc(i + 1);
         assert(ptr2[i]);
     }

     for (int i = 0; i < 1000; ++i)
     {
         ptr3[i] = (char*) malloc(rand() % 100 + 1);
         assert(ptr3[i]);
     }

     for (int i = 0; i < 5; ++i)
     {
         free(ptr1[i]);
     }

     for (int i = 63; i >= 0; --i)
     {
         free(ptr2[i]);
     }

     for (int i = 0; i < 1000; ++i)
     {
         free(ptr3[i]);
     }
}

int main(int argc, char* argv[])
{
    struct timeval t_start;
    struct timeval t_end;
    char* ptr2[1024];
    char* ptr3[1024];

    MemInitPool(pool, POOL_SIZE);
    gettimeofday(&t_start, NULL);
    test1();
    gettimeofday(&t_end, NULL);
    printf("MemMgr Test1 took\n");
    printf("\t %lld \n", (t_end.tv_sec - t_start.tv_sec)*1000000LL + t_end.tv_usec - t_start.tv_usec);
    MemDeInitPool();

    MemInitPool(pool, POOL_SIZE);
    gettimeofday(&t_start, NULL);
    test2();
    gettimeofday(&t_end, NULL);
    printf("malloc Test2 took\n");
    printf("\t %lld \n", (t_end.tv_sec - t_start.tv_sec)*1000000LL + t_end.tv_usec - t_start.tv_usec);
    MemDeInitPool();

    MemInitPool(pool, POOL_SIZE);
    gettimeofday(&t_start, NULL);

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

    gettimeofday(&t_end, NULL);
    printf("MemMgr Allocations took\n");
    printf("\t %lld \n", (t_end.tv_sec - t_start.tv_sec)*1000000LL + t_end.tv_usec - t_start.tv_usec);

    gettimeofday(&t_start, NULL);

    for (int i = 63; i >= 0; --i)
    {
        MemFree(ptr2[i]);
    }

    for (int i = 0; i < 1000; ++i)
    {
        MemFree(ptr3[i]);
    }

    gettimeofday(&t_end, NULL);
    printf("MemMgr Free took\n");
    printf("\t %lld \n", (t_end.tv_sec - t_start.tv_sec)*1000000LL + t_end.tv_usec - t_start.tv_usec);

    gettimeofday(&t_start, NULL);

    for (int i = 63; i >= 0; --i)
    {
        ptr2[i] = (char*) malloc(i + 1);
        assert(ptr2[i]);
    }

    for (int i = 0; i < 1000; ++i)
    {
        ptr3[i] = (char*) malloc(rand() % 100 + 1);
        assert(ptr3[i]);
    }

    gettimeofday(&t_end, NULL);
    printf("malloc Allocations took\n");
    printf("\t %lld \n", (t_end.tv_sec - t_start.tv_sec)*1000000LL + t_end.tv_usec - t_start.tv_usec);

    gettimeofday(&t_start, NULL);

    for (int i = 63; i >= 0; --i)
    {
           free(ptr2[i]);
    }

    for (int i = 0; i < 1000; ++i)
    {
        free(ptr3[i]);
    }

    gettimeofday(&t_end, NULL);
    printf("malloc Free took\n");
    printf("\t %lld \n", (t_end.tv_sec - t_start.tv_sec)*1000000LL + t_end.tv_usec - t_start.tv_usec);
    MemDeInitPool();
    return 0;
}
