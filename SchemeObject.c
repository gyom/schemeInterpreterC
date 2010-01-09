/*
 *  SchemeObject.c
 *  scheme_compiler
 *
 */

#include "SchemeObject.h"

int SchemeObject_get_integer(SchemeObject * E) {
	assert(E->type == INTEGER);
	return (E->data.val_integer);
}

double SchemeObject_get_double(SchemeObject * E) {
	assert(E->type == DOUBLE);
	return (E->data.val_double);
}

char SchemeObject_get_char(SchemeObject * E) {
	assert(E->type == CHAR);
	return (E->data.val_char);
}

SchemeObject * SchemeObject_car(SchemeObject * P) {
	assert(P->type == PAIR);
	return (P->data.val_pair.car);
}

SchemeObject * SchemeObject_cdr(SchemeObject * P) {
	assert(P->type == PAIR);
	return (P->data.val_pair.cdr);
}

SchemeObject * SchemeObject_first(SchemeObject * P) {
	return SchemeObject_car(P);	
}

SchemeObject * SchemeObject_rest(SchemeObject * P) {
	return SchemeObject_cdr(P);	
}

SchemeObject * SchemeObject_second(SchemeObject * P) {
	return SchemeObject_car(SchemeObject_cdr(P));	
}

SchemeObject * SchemeObject_third(SchemeObject * P) {
	return SchemeObject_car(SchemeObject_cdr(SchemeObject_cdr(P)));	
}

SchemeObject * SchemeObject_make_integer(MemorySpace * ms, int value) {
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = INTEGER;
	result->data.val_integer = value;
	return result;
}

SchemeObject * SchemeObject_make_double(MemorySpace * ms, double value) {
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = DOUBLE;
	result->data.val_double = value;
	return result;
}

SchemeObject * SchemeObject_make_empty(MemorySpace * ms) {
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = EMPTY;
	return result;
}

SchemeObject * SchemeObject_make_special_symbol_s(MemorySpace * ms, SpecialSymbol S) {
	SchemeObject * result = SchemeObject_make_empty(ms);
	result->type = SPECIAL_SYMBOL;
	result->data.val_special_symbol = S;
	return result;
}

SchemeObject * SchemeObject_make_string(MemorySpace * ms, char * str) {
	int len;
	int size;
	
	//avoid strlen for fun
	//while(*str++ != '\0') len++;
	len = strlen(str);
	
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = STRING;
	
	size = len*sizeof(char);
	result->data.val_memorychunk.size = size;
	result->data.val_memorychunk.data = ms->allocate(ms, size);
	
	// I decided not to use the final '\0' since I'm keeping
	// track of the size with the size component.
	memcpy(result->data.val_memorychunk.data, str, size);
	return result;
}

SchemeObject * SchemeObject_make_symbol(MemorySpace * ms, char * str) {
	SchemeObject * result = SchemeObject_make_string(ms, str);
	result->type = SYMBOL;
	return result;
}

SchemeObject * SchemeObject_make_char(MemorySpace * ms, char c) {
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = CHAR;
	result->data.val_char = c;
	return result;
}

SchemeObject * SchemeObject_make_pair(MemorySpace * ms, SchemeObject * left, SchemeObject * right) {
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = PAIR;
	result->data.val_pair.car = left;
	result->data.val_pair.cdr = right;
	return result;
}

SchemeObject * SchemeObject_make_atomic_function(MemorySpace * ms, void * ptr_C_impl) {
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = ATOMIC_FUNCTION;
	result->data.val_pointer = ptr_C_impl;
	return result;
}

SchemeObject * SchemeObject_make_composite_function(MemorySpace * ms, SchemeObject * arg_symbols, SchemeObject * body, SchemeObject * enclosed_env) {
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = COMPOSITE_FUNCTION;
	result->data.val_lambda.arg_symbols = arg_symbols;
	result->data.val_lambda.body = body;
	result->data.val_lambda.enclosed_env = enclosed_env;
	return result;
}

SchemeObject * SchemeObject_make_exec_apply(MemorySpace * ms, SchemeObject * func, SchemeObject * resolved_args, SchemeObject * output, SchemeObject * continuation) {
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = EXEC_APPLY;
	result->data.val_apply.func = func;
	result->data.val_apply.resolved_args = resolved_args;
	result->data.val_apply.output = output;
	result->data.val_apply.continuation = continuation;
	return result;
}

