/*
 *  garbage_collection.c
 *  scheme_compiler
 */

#include "SchemeObject.h"

SchemeObject * SchemeObject_exec_get_output(SchemeObject * E) {
	if (SchemeObject_is_exec_apply(E))
		return E->data.val_apply.output;
	else if (SchemeObject_is_exec_eval(E))
		return E->data.val_eval.output;
	else if (SchemeObject_is_exec_evalseq(E))
		return E->data.val_eval.output;
	else
		return NULL;
}

SchemeObject *** GarbageCollection_get_descendants_origins(SchemeObject * E) {
	/*
	 	Get all the elements that E uses to point to other locations.
	 	The return argument will need to be freed from the function calling
	 	this one. It's possible that the value of those descendants be NULL.
	 	This is alright. I had some idea about skipping those, but
	 	I think this current design is better.
	 */
	
	SchemeObject *** result;
	
	//printf("sizeof(SchemeObject **)) = %ld\n", sizeof(SchemeObject **));
	
	if (E == NULL) {
		return NULL;
	} else if (	SchemeObject_is_integer(E) 	||
				SchemeObject_is_string(E) 	||
				SchemeObject_is_double(E) 	||
				SchemeObject_is_empty(E) 	||
				SchemeObject_is_string(E)	||
				SchemeObject_is_symbol(E) 	||
			   	SchemeObject_is_char(E)) {
		return NULL;
	} else if (SchemeObject_is_pair(E)) {
		result = malloc(3 * sizeof(SchemeObject **));
		result[0] = & (E->data.val_pair.car);
		result[1] = & (E->data.val_pair.cdr);
		result[2] = NULL;
		return result;
	} else if (SchemeObject_is_exec_apply(E) ) {
		result = malloc(5 * sizeof(SchemeObject **));
		result[0] = & (E->data.val_apply.func);
		result[1] = & (E->data.val_apply.resolved_args);
		result[2] = & (E->data.val_apply.output);
		result[3] = & (E->data.val_apply.continuation);
		result[4] = NULL;
		return result;
	} else if (SchemeObject_is_exec_eval(E) || SchemeObject_is_exec_evalseq(E)) {
		result = malloc(5 * sizeof(SchemeObject **));
		result[0] = & (E->data.val_eval.expr);
		result[1] = & (E->data.val_eval.env);
		result[2] = & (E->data.val_eval.output);
		result[3] = & (E->data.val_eval.continuation);
		result[4] = NULL;
		return result;
	} else if (SchemeObject_is_capturedcontinuation(E) ) {
		result = malloc(3 * sizeof(SchemeObject **));
		result[0] = & (E->data.val_capturedcontinuation.output);
		result[1] = & (E->data.val_capturedcontinuation.continuation);
		result[2] = NULL;
		#ifdef DEBUG_CALLCC
		printf("GC found a captured continuation, output = %p, continuation = %p\n", result[0], result[1]);
		#endif
		return result;
	} else if (SchemeObject_is_atomic_function(E) ) {
		return NULL;
	} else if (SchemeObject_is_composite_function(E) ) {
		result = malloc(4 * sizeof(SchemeObject **));
		result[0] = & (E->data.val_lambda.arg_symbols);
		result[1] = & (E->data.val_lambda.body);
		result[2] = & (E->data.val_lambda.enclosed_env);
		result[3] = NULL;
		return result;
	} else if (SchemeObject_is_special_symbol(E)) {
		return NULL;
	} else
		printf("Garbage collector couldn't match the type of element fed to GarbageCollection_get_descendants_origins(...)\n");
	
	return NULL;
}

SchemeObject ** GarbageCollection_get_descendants(SchemeObject * E, int * count_output) {
	SchemeObject *** origins = GarbageCollection_get_descendants_origins(E);

	int count = 0;
	if (origins == NULL) {
		if (count_output != NULL)
			*count_output = count;
		return NULL;
	}
	
	SchemeObject *** ptr = origins;
	while(*(ptr++) != NULL)
		count++;
	
	SchemeObject ** results = calloc(count+1, sizeof(SchemeObject *));
	ptr = origins;
	int i;
	/* 	This will sometimes put NULL in the result, but it'll never dereference the null pointer. */
	for(i = 0; i < count; ++i)
		results[i] = *(origins[i]);
	
	results[count] = NULL;
	/* the optional output argument */
	if (count_output != NULL)
		*count_output = count;
	
	return results;
}

