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

void test9() {
	printf("-----test9 : whitespace? and empty? -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_parser_environment(ms);
	
	SchemeObject * Lt = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "whitespace?"), SchemeObject_make_char(ms, ' '));
	SchemeObject * Lf = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "whitespace?"), SchemeObject_make_char(ms, 'd'));
	assert(SchemeObject_eq(SchemeObject_make_special_symbol_s(ms, TRUE), evaluate(ms, Lt, env)));
	assert(SchemeObject_eq(SchemeObject_make_special_symbol_s(ms, FALSE), evaluate(ms, Lf, env)));
	
	assert(SchemeObject_eq(SchemeObject_make_special_symbol_s(ms, TRUE), SchemeObject_make_special_symbol_s(ms, TRUE)));
	assert(SchemeObject_eq(SchemeObject_make_special_symbol_s(ms, FALSE), SchemeObject_make_special_symbol_s(ms, FALSE)));
	
	SchemeObject * emptyQ_test_t = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_empty(ms));
	SchemeObject * emptyQ_test_f = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, QUOTE), SchemeObject_make_integer(ms, 634), SchemeObject_make_integer(ms, 997)));
	assert(SchemeObject_is_empty(SchemeObject_make_empty(ms)));
	assert(!SchemeObject_is_empty(SchemeObject_make_list_2(ms, SchemeObject_make_integer(ms, 1), SchemeObject_make_integer(ms, 1))));

	assert(SchemeObject_eq(SchemeObject_make_special_symbol_s(ms, TRUE), SchemeObject_make_bool(ms, 1)));
	assert(SchemeObject_eq(SchemeObject_make_special_symbol_s(ms, TRUE), SchemeObject_make_bool(ms, -1)));
	assert(SchemeObject_eq(SchemeObject_make_special_symbol_s(ms, FALSE), SchemeObject_make_bool(ms, 0)));
	
	assert(SchemeObject_eq(SchemeObject_make_special_symbol_s(ms, TRUE), evaluate(ms, emptyQ_test_t, env)));
	assert(SchemeObject_eq(SchemeObject_make_special_symbol_s(ms, FALSE), evaluate(ms, emptyQ_test_f, env)));
	
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

/* not used for anything except to check that recursion works like it should */
SchemeObject * make_basic_recursion_test_environment(MemorySpace * ms) {
	
	SchemeObject * env = SchemeObject_make_pair(ms, SchemeObject_make_empty(ms), make_base_environment(ms));

	/*
	(define f
	 	(lambda (x y) (if (eq? x 1)
					   	y
	 					(g (+ x -1) (* 2 y)))))
	*/
	SchemeObject * f = evaluate(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "x"), SchemeObject_make_symbol(ms, "y")),
															 	 SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "x"), SchemeObject_make_integer(ms, 1)),
																												  							SchemeObject_make_symbol(ms, "y"),
																																			SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "g"), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_integer(ms, -1), SchemeObject_make_symbol(ms, "x")), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "*"), SchemeObject_make_integer(ms, 2), SchemeObject_make_symbol(ms, "y")))
																						  )), env);
	/*
	 (define g
	 	(lambda (x y) (if (eq? x 1)
	 					y
						(f (+ x -1) (* 3 y)))))
	 */
	SchemeObject * g = evaluate(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "x"), SchemeObject_make_symbol(ms, "y")),
															 SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "x"), SchemeObject_make_integer(ms, 1)),
																					  SchemeObject_make_symbol(ms, "y"),
																					  SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "f"), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_integer(ms, -1), SchemeObject_make_symbol(ms, "x")), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "*"), SchemeObject_make_integer(ms, 3), SchemeObject_make_symbol(ms, "y")))
																					  )), env);
	
	
	SchemeObject * frame =	SchemeObject_make_list_2(ms, 	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "f"), f),
															SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "g"), g));
	
	SchemeObject_copy(ms, SchemeObject_list_ref_n(env, 1), frame);
	return env;
}

void test11() {
	printf("----- test11 : basic recursivity with hard-coded environment -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_basic_recursion_test_environment(ms);
	
	SchemeObject * A = SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "f"), SchemeObject_make_integer(ms, 5), SchemeObject_make_integer(ms, 3));
	SchemeObject * B = SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "f"), SchemeObject_make_integer(ms, 4), SchemeObject_make_integer(ms, -2));
												 
	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 108), evaluate(ms, A, env)));
	assert(SchemeObject_eq(SchemeObject_make_integer(ms, -24), evaluate(ms, B, env)));
	
	ms->destroy(ms);
	return;
}

