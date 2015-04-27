/*
 * main.cpp
 *
 *  Created on: Apr 27, 2015
 *      Author: routhu
 */

#include "memmgr.h"
#include <stdio.h>

#define POOL_SIZE (8*1024)
char pool[POOL_SIZE];

int main(int argc, char* argv[])
{
	char* ptr[100];
	printf("Memmgr\n");
	InitMemPool(pool, POOL_SIZE);
	PrintStatus(pool);

	for (int i = 0; i < 5; ++i)
	{
		ptr[i] = (char*)Mem_malloc(1024);
		PrintStatus(pool);
	}

	printf("Freeing\n");
	for (int i = 0; i < 5; ++i)
	{
		Mem_free(ptr[i]);
		PrintStatus(pool);
	}


	return 0;
}

