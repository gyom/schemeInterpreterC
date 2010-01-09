/*
 *  SchemeObject_tests.c
 *  scheme_compiler
 */

#include "SchemeObject_tests.h"

void test1() {
	printf("-----test1-----\n");
	MemorySpace * ms = make_MemorySpace(10000);
	
	SchemeObject * frame1 = \
	SchemeObject_make_pair(ms, \
						   SchemeObject_make_single_binding(ms, SchemeObject_make_symbol(ms, "x"), SchemeObject_make_integer(ms, 4)), \
						   SchemeObject_make_pair(ms, \
												  SchemeObject_make_single_binding(ms, SchemeObject_make_symbol(ms, "y"), SchemeObject_make_integer(ms, 7)), \
												  SchemeObject_make_empty(ms)));
	SchemeObject * frame2 = \
	SchemeObject_make_pair(ms, \
						   SchemeObject_make_single_binding(ms, SchemeObject_make_symbol(ms, "b"), SchemeObject_make_double(ms, -2.34)), \
						   SchemeObject_make_pair(ms, \
												  SchemeObject_make_single_binding(ms, SchemeObject_make_symbol(ms, "a"), SchemeObject_make_double(ms, 13.1)), \
												  SchemeObject_make_empty(ms)));
	
	SchemeObject * env = SchemeObject_make_pair(ms, frame1, SchemeObject_make_pair(ms, frame2, SchemeObject_make_empty(ms)));
	
	
	assert(SchemeObject_is_empty(   SchemeObject_make_empty(ms)));
	assert(SchemeObject_is_integer( SchemeObject_make_integer(ms, 10)));
	assert(SchemeObject_is_double(  SchemeObject_make_double(ms, 10.0)));
	assert(SchemeObject_is_double(  SchemeObject_make_double(ms, 10)));
	
	assert(SchemeObject_eq(SchemeObject_make_symbol(ms, "x"), SchemeObject_make_symbol(ms, "x")));
	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 35), SchemeObject_make_integer(ms, 35)));
	assert(!SchemeObject_eq(SchemeObject_make_integer(ms, 35), SchemeObject_make_integer(ms, 36)));
	assert(SchemeObject_eq(SchemeObject_make_double(ms, 35), SchemeObject_make_double(ms, 35)));
	assert(SchemeObject_eq(SchemeObject_make_string(ms, "lupi"), SchemeObject_make_string(ms, "lupi")));
	assert(!SchemeObject_eq(SchemeObject_make_string(ms, "lupitron"), SchemeObject_make_string(ms, "lupi")));
	
	/*
	 SchemeObject_print(frame1);
	 printf("\n*NEWLINE\n");
	 SchemeObject_print(frame2);
	 printf("\n*NEWLINE\n");
	 SchemeObject_print(env);
	 printf("\n*NEWLINE\n");
	 SchemeObject_print(frame_binding_lookup(frame1, SchemeObject_make_symbol(ms, "x")));
	 printf("\n*NEWLINE\n");
	 SchemeObject_print(env_value_lookup(env, SchemeObject_make_symbol(ms, "x")));
	 printf("\n*NEWLINE\n");
	 SchemeObject_print(env_value_lookup(env, SchemeObject_make_symbol(ms, "a")));
	 printf("\n*NEWLINE\n");
	 */
	
	assert(SchemeObject_eq(env_value_lookup(env, SchemeObject_make_symbol(ms, "x")), SchemeObject_make_integer(ms, 4)));
	assert(SchemeObject_eq(env_value_lookup(env, SchemeObject_make_symbol(ms, "a")), SchemeObject_make_double(ms, 13.1)));
	
	ms->destroy(ms);
	return;
}

