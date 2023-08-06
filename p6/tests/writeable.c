
/* Write to a chunk from Mem_alloc and check the value */
#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main()
{
	assert(Mem_Init(4096,0) == 0);
	int* ptr = (int*) Mem_Alloc(sizeof(int));
	assert(ptr != NULL);
	*ptr = 42;
	assert(*ptr == 42);
	exit(0);
}
