/*
 *  s1nt_parsing.c
 *  scheme_compiler
 */

#include "SchemeObject.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {PARSE, EVALUATE, DONE} Phase;

SchemeObject * sn1t_to_sn1m_make_environment(MemorySpace * ms) {
	
	/* 	We just need the most trivial of environments to evaluate this and get the right functions for
	 things like "+" and "and". However, since we're dealing with recursive definitions, we also need
	 all the definitions to be in the environment supplied. This is why we have to do this little trick here.
	 We haven't coded (define ...) yet, so we can't use it.
	 */
	SchemeObject * env = SchemeObject_make_pair(ms, SchemeObject_make_empty(ms), make_base_environment(ms));
	
	/*	(define (whitespace? A)
	 (or (eq? A #\space)
	 (eq? A #\newline)
	 (eq? A #\tab)))
	 */
	SchemeObject * define_whitespace = 	evaluate(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "A")),
																			  SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "or"),
																									   SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "A"), SchemeObject_make_char(ms, ' ')),
																									   SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "A"), SchemeObject_make_char(ms, '\n')),
																									   SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "A"), SchemeObject_make_char(ms, '\t'))
																									   )), env);
	
	/*	(define (parse L)
	 (if		(empty? L)
	 empty
	 (if	(eq? (car L) #\( )
	 (iterated-read-next-token (cdr L))
	 (analyze-atomic-string-token (list->string L)))
	 )))	
	 */
	
	SchemeObject * define_parse = 	evaluate(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "L")),
																		  SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_symbol(ms, "L")),
																								   SchemeObject_make_empty(ms),
																								   SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L")), SchemeObject_make_char(ms, '(')),
																															SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "iterated-read-next-token"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_symbol(ms, "L"))),
																															SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "analyze-atomic-string-token"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "list->string"), SchemeObject_make_symbol(ms, "L")))
																															))), env);
	
	/*
	 (define (iterated-read-next-token L)
	 (if 	(empty? L)
	 empty
	 (let* ((E (read-next-token '() L 0 #t))
	 (token (car E))
	 (rest (cadr E)))
	 (if (not (empty? token))
	 (cons (parse token) (iterated-read-next-token rest))
	 empty))))
	 */
	
	SchemeObject * define_iterated_read_next_token = evaluate(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "L")),
																						   SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_symbol(ms, "L")),
																													SchemeObject_make_empty(ms),
																													SchemeObject_make_list_2(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "E")),
																																										  /* body */
																																										  SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "not"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "E")))),
																																																   SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "cons"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "parse"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "E"))), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "iterated-read-next-token"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_symbol(ms, "E"))))),
																																																   SchemeObject_make_empty(ms))),
																																			 /* value for E */ SchemeObject_make_list_5(ms, SchemeObject_make_symbol(ms, "read-next-token"), SchemeObject_make_empty(ms), SchemeObject_make_symbol(ms, "L"), SchemeObject_make_integer(ms, 0), SchemeObject_make_special_symbol_s(ms, TRUE))
																																			 ))), env);
	
	/*	Let's use a few shortcuts to expres the "read-next-token" function better.
	 
	 added : 		(cons (car L) accum))
	 remaining : 	(cdr L)
	 recur_ad_1_f :	(read-next-token added remaining (+ parencount 1) #f)
	 recur_ac_0_t :	(read-next-token accum remaining parencount #t)
	 recur_ad_0_f :	(read-next-token added remaining parencount #f)
	 recur_ad_m1_f :	(read-next-token added remaining (- parencount 1) #f)
	 */
	SchemeObject * added = SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "cons"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L")), SchemeObject_make_symbol(ms, "accum"));
	SchemeObject * remaining = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_symbol(ms, "L"));
	SchemeObject * recur_ad_1_f = SchemeObject_make_list_5(ms, SchemeObject_make_symbol(ms, "read-next-token"), added, remaining, SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_symbol(ms, "parencount"), SchemeObject_make_integer(ms, 1)), SchemeObject_make_special_symbol_s(ms, FALSE));
	SchemeObject * recur_ac_0_t = SchemeObject_make_list_5(ms, SchemeObject_make_symbol(ms, "read-next-token"), SchemeObject_make_symbol(ms, "accum"), remaining, SchemeObject_make_symbol(ms, "parencount"), SchemeObject_make_special_symbol_s(ms, TRUE));
	SchemeObject * recur_ad_0_f = SchemeObject_make_list_5(ms, SchemeObject_make_symbol(ms, "read-next-token"), added, remaining, SchemeObject_make_symbol(ms, "parencount"), SchemeObject_make_special_symbol_s(ms, FALSE));
	SchemeObject * recur_ad_m1_f = SchemeObject_make_list_5(ms, SchemeObject_make_symbol(ms, "read-next-token"), added, remaining, SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_symbol(ms, "parencount"), SchemeObject_make_integer(ms, -1)), SchemeObject_make_special_symbol_s(ms, FALSE));
	
	/*	Expressing this part as "cond1".
	 (cond 	((eq? (car L) #\() recur_ad_1_f)
	 ((eq? (car L) #\)) recur_ac_0_t)
	 ((whitespace? (car L)) recur_ac_0_t)
	 (#t recur_ad_0_f))
	 
	 ->	(if (eq? (car L) #\()
	 recur_ad_1_f
	 (if (eq? (car L) #\))
	 recur_ac_0_t
	 (if (whitespace? (car L))
	 recur_ac_0_t
	 recur_ad_0_f)))
	 */
	
	SchemeObject * cond1 = SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L")), SchemeObject_make_char(ms, '(')),
													recur_ad_1_f,
													SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L")), SchemeObject_make_char(ms, ')')),
																			 recur_ac_0_t,
																			 SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "whitespace?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L"))),
																									  recur_ac_0_t,
																									  recur_ad_0_f)));
	/* Expressing this part as "cond2".
	 (cond 	((eq? (car L) #\( ) recur_ad_1_f)
	 ((eq? (car L) #\) ) recur_ad_m1_f)
	 (#t recur_ad_0_f)
	 
	 ->	(if	(eq? (car L) #\( )
	 recur_ad_1_f
	 (if	(eq? (car L) #\) )
	 recur_ad_m1_f
	 recur_ad_0_f))
	 */
	SchemeObject * cond2 = SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L")), SchemeObject_make_char(ms, '(')),
													recur_ad_1_f,
													SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L")), SchemeObject_make_char(ms, ')')),
																			 recur_ad_m1_f,
																			 recur_ad_0_f));
	
	/* Expressing this as "big_or".
	 (or (whitespace? (car L)) (eq? #\) (car L))	(eq? #\( (car L)))
	 */
	SchemeObject * big_or = SchemeObject_make_list_4(ms,	SchemeObject_make_symbol(ms, "or"),
													 SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "whitespace?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L"))),
													 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_char(ms, ')'), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L"))),
													 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_char(ms, '('), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L"))));
	
	/*
	 (define (read-next-token accum L parencount init)
	 (if (empty? L)
	 (list (reverse accum) '())
	 (if init
	 *cond1*
	 (if (eq? parencount 0)
	 (if *big_or*
	 (list (reverse accum) L)
	 recur_ad_0_f)
	 *cond2*
	 ))))))
	 */
	
	SchemeObject * define_read_next_token = evaluate(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA),
																				  SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "accum"), SchemeObject_make_symbol(ms, "L"), SchemeObject_make_symbol(ms, "parencount"), SchemeObject_make_symbol(ms, "init")),
																				  /* body */
																				  SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_symbol(ms, "L")),
																										   SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "list"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "reverse"), SchemeObject_make_symbol(ms, "accum")), SchemeObject_make_empty(ms)),
																										   SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_symbol(ms, "init"),
																																	cond1,
																																	SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "parencount"), SchemeObject_make_integer(ms, 0)),
																																							 SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	big_or,
																																													  SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "list"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "reverse"), SchemeObject_make_symbol(ms, "accum")), SchemeObject_make_symbol(ms, "L")),
																																													  recur_ad_0_f),
																																							 cond2)))), env);
	/* 	I have to call eval on the quantities on the right, here, because they only get evaluated once.
	 For example, if I had used the "whitespace?" function, it'd be fetched from the environment, but not
	 evaluated again (after its evaluation brought the definition from the environment).
	 
	 We can't have that, so we have to evaluate the thing before defining the environment.
	 */
	
	SchemeObject * frame =	SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty"), SchemeObject_make_empty(ms)),
												   SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "whitespace?"), define_whitespace),
																		  SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "read-next-token"), define_read_next_token),
																								 SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "parse"), define_parse),
																														SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "iterated-read-next-token"), define_iterated_read_next_token),
																																			   SchemeObject_make_empty(ms)   )))));
	
	SchemeObject_copy(ms, SchemeObject_list_ref_n(env, 1), frame);
	
	return env;
}

