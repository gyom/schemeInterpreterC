/*
 *  atomic_functions.c
 *  scheme_compiler
 */

#include "SchemeObject.h"
#include <stdlib.h>
#include <ctype.h> // for isdigit(char c);

SchemeObject * sum_wrapper(MemorySpace *ms, SchemeObject * L) {
	printf("called sum_wrapper\n");
	int i_total = 0;
	double d_total = 0;
	int using_integers = 1; // for lack of boolean
	
	SchemeObject * content;
	
	if ( !SchemeObject_is_pair(L) )
		return SchemeObject_make_empty(ms);
	else
		do {
			content = SchemeObject_car(L);
			// doesn't influence the original data in L, but just
			// a way to save on a variable and be more concise
			L = SchemeObject_cdr(L); 
			
			if (using_integers != 0) {
				if (SchemeObject_is_integer(content))
					i_total += SchemeObject_get_integer(content);
				else {
					assert(SchemeObject_is_double(content)); // the only other possibility
					using_integers = 0; // switch to double
					d_total = i_total; // transfer total
					d_total += SchemeObject_get_double(content); // count in double from then on
				}
			} else {
				assert(SchemeObject_is_double(content) || SchemeObject_is_integer(content));
				if (SchemeObject_is_double(content))
					d_total += SchemeObject_get_double(content);
				else if (SchemeObject_is_integer(content))
					d_total += SchemeObject_get_integer(content);
			}
		} while( !SchemeObject_is_empty(L) );
	return (using_integers ? SchemeObject_make_integer(ms, i_total) : SchemeObject_make_double(ms, d_total));
}

SchemeObject * eq_wrapper(MemorySpace *ms, SchemeObject * L) {
	assert(SchemeObject_length(L) >= 2);
	SchemeObject * A = SchemeObject_first(L);
	SchemeObject * B = SchemeObject_second(L);
	return SchemeObject_make_bool(ms, SchemeObject_eq(A, B));
}

SchemeObject * newline_wrapper(MemorySpace * ms, SchemeObject * L) {
	/* it might be possible to use the argument L to carry some output port (like a file that we write to) */
	printf("\n");
	return NULL;
}

SchemeObject * print_wrapper(MemorySpace * ms, SchemeObject * L) {
	while( SchemeObject_is_pair(L)) {
		SchemeObject_print(SchemeObject_car(L));
		L = SchemeObject_cdr(L);
	}
	return NULL;
}


SchemeObject * print_details_wrapper(MemorySpace * ms, SchemeObject * L) {
 	while( SchemeObject_is_pair(L)) {
		SchemeObject_print(SchemeObject_car(L));
		//SchemeObject_print_details(SchemeObject_car(L));
		L = SchemeObject_cdr(L);
	}
	return NULL;
}


SchemeObject * stringQ_wrapper(MemorySpace * ms, SchemeObject * L) {
	return SchemeObject_make_bool(ms, SchemeObject_is_string(SchemeObject_car(L)));
}

SchemeObject * numberQ_wrapper(MemorySpace * ms, SchemeObject * L) {
	return SchemeObject_make_bool(ms, SchemeObject_is_integer(SchemeObject_car(L)) || SchemeObject_is_double(SchemeObject_car(L)));
}

SchemeObject * symbolQ_wrapper(MemorySpace * ms, SchemeObject * L) {
	return SchemeObject_make_bool(ms, SchemeObject_is_symbol(SchemeObject_car(L)) );
}

SchemeObject * charQ_wrapper(MemorySpace * ms, SchemeObject * L) {
	return SchemeObject_make_bool(ms, SchemeObject_is_char(SchemeObject_car(L)));
}

SchemeObject * emptyQ_wrapper(MemorySpace * ms, SchemeObject * L) {
	return SchemeObject_make_bool(ms, SchemeObject_is_empty(SchemeObject_car(L)));
}

SchemeObject * car_wrapper(MemorySpace * ms, SchemeObject * L) {
	return SchemeObject_car(SchemeObject_car(L));
}

SchemeObject * cdr_wrapper(MemorySpace * ms, SchemeObject * L) {
	return SchemeObject_cdr(SchemeObject_car(L));
}