SchemeObject * SchemeObject_make_exec_eval(MemorySpace * ms, SchemeObject * expr, SchemeObject * env, SchemeObject * output, SchemeObject * continuation) {
	SchemeObject * result = ms->allocate(ms, sizeof(SchemeObject));
	assert(result != NULL);
	result->type = EXEC_EVAL;
	result->data.val_eval.expr = expr;
	result->data.val_eval.env = env;
	result->data.val_eval.output = output;
	result->data.val_eval.continuation = continuation;
	return result;
}

SchemeObject * SchemeObject_make_exec_evalseq(MemorySpace * ms, SchemeObject * expr, SchemeObject * env, SchemeObject * output, SchemeObject * continuation) {
	SchemeObject * result = SchemeObject_make_exec_eval(ms, expr, env, output, continuation);
	result->type = EXEC_EVALSEQ;
	return result;
}

SchemeObject * SchemeObject_make_capturedcontinuation(MemorySpace * ms, SchemeObject * output, SchemeObject * continuation) {
	SchemeObject * result = SchemeObject_make_empty(ms);	
	result->type = CAPTURED_CONTINUATION;
	result->data.val_capturedcontinuation.output = output;
	result->data.val_capturedcontinuation.continuation = continuation;
	return result;
}

/* these boolean functions look strange due to the lack of boolean values in C */
int SchemeObject_is_pair(SchemeObject * E) {
	return ((E != NULL) && (E->type == PAIR));
}
int SchemeObject_is_integer(SchemeObject * E) {
	return ((E != NULL) && (E->type == INTEGER));
}
int SchemeObject_is_double(SchemeObject * E) {
	return ((E != NULL) && (E->type == DOUBLE));
}
int SchemeObject_is_string(SchemeObject * E) {
	return ((E != NULL) && (E->type == STRING));
}
int SchemeObject_is_char(SchemeObject * E) {
	return ((E != NULL) && (E->type == CHAR));
}
int SchemeObject_is_symbol(SchemeObject * E) {
	return ((E != NULL) && (E->type == SYMBOL));
}
int SchemeObject_is_empty(SchemeObject * E) {
	return ((E != NULL) && (E->type == EMPTY));
}
int SchemeObject_is_atomic_function(SchemeObject * E) {
	return ((E != NULL) && (E->type == ATOMIC_FUNCTION));
}
int SchemeObject_is_composite_function(SchemeObject * E) {
	return ((E != NULL) && (E->type == COMPOSITE_FUNCTION));
}
int SchemeObject_is_exec_apply(SchemeObject * E) {
	return ((E != NULL) && (E->type == EXEC_APPLY));
}
int SchemeObject_is_exec_eval(SchemeObject * E) {
	return ((E != NULL) && (E->type == EXEC_EVAL));
}
int SchemeObject_is_exec_evalseq(SchemeObject * E) {
	return ((E != NULL) && (E->type == EXEC_EVALSEQ));
}
int SchemeObject_is_special_symbol(SchemeObject * E) {
	return ((E != NULL) && (E->type == SPECIAL_SYMBOL));
}
int SchemeObject_is_special_symbol_s(SchemeObject * E, SpecialSymbol S) {
	return (SchemeObject_is_special_symbol(E) && (E->data.val_special_symbol == S));
}
int SchemeObject_is_capturedcontinuation(SchemeObject * E) {
	return ((E != NULL) && (E->type == CAPTURED_CONTINUATION));
}

int SchemeObject_is_self_evaluating(SchemeObject * E) {
	int result = 0;
	result |= SchemeObject_is_integer(E);
	result |= SchemeObject_is_double(E);
	result |= SchemeObject_is_string(E);
	result |= SchemeObject_is_char(E);
	result |= SchemeObject_is_special_symbol(E);
	/* 	These functions are already evaluated in the sense
	 	that they are ready to be applied to something. */
	result |= SchemeObject_is_atomic_function(E);
	result |= SchemeObject_is_composite_function(E);
	return result;
}

int SchemeObject_eq_memorychunks(SchemeObject * x, SchemeObject * y) {
	
	int size1 = x->data.val_memorychunk.size;
	int size2 = y->data.val_memorychunk.size;
	
	// it can't be the same data if the lengths aren't the same
	if (size1 != size2)
		return 0;
	return !memcmp( x->data.val_memorychunk.data, y->data.val_memorychunk.data, size1);	
}