void GarbageCollection_first_step_shallow_copy(SchemeObject * dest, SchemeObject * source) {
	*dest = *source;
	dest->garbage_collection_redirection_address = source;
	source->garbage_collection_redirection_address = dest;
}

int GarbageCollection_has_already_been_moved(SchemeObject * source) {
	return (source->garbage_collection_redirection_address != NULL);
}

void GarbageCollection_reset_redirection_address(SchemeObject * E) {
	E->garbage_collection_redirection_address = NULL;
	return;
}

SchemeObject * GarbageCollection_get_redirection_address(SchemeObject * E) {
	return (E->garbage_collection_redirection_address);
}


SchemeObject * GarbageCollection_floodfill_move_to_new_MemorySpace(MemorySpace * new_ms, SchemeObject * start) {
	/*
	 	Starts the floodfill from the object "start".
	 	The details of this are found in my notes, 2010 jan 13.
	 */

	/*
		The loop invariant is chosen to be such that all the quantities to be expanded
		have already been copied to the new memory space. The act of "expanding" finds
	 	which quantities are referred to by that variable. We copy those that have not been
	 	already copied, and then move the "next_to_expand" up by one in the memory space
	 	(because we assume that only those blocks are found at first).
	 
	 	We are assuming that the "garbage_collection_redirection_address" component of
	 	the original SchemeObjects will be NULL at first. This is one of the things
	 	that we will have to check throughout the code. Lots of potential bugs when I create
	 	structures on the fly without calling the "SchemeObject_make..." functions.
	 */
	
	SchemeObject ** descendants;
	SchemeObject * next_to_expand;
	SchemeObject * next_to_copy;
	SchemeObject * newly_allocated;
	
	SchemeObject * start_of_newly_allocated_SchemeObject_zone = new_ms->current_allocation_position;
	SchemeObject * end_of_newly_allocated_SchemeObject_zone;
	int i, count, count_reallocated;
	
	#ifdef DEBUG_GC
		printf("Setting up first element.\n");
	#endif
	
	newly_allocated = new_ms->allocate(new_ms, sizeof(SchemeObject));
	GarbageCollection_first_step_shallow_copy(newly_allocated, start);
	next_to_expand = newly_allocated;
	
	/* 	This is a weird way to say that we're increasing the "next_to_expand"
		until we reach the point where nothing is allocated yet. Since we add
	 	every element once (and once only), we're doing our flood-fill that way.
	 */
	assert(sizeof(long long) >= sizeof(void *));
	
	#ifdef DEBUG_GC
		printf("Before expansion loop.\n");
		printf("newly_allocated = %p\n", newly_allocated);
		printf("next_to_expand = %p\n", next_to_expand);
		printf("new_ms->current_allocation_position = %p\n", (new_ms->current_allocation_position));
	#endif
	/* 	I don't really like the idea of having to convert my pointers to long long before comparing them,
	 but the compiler likes it even less if I don't do it. At least I have an assert statement to make
	 sure that the long long variables are enough to store the pointer values. It'd be bad to have pointers
	 written in 64 bits getting squished into 32 bit variables before being compared.
	 */
	while( (long long)next_to_expand < (long long)(new_ms->current_allocation_position) ) {

		//printf("Getting descendants of ");
		//SchemeObject_print_details(next_to_expand);
		//printf("\n");
		descendants = GarbageCollection_get_descendants(next_to_expand, & count);
		count_reallocated = 0;
		//printf("Iterating over descendants.\n");

		if (descendants != NULL) {
			for(i=0; i < count; ++i) {
				#ifdef DEBUG_GC
				printf("descendant[%d] = %p\n", i, descendants[i]);
				#endif
				next_to_copy = descendants[i];
				if (next_to_copy != NULL) {
					if (!GarbageCollection_has_already_been_moved(next_to_copy)) {
						newly_allocated = new_ms->allocate(new_ms, sizeof(SchemeObject));
						GarbageCollection_first_step_shallow_copy(newly_allocated, next_to_copy);
						count_reallocated++;
						#ifdef DEBUG_GC
						printf("Copied element from %p in old memory space to %p in new memory space.\n", next_to_copy, newly_allocated);
						#endif
					} else {
						#ifdef DEBUG_GC
						printf("Element at %p was already moved into the new memory space.\n", next_to_copy);
						#endif
					}
				} else {
					#ifdef DEBUG_GC
					printf("descendants[%d] is NULL\n", i);
					#endif
				}
			}
			#ifdef DEBUG_GC
			printf("Attempt to add %d elements to GC. Reallocated %d . Now at %p of %p.\n", count, count_reallocated, next_to_expand, (new_ms->current_allocation_position));
			#endif
			free(descendants);
		}
		
		/* 	Assuming regularity of MemorySpace allocations.
		 	Allocating strings in between the SchemeObjects would be ruinous.
		 */
		next_to_expand = next_to_expand + 1;
	}

	#ifdef DEBUG_GC
	printf("Done with expansion.\n");
	#endif
	
	end_of_newly_allocated_SchemeObject_zone = new_ms->current_allocation_position;
	/*	
	 	Now we have the following property :
	 		Everything between these two pointer values
				start_of_newly_allocated_SchemeObject_zone
				end_of_newly_allocated_SchemeObject_zone
	 		is only SchemeObjects, lined one after the other, and these
	 		objects are everything that will be needed to form the basic
	 		of the compressed representation with the exception of strings/symbols/memory_chunks.
	 		
	 		We will call GarbageCollection_reset_redirection_address( E ) on all the E fitting
	 		between those two values.
	 */
	
	SchemeObject *** descendants_origins_E;
	SchemeObject ** descendants_F;
	SchemeObject * E; 	SchemeObject * F;
	SchemeObject * moved_to;
	int countF = 0;
	#ifdef DEBUG_GC
	printf("Will analyze blocks copied blocks from %p to %p.\n", start_of_newly_allocated_SchemeObject_zone, end_of_newly_allocated_SchemeObject_zone);
	#endif
	for(E = start_of_newly_allocated_SchemeObject_zone; E < end_of_newly_allocated_SchemeObject_zone; E++) {
		/* 	Same ordering as GarbageCollection_get_descendants(E), but get the pointers to the fields
		 	of the SchemeObjects so we can replace the relevant fields easily.
		 */
		
		/* 	Now that all the basic blocks are laid out, we can copy the memory chunks without worries.
		 	Assuming that the new memory space isn't smaller than the previous one, we necessarily have enough
		 	room (even if nothing was freed during garbage collection).
		 */
		
		F = GarbageCollection_get_redirection_address(E);
		assert(E->type == F->type);
		#ifdef DEBUG_GC
		//printf("analyzing moved block at position %p, ", E);
		//printf("moved from %p\n", F);
		#endif
		/* no need to copy the data size since it was copied with the rest during the shallow copy */
		if (SchemeObject_is_string(F) || SchemeObject_is_symbol(F) || SchemeObject_is_memorychunk(F)) {
			E->data.val_memorychunk.data = new_ms->allocate(new_ms, F->data.val_memorychunk.size);
			assert(E->data.val_memorychunk.data != NULL);
			memcpy(E->data.val_memorychunk.data, F->data.val_memorychunk.data, F->data.val_memorychunk.size);
		}
		
		countF = 0;
		descendants_origins_E = GarbageCollection_get_descendants_origins(E);
		descendants_F = GarbageCollection_get_descendants(F, &countF);
		
		if ((descendants_F != NULL) && (descendants_origins_E != NULL)) {
			for(i=0; i< countF; i++) {
				if (descendants_F[i] == NULL)
					*(descendants_origins_E[i]) = NULL; // should be NULL already by construction
				else {
					/* 	moved_to will now contain the addresses in the new memory space
					 	corresponding to the ordered descendants of F */
					moved_to = GarbageCollection_get_redirection_address(descendants_F[i]);					
					/*	It can't be NULL, because only pointers pointing to NULL can be moved to the new
						memory space and be pointing to NULL.
					 */
					assert(moved_to != NULL);
					*(descendants_origins_E[i]) = moved_to;
				}
			}
			#ifdef DEBUG_GC
			int countE = 0;
			SchemeObject ** descendants_E = GarbageCollection_get_descendants(E, &countE);
			assert(countE == countF);	
			for(i=0; i< countF; i++) {
				if (descendants_F[i] != NULL) {
					assert(descendants_F[i] == GarbageCollection_get_redirection_address(descendants_E[i]));
					assert(descendants_E[i] == GarbageCollection_get_redirection_address(descendants_F[i]));
				} else
					assert(descendants_E[i] == NULL);
			}
			#endif
			
			free(descendants_origins_E);
			free(descendants_F);
		} else {
			/* If one is NULL, the other should be NULL too. Otherwise, it's a symptom of an error. */
			assert(!(descendants_F || descendants_origins_E));
		}
	}
	
	for(E = start_of_newly_allocated_SchemeObject_zone; E < end_of_newly_allocated_SchemeObject_zone; E++) {
		GarbageCollection_reset_redirection_address(E);
	}
	
	return start_of_newly_allocated_SchemeObject_zone;
}

