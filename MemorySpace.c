/*
 *  memory_spaces.c
 *  scheme_compiler
 *
 */

#include "MemorySpace.h"

/* 	Signaling the memory allocation errors clearly doesn't really appear to be
 	something we'd like to do only while debugging. Maybe it should rethink this.	
 */
#ifdef DEBUG_MEMORYSPACE
#include <stdio.h>
#endif

void * allocate(struct _MemorySpace * this, int n_chunks) {
	// in case we don't have enough space to return
	if (this->current_allocation_position + n_chunks > this->end) {
		#ifdef DEBUG_MEMORYSPACE
		printf("We ran out of memory to allocate.\n");
		printf("total : %d, used : %d\n", this->n_chunks_total, this->n_chunks_used);
		#endif
		return NULL;
	} else {
		void * start_of_segment = this->current_allocation_position;
		this->current_allocation_position += n_chunks;
		this->n_chunks_used += n_chunks;
		return start_of_segment;
	}
}

void init(struct _MemorySpace * this, int n_chunks_total) {
	// assuming it all works fine
	this->start = malloc(n_chunks_total);
	this->end = this->start + n_chunks_total;
	this->current_allocation_position = this->start;
	this->n_chunks_total = n_chunks_total;
	this->n_chunks_used = 0;
}

void destroy(struct _MemorySpace * this) {
	free(this->start);	
	this->start = NULL;
	this->end = NULL;
	this->current_allocation_position = NULL;
	this->n_chunks_total = 0;
	this->n_chunks_used = 0;
}

MemorySpace * make_MemorySpace(int n_chunks) {
	MemorySpace * myspace = malloc(sizeof(MemorySpace));
	/*	We're using NULL at many places to indicate that a pointer
		hasn't been allocated memory successfully. This means that we
	 	can't have the start of the memory space be at NULL by accident.
	 	We're not supposed to be able to get NULL by a regular malloc,
	 	though, so it's probably all fine.
	 */
	myspace->allocate = &allocate;
	myspace->destroy = &destroy;
	myspace->init = &init;
	myspace->init(myspace, n_chunks);
	return myspace;
}

#ifdef TEST_MEMORYSPACE_ALONE
/* A little demo. Comment out if not used. */
int main() {
	MemorySpace * ms = make_MemorySpace(100);

	int * A;
	
	A = ms->allocate(ms, sizeof(int));
	*A = 0;
	A = ms->allocate(ms, sizeof(int));
	*A = 1;
	A = ms->allocate(ms, sizeof(int));
	*A = 2;
	
	ms->destroy(ms);

	return 0;
}
#endif