#include <assert.h>
#include <stdlib.h>
#include "mem.h"

int main()
{
	assert(Mem_Init(4096,0) == 0);
	//Mem_Dump():
	void* ptr = Mem_Alloc(8);
	Mem_Dump();
	//assert(ptr != NULL);
	exit(0);
}
