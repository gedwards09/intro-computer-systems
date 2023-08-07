#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

#define MinSize 4
#define BestFit 0
#define FirstFit 1
#define WorstFit 2

int fit;
void split(void* bhp, int size);
void coalesce(void* blk);

/* struct header for each block */
typedef struct block_hd
{
	/* 
	 * Address of next block in memory
	 * Blocks ordered in increasing order of address
	 */
	struct block_hd* next;

	/* 
	 * Size and status of the block in memory
     * Size is always a multiple of 4, i.e. last two bits are always zero
	 * LSB used to store status of the memory block
	 *     LSB = 0 => free block
	 *     LSB = 1 => allocated/busy block
	 * Size does not include the space required to store the header
	 */
	int size_status;
} block_header;

/* Global variable pointing to first (lowest address) block */
block_header* list_head = NULL;

/*
 * Function used to Initialize the memory allocator
 * Not intended to be called more than once by a program
 * @param sizeOfRegion The size of the chunk which needs to be allocated
 * @param policy indicates the policy to use e.g. best fir is 0
 * @return 0 on success and -1 on failure
 */
int Mem_Init(int sizeOfRegion, int policy)
{
	int pagesize;
	int padsize;
	int fd;
	int alloc_size;
	void* space_ptr;
	static int allocated_once = 0;

	if (0 != allocated_once)
	{
		fprintf(stderr, "Error:mem.c: Mem_Init has allocated space during "
			"a previous call\n");
		return -1;
	}

	// Get pagesize
	pagesize = getpagesize();

	// Calculate padsize as the padding required to round up sizeOfRegion to 
	// a multiple of pagesize
	padsize = sizeOfRegion % pagesize;
	padsize = (pagesize - padsize) % pagesize;

	alloc_size = sizeOfRegion + padsize;
	
	// Using mmap to allocate memory
	fd = open("/dev/zero", O_RDWR);
	if (-1 == fd )
	{
		fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
		return -1;
	}
	space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if(MAP_FAILED == space_ptr)
	{
		fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
		return -1; 
	}
	
	allocated_once = 1;

	//To begin with there is one big, free block
	list_head = (block_header*) space_ptr;
	list_head->next = NULL;
	//'size' stored in block size excludes the space for the header
	list_head->size_status = alloc_size - (int)sizeof(block_header);
	fit = policy;
	return 0;
}

/*
 * Function for allocating 'size' bytes.
 * @param size Size of block to allocate
 * @returns address of allocated block on success, NULL on failure
 */
void* Mem_Alloc(int size)
{
	//Round up to multiple of 4
	if (size % 4)
	{
		size = (size + 3) >> 2 << 2;
	}

	block_header* ptr = NULL;
	block_header* current = list_head;
	int fitCondition;
	
	while (current != NULL)
	{
		if ((current->size_status & 1) == 0 && current->size_status >= size)
		{
			fitCondition = fit == FirstFit;
			fitCondition = fit == BestFit
				? ptr == NULL || ptr->size_status > current->size_status
				: fitCondition;
			fitCondition = fit == WorstFit
				? ptr == NULL || ptr->size_status < current->size_status
				: fitCondition;
			if (fitCondition)
			{
				ptr = current;
				if (fit == FirstFit)
				{
					break;
				}
			}
		}
		current = current->next;
	}


	if (ptr)
	{
		split(ptr, size);
		//set LSB to allocated
		ptr->size_status |= 1;
	}

	return (void*)ptr;
}

/*
 * Helper function to split a block based on size, if necessary
 * @param bhp pointer to free block to split
 * @param size size of the block to create
 */
void split(void* ptr, int size)
{
	block_header* bhp = (block_header*)ptr;
	//not enough size to split
	if (bhp->size_status < size + (int)sizeof(block_header) + MinSize)
	{
		return;
	}

	ptr += (int)sizeof(block_header) + size;
	block_header* new = (block_header*) ptr;
	
	new->size_status = bhp->size_status - size - (int)sizeof(block_header);
	bhp->size_status = size;

	new->next = bhp->next;
	bhp->next = new;
}

