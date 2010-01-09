/*
 *  memory_spaces.h
 *  scheme_compiler
 */

#ifndef MEMORY_SPACE_H
#define MEMORY_SPACE_H

#include <stdlib.h>

struct _MemorySpace;

typedef struct _MemorySpace {
	void * start;
	void * current_allocation_position;
	void * end;
	int n_chunks_total;
	int n_chunks_used;
	//int n_allocations_since_last_compaction;
	
	// returns a pointer to the start of the allocated region. NULL if we're full.
	void * (* allocate)(struct _MemorySpace *, int); //= &allocate; // &allocate ?
	void (* destroy)(struct _MemorySpace *); //= &destroy;
	void (* init)(struct _MemorySpace *, int); // = NULL;
	
} MemorySpace;

MemorySpace * make_MemorySpace(int n_chunks);

#endif