void test12() {
	printf("----- test12 : more difficult lambda expressions to debug some problem with environment lookups -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_environment(ms);
	
	/*
	 	f = (lambda (X) (display X) ((lambda (X) (* 7 X)) (+ 1 X)))
	 */
	
	SchemeObject * f = evaluate(ms, SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "X")),
															 SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "display"), SchemeObject_make_symbol(ms, "X")),
															 SchemeObject_make_list_2(ms,
																					  SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "X")),
																											   SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "*"), SchemeObject_make_integer(ms, 7), SchemeObject_make_symbol(ms, "X"))),
																					  SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_integer(ms, 1), SchemeObject_make_symbol(ms, "X")))
															 ), env);
	
	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 14), evaluate(ms, SchemeObject_make_list_2(ms, f, SchemeObject_make_integer(ms, 1)), env)));
   	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 42), evaluate(ms, SchemeObject_make_list_2(ms, f, SchemeObject_make_integer(ms, 5)), env)));						   
	
	ms->destroy(ms);
	return;
}

void test13() {
	printf("----- test13 : define and set! -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_environment(ms);
	
	/*
	 f = (lambda () (define X -2) (define Y 7) (+ X Y))
	 */
	SchemeObject * f = evaluate(ms, SchemeObject_make_list_5(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_empty(ms),
															 SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, DEFINE), SchemeObject_make_symbol(ms, "X"), SchemeObject_make_integer(ms, -2)),
															 SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, DEFINE), SchemeObject_make_symbol(ms, "Y"), SchemeObject_make_integer(ms, 7)),
															 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_symbol(ms, "X"), SchemeObject_make_symbol(ms, "Y"))), env);
	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 5), evaluate(ms, SchemeObject_make_list_1(ms, f), env)));
	
	
	/*
	 g = (lambda () (define X -2) (display "X : " X) (newline) (set! X 7) (+ X X))
	 */
	SchemeObject * g = evaluate(ms, SchemeObject_make_list_7(ms,
															 SchemeObject_make_special_symbol_s(ms, LAMBDA),
															 SchemeObject_make_empty(ms),
															 SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, DEFINE), SchemeObject_make_symbol(ms, "X"), SchemeObject_make_integer(ms, -2)),
															 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "display"), SchemeObject_make_string(ms, "X : "), SchemeObject_make_symbol(ms, "X")),
															 SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "newline")),
															 SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, SET), SchemeObject_make_symbol(ms, "X"), SchemeObject_make_integer(ms, 7)),
															 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_symbol(ms, "X"), SchemeObject_make_symbol(ms, "X"))), env);
	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 14), evaluate(ms, SchemeObject_make_list_1(ms, g), env)));
	
	
	ms->destroy(ms);
	return;
}

void test14() {
	printf("-----test14 : counting characters -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_environment(ms);
	
	SchemeObject * definition = SchemeObject_make_list_3(ms, 	SchemeObject_make_special_symbol_s(ms, DEFINE),
																SchemeObject_make_symbol(ms, "count-char"),
														 		SchemeObject_make_list_3(ms,	SchemeObject_make_special_symbol_s(ms, LAMBDA),
																						 		SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "L"), SchemeObject_make_symbol(ms, "character"), SchemeObject_make_symbol(ms, "accum")),
																						 		SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_symbol(ms, "L")),
																														 												SchemeObject_make_symbol(ms, "accum"),
																																										SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "character"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L"))),
																																				  																								SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "count-char"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_symbol(ms, "L")), SchemeObject_make_symbol(ms, "character"), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_symbol(ms, "accum"), SchemeObject_make_integer(ms, 1))),
																																																												SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "count-char"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_symbol(ms, "L")), SchemeObject_make_symbol(ms, "character"), SchemeObject_make_symbol(ms, "accum"))
																																																 ))));
	
	SchemeObject * L = SchemeObject_make_list_1(ms, SchemeObject_make_list_5(ms, 	SchemeObject_make_special_symbol_s(ms, LAMBDA),
																			 		SchemeObject_make_empty(ms),
																			 		definition,
																					SchemeObject_make_list_3(ms, 	SchemeObject_make_special_symbol_s(ms, DEFINE),
																											  		SchemeObject_make_symbol(ms, "str"),
																											  		SchemeObject_make_string(ms, "Le baobab est barbu.")),
																			 		SchemeObject_make_list_4(ms,	SchemeObject_make_symbol(ms, "count-char"),
																													SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_symbol(ms, "str")),	
																													SchemeObject_make_char(ms, 'a'),
																											 		SchemeObject_make_integer(ms, 0))));
	

	assert(SchemeObject_eq(SchemeObject_make_integer(ms, 3), evaluate(ms, L, env)));

	ms->destroy(ms);
	return;
}