/* not really to be used, I guess */
SchemeObject * cons_wrapper(MemorySpace * ms, SchemeObject * L) {
	SchemeObject * E = SchemeObject_car(L);
	return SchemeObject_make_pair(ms, SchemeObject_first(E), SchemeObject_second(E));
}

/* I think that's how it works */
SchemeObject * list_wrapper(MemorySpace * ms, SchemeObject * L) {
	return L;
}

SchemeObject * readfile_wrapper(MemorySpace * ms, SchemeObject * L) {
	SchemeObject * E = SchemeObject_car(L);
	assert(SchemeObject_is_string(E));
	int len = E->data.val_memorychunk.size;
	assert(len!=0);

	char * filename = malloc(sizeof(char)*(len+1));
	strncpy(filename, E->data.val_memorychunk.data, len);
	filename[len] = '\0';
	FILE * fp = fopen(filename, "r");
	free(filename);
	
	if (!fp)
		return SchemeObject_make_string(ms, "");
	else {
		/* ripped from http://www.cplusplus.com/reference/clibrary/cstdio/fread/ */
		fseek(fp , 0 , SEEK_END);
		long lSize = ftell(fp);
		rewind(fp);

		char * str = malloc(sizeof(char)*(lSize+1));
		assert(str != NULL);
		
		fread(str, sizeof(char), lSize, fp);
		fclose(fp);
		str[lSize] = '\0';
		SchemeObject * result = SchemeObject_make_string(ms, str);
		free(str);
		return result;
	}
	return NULL;
}

SchemeObject * string_to_list_wrapper(MemorySpace * ms, SchemeObject * L) {
	SchemeObject * E = SchemeObject_car(L);
	assert(SchemeObject_is_string(E));
	int len = E->data.val_memorychunk.size;
	assert(len!=0);
	
	char * str = malloc(sizeof(char)*(len+1));
	strncpy(str, E->data.val_memorychunk.data, len);
	str[len] = '\0';
	
	SchemeObject * result = SchemeObject_make_empty(ms);
	int i;
	for( i=0; i<len; ++i)
		result = SchemeObject_make_pair(ms, SchemeObject_make_char(ms, str[len-1-i]), result);
	
	free(str);
	return result;
}

SchemeObject * list_to_string_wrapper(MemorySpace * ms, SchemeObject * L) {
	SchemeObject * E = SchemeObject_car(L);
	int len = SchemeObject_length(E);
	char * str = malloc(sizeof(char)*(len+1));
	str[len] = '\0';
	
	int i;
	for(i=0;i<len;++i) {
		assert(SchemeObject_is_pair(E));
		assert(SchemeObject_is_char(SchemeObject_car(E)));
		str[i] = SchemeObject_get_char(SchemeObject_car(E));
		E = SchemeObject_cdr(E);
	}
	
	SchemeObject * result = SchemeObject_make_string(ms, str);
	free(str);
	return result;
}

/* not copying objects here, like car and cdr */
SchemeObject * reverse_wrapper(MemorySpace * ms, SchemeObject * L) {
	SchemeObject * E = SchemeObject_car(L);
	SchemeObject * result = SchemeObject_make_empty(ms);
	while( SchemeObject_is_pair(E) ) {
		result = SchemeObject_make_pair(ms, SchemeObject_car(E), result);
		E = SchemeObject_cdr(E);
	}
	return result;
}

SchemeObject * or_wrapper(MemorySpace * ms, SchemeObject * L) {
	SchemeObject * E;
	while( SchemeObject_is_pair(L) ) {
		E = SchemeObject_car(L);
		if (SchemeObject_is_special_symbol_s(E, TRUE))
			return SchemeObject_make_special_symbol_s(ms, TRUE);
		L = SchemeObject_cdr(L);
	}
	return SchemeObject_make_special_symbol_s(ms, FALSE);
}

SchemeObject * and_wrapper(MemorySpace * ms, SchemeObject * L) {
	SchemeObject * E;
	while( SchemeObject_is_pair(L) ) {
		E = SchemeObject_car(L);
		if (SchemeObject_is_special_symbol_s(E, FALSE))
			return SchemeObject_make_special_symbol_s(ms, FALSE);
		L = SchemeObject_cdr(L);
	}
	return SchemeObject_make_special_symbol_s(ms, TRUE);
}