void free_if_nonnull(void * A) {
	if (A != NULL)
		free(A);
	return;
}

/*	Blows up the stack with recursion if we don't use a maximal depth.
 	I got to a point where gdb could fetch frame 100 000.
 	This function is mostly used for debugging faster, but not really meant as
 	a part of the final version.
 */
int GarbageCollector_stackabuse_checkisomorphism(SchemeObject * A, SchemeObject * B, int currentdepth, int maxdepth) {
	
	/* don't look too deep or you'll blow up the stack */
	if (currentdepth > maxdepth)
		return 1;
	
	if ((A == NULL) || (B == NULL)) {
		int result = ((A == NULL) && (B == NULL));
		if (result == 0) {
			printf("different elements, one of them being NULL :\n");
			SchemeObject_print(A); printf("\n");
			SchemeObject_print(B); printf("\n");
		}
		return result;
	} else if ( A->type != B->type ) {
		printf("elements of different types : %d, %d\n", A->type, B->type);
		SchemeObject_print(A); printf("\n");
		SchemeObject_print(B); printf("\n");
		return 0;
		
	/* assuming from then on that A and B are of the same type */
	} else if (	SchemeObject_is_integer(A) 	||
				SchemeObject_is_string(A) 	||
				SchemeObject_is_double(A) 	||
				SchemeObject_is_empty(A) 	||
				SchemeObject_is_string(A)	||
			 	SchemeObject_is_symbol(A) 	||
			 SchemeObject_is_char(A)) {
		int result = SchemeObject_eq(A,B);
		if (result == 0) {
			printf("different elements :\n");
			SchemeObject_print(A); printf("\n");
			SchemeObject_print(B); printf("\n");
		}
		return result;
	} else {
		int countA, countB;
		SchemeObject ** descendantsA = GarbageCollection_get_descendants(A, &countA);
		SchemeObject ** descendantsB = GarbageCollection_get_descendants(B, &countB);

		if (countA != countB) {
			free_if_nonnull(descendantsA);
			free_if_nonnull(descendantsB);
			printf("different elements, not the same count of descendants : %d, %d\n", countA, countB);
			SchemeObject_print(A); printf("\n");
			SchemeObject_print(B); printf("\n");
			return 0;
		}
		if ( (descendantsA != NULL) && (descendantsB != NULL) ) {
			int result = 1; int i;
			for(i=0;i<countA;i++)
				result = result && GarbageCollector_stackabuse_checkisomorphism(descendantsA[i], descendantsB[i], currentdepth + 1, maxdepth);
			if (result == 0) {
				printf("different elements :\n");
				SchemeObject_print(A); printf("\n");
				SchemeObject_print(B); printf("\n");
				printf("non-matching descendants are : \n");
				for(i=0;i<countA;i++) {
					if (GarbageCollector_stackabuse_checkisomorphism(descendantsA[i], descendantsB[i], currentdepth + 1, maxdepth)) {
						SchemeObject_print(descendantsA[i]); printf("\n");
						SchemeObject_print(descendantsB[i]); printf("\n");
					}
				}
			}
			free_if_nonnull(descendantsA);
			free_if_nonnull(descendantsB);
			return result;
		} else if ((descendantsA == NULL) || (descendantsB == NULL)) {
			int result = ((descendantsA == NULL) && (descendantsB == NULL));
			free_if_nonnull(descendantsA);
			free_if_nonnull(descendantsB);
			if (result == 0) {
				printf("different elements :\n");
				SchemeObject_print(A); printf("\n");
				SchemeObject_print(B); printf("\n");
			}
			return result;
		}
	}
	printf("Something is missing in GarbageCollector_stackabuse_checkisomorphism.\n");
	SchemeObject_print_details(A);
	SchemeObject_print_details(B);
	return 0;
}