int SchemeObject_eq(SchemeObject * x, SchemeObject * y) {
	if (SchemeObject_is_integer(x) && SchemeObject_is_integer(y))
		return (SchemeObject_get_integer(x) == SchemeObject_get_integer(y));
	else if (SchemeObject_is_double(x) && SchemeObject_is_integer(y))
		return (SchemeObject_get_double(x) == SchemeObject_get_integer(y));
	else if (SchemeObject_is_integer(x) && SchemeObject_is_double(y))
		return (SchemeObject_get_integer(x) == SchemeObject_get_double(y));
	else if (SchemeObject_is_double(x) && SchemeObject_is_double(y))
		return (SchemeObject_get_double(x) == SchemeObject_get_double(y));
	else if (SchemeObject_is_string(x) && SchemeObject_is_string(y))
		return SchemeObject_eq_memorychunks(x,y);
	else if (SchemeObject_is_symbol(x) && SchemeObject_is_symbol(y))
		return SchemeObject_eq_memorychunks(x,y);
	else if (SchemeObject_is_char(x) && SchemeObject_is_char(y))
		return (SchemeObject_get_char(x) == SchemeObject_get_char(y));

	/* if everything failed */
	return 0;
}

void SchemeObject_print(SchemeObject * E) {
	if (E == NULL) {
		printf("NULL");
	} else if (SchemeObject_is_integer(E))
		printf("%d", SchemeObject_get_integer(E));
	else if (SchemeObject_is_double(E))
		printf("%f", SchemeObject_get_double(E));
	else if (SchemeObject_is_empty(E))
		printf("()");
	else if (SchemeObject_is_string(E) || SchemeObject_is_symbol(E)) {
		char * str = E->data.val_memorychunk.data;
		int i;
		for( i = 0 ; i < E->data.val_memorychunk.size; ++i)
			printf("%c", str[i]);
	} else if (SchemeObject_is_char(E))
		printf("%c", SchemeObject_get_char(E));
	else if (SchemeObject_is_pair(E)) {
		/* 	abusing the C stack because this should be done with Scheme
			if I really wanted to do the whole thing. I already accepted
		 	the C environment lookup, so this seems okay.
		 */
		printf("( ");
		while(SchemeObject_is_pair(E)) {
			SchemeObject_print(SchemeObject_car(E));
			printf("  ");
			E = SchemeObject_cdr(E);
		}
		printf(")");
	} else if (SchemeObject_is_exec_apply(E) ) {
		printf("[exec_apply]");
	} else if (SchemeObject_is_exec_eval(E) ) {
		printf("[exec_eval]");
	} else if (SchemeObject_is_atomic_function(E) ) {
		printf("{atomic_function}");
	} else if (SchemeObject_is_composite_function(E) ) {
		printf("{composite_function}");
	} else if (SchemeObject_is_special_symbol_s(E, LAMBDA)) {
		printf("{ssymb:lambda}");
	} else if (SchemeObject_is_special_symbol_s(E, SET)) {
		printf("{ssymb:set!}");
	} else if (SchemeObject_is_special_symbol_s(E, CALLCC)) {
		printf("{ssymb:call/cc}");
	} else if (SchemeObject_is_special_symbol_s(E, IF)) {
		printf("{ssymb:if}");
	} else if (SchemeObject_is_special_symbol_s(E, DEFINE)) {
		printf("{ssymb:define}");
	} else if (SchemeObject_is_special_symbol_s(E, QUOTE)) {
		printf("{ssymb:quote}");
	} else if (SchemeObject_is_special_symbol_s(E, TRUE)) {
		printf("{ssymb:#t}");
	} else if (SchemeObject_is_special_symbol_s(E, FALSE)) {
		printf("{ssymb:#f}");
	} else
		printf("-X-"); /* some default symbol for unknowns */
			
	return;
}

/* 	Should I return copies of the selected expressions instead of direct references ?
 	No. That shouldn't be necessary, because the objects returned are already the results
 	of evaluations. Don't use the result of an "if" to write inside a variable, though.
 	This won't occur with normal use of Scheme, but we could ourselves make funny things
 	with (if #t x y), I think.
 
 	It's important to note also that, although this function is called "if_wrapper", it does
 	not behave like the other wrappers "sum_wrapper", "and_wrapper", etc. The "if_wrapper" is
 	not meant to be an atomic function because of the special behavior that if expressions have.
 	They don't evaluate all branches before evaluating the condition (duh). The "if_wrapper" is
 	just a final step from the "execute" dispatch routine after the condition is evaluated but
 	before any of the branches are considered for evaluation.
 */
SchemeObject * if_wrapper(MemorySpace * ms, SchemeObject * L) {
	printf("During if_wrapper : "); SchemeObject_print(L); printf("\n");
	if( SchemeObject_is_special_symbol_s( SchemeObject_first(L), TRUE) )
		return SchemeObject_second(L);
	else
		return SchemeObject_third(L); // possibly an empty list if nothing if found there
}