void test15() {
	printf("-----test15 : assert, list, cons, car, cdr, list->string, string->list -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_environment(ms);
	
	/*
	 (assert #t)
	 (assert (not #f))
	 */

	SchemeObject * L = SchemeObject_make_list_1(ms, SchemeObject_make_list_4(ms, 	SchemeObject_make_special_symbol_s(ms, LAMBDA),
																			 		SchemeObject_make_empty(ms),
																			 		SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "assert"), SchemeObject_make_special_symbol_s(ms, TRUE)),
																					SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "assert"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "not"), SchemeObject_make_special_symbol_s(ms, FALSE)))
																			 ));
	evaluate(ms, L, env);
	
	
	/*
	 (define A (cons 1 (cons 2 (cons 3 empty))))
	 (assert (eq? (car A) 1))
	 (assert (eq? (car (cdr A)) 2))
	 */
	L = SchemeObject_make_list_1(ms, SchemeObject_make_list_5(ms, 	SchemeObject_make_special_symbol_s(ms, LAMBDA),
															  SchemeObject_make_empty(ms),
															  SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, DEFINE), SchemeObject_make_symbol(ms, "A"), 	SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "cons"), SchemeObject_make_integer(ms, 1),
																																												SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "cons"), SchemeObject_make_integer(ms, 2),
																																												SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "cons"), SchemeObject_make_integer(ms, 3),
																																												SchemeObject_make_empty(ms))))),
															  SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "assert"), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "A")), SchemeObject_make_integer(ms, 1))),
															  SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "assert"), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_symbol(ms, "A"))), SchemeObject_make_integer(ms, 2)))
															  ));
	evaluate(ms, L, env);
	
	/*
	 (define A (list 1 2 3))
	 (assert (eq? (car A) 1))
	 (assert (eq? (car (cdr A)) 2))
	 */
	L = SchemeObject_make_list_1(ms, SchemeObject_make_list_5(ms, 	SchemeObject_make_special_symbol_s(ms, LAMBDA),
															  SchemeObject_make_empty(ms),
															  SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, DEFINE), SchemeObject_make_symbol(ms, "A"), SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "list"), SchemeObject_make_integer(ms, 1), SchemeObject_make_integer(ms, 2), SchemeObject_make_integer(ms, 3))),
															  SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "assert"), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "A")), SchemeObject_make_integer(ms, 1))),
															  SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "assert"), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_symbol(ms, "A"))), SchemeObject_make_integer(ms, 2)))
															  ));
	evaluate(ms, L, env);
	
	/*
	 (define A "chien  () lupi..")
	 (assert (eq? (list->string (string->list A)) A)) 
	 */
	L = SchemeObject_make_list_1(ms, SchemeObject_make_list_4(ms, 	SchemeObject_make_special_symbol_s(ms, LAMBDA),
															  SchemeObject_make_empty(ms),
															  SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, DEFINE), SchemeObject_make_symbol(ms, "A"), SchemeObject_make_string(ms, "chien  () lupi..")),
															  SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "assert"), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "list->string"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_symbol(ms, "A"))), SchemeObject_make_symbol(ms, "A")))
															  ));
	evaluate(ms, L, env);
	
	//assert(SchemeObject_eq(SchemeObject_make_integer(ms, 3), evaluate(ms, L, env)));
	
	ms->destroy(ms);
	return;
}

void test16() {
	printf("-----test16 : parse -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_parser_environment(ms);
	
	SchemeObject * L, * M;
	
	/*  (lambda (x) (+ 1 x)) -> (lambda (x) (+ 1 x))   */
	L = SchemeObject_make_list_5(ms, 	SchemeObject_make_symbol(ms,"read-next-token"),
								 		SchemeObject_make_empty(ms),
										SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, "(lambda (x) (+ 1 x))")),
										SchemeObject_make_integer(ms, 0),
								 		SchemeObject_make_special_symbol_s(ms, TRUE)
								 );
	M = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "list->string"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), L));
	
	assert(SchemeObject_eq(SchemeObject_make_string(ms, "(lambda (x) (+ 1 x))"), evaluate(ms, M, env)));
	
	/*  lambda (x) (+ 1 x)) -> lambda   */
	L = SchemeObject_make_list_5(ms, 	SchemeObject_make_symbol(ms,"read-next-token"),
								 SchemeObject_make_empty(ms),
								 SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, "lambda (x) (+ 1 x)")),
								 SchemeObject_make_integer(ms, 0),
								 SchemeObject_make_special_symbol_s(ms, TRUE)
								 );
	M = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "list->string"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), L));
	assert(SchemeObject_eq(SchemeObject_make_string(ms, "lambda"), evaluate(ms, M, env)));
	
	
	
	L = SchemeObject_make_list_2(ms, 	SchemeObject_make_symbol(ms,"parse"),
										SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, "(lambda (x) (+ 1 x))")));
	//SchemeObject_print(evaluate(ms, L, env));
	assert(SchemeObject_eq( SchemeObject_make_double(ms, 5.2877), evaluate(ms, SchemeObject_make_list_2(ms, evaluate(ms, L, env),	SchemeObject_make_double(ms, 4.2877)), env)));
	
	ms->destroy(ms);
	return;
}