SchemeObject * analyze_atomic_string_token(MemorySpace * ms, SchemeObject * L) {
	SchemeObject * E = SchemeObject_car(L);
	assert(SchemeObject_is_string(E));
	int len = E->data.val_memorychunk.size;
	assert(len!=0);
	
	char * str = malloc(sizeof(char)*(len+1));
	strncpy(str, E->data.val_memorychunk.data, len);
	str[len] = '\0';
	
	SchemeObject * result;
	
	if (!strcmp(str, "lambda")) {
		result = SchemeObject_make_special_symbol_s(ms, LAMBDA);
	} else if (!strcmp(str, "if")) {
		result = SchemeObject_make_special_symbol_s(ms, IF);
	} else if (!strcmp(str, "#t")) {
		result = SchemeObject_make_special_symbol_s(ms, TRUE);
	} else if (!strcmp(str, "#f")) {
		result = SchemeObject_make_special_symbol_s(ms, FALSE);
	} else if (!strcmp(str, "call/cc")) {
		result = SchemeObject_make_special_symbol_s(ms, CALLCC);
	} else if (!strcmp(str, "set!")) {
		result = SchemeObject_make_special_symbol_s(ms, SET);
	} else if (!strcmp(str, "define")) {
		result = SchemeObject_make_special_symbol_s(ms, DEFINE);
	} else if (!strcmp(str, "quote")) {
		result = SchemeObject_make_special_symbol_s(ms, QUOTE);
	} else if (!strcmp(str, "#\\space")) {
		result = SchemeObject_make_char(ms, ' ');
	} else if (!strcmp(str, "#\\("))  {
		result = SchemeObject_make_char(ms, '(');
	} else if (!strcmp(str, "#\\)"))  {
		result = SchemeObject_make_char(ms, ')');
	} else if (!strcmp(str, "#\\newline")) {
		result = SchemeObject_make_char(ms, '\n');
	} else if (!strcmp(str, "#\\tab")) {
		result = SchemeObject_make_char(ms, '\t');
	} else if ( (len >= 2) && (str[0] == '\"') && (str[len-1] == '\"') ) {
		str[len-1] = '\0';
		result = SchemeObject_make_string(ms, str);
	} else if (isdigit(str[0]) || (str[0] == '-')) {
		if (atoi(str) == strtod(str, NULL))
			result = SchemeObject_make_integer(ms, atoi(str));
		else
			result = SchemeObject_make_double(ms, strtod(str, NULL));
	} else {
		result = SchemeObject_make_symbol(ms, str);
	}
	
	free(str);
	//printf("atomic conversion will yield ");
	//SchemeObject_print(result);
	//printf("\n");
	return result;
}

SchemeObject * make_atomic_functions_frame(MemorySpace * ms) {
	SchemeObject * frame = SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "+"), SchemeObject_make_atomic_function(ms, & sum_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_atomic_function(ms, & car_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_atomic_function(ms, & cdr_wrapper)),												   
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "symbol?"), SchemeObject_make_atomic_function(ms, & symbolQ_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string?"), SchemeObject_make_atomic_function(ms, & stringQ_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "char?"), SchemeObject_make_atomic_function(ms, & charQ_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "number?"), SchemeObject_make_atomic_function(ms, & numberQ_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "print"), SchemeObject_make_atomic_function(ms, & print_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "print-details"), SchemeObject_make_atomic_function(ms, & print_details_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_atomic_function(ms, & eq_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "list"), SchemeObject_make_atomic_function(ms, & list_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "newline"), SchemeObject_make_atomic_function(ms, & newline_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cons"), SchemeObject_make_atomic_function(ms, & cons_wrapper)),
   							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "or"), SchemeObject_make_atomic_function(ms, & or_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "and"), SchemeObject_make_atomic_function(ms, & and_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "string-to-list"), SchemeObject_make_atomic_function(ms, & string_to_list_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "list-to-string"), SchemeObject_make_atomic_function(ms, & list_to_string_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "readfile"), SchemeObject_make_atomic_function(ms, & readfile_wrapper)),
  							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "reverse"), SchemeObject_make_atomic_function(ms, & reverse_wrapper)),
   							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_atomic_function(ms, & emptyQ_wrapper)),
							SchemeObject_make_pair(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "analyze-atomic-string-token"), SchemeObject_make_atomic_function(ms, & analyze_atomic_string_token)),							   
							SchemeObject_make_empty(ms))))))))))))))))))))));
	
	return frame;
}

