#include <stdio.h>
#include <stdlib.h>

#define DEBUG_KEY 0x000000

int allocatedBytes = 0;
int timesAllocated = 0;

int deallocatedBytes = 0;
int timesDeallocated = 0;

typedef struct {
	int size;
	int key;
	} DEBUGTAG;

void *debugMalloc(int size)
{
	void *p = malloc(sizeof(DEBUGTAG) + size + sizeof(DEBUGTAG));
	DEBUGTAG *tag;
	
	if (p == NULL) return NULL;
	
	timesAllocated++;
	allocatedBytes += size;
	
	tag = (DEBUGTAG *)p;
	tag->size = size;
	tag->key = DEBUG_KEY;
	p = tag+1;
	
	tag = (DEBUGTAG *)((char*)p+size);
	tag->size = size;
	tag->key = DEBUG_KEY;
	
	return p;
}

void debugFree(void *ptr)
{
	DEBUGTAG *tag1 = (DEBUGTAG *)ptr - 1;
	DEBUGTAG *tag2 = (DEBUGTAG *)((char *)ptr + tag1->size);
	
	if ((tag1->size != tag2->size)|| (tag1->key != tag2->key))
	{
		printf("Warning! Memory management corruption, memory used out of bounds.");
	}
	
	timesDeallocated++; 
	deallocatedBytes += tag1->size;
	
	free(tag1);
}

void debugStop()
{
	int percentage = (deallocatedBytes/allocatedBytes)*100;

	printf("Application excecution ended.\n");
	printf("%d bytes allocated in %d alloc's\n", allocatedBytes, timesAllocated);
	printf("%d bytes deallocated in %d free's\n", deallocatedBytes, timesDeallocated);
	printf("--------------------------------------------\n");
	printf("app deallocated %d%% of it's allocated memory\n", percentage );

	if (deallocatedBytes == allocatedBytes)
	{
		printf("Congrats the application released all it's memory\n");
	} else {
		printf("Warning! memory leaks found\n");
	}
}