void test_printing_base_parser() {
	printf("-----test printing_base_parser -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_parser_environment(ms);
	
	SchemeObject * L;
	
	L = evaluate(ms, SchemeObject_make_list_1(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_empty(ms), SchemeObject_make_symbol(ms, "parse"))), env);
	SchemeObject_print(L);
	printf("\n");
	SchemeObject_print(L->data.val_lambda.arg_symbols);
	printf("\n");
	SchemeObject_print(L->data.val_lambda.body);
	printf("\n");
	 
	L = evaluate(ms, SchemeObject_make_list_1(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_empty(ms), SchemeObject_make_symbol(ms, "iterated-read-next-token"))), env);	
	SchemeObject_print(L);
	printf("\n");
	SchemeObject_print(L->data.val_lambda.arg_symbols);
	printf("\n");
	SchemeObject_print(L->data.val_lambda.body);
	printf("\n");
	 
	L = evaluate(ms, SchemeObject_make_list_1(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_empty(ms), SchemeObject_make_symbol(ms, "read-next-token"))), env);	
	SchemeObject_print(L);
	printf("\n");
	SchemeObject_print(L->data.val_lambda.arg_symbols);
	printf("\n");
	SchemeObject_print(L->data.val_lambda.body);
	printf("\n");
	
	
	ms->destroy(ms);
	return;
}

/* first time we get the parser working with sn1t */
void test17() {
	printf("-----test17 : first parsed assertions -----\n");
	//MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	//SchemeObject * env = make_base_parser_environment(ms);
	
	/*
	 ((lambda ()
	 	(define A (cons 1 (cons 2 empty)))
	 	(define f (lambda (x y) (* x (+ y -7))))

	 	(assert (eq? (car A) 1))
		(assert (not (eq? (car A) 2)))
	 	(assert (eq? (car (cdr (car A))) 2))
	 	(assert (empty? empty))
	 	(assert (eq? 3 (+ 1 1 1)))
	 
	 	(assert (eq? (f (car A) 0) -7))
		(assert (eq? (f 12 (car A)) -72))
	
	 	(display "le Chien est con !")
	 	(newline)
	 	(display "le chien est encore con, mais il ne l'est pas.")
	 ))
	 */
	
	char * str = 	"((lambda ()											"
	"		(define A (cons 1 (cons 2 empty)))					"
	"		(define f (lambda (x y) (* x (+ y -7))))			"
	"															"
	"		(assert (eq? (car A) 1))							"
	"		(assert (not (eq? (car A) 2)))						"
	"		(assert (eq? (car (cdr A)) 2))				"
	"		(assert (empty? empty))								"
	"		(assert (eq? 3 (+ 1 1 1)))							"
	"															"
	"		(assert (eq? (f (car A) 0) -7))						"
	"		(assert (eq? (f 12 (car A)) -72))					"
	"															"
	"		(display \"le_Chien_est_con_!\")					"
	"		(newline)											"
	"		(display \"le_chien_est_encore_con.\")				"
	"		873423.43											"
	"))";											
		
	
	//char * str = "(list 1 2 3 (list 4 5) (list 6 7 8))";
	
	//char * str = "((lambda (x) (display \"hello\") (+ 1 x)) 7.2)";
	
	/*
	SchemeObject * L = SchemeObject_make_list_2(ms, 	SchemeObject_make_symbol(ms,"parse"),
														SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, str)));
	SchemeObject * compiled_code_Sn1m = evaluate(ms, L, env);
	evaluate(ms, compiled_code_Sn1m, env);
	*/
	
	MemorySpace * ms;
	SchemeObject * out = sn1t_automem_parse_evaluate(str, &ms);
	//SchemeObject_print(sn1t_automem_parse_evaluate(str, &ms));
	printf("\n");
	SchemeObject_print(out);
	printf("\n");
	
	ms->destroy(ms);
	return;
}

void test18() {
	printf("-----test18 : garbage collection -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_parser_environment(ms);
	
	char * str = "(list 1 2 (list \"chien\" 4 5) (quote arbre) (list 6 7))";
	
	SchemeObject * L = SchemeObject_make_list_2(ms, 	SchemeObject_make_symbol(ms,"parse"),
														SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, str)));

	SchemeObject * evaluated_L = evaluate(ms, L, env);
	MemorySpace * new_ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	
	SchemeObject * evaluated_L_t = GarbageCollection_floodfill_move_to_new_MemorySpace(new_ms, evaluated_L);
	SchemeObject * env_t = GarbageCollection_floodfill_move_to_new_MemorySpace(new_ms, env);
	
	printf("\n");
	printf("I   : ");	SchemeObject_print(evaluated_L);	printf("\n");
	printf("II  : ");	SchemeObject_print(evaluated_L_t);	printf("\n");
	printf("Ie  : ");	SchemeObject_print(evaluate(ms, evaluated_L, env)); 			printf("\n");
	
	printf("IIe : ");	SchemeObject_print(evaluate(new_ms, evaluated_L_t, env_t)); 	printf("\n");
	printf("\n");
	
	
	assert(GarbageCollector_stackabuse_checkisomorphism(evaluated_L, evaluated_L_t, 0, 100));
	//assert(GarbageCollector_stackabuse_checkisomorphism(env, env_t));
	//assert(SchemeObject_eq(evaluated_L, evaluated_L_t)); // won't work for lists
	
	ms->destroy(ms);
	new_ms->destroy(new_ms);
		   
	return;
}