int SchemeObject_copy(MemorySpace * ms, SchemeObject * dest, SchemeObject * source) {
	memcpy(dest, source, sizeof(SchemeObject));
	/* if we need to reallocate the memory */
	if (SchemeObject_is_string(source) | SchemeObject_is_symbol(source)) {
		dest->data.val_memorychunk.data = ms->allocate(ms, source->data.val_memorychunk.size);
		if (dest->data.val_memorychunk.data == NULL)
			return 0;
		memcpy(dest->data.val_memorychunk.data, source->data.val_memorychunk.data, source->data.val_memorychunk.size);
	}
	return 1;
}

SchemeObject * SchemeObject_make_list_n(MemorySpace * ms, int length) {
	SchemeObject * result = SchemeObject_make_empty(ms);
	int i=0;
	while(i++ < length)
		result = SchemeObject_make_pair(ms, SchemeObject_make_empty(ms), result);
	return result;
}

SchemeObject * SchemeObject_zip(MemorySpace * ms, SchemeObject * A, SchemeObject * B) {
	SchemeObject * result = SchemeObject_make_empty(ms);
	SchemeObject * tmp;
	
	while(SchemeObject_is_pair(A) && SchemeObject_is_pair(B)) {
		tmp = SchemeObject_make_pair(ms, SchemeObject_car(A), \
				SchemeObject_make_pair(ms, SchemeObject_car(B), \
				SchemeObject_make_empty(ms) ));
		A = SchemeObject_cdr(A);
		B = SchemeObject_cdr(B);
		result = SchemeObject_make_pair(ms, tmp, result);
	}
	return result;
}

int SchemeObject_length(SchemeObject * L) {
	int accum = 0;
	while( SchemeObject_is_pair(L) ) {
		accum++;
		L = SchemeObject_cdr(L);
	}
	return accum;
}

/* 	This function was a necessary tool to make sure that the sequential
 	evaluations (further below in the "execute" function) we not being
 	done in reverse order due to the way in which we initially added them
 	to the execution stack, stringing the continuations one after the other.
 */
SchemeObject * SchemeObject_reverse_reference_list(MemorySpace * ms, SchemeObject * L) {
	SchemeObject * result = SchemeObject_make_empty(ms);
	while(!SchemeObject_is_empty(L)) {
		result = SchemeObject_make_pair(ms, SchemeObject_car(L), result);
		L = SchemeObject_cdr(L);
	}	
	return result;
}

/* with 1 being the first element, the car, and not 0 */
SchemeObject * SchemeObject_list_ref_n(SchemeObject * L, int n) {
	while( !SchemeObject_is_empty(L) ) {
		if ( n == 1 )
			return SchemeObject_car(L);
		else {
			n--;
			L = SchemeObject_cdr(L);
		}
	}
	return NULL;
}

/* 	Seems like the natural thing to do to implement the "define".
 	I'm not sure I'd recommend those functions for now for other uses.
 */
void SchemeObject_setcar(SchemeObject * A, SchemeObject * B) {
	assert( SchemeObject_is_pair(A) );
	A->data.val_pair.car = B;
	return;
}
void SchemeObject_setcdr(SchemeObject * A, SchemeObject * B) {
	assert( SchemeObject_is_pair(A) );
	A->data.val_pair.cdr = B;
	return;
}

SchemeObject * SchemeObject_make_bool(MemorySpace * ms, int E) {
	if (E)
		return SchemeObject_make_special_symbol_s(ms, TRUE);
	else
		return SchemeObject_make_special_symbol_s(ms, FALSE);
	return NULL;
}

SchemeObject * SchemeObject_make_list_1(MemorySpace * ms, SchemeObject * A) {
	SchemeObject * result = SchemeObject_make_list_n(ms, 1);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,1), A);
	return result;	
}

SchemeObject * SchemeObject_make_list_2(MemorySpace * ms, SchemeObject * A, SchemeObject * B) {
	SchemeObject * result = SchemeObject_make_list_n(ms, 2);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,1), A);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,2), B);
	return result;	
}

SchemeObject * SchemeObject_make_list_3(MemorySpace * ms, SchemeObject * A, SchemeObject * B, SchemeObject * C) {
	SchemeObject * result = SchemeObject_make_list_n(ms, 3);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,1), A);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,2), B);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,3), C);
	return result;	
}

SchemeObject * SchemeObject_make_list_4(MemorySpace * ms, SchemeObject * A, SchemeObject * B, SchemeObject * C, SchemeObject * D) {
	SchemeObject * result = SchemeObject_make_list_n(ms, 4);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,1), A);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,2), B);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,3), C);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,4), D);
	return result;	
}

SchemeObject * SchemeObject_make_list_5(MemorySpace * ms, SchemeObject * A, SchemeObject * B, SchemeObject * C, SchemeObject * D, SchemeObject * E) {
	SchemeObject * result = SchemeObject_make_list_n(ms, 5);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,1), A);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,2), B);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,3), C);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,4), D);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(result,5), E);
	return result;	
}

SchemeObject * frame_binding_lookup(SchemeObject * frame, SchemeObject * symbol) {

	SchemeObject * binding;
	
	while( !SchemeObject_is_empty(frame) ) {
		assert(SchemeObject_is_pair(frame));
		binding = SchemeObject_car(frame);
		assert(SchemeObject_is_pair(binding));
		if(SchemeObject_eq(SchemeObject_car(binding), symbol))
			return binding;
		frame = SchemeObject_cdr(frame);
	}
	return NULL;
}

SchemeObject * env_binding_lookup(SchemeObject * env, SchemeObject * symbol) {
	/*
	 This function returns NULL if it doesn't find the binding.
	 Otherwise, it returns the binding, the '(x 1), so that
	 we can implement the environment variable value lookup and
	 the set! function with the same code.
	 */
	SchemeObject * frame;
	SchemeObject * binding;
	
	while( !SchemeObject_is_empty(env) ) {
		assert(SchemeObject_is_pair(env));
		frame = SchemeObject_car(env);

		binding = frame_binding_lookup(frame, symbol);
		if (binding != NULL)
			return binding;
		/* otherwise go to the next place in the environment */
		env = SchemeObject_cdr(env);
	}
	/* binding not found */
	return NULL;
}

SchemeObject * env_value_lookup(SchemeObject * env, SchemeObject * symbol) {
	SchemeObject * binding = env_binding_lookup(env, symbol);
	
	if(binding != NULL)
		return SchemeObject_second(binding);
	else
		return NULL;
}

int env_set_existing_binding(MemorySpace * ms, SchemeObject * env, SchemeObject * symbol, SchemeObject * value) {
	SchemeObject * binding = env_binding_lookup(env, symbol);
	
	assert(binding != NULL);
	
	SchemeObject * valueholder = ms->allocate(ms, sizeof(SchemeObject));
	SchemeObject_copy(ms, valueholder, value);
	
	/* useless return value as copies should always succeed */
	return SchemeObject_copy(ms, SchemeObject_cdr(binding), SchemeObject_make_pair(ms, value, SchemeObject_make_empty(ms)));
}

SchemeObject * SchemeObject_make_single_binding(MemorySpace * ms, SchemeObject * symbol, SchemeObject * value) {
	SchemeObject * result = SchemeObject_make_pair(ms, symbol, SchemeObject_make_pair(ms, value, SchemeObject_make_empty(ms)));
	return result;	
}

SchemeObject * execute(MemorySpace * ms, SchemeObject * location) {

	if( SchemeObject_is_exec_apply(location) ) {
		/* extract some information for convenience */
		SchemeObject * func = location->data.val_apply.func;	
		SchemeObject * resolved_args = location->data.val_apply.resolved_args;
		SchemeObject * output = location->data.val_apply.output;
		SchemeObject * continuation = location->data.val_apply.continuation;

		/* we have two cases : atomic and composite */
		if( SchemeObject_is_atomic_function(func) ) {
			/* a properly typed function pointer for the primitive */
			SchemeObject * (*f)(MemorySpace *, SchemeObject *) = NULL;
			f = func->data.val_pointer;
			SchemeObject * tmpout = f(ms, resolved_args);
			/* 	Some functions, such as "print", don't allocate
			 	anything as output. Should I allocate stupidly empty
			 	arrays to serve as outputs to prevent this problem from
			 	leaking in the future ?
			 */
			if (tmpout != NULL) {
				SchemeObject_copy(ms, output, tmpout);
				//else
				//	SchemeObject_copy(ms, output, SchemeObject_make_empty(ms));
				#ifdef DEBUG_EXECUTE
					printf("execute apply found atomic_function\n");
					SchemeObject_print(output);
					printf("\ncontinuing to %p\n", continuation);
				#endif
			}
			return continuation; /* the natural next step */
		} else if( SchemeObject_is_composite_function(func) ) {
			SchemeObject * newenv = SchemeObject_make_pair(ms, SchemeObject_zip(ms, func->data.val_lambda.arg_symbols, resolved_args), func->data.val_lambda.enclosed_env);
			SchemeObject * newexec = SchemeObject_make_exec_evalseq(ms, func->data.val_lambda.body, newenv, output, continuation);
			#ifdef DEBUG_EXECUTE
				printf("execute apply found composite_function\n");
				SchemeObject_print(func);
				printf("extended the environment for evaluation with the frame : ");
				SchemeObject_print(SchemeObject_first(newenv));
				printf("\ncontinuing to %p\n", newexec);
			#endif
			return newexec;
		} else if (SchemeObject_is_capturedcontinuation(func)) {
			/* 	If we have (k (+ 1 2)) we want to copy that 3 to
			 	where the output was going when we captured "k".
			 	Then we continue over in that place. The arguments are
			 	always evaluated by the time we reach statements with
			 	apply executions like here.
			 */
			#ifdef DEBUG_EXECUTE
				printf("restoring previously captured continuation\n");
				printf("\ncontinuing to %p instead of %p\n", func->data.val_capturedcontinuation.continuation, continuation);
			#endif
			SchemeObject_copy(ms, func->data.val_capturedcontinuation.output, SchemeObject_first(resolved_args));
			return func->data.val_capturedcontinuation.continuation;
		} else {
			printf("Found an apply execution block that couldn't be processed.\n");
			printf("%p\tfunc = ", func); SchemeObject_print(func); printf("\n");
			printf("%p\tresolved_args = ", resolved_args); SchemeObject_print(resolved_args); printf("\n");
			printf("%p\toutput = ", output); SchemeObject_print(output); printf("\n");
			printf("%p\tcontinuation = ", continuation); SchemeObject_print(continuation); printf("\n");
			return NULL;
		}
	} else if( SchemeObject_is_exec_eval(location) ) {
		SchemeObject * expr = location->data.val_eval.expr;	
		SchemeObject * env = location->data.val_eval.env;
		SchemeObject * output = location->data.val_apply.output;
		SchemeObject * continuation = location->data.val_apply.continuation;
		
		/* Not handling (semiquote ... (unquote ...) ). */
		if( SchemeObject_is_symbol(expr) ) {
			SchemeObject * value = env_value_lookup(env, expr);
			assert(value != NULL);
			SchemeObject_copy(ms, output, value);
			#ifdef DEBUG_EXECUTE
				printf("execute eval found symbol. looking up value in env.\n");
				SchemeObject_print(output);
				printf("\ncontinuing to %p\n", continuation);
			#endif
			return continuation;
		} else if( SchemeObject_is_self_evaluating(expr) ) {
			SchemeObject_copy(ms, output, expr);
			#ifdef DEBUG_EXECUTE
				printf("execute eval found self evaluating expression.\n");
				SchemeObject_print(output);
				printf("\ncontinuing to %p\n", continuation);
			#endif
			return continuation;
		} else if( SchemeObject_is_pair(expr) ) {
			SchemeObject * head = SchemeObject_car(expr);
			#ifdef DEBUG_EXECUTE
				printf("execute eval found pair. will determine the type.\n");
				SchemeObject_print(head);
				printf("\n");
			#endif
			if( SchemeObject_is_special_symbol_s(head, LAMBDA) ) {
				/*
				 A very important nuance is expressed by choosing
					 SchemeObject_cdr(SchemeObject_cdr(expr))
				 over
				 	SchemeObject_third(expr)
				 when constructing the composite function.
				 It's important to get the function body right when we have
				 lambdas with more than one statement in the body.
				 For example, if we had
				 	(lambda (x) (lambda (t) 1) (+ x 2)),
				 which might be a little pointless but a valid definition,
				 we don't want to evaluate that function (lambda (t) 1)
				 on the result of (+ x 2).
				 */
				SchemeObject_copy(ms, output, SchemeObject_make_composite_function(ms, SchemeObject_second(expr), SchemeObject_cdr(SchemeObject_cdr(expr)), env) );
				#ifdef DEBUG_EXECUTE
					printf("execute eval found special symbol lambda.\n");
					SchemeObject_print(output);
					printf("\ncontinuing to %p\n", continuation);
				#endif
				return continuation;
			} else if( SchemeObject_is_special_symbol_s(head, QUOTE) ) {
				SchemeObject_copy(ms, output, SchemeObject_second(expr));
				/*	I will do an expansion somewhere else, so that I get
				 (quote 1) -> 1
				 (quote 1 2 3) -> 2 and not (list 1 2 3)
				 but as long as the conversions are done during preprocessing
				 this will convert
				 (quote (list 1 2 3)) -> (list (quote 1) (quote 2) (quote 3)).
				 */
				#ifdef DEBUG_EXECUTE
					printf("execute eval found special symbol quote.\n");
					SchemeObject_print(output);
					printf("\ncontinuing to %p\n", continuation);
				#endif
				return continuation;
			} else if( SchemeObject_is_special_symbol_s(head, SET) ) {
				SchemeObject_copy(ms, output, SchemeObject_make_empty(ms));
				SchemeObject * symbol = SchemeObject_second(expr);
				SchemeObject * unevaluated_expr = SchemeObject_third(expr);
				SchemeObject * binding = env_binding_lookup(env, symbol);
				assert(binding != NULL); // failed set!, might want to crash here
				SchemeObject * destination = SchemeObject_second(binding);
				SchemeObject * new_task = SchemeObject_make_exec_eval(ms, unevaluated_expr, env, destination, continuation);
				return new_task;
			} else if( SchemeObject_is_special_symbol_s(head, IF) ) {
				#ifdef DEBUG_EXECUTE
					printf("execute eval found special symbol if.\n");
					SchemeObject_print(SchemeObject_cdr(expr));
					printf("\n");
				#endif
				SchemeObject * what_the_if_picked = SchemeObject_make_empty(ms);
				SchemeObject * evalselection_task = SchemeObject_make_exec_eval(ms, what_the_if_picked, env, output, continuation);
				
				SchemeObject * single_resolved_arg_plus_two_bodies = SchemeObject_make_list_n(ms, 3);
				SchemeObject_copy(ms, SchemeObject_list_ref_n(single_resolved_arg_plus_two_bodies,2), SchemeObject_list_ref_n(expr, 3));
				SchemeObject_copy(ms, SchemeObject_list_ref_n(single_resolved_arg_plus_two_bodies,3), SchemeObject_list_ref_n(expr, 4));
				SchemeObject * if_task = SchemeObject_make_exec_apply(ms, SchemeObject_make_atomic_function(ms, & if_wrapper), single_resolved_arg_plus_two_bodies, what_the_if_picked, evalselection_task);
				
				SchemeObject * condition_evaluation_task = SchemeObject_make_exec_eval(ms, SchemeObject_second(expr), env, SchemeObject_list_ref_n(single_resolved_arg_plus_two_bodies,1), if_task);
				return condition_evaluation_task;
			/* end of set! case */
			}  else if ( SchemeObject_is_special_symbol_s(head, CALLCC)) {
				SchemeObject * k = SchemeObject_make_capturedcontinuation(ms, output, continuation);
				SchemeObject * next_task = SchemeObject_make_exec_eval(ms, 
																   SchemeObject_make_pair(ms, SchemeObject_second(expr),
																						  SchemeObject_make_pair(ms, k, SchemeObject_make_empty(ms))),
																   env, output, continuation);
				#ifdef DEBUG_EXECUTE
					printf("capturing continuation going to %p\n", next_task);
				#endif
				return next_task;
			} else if ( SchemeObject_is_special_symbol_s(head, DEFINE)) {
				/* 	Supports only the kind of define that goes like
				 	(define f (lambda (t) (+ 1 t))
				 	and not more compact version to define functions.
				 	I'm a bit unsure at the moment what to do about cases
				 	such as
				 		((lambda (x) (+ 2 (begin (define x 2) x) 1)
				 	where defines would go into places where I don't provide
				 	a blank frame for every application.
				 */
				SchemeObject * name = SchemeObject_second(expr);
				assert(!SchemeObject_is_empty(env));
				SchemeObject * frame = SchemeObject_car(env); // doubt
				SchemeObject * binding = frame_binding_lookup(frame, name);
				
				SchemeObject * output_defined_to;
				if( !binding )
					output_defined_to = SchemeObject_second(binding);
				else {
					output_defined_to = SchemeObject_make_empty(ms);
					SchemeObject * newframe = SchemeObject_make_pair(ms, SchemeObject_make_pair(ms, name, SchemeObject_make_pair(ms, output_defined_to, SchemeObject_make_empty(ms))), frame);
					SchemeObject_setcar(env, newframe);
				}
				return SchemeObject_make_exec_eval(ms, SchemeObject_third(expr), env, output_defined_to, continuation);
			} else {
				#ifdef DEBUG_EXECUTE
				printf("execute eval processing general case (f a1 a2 ... an) with ");
				SchemeObject_print(expr);
				printf("\n");
				#endif
				
				int length = SchemeObject_length(expr) - 1; /* exclude the function at the start of the list */
				SchemeObject * resolved_args = SchemeObject_make_list_n(ms, length);
				SchemeObject * func = SchemeObject_make_empty(ms);
				/* two of these arguments aren't computed when we assign memory */
				SchemeObject * next_task = SchemeObject_make_exec_apply(ms, func, resolved_args, output, continuation);
			
				int i;
				SchemeObject * L_out_ai = SchemeObject_reverse_reference_list(ms, resolved_args);
				SchemeObject * L_ai = SchemeObject_reverse_reference_list(ms, SchemeObject_rest(expr));
				/* we're evaluating arguments in order now, with the list reversal */
				for(i=0;i<length;++i) {
					next_task = SchemeObject_make_exec_eval(ms, SchemeObject_first(L_ai), env, SchemeObject_first(L_out_ai), next_task);					
					#ifdef DEBUG_EXECUTE
					printf("added one more eval task to the stack : ");
					SchemeObject_print(SchemeObject_first(L_ai));
					printf("\n");
					printf("next_task is now %p\n", next_task);
					#endif
					
					/* advance */
					L_ai = SchemeObject_rest(L_ai);
					L_out_ai = SchemeObject_rest(L_out_ai);
				}
				
				/* evaluate the function in the head first */				
				next_task = SchemeObject_make_exec_eval(ms, head, env, func, next_task);
				#ifdef DEBUG_EXECUTE
				printf("added eval task for the head : ");
				SchemeObject_print(head);
				printf("\n");
				printf("next_task is now %p\n", next_task);
				#endif
				
				return next_task;
			}
		}
		/* not a pair */
	} else if (SchemeObject_is_exec_evalseq(location) ) {	
		SchemeObject * expr = location->data.val_eval.expr;	
		SchemeObject * env = location->data.val_eval.env;
		SchemeObject * output = location->data.val_apply.output;
		SchemeObject * continuation = location->data.val_apply.continuation;
		
		#ifdef DEBUG_EXECUTE
		printf("execute evalseq (a1 a2 ... an) with ");
		SchemeObject_print(expr);
		printf("\n");
		#endif
				
		int length = SchemeObject_length(expr);
		if (length == 0)
			return continuation;
		
		/* string the tasks into continuations one by one */				
		SchemeObject * next_task = continuation;
		
		int i;
		SchemeObject * L_ai = SchemeObject_reverse_reference_list(ms, expr);
		SchemeObject * junk = NULL;
		
		/* do the first case manually */
		next_task = SchemeObject_make_exec_eval(ms, SchemeObject_first(L_ai), env, output, next_task);
		#ifdef DEBUG_EXECUTE
			printf("added one more eval task to the stack : ");
			SchemeObject_print(SchemeObject_first(L_ai));
			printf("\n");
			printf("next_task is now %p\n", next_task);
		#endif
		if (length >= 2)
			junk = SchemeObject_make_empty(ms);
		
		for(i=1;i<length;++i) {
			//only the last element writes to the output
			next_task = SchemeObject_make_exec_eval(ms, SchemeObject_first(L_ai), env, junk, next_task);					
			#ifdef DEBUG_EXECUTE
				printf("added one more eval task to the stack : ");
				SchemeObject_print(SchemeObject_first(L_ai));
				printf("\n");
				printf("next_task is now %p\n", next_task);
			#endif
			/* advance */
			L_ai = SchemeObject_rest(L_ai);
		}
		return next_task;
		
	} else /* not an eval execution block */{
		printf("Found an unknown execution block.\n");
		return NULL;
	}
	return NULL;
}

SchemeObject * evaluate(MemorySpace * ms, SchemeObject * expr, SchemeObject * env) {
		
	SchemeObject * output = SchemeObject_make_empty(ms);
	SchemeObject * continuation = SchemeObject_make_exec_eval(ms, expr, env, output, NULL);
	
	do {
		continuation = execute(ms, continuation);
		//printf("main loop will execute : %p\n", continuation);
	} while(continuation != NULL);

	return output;
}

#ifdef TEST_SCHEMEOBJECT_ALONE

int main() {
	
	//SchemeObject * plus_impl = SchemeObject_make_atomic_function(ms, & sum_wrapper);

	/*test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test7();
	test8();*/
	test9();
	test10();
	
	return 0;
}
#endif