/* 	That MemorySpace argument is used for output, so that we can free the memory space
 	containing the returned object eventually. The output is a SchemeObject and not a
 	SchemeObject* because I can't have the output be on a memory space thrown away.
 
 	I could copy it to the end of the array (an allocated place on the MemorySpace),
 	but I'm not sure I can convince myself that it won't have references to it floating
 	around.
 */
SchemeObject * sn1t_automem_parse_evaluate(char * str, MemorySpace ** out_ms) {

	if (str == NULL) {
		printf("NULL string passed to sn1t_automem_parse_evaluate\n");	
		return NULL;
	}
	
	if (str[0] != '(') {
		printf("It's stupid, but the base parser that we wrote can only parse strings that start with an opening parenthesis (.\n");	
		printf("Just do that please. Aborting execution.\n");
		return NULL;
	}
	
	SchemeObject * env;
	MemorySpace * ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
	MemorySpace * new_ms;
	//SchemeObject * env = sn1t_to_sn1m_make_environment(ms);
	
	SchemeObject * expr = SchemeObject_make_list_2(ms, 	SchemeObject_make_symbol(ms,"parse"),
												SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string->list"), SchemeObject_make_string(ms, str)));
	
	SchemeObject * parsed_expr = SchemeObject_make_empty(ms);
	SchemeObject * output = SchemeObject_make_empty(ms);
	SchemeObject * continuation; // = SchemeObject_make_exec_eval(ms, expr, env, parsed_expr, NULL);
	
	SchemeObject * data_to_track;
	SchemeObject * transferred_data_to_track;
	
	Phase phase = PARSE;
	/* 	The output is saved because we wrap both the output and the current continuation
	 	into a list of two elements before calling the garbage collection step. This way,
	 	we track two elements instead of just one using an algorithm that's essentially just
	 	tracking one explicitly. Naturally, we get a new list of two elements, but that list
	 	is now allocated on the new memory space.
	 */
	while(phase != DONE) {
		if (phase == PARSE) {
			#ifdef DEBUG_SN1T
			printf("Starting phase PARSE.\nexpr = ");
			SchemeObject_print_details(expr);
			printf("\n");
			#endif
			env = sn1t_to_sn1m_make_environment(ms);
			continuation = SchemeObject_make_exec_eval(ms, expr, env, parsed_expr, NULL);
			phase = EVALUATE; //increment now or at the end ... the value isn't used until next iteration
		} else if (phase == EVALUATE) {
			#ifdef DEBUG_SN1T
			printf("Starting phase EVALUATE.\nparsed_expr = ");
			SchemeObject_print_details(parsed_expr);
			printf("\n");
			#endif
			env = sn1t_to_sn1m_make_environment(ms);
			continuation = SchemeObject_make_exec_eval(ms, parsed_expr, env, output, NULL);
			phase = DONE;
		}

		do {
			if (ms->needs_reallocation(ms)) {

				new_ms = make_MemorySpace(DEFAULT_MEMORY_FOR_MACHINE);
				data_to_track = SchemeObject_make_pair(ms, expr, SchemeObject_make_pair(ms, parsed_expr, SchemeObject_make_pair(ms, output, SchemeObject_make_pair(ms, continuation, SchemeObject_make_empty(ms)))));
				transferred_data_to_track = GarbageCollection_floodfill_move_to_new_MemorySpace(new_ms, data_to_track);
				/*	The reason why we need to keep the 'parsed_expr' is that it's easy to find ourselves
				 	otherwise with the parsed code floating somewhere. We really need to keep track of it
				 	so that reallocations are done properly during the PARSE phase.
				 */
				expr = SchemeObject_list_ref_n(transferred_data_to_track,1);
				parsed_expr = SchemeObject_list_ref_n(transferred_data_to_track,2);
				output = SchemeObject_list_ref_n(transferred_data_to_track,3);
				continuation = SchemeObject_list_ref_n(transferred_data_to_track,4);
				ms->destroy(ms);
				ms = new_ms;
			}
			continuation = execute(ms, continuation);
		} while(continuation != NULL);
	}
	
	/* Don't free ms at the end. */
	*out_ms = ms;
	return output;
}