/* not working alright, but I want to trim the code a bit */
void test19() {
	printf("-----test19 : garbage collection -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_parser_environment(ms);
	
	/*
	char * str = "(list 1 2 (list \"chien\" 4 5) (quote arbre) (list 6 7))";
	SchemeObject * L = SchemeObject_make_list_2(ms, 	SchemeObject_make_symbol(ms,"parse"),
												SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, str)));
	*/
	
	/* let's try this instead */
	char * str = "((lambda (x) (display \"allo\") (+ 1 x)) 7)";
	SchemeObject * L = SchemeObject_make_list_2(ms, 	SchemeObject_make_symbol(ms,"parse"),
												SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, str)));
	printf("will evaluate the basic expressions\n");
	SchemeObject * expr = evaluate(ms, L, env);	
	SchemeObject_print(L);		printf("\n");
	SchemeObject_print(expr);	printf("\n");
	
	SchemeObject * parsed_expr = SchemeObject_make_empty(ms);
	SchemeObject * continuation = SchemeObject_make_exec_eval(ms, expr, env, parsed_expr, NULL);
	//SchemeObject * continuation = SchemeObject_make_exec_eval(ms, expr, env, SchemeObject_make_empty(ms), NULL);
	
	int i=0; int threshold = 2;
	do {
		continuation = execute(ms, continuation);
		//printf("main loop will execute : %p\n", continuation);
		if (i++ > threshold) {
			printf("threshold reached, breaking with non-null continuation\n");
			break;
		}
	} while(continuation != NULL);
	
	MemorySpace * new_ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * data_to_track = SchemeObject_make_list_4(ms, expr, parsed_expr, continuation, SchemeObject_make_string(ms, "lupitron"));
	printf("data_to_track = %p, parsed_expr = %p, exec bloc output = %p\n", data_to_track, parsed_expr, SchemeObject_exec_get_output(continuation));
	SchemeObject * transferred_data_to_track = GarbageCollection_floodfill_move_to_new_MemorySpace(new_ms, data_to_track);
	SchemeObject * expr_t = SchemeObject_list_ref_n(transferred_data_to_track,1);
	SchemeObject * parsed_expr_t = SchemeObject_list_ref_n(transferred_data_to_track,2);
	SchemeObject * continuation_t = SchemeObject_list_ref_n(transferred_data_to_track,3);
	//SchemeObject * lupitron_t = SchemeObject_list_ref_n(transferred_data_to_track,4);
	assert(SchemeObject_eq(SchemeObject_list_ref_n(transferred_data_to_track,4), SchemeObject_list_ref_n(data_to_track,4)));
	printf("transferred_data_to_track = %p, parsed_expr_t = %p, exec bloc output _t = %p\n", transferred_data_to_track, parsed_expr_t, SchemeObject_exec_get_output(continuation_t));
	
	assert(GarbageCollector_stackabuse_checkisomorphism(expr, expr_t, 0, 10));
	assert(GarbageCollector_stackabuse_checkisomorphism(parsed_expr, parsed_expr_t, 0, 10));
	assert(GarbageCollector_stackabuse_checkisomorphism(continuation, continuation_t, 0, 10));
	//assert(GarbageCollector_stackabuse_checkisomorphism(env, env_t));
	//assert(SchemeObject_eq(evaluated_L, evaluated_L_t)); // won't work for lists
	
	SchemeObject_print_details(expr);		 	printf("\n");
	SchemeObject_print_details(expr_t); 		printf("\n");
	SchemeObject_print_details(continuation); 	printf("\n");
	SchemeObject_print_details(continuation_t); printf("\n");
	SchemeObject_print_details(parsed_expr); 	printf("\n");
	SchemeObject_print_details(parsed_expr_t); 	printf("\n");

	int iterations = 0;	int iterations_t = 0;
	SchemeObject * last_continuation;
	SchemeObject * last_continuation_t;
	printf("Attempting to finish the transferred computation after memory reallocation.\n");
	/*
	do {
		if(continuation != NULL) {
			last_continuation = continuation; // to monitor something
			continuation = execute(ms, continuation);
			iterations++;
		}
		if(continuation_t != NULL) {
			last_continuation_t = continuation_t; // to monitor something
			continuation_t = execute(new_ms, continuation_t);
			iterations_t++;
		}
		//assert(GarbageCollector_stackabuse_checkisomorphism(expr, expr_t, 0, 10));
		//assert(GarbageCollector_stackabuse_checkisomorphism(parsed_expr, parsed_expr_t, 0, 10));
		//assert(GarbageCollector_stackabuse_checkisomorphism(continuation, continuation_t, 0, 10));
	} while((continuation != NULL) || (continuation_t != NULL));
	*/
	do {
		if(continuation != NULL) {
			last_continuation = continuation; // to monitor something
			continuation = execute(ms, continuation);
			iterations++;
		}
	} while(continuation != NULL);
	ms->destroy(ms);
	do {
		if(continuation_t != NULL) {
			last_continuation_t = continuation_t; // to monitor something
			continuation_t = execute(new_ms, continuation_t);
			iterations_t++;
		}
	} while(continuation_t != NULL);
	SchemeObject_print_details(parsed_expr_t); 	printf("\n");

	assert(SchemeObject_eq(SchemeObject_list_ref_n(transferred_data_to_track,4), SchemeObject_list_ref_n(data_to_track,4)));
	//assert(SchemeObject_eq(SchemeObject_list_ref_n(transferred_data_to_track,3), SchemeObject_list_ref_n(data_to_track,3)));
	
	printf("iterations = %d, iterations_t = %d. Both should be the same value.\n", iterations, iterations_t);
	assert(iterations == iterations_t);
	
	printf("data_to_track = %p, parsed_expr = %p, exec bloc output = %p,  last exec bloc output = %p\n", data_to_track, parsed_expr, SchemeObject_exec_get_output(continuation), SchemeObject_exec_get_output(last_continuation));
	printf("transferred_data_to_track = %p, parsed_expr_t = %p, exec bloc output _t = %p, last exec bloc output _t = %p\n", transferred_data_to_track, parsed_expr_t, SchemeObject_exec_get_output(continuation_t), SchemeObject_exec_get_output(last_continuation_t));
	
	
	printf("last continuations : \n");
	SchemeObject_print_details(last_continuation);	 	printf("\n");
	SchemeObject_print_details(last_continuation_t); 	printf("\n");
	printf("expressions : \n");
	SchemeObject_print_details(expr); 	printf("\n");
	SchemeObject_print_details(expr_t); 	printf("\n");
	SchemeObject_print_details(parsed_expr); 	printf("\n");
	SchemeObject_print_details(parsed_expr_t); 	printf("\n");
	
	assert(GarbageCollector_stackabuse_checkisomorphism(expr, expr_t, 0, 10));
	assert(GarbageCollector_stackabuse_checkisomorphism(parsed_expr, parsed_expr_t, 0, 10));
	assert(GarbageCollector_stackabuse_checkisomorphism(continuation, continuation_t, 0, 10));
	
	ms->destroy(ms);
	new_ms->destroy(new_ms);
	
	return;
}