/*
 * Function for freeing up previously allocated block
 * @param ptr Address of the block to be freed
 * @returns 0 on success, -1 on failure
 */
int Mem_Free(void* ptr)
{
	if (ptr == NULL)
	{
		return -1;
	}

	block_header* blk = (block_header*)ptr;

	//find previous block and check that blk was allocated by Mem_Alloc
	block_header* current = list_head;
	block_header* prev = NULL;
	while (current != blk && current->next != NULL)
	{
		prev = current;
		current = current->next;
	}

	if (current != blk)
	{
		return -1;
	}

	//free block
	blk->size_status &= ~1;
	//coalesce with previous and next block if able
	coalesce(blk);
	coalesce(prev);
	return 0;
}

/*
 * Helper function to coalesce this block with the next
 * No action if either block is occupied.
 * @param ptr Pointer to the block to coalesce with the next
 */
void coalesce(void* ptr)
{
	if (ptr == NULL)
	{
		return;
	}

	block_header* blk = (block_header*)ptr;
	if (blk->size_status & 1 
		|| blk->next == NULL || blk->next->size_status & 1)
	{
		return;
	}

	blk->size_status += blk->next->size_status + (int)sizeof(block_header);
	blk->next = blk->next->next;
}

/*
 * Prints out a list of all the blocks. Used for debug.
 * No.		: Serial number of the block
 * Status	: free/busy
 * Begin	: Address of the first useful byte in the block
 * End		: Address of the last byte in the block
 * Size		: Size of the block (excluding the header)
 * t_Size	: Size of the block (including the header)
 * t_Begin	: Address of the first byte in the block (where header starts)
 */
void Mem_Dump()
{
	int counter;
	block_header* current = NULL;
	char* t_Begin = NULL;
	char* Begin = NULL;
	int Size;
	int t_Size;
	char* End = NULL;
	int free_size;
	int busy_size;
	int total_size;
	char status[5];

	free_size = 0;
	busy_size = 0;
	total_size = 0;
	current = list_head;
	counter = 1;
	fprintf(stdout, "**********************************Block list***********************************\n");
	fprintf(stdout, "No.\tStatus\tBegin\t\tEnd\t\tSize\tt_Size\tt_Begin\n");
	fprintf(stdout, "-------------------------------------------------------------------------------\n");
	while (NULL != current)
	{
		t_Begin = (char*)current;
		Begin = t_Begin + (int)sizeof(block_header);
		Size = current->size_status;
		strcpy(status, "Free");
		if (Size & 1)
		{
			strcpy(status, "Busy");
			//Minus one for ignoring status in busy block
			Size = Size - 1;
			t_Size = Size + (int)sizeof(block_header);
			busy_size = busy_size + t_Size;
		}
		else
		{
			t_Size = Size + (int)sizeof(block_header);
			free_size = free_size + t_Size;
		}
		End = Begin + Size;
	fprintf(stdout, "%d\t%s\t0x%08lx\t0x%08lx\t%d\t%d\t0x%08lx\n", counter, status,
			(unsigned long int)Begin, (unsigned long int)End, Size, t_Size, 
			(unsigned long int)t_Begin);
		total_size = total_size + t_Size;
		current = current->next;
		counter = counter + 1;
	}
	fprintf(stdout, "-------------------------------------------------------------------------------\n");
	fprintf(stdout, "*******************************************************************************\n");
	fprintf(stdout, "Total busy size = %d\n", busy_size);
	fprintf(stdout, "Total free size = %d\n", free_size);
	fprintf(stdout, "Total size = %d\n", busy_size + free_size);
	fprintf(stdout, "*******************************************************************************\n");
	fflush(stdout);
	return;
}