SchemeObject * make_base_environment(MemorySpace * ms) {
	SchemeObject * env = SchemeObject_make_empty(ms);
		
	return SchemeObject_make_pair(ms, make_atomic_functions_frame(ms), env);												   
}

SchemeObject * make_base_parser_frame(MemorySpace * ms) {

	/*	(define (whitespace? A)
	 		(or (eq? A #\space)
				(eq? A #\newline)
				(eq? A #\tab)))
	*/
	SchemeObject * define_whitespace = SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "whitespace?"),
																SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "A")),
																SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "or"),
																						 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "A"), SchemeObject_make_char(ms, ' ')),
																						 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "A"), SchemeObject_make_char(ms, '\n')),
																						 SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "A"), SchemeObject_make_char(ms, '\t'))
																						 )));
																																																																																		
	/*	(define (parse L)
	 		(if		(empty? L)
					empty
	 				(if	(eq? (car L) #\( )
	 					(iterated-read-next-token (cdr L))
	 					(analyze-atomic-string-token (list->string L)))
	 	)))	
	*/
	 
	SchemeObject * define_parse = 	SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "L")),
									SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_symbol(ms, "L")),
																												SchemeObject_make_empty(ms),
																												SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "L")), SchemeObject_make_char(ms, '(')),
																																															SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "iterated-read-next-token"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_symbol(ms, "L"))),
																																															SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "analyze-atomic-string-token"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "list->string"), SchemeObject_make_symbol(ms, "L")))
															)));

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
	
	SchemeObject * define_iterated_read_next_token = SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "L")),
															SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_symbol(ms, "L")),
																																		SchemeObject_make_empty(ms),
																																		SchemeObject_make_list_2(ms, SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA), SchemeObject_make_list_1(ms, SchemeObject_make_symbol(ms, "E")),
																																								/* body */
																																								SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "not"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "empty?"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "E")))),
																																																											SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "cons"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "parse"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "car"), SchemeObject_make_symbol(ms, "E"))), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "iterated-next-token"), SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "cdr"), SchemeObject_make_symbol(ms, "E")))),
																																														 													SchemeObject_make_empty(ms))),
																																									 /* value for E */ SchemeObject_make_list_5(ms, SchemeObject_make_symbol(ms, "read-next-token"), SchemeObject_make_empty(ms), SchemeObject_make_symbol(ms, "L"), SchemeObject_make_integer(ms, 0), SchemeObject_make_special_symbol_s(ms, TRUE))
																																															  )));
																												  
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
	
	SchemeObject * define_read_next_token = SchemeObject_make_list_3(ms, SchemeObject_make_special_symbol_s(ms, LAMBDA),
																	 	 SchemeObject_make_list_4(ms, SchemeObject_make_symbol(ms, "accum"), SchemeObject_make_symbol(ms, "L"), SchemeObject_make_symbol(ms, "parencount"), SchemeObject_make_symbol(ms, "init")),
																	/* body */
																	 SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "emtpy?"), SchemeObject_make_symbol(ms, "L")),
																							  													SchemeObject_make_list_2(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "reverse"), SchemeObject_make_symbol(ms, "accum")), SchemeObject_make_empty(ms)),
																							  													SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF), 	SchemeObject_make_symbol(ms, "init"),
																																										 													cond1,
																																										 													SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	SchemeObject_make_list_3(ms, SchemeObject_make_symbol(ms, "eq?"), SchemeObject_make_symbol(ms, "parencount"), SchemeObject_make_integer(ms, 0)),
																																																																										SchemeObject_make_list_4(ms, SchemeObject_make_special_symbol_s(ms, IF),	big_or,
																																																																																 													SchemeObject_make_list_2(ms, SchemeObject_make_list_2(ms, SchemeObject_make_symbol(ms, "reverse"), SchemeObject_make_symbol(ms, "accum")), SchemeObject_make_symbol(ms, "L")),
																																																																																													recur_ad_0_f),
																																																																										cond2))));
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
	
	return frame;
}