void test20() {
	printf("-----test20 : garbage collection, controled reallocation -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_parser_environment(ms);
	
	/*char * str = 	"((lambda ()											"
	"		(define A (cons 1 (cons 2 empty)))					"
	"		(define f (lambda (x y) (* x (+ y -7))))			"
	"															"
	"		(assert (eq? (car A) 1))							"
	"		(assert (not (eq? (car A) 2)))						"
	"		(assert (eq? (car (cdr (car A))) 2))				"
	"		(assert (empty? empty))								"
	"		(assert (eq? 3 (+ 1 1 1)))							"
	"															"
	"		(assert (eq? (f (car A) 0) -7))						"
	"		(assert (eq? (f 12 (car A)) -72))					"
	"															"
	"		(display \"le Chien est con !\")					"
	"		(newline)											"
	"		(display \"le chien est encore con.\")				"
	") )";	*/
	char * str = "((lambda (x) (display \"allo\") (+ 1 x)) 7)";
	SchemeObject * L = SchemeObject_make_list_2(ms, 	SchemeObject_make_symbol(ms,"parse"),
												SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, str)));
	printf("will evaluate the basic expressions\n");
	SchemeObject * expr = evaluate(ms, L, env);	
	SchemeObject_print(L);		printf("\n");
	SchemeObject_print(expr);	printf("\n");
	
	SchemeObject * output = SchemeObject_make_empty(ms);
	env = SchemeObject_make_pair(ms, SchemeObject_make_list_1(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "OUTPUT"), output)), env);
	SchemeObject * continuation = SchemeObject_make_exec_eval(ms, expr, env, output, NULL);
	//SchemeObject * continuation = SchemeObject_make_exec_eval(ms, expr, env, SchemeObject_make_empty(ms), NULL);
	
	int i=0; int threshold = 1;
	do {
		continuation = execute(ms, continuation);
		//printf("main loop will execute : %p\n", continuation);
		if (i++ > threshold) {
			printf("threshold reached, breaking with non-null continuation\n");
			break;
		}
	} while(continuation != NULL);
	
	MemorySpace * new_ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * data_to_track = SchemeObject_make_pair(ms, output, SchemeObject_make_pair(ms, continuation, SchemeObject_make_pair(ms, output, SchemeObject_make_empty(ms))));
	//SchemeObject * data_to_track = SchemeObject_make_list_3(ms, output, continuation, output);
	SchemeObject * data_to_track_t = GarbageCollection_floodfill_move_to_new_MemorySpace(new_ms, data_to_track);
	//continuation = GarbageCollection_floodfill_move_to_new_MemorySpace(new_ms, continuation);
	SchemeObject * output_t = SchemeObject_list_ref_n(data_to_track_t,1);
	continuation = SchemeObject_list_ref_n(data_to_track_t,2);
	SchemeObject * output_t2 = SchemeObject_list_ref_n(data_to_track_t,3);
	
	printf("data_to_track = %p\n", data_to_track);
	printf("data_to_track_t = %p\n", data_to_track_t);
	printf("output = %p\n", output);
	printf("output_t = %p\n", output_t);
	printf("output_t2 = %p\n", output_t2);
	ms->destroy(ms);
	ms = new_ms;
	
	SchemeObject * last_continuation;
	printf("Attempting to finish the transferred computation after memory reallocation.\n");
	do {
		if(continuation != NULL) {
			last_continuation = continuation; // to monitor something
			continuation = execute(ms, continuation);
		}
	} while(continuation != NULL);

	printf("last continuations : \n");
	SchemeObject_print_details(last_continuation);	 	printf("\n");
	
	SchemeObject_print_details(SchemeObject_exec_get_output(last_continuation));	printf("\n");
	//printf("expressions : \n");
	
	//env = make_base_parser_environment(ms);
	//evaluate(ms, SchemeObject_make_symbol(ms, "OUTPUT"), env);
	
	new_ms->destroy(new_ms);
	
	return;
}