void test2() {
	printf("-----test2-----\n");
	MemorySpace * ms = make_MemorySpace(10000);
	
	SchemeObject * env = SchemeObject_make_empty(ms);
	env = SchemeObject_make_pair(ms, \
		SchemeObject_zip(ms,	\
			SchemeObject_make_pair(ms, SchemeObject_make_symbol(ms, "x"),	\
			SchemeObject_make_pair(ms, SchemeObject_make_symbol(ms, "y"),	\
			SchemeObject_make_pair(ms, SchemeObject_make_symbol(ms, "z"),	\
			SchemeObject_make_empty(ms)))), \
			SchemeObject_make_pair(ms, SchemeObject_make_integer(ms, 1),	\
			SchemeObject_make_pair(ms, SchemeObject_make_integer(ms, 2),	\
			SchemeObject_make_pair(ms, SchemeObject_make_integer(ms, 3),	\
			SchemeObject_make_empty(ms))))), \
		SchemeObject_make_empty(ms));

	SchemeObject_print(env);
	printf("\n");
	SchemeObject_print(env_value_lookup(env, SchemeObject_make_symbol(ms, "z")));
	printf("\n");
	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 3), \
						   env_value_lookup(env, SchemeObject_make_symbol(ms, "z"))));
	
	SchemeObject * output = SchemeObject_make_empty(ms);
	SchemeObject * task1 = SchemeObject_make_exec_eval(ms, SchemeObject_make_symbol(ms,"x"), env, output, NULL);
	execute(ms, task1);
	assert(SchemeObject_eq(output, SchemeObject_make_integer(ms,1))); 
	
	//SchemeObject_print(output); // expecting 1
	//printf("\n");
	SchemeObject * task2 = SchemeObject_make_exec_eval(ms,
		SchemeObject_make_pair(ms, SchemeObject_make_special_symbol_s(ms, QUOTE),
							   SchemeObject_make_pair(ms, SchemeObject_make_symbol(ms, "lupi"), SchemeObject_make_empty(ms))), env, output, NULL);

	execute(ms, task2);
	assert(SchemeObject_eq(output, SchemeObject_make_symbol(ms,"lupi"))); 
	
	//SchemeObject_print(output);
	//printf("\n");
	ms->destroy(ms);
	return;
}


void test3() {
	printf("-----test3-----\n");
	MemorySpace * ms = make_MemorySpace(10000);
	
	SchemeObject * env = SchemeObject_make_empty(ms);
	env = SchemeObject_make_pair(ms,
		SchemeObject_zip(ms,
		SchemeObject_make_pair(ms, SchemeObject_make_symbol(ms, "+"),
		SchemeObject_make_pair(ms, SchemeObject_make_symbol(ms, "y"),
		SchemeObject_make_pair(ms, SchemeObject_make_symbol(ms, "z"),
		SchemeObject_make_empty(ms)))),
		SchemeObject_make_pair(ms, SchemeObject_make_atomic_function(ms, & sum_wrapper),
		SchemeObject_make_pair(ms, SchemeObject_make_integer(ms, 2),
		SchemeObject_make_pair(ms, SchemeObject_make_integer(ms, 3),
		SchemeObject_make_empty(ms))))),
		SchemeObject_make_empty(ms));
	
	SchemeObject * L = SchemeObject_make_list_n(ms, 3);
	SchemeObject_copy(ms, SchemeObject_first(L), SchemeObject_make_integer(ms, 1)); 
	SchemeObject_copy(ms, SchemeObject_second(L), SchemeObject_make_double(ms, -0.9)); 
	SchemeObject_copy(ms, SchemeObject_third(L), SchemeObject_make_double(ms, 4.5)); 
	
	SchemeObject_print(env);
	printf("\n");
	SchemeObject_print(env_value_lookup(env, SchemeObject_make_symbol(ms, "+")));
	printf("\n");

	SchemeObject * output = SchemeObject_make_empty(ms);

	SchemeObject * expr = SchemeObject_make_list_n(ms, 2);
	SchemeObject_copy(ms, SchemeObject_first(expr), SchemeObject_make_symbol(ms, "+"));
	SchemeObject_copy(ms, SchemeObject_cdr(expr), L);

	SchemeObject_print(expr);
	printf("\n");
	SchemeObject * continuation = SchemeObject_make_exec_eval(ms, expr, env, output, NULL);
	do {
		printf("main loop about to execute continuation at %p\n", continuation);
		continuation = execute(ms, continuation);
		//SchemeObject_print(continuation);
	} while(continuation != NULL);
	
	SchemeObject_print(output); // expecting 4.6
	printf("\n");
	
	assert(SchemeObject_eq(output, SchemeObject_make_double(ms, 4.6)));
	
	ms->destroy(ms);
	return;
}

