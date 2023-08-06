/* Check for best fit implementation */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main()
{
	assert(Mem_Init(4096,0) == 0);
	int nblocks = 9;
	int sizes[] = { 300, 200, 200, 100, 200, 800, 500, 700, 300 };
	void* ptr[9];
	void* test;

	for (int i=0; i < nblocks; i++)
	{
		ptr[i] = Mem_Alloc(sizes[i]);
		assert(ptr[i] != NULL);
	}

	for (int i=1; i < nblocks; i += 2)
	{
		assert(Mem_Free(ptr[i]) == 0);
	}

	test = Mem_Alloc(50);

	Mem_Dump();

	assert(
			(
				((unsigned long int)test >= (unsigned long int)ptr[3])
				&&
				((unsigned long int)test < (unsigned long int)ptr[4])
			)
			||
			(
				((unsigned long int)test >= (unsigned long int)ptr[3])
				&&
				((unsigned long int)test < (unsigned long int)ptr[2])
			)
		);
	exit(0);
}