/* first time we get the parser working, now let's use it with test17() to make sn1t work */
void test21() {
	printf("-----test21 : garbage collection again, simplified -----\n");
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	SchemeObject * env = make_base_parser_environment(ms);
	
	char * str = 	"((lambda ()											"
	 "		(define A (cons 1 (cons 2 empty)))					"
	 "		(define f (lambda (x y) (* x (+ y -7))))			"
	 "															"
	 "		(assert (eq? (car A) 1))							"
	 "		(assert (not (eq? (car A) 2)))						"
	 "		(assert (eq? (car (cdr A)) 2))				"
	 "		(assert (empty? empty))								"
	 "		(assert (eq? 3 (+ 1 1 1)))							"
	 "															"
	 "		(assert (eq? (f (car A) 0) -7))						"
	 "		(assert (eq? (f 12 (car A)) -72))					"
	 "															"
	 "		(display \"le_Chien_est_con_!\")					"
	 "		(newline)											"
	 "		(display \"le_chien_est_encore_con.\")				"
	 "))";
	
	/*char * str = 	"((lambda (W)											"
		 "		(define A (cons 1 (cons 2 empty)))					"
	"		(assert (eq? (car A) 1))							"
	"		(assert (not (eq? (car A) 2)))						"
	"		(display \"le_Chien_est_con_!\")					"
	"		(newline)											"
	"		(display A)				"
	"		(newline)											"
	") (cons 1 (cons 2 empty)))";*/
	
	//char * str = "((lambda (x) (display \"allo\") (+ 1 x)) 7)";
	SchemeObject * L = SchemeObject_make_list_2(ms, 	SchemeObject_make_symbol(ms,"parse"),
												SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, str)));
	printf("will evaluate the basic expressions\n");
	
	MemorySpace * new_ms;
	SchemeObject * data_to_track;	SchemeObject * data_to_track_t;
	
	SchemeObject * continuation;
	// Now we want to have a loop that essentially evaluates this.
	//	SchemeObject * expr = evaluate(ms, L, env);
	SchemeObject * expr = SchemeObject_make_empty(ms);
	continuation = SchemeObject_make_exec_eval(ms, L, env, expr, NULL);
	do {
		continuation = execute(ms, continuation);
		//printf("main loop will execute : %p\n", continuation);
		if (ms->needs_reallocation(ms)) {
			printf("reallocating. ");
			new_ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
			data_to_track = SchemeObject_make_pair(ms, expr, SchemeObject_make_pair(ms, continuation, SchemeObject_make_empty(ms)));
			data_to_track_t = GarbageCollection_floodfill_move_to_new_MemorySpace(new_ms, data_to_track);
			expr = SchemeObject_list_ref_n(data_to_track_t,1);
			continuation = SchemeObject_list_ref_n(data_to_track_t,2);
			
			ms->destroy(ms);
			ms = new_ms;
			printf("now using %d bytes.\n", ms->n_chunks_used);
		}
	} while(continuation != NULL);
	
	//SchemeObject_print(L);		printf("\n");	//it's gone
	printf("expr : ");	SchemeObject_print(expr);	printf("\n");
	printf("expr in details : ");	SchemeObject_print_details(expr);	printf("\n");
	
	env = make_base_parser_environment(ms);
	SchemeObject * output = SchemeObject_make_empty(ms);
	continuation = SchemeObject_make_exec_eval(ms, expr, env, output, NULL);
	
	do {
		continuation = execute(ms, continuation);
		//printf("main loop will execute : %p\n", continuation);
		if (ms->needs_reallocation(ms)) {
			printf("reallocating. ");
			new_ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
			data_to_track = SchemeObject_make_pair(ms, output, SchemeObject_make_pair(ms, continuation, SchemeObject_make_empty(ms)));
			data_to_track_t = GarbageCollection_floodfill_move_to_new_MemorySpace(new_ms, data_to_track);
			output = SchemeObject_list_ref_n(data_to_track_t,1);
			continuation = SchemeObject_list_ref_n(data_to_track_t,2);

			//printf("data_to_track = %p\n", data_to_track);
			//printf("data_to_track_t = %p\n", data_to_track_t);
			//printf("output = %p\n", output);
			ms->destroy(ms);
			ms = new_ms;
			printf("now using %d bytes.\n", ms->n_chunks_used);
		}
	} while(continuation != NULL);
	
	SchemeObject_print_details(output);		printf("\n");
	
	ms->destroy(ms);
	
	return;
}