void test4() {
	printf("-----test4-----\n");
	MemorySpace * ms = make_MemorySpace(10000);
	
	SchemeObject * L = SchemeObject_make_list_n(ms, 3);
	SchemeObject_copy(ms, SchemeObject_first(L), SchemeObject_make_integer(ms, 1)); 
	SchemeObject_copy(ms, SchemeObject_second(L), SchemeObject_make_double(ms, -0.9)); 
	SchemeObject_copy(ms, SchemeObject_third(L), SchemeObject_make_double(ms, 4.5)); 
	
	SchemeObject * output = SchemeObject_make_empty(ms);
		
	SchemeObject_print(L);
	printf("\n");

	SchemeObject * task = SchemeObject_make_exec_apply(ms, SchemeObject_make_atomic_function(ms, & sum_wrapper), L, output, NULL);
	SchemeObject * continuation;
	do {
		continuation = execute(ms, task);
		//SchemeObject_print(continuation);
	} while(continuation != NULL);
	
	SchemeObject_print(output); // expecting 4.6
	printf("\n");
	
	assert(SchemeObject_eq(output, SchemeObject_make_double(ms, 4.6)));
	
	ms->destroy(ms);
	return;
}

void test5() {
	printf("-----test5-----\n");
	MemorySpace * ms = make_MemorySpace(10000);

	/* let f be (lambda (x) (+ x 1)) */
	SchemeObject * plus = SchemeObject_make_atomic_function(ms, & sum_wrapper);
	SchemeObject * f = SchemeObject_make_list_n(ms, 3);
	SchemeObject_copy(ms, SchemeObject_list_ref_n(f, 1), SchemeObject_make_special_symbol_s(ms, LAMBDA));
	SchemeObject_copy(ms, SchemeObject_list_ref_n(f, 2), SchemeObject_make_pair(ms, SchemeObject_make_symbol(ms, "x"), SchemeObject_make_empty(ms))); 
	SchemeObject_copy(ms, SchemeObject_list_ref_n(f, 3), SchemeObject_make_pair(ms, plus, SchemeObject_make_pair(ms, SchemeObject_make_symbol(ms, "x"), SchemeObject_make_pair(ms, SchemeObject_make_integer(ms, 1), SchemeObject_make_empty(ms)))));	
	
	SchemeObject * L = SchemeObject_make_list_n(ms, 2);
	SchemeObject_copy(ms, SchemeObject_first(L), f);
	SchemeObject_copy(ms, SchemeObject_second(L), SchemeObject_make_integer(ms, -14));
	
	SchemeObject * env = SchemeObject_make_empty(ms);
	
	SchemeObject * output = SchemeObject_make_empty(ms);
	
	SchemeObject_print(L);
	printf("\n");
	
	SchemeObject * continuation = SchemeObject_make_exec_eval(ms, L, env, output, NULL);
	do {
		continuation = execute(ms, continuation);
		printf("main loop will execute : %p\n", continuation);
	} while(continuation != NULL);
	
	SchemeObject_print(output); // expecting -13
	printf("\n");
	
	assert(SchemeObject_eq(output, SchemeObject_make_integer(ms, -13)));
	
	ms->destroy(ms);
	return;
}

void test6() {
	printf("-----test6-----\n");
	MemorySpace * ms = make_MemorySpace(100000);
	SchemeObject * env = make_base_environment(ms);

	SchemeObject * L1 = SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "list"), SchemeObject_make_integer(ms, 3), SchemeObject_make_integer(ms, 4), SchemeObject_make_double(ms, 4.2));
	SchemeObject * L2 = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), L1);
	SchemeObject * L3 = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), L1);

	SchemeObject * L = SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "list"), L1, L2, L3);
	
	SchemeObject * output = SchemeObject_make_empty(ms);
	SchemeObject * continuation = SchemeObject_make_exec_eval(ms, L, env, output, NULL);
	do {
		continuation = execute(ms, continuation);
		printf("main loop will execute : %p\n", continuation);
	} while(continuation != NULL);
	
	SchemeObject_print(output);
	printf("\n");
	
	//assert(SchemeObject_eq(output, SchemeObject_make_integer(ms, -13)));
	
	ms->destroy(ms);
	return;
}