void test22() {
	printf("-----test22 : testing continuations -----\n");
	
	/*
	 	((lambda ()
	 		(define f	(lambda (x) (not (eq? x 3))))
	 		(define find (lambda (L f k)
	 							(if 	(empty? L)
										empty
										(if (f (car L))
	 										(k (car L))
	 										(find (cdr L) f k)))))
	 		(define L (list 3 3 3 3 -1 3 3 87.2 3 3))
	 		(define result (call/cc (lambda (k) (find L f k))))
			(display result)
	 		(newline)
	 		result
		 ))
	 */
	
	char * str = 	"((lambda ()"
	" (define f	(lambda (x) (not (eq? x 3))))"
	" (define find (lambda (L f k)"
	" (if 	(empty? L) "
	" empty"
	" (if (f (car L))"
	" (k (car L))"
	" (find (cdr L) f k)))))"
	" (define L (list 3 3 3 3 -1 3 3 87.2 3 3))"
	" (define result (call/cc (lambda (k) (find L f k))))"
	" (display result)"
	" (newline)"
	" result"
	" ))";											
	
	/*char * str = "((lambda ()"
		"(call/cc (lambda (k) (k 3372.20)))"
	"))";*/
	
	/*char * str = "((lambda (f)"
	 "(display (meminfo))"
	 "(f f)"
	 ") (call/cc (lambda (k) (k k))) )";*/
	
	MemorySpace * ms;
	SchemeObject * out = sn1t_automem_parse_evaluate(str, &ms);

	printf("\n");
	SchemeObject_print(out);
	printf("\n");
	
	ms->destroy(ms);
	return;
}

void test23() {
	printf("-----test23 : bending continuations further -----\n");
	
	/*
	 (define map
	 	(lambda (f L)
	 			(if (empty? L)
					 empty
					 (cons (f (car L)) (map f (cdr L))))))
	 
	 (define h
		 (lambda ()
			(define A
				(call/cc (lambda (throw)
					(define f
	 					(lambda (x)
	 						(if (eq? x 0)
	 							(call/cc (lambda (continue) (throw continue)))
	 							(* 7 x))))
	 							(map f (list 1 0 2 3 0)))))
	 							(if (continuation? A)
	 								(A 'lupi)
	 								A)))
	 */
	
    char * str = "((lambda ()												"
    "       (define map														"
    "		    (lambda (f L)												"
    "			    (if (empty? L)											"
    "				    empty												"
    "                   (cons (f (car L)) (map f (cdr L))))))				"
    "																		"
    "       (define h														"
    "           (lambda ()													"
    "               (define A												"
    "                   (call/cc (lambda (throw)							"
    "                       (define f										"
    "                           (lambda (x)									"
    "                               (if (eq? x 0)							"
    "	                                (call/cc (lambda (continue)         "
    "                                               (throw continue)))      "
    "	                                (* 7 x))))							"
    "                       (map f (list 1 0 2 3 0)))))						"
    "               (if (continuation? A)									"
    "   			    (A -1)												"
    "					A)))												"
    "		(h)																"
    "))                                                                     ";
	
	MemorySpace * ms;
	SchemeObject * out = sn1t_automem_parse_evaluate(str, &ms);
	
	printf("\n");
	SchemeObject_print(out);
	printf("\n");
	
	ms->destroy(ms);
	return;
}