void test7() {
	printf("-----test7-----\n");
	MemorySpace * ms = make_MemorySpace(100000);
	SchemeObject * env = make_base_environment(ms);
	
	SchemeObject * L1 = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "readfile"), SchemeObject_make_string(ms, "lupi.txt"));
	SchemeObject * L2 = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "print"), L1);
	SchemeObject * L3 = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string-to-list"), L1);
	
	SchemeObject * L = SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "list"), L1, L2, L3);
	
	SchemeObject * output = SchemeObject_make_empty(ms);
	SchemeObject * continuation = SchemeObject_make_exec_eval(ms, L, env, output, NULL);
	do {
		continuation = execute(ms, continuation);
		printf("main loop will execute : %p\n", continuation);
	} while(continuation != NULL);
	
	SchemeObject_print(output);
	printf("\n");
	
	//assert(SchemeObject_eq(output, SchemeObject_make_integer(ms, -13)));
	
	ms->destroy(ms);
	return;
}

void test8() {
	printf("-----test8-----\n");
	MemorySpace * ms = make_MemorySpace(100000);
	SchemeObject * env = make_base_environment(ms);
	
	SchemeObject * L1 = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "analyze_atomic_string_token"), SchemeObject_make_string(ms, "lambda"));
	//SchemeObject * L1 = SchemeObject_make_empty(ms);
	SchemeObject * L2 = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "analyze_atomic_string_token"), SchemeObject_make_string(ms, "-21.1233"));
	SchemeObject * L3 = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "analyze_atomic_string_token"), SchemeObject_make_string(ms, "544888"));
	
	SchemeObject * L = SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "list"), L1, L2, L3);
	
	SchemeObject * output = SchemeObject_make_empty(ms);
	SchemeObject * continuation = SchemeObject_make_exec_eval(ms, L, env, output, NULL);
	do {
		continuation = execute(ms, continuation);
		printf("main loop will execute : %p\n", continuation);
	} while(continuation != NULL);
	
	SchemeObject_print(output);
	printf("\n");
	
	//assert(SchemeObject_eq(output, SchemeObject_make_integer(ms, -13)));
	
	ms->destroy(ms);
	return;
}

/* not really working alright, needs to be debugged */
void test9() {
	printf("-----test9-----\n");
	MemorySpace * ms = make_MemorySpace(100000);
	SchemeObject * env = SchemeObject_make_pair(ms, make_base_parser_frame(ms), make_base_environment(ms));
	
	SchemeObject * L = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "parse"), SchemeObject_make_string(ms, "(+ 1 2)"));
	
	SchemeObject_print(evaluate(ms, L, env));
	printf("\n");
	
	//assert(SchemeObject_eq(output, SchemeObject_make_integer(ms, -13)));
	
	ms->destroy(ms);
	return;
}

void test10() {
	printf("-----test10-----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_environment(ms);
	
	SchemeObject * L3 = SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), SchemeObject_make_special_symbol_s(ms, TRUE),
																								SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_integer(ms, 1), SchemeObject_make_integer(ms, 2)),
																								SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_integer(ms, 3), SchemeObject_make_integer(ms, 2)));
	SchemeObject * L5 = SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), SchemeObject_make_special_symbol_s(ms, FALSE),
												 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_integer(ms, 1), SchemeObject_make_integer(ms, 2)),
												 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_integer(ms, 3), SchemeObject_make_integer(ms, 2)));
	
	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 3), evaluate(ms, L3, env)));
	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 5), evaluate(ms, L5, env)));
	
	ms->destroy(ms);
	return;
}