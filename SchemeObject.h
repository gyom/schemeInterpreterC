/*
 *  SchemeObject.h
 *  scheme_compiler
 *
 */

#ifndef SCHEME_OBJECT_H
#define SCHEME_OBJECT_H

#include "MemorySpace.h"
#include <assert.h>
// for memcpy
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "SchemeObject_tests.h"

#define DEFAULT_MEMORY_FOR_MACHINE 100000

struct _SchemeObject;

typedef enum {INTEGER, DOUBLE, CHAR, PAIR, POINTER, MEMORY_CHUNK, SYMBOL, STRING, EMPTY, ATOMIC_FUNCTION, COMPOSITE_FUNCTION, EXEC_EVAL, EXEC_APPLY, SPECIAL_SYMBOL, CAPTURED_CONTINUATION, EXEC_EVALSEQ} DataType;

typedef enum {FALSE, TRUE, LAMBDA, QUOTE, SET, CALLCC, IF, DEFINE} SpecialSymbol;

typedef struct _ConsPair {
	struct _SchemeObject * car;
	struct _SchemeObject * cdr;	
} ConsPair;

typedef struct _LambdaTriplet {
	struct _SchemeObject * arg_symbols;
	struct _SchemeObject * body;
	struct _SchemeObject * enclosed_env;
} LambdaTriplet;

typedef struct _ExecEval {
	struct _SchemeObject * expr;
	struct _SchemeObject * env;
	struct _SchemeObject * output;
	struct _SchemeObject * continuation;
} ExecEval;

typedef struct _ExecApply {
	struct _SchemeObject * func;
	struct _SchemeObject * resolved_args;
	struct _SchemeObject * output;
	struct _SchemeObject * continuation;
} ExecApply;

typedef struct _CapturedContinuation {
	struct _SchemeObject * continuation;
	struct _SchemeObject * output;
} CapturedContinuation;

typedef struct _MemoryChunk {
	int size;
	void * data;
} MemoryChunk;

typedef union _Content {
	int val_integer;
	double val_double;
	ConsPair val_pair;
	void * val_pointer;
	char val_char;
	ExecApply val_apply;
	ExecEval val_eval;
	LambdaTriplet val_lambda;
	MemoryChunk val_memorychunk;
	SpecialSymbol val_special_symbol;
	CapturedContinuation val_capturedcontinuation;
} Content;

typedef struct _SchemeObject {
	DataType type;
	Content data;
	int garbage_collection_flag;
} SchemeObject;



int SchemeObject_get_integer(SchemeObject * E);
double SchemeObject_get_double(SchemeObject * E);
char SchemeObject_get_char(SchemeObject * E);

SchemeObject * SchemeObject_car(SchemeObject * P);
SchemeObject * SchemeObject_cdr(SchemeObject * P);
SchemeObject * SchemeObject_first(SchemeObject * P);
SchemeObject * SchemeObject_rest(SchemeObject * P);
SchemeObject * SchemeObject_second(SchemeObject * P);
SchemeObject * SchemeObject_third(SchemeObject * P);

SchemeObject * SchemeObject_make_integer(MemorySpace * ms, int value);
SchemeObject * SchemeObject_make_double(MemorySpace * ms, double value);
SchemeObject * SchemeObject_make_empty(MemorySpace * ms);
SchemeObject * SchemeObject_make_string(MemorySpace * ms, char * str);
SchemeObject * SchemeObject_make_symbol(MemorySpace * ms, char * str);
SchemeObject * SchemeObject_make_char(MemorySpace * ms, char c);
SchemeObject * SchemeObject_make_special_symbol_s(MemorySpace * ms, SpecialSymbol S);
SchemeObject * SchemeObject_make_pair(MemorySpace * ms, SchemeObject * left, SchemeObject * right);
SchemeObject * SchemeObject_make_atomic_function(MemorySpace * ms, void * ptr_C_impl);
SchemeObject * SchemeObject_make_composite_function(MemorySpace * ms, SchemeObject * arg_symbols, SchemeObject * body, SchemeObject * enclosed_env);
SchemeObject * SchemeObject_make_exec_apply(MemorySpace * ms, SchemeObject * func, SchemeObject * resolved_args, SchemeObject * output, SchemeObject * continuation);
SchemeObject * SchemeObject_make_exec_eval(MemorySpace * ms, SchemeObject * expr, SchemeObject * env, SchemeObject * output, SchemeObject * continuation);

int SchemeObject_length(SchemeObject * L);

/* these boolean functions look strange due to the lack of boolean values in C */
int SchemeObject_is_pair(SchemeObject * E);
int SchemeObject_is_integer(SchemeObject * E);
int SchemeObject_is_double(SchemeObject * E);
int SchemeObject_is_empty(SchemeObject * E);
int SchemeObject_is_symbol(SchemeObject * E);
int SchemeObject_is_string(SchemeObject * E);
int SchemeObject_is_char(SchemeObject * E);
int SchemeObject_eq(SchemeObject * x, SchemeObject * y);
int SchemeObject_eq_memorychunks(SchemeObject * x, SchemeObject * y);

int SchemeObject_is_atomic_function(SchemeObject * E);
int SchemeObject_is_composite_function(SchemeObject * E);
int SchemeObject_is_exec_apply(SchemeObject * E);
int SchemeObject_is_exec_eval(SchemeObject * E);
int SchemeObject_is_special_symbol(SchemeObject * E);
int SchemeObject_is_special_symbol_s(SchemeObject * E, SpecialSymbol S);
int SchemeObject_is_self_evaluating(SchemeObject * E);

void SchemeObject_print(SchemeObject * E);
int SchemeObject_copy(MemorySpace * ms, SchemeObject * dest, SchemeObject * source);
SchemeObject * SchemeObject_make_list_n(MemorySpace * ms, int length);
SchemeObject * SchemeObject_zip(MemorySpace * ms, SchemeObject * A, SchemeObject * B);
SchemeObject * SchemeObject_reverse_reference_list(MemorySpace * ms, SchemeObject * L);
SchemeObject * SchemeObject_list_ref_n(SchemeObject * L, int n);
SchemeObject * SchemeObject_make_bool(MemorySpace * ms, int E);
SchemeObject * SchemeObject_make_list_1(MemorySpace * ms, SchemeObject * A);
SchemeObject * SchemeObject_make_list_2(MemorySpace * ms, SchemeObject * A, SchemeObject * B);
SchemeObject * SchemeObject_make_list_3(MemorySpace * ms, SchemeObject * A, SchemeObject * B, SchemeObject * C);
SchemeObject * SchemeObject_make_list_4(MemorySpace * ms, SchemeObject * A, SchemeObject * B, SchemeObject * C, SchemeObject * D);
SchemeObject * SchemeObject_make_list_5(MemorySpace * ms, SchemeObject * A, SchemeObject * B, SchemeObject * C, SchemeObject * D, SchemeObject * E);

SchemeObject * frame_binding_lookup(SchemeObject * frame, SchemeObject * symbol);
SchemeObject * env_binding_lookup(SchemeObject * env, SchemeObject * symbol);
SchemeObject * env_value_lookup(SchemeObject * env, SchemeObject * symbol);
int env_set_existing_binding(MemorySpace * ms, SchemeObject * env, SchemeObject * symbol, SchemeObject * value);
SchemeObject * SchemeObject_make_single_binding(MemorySpace * ms, SchemeObject * symbol, SchemeObject * value);
SchemeObject * execute(MemorySpace * ms, SchemeObject * location);

SchemeObject * evaluate(MemorySpace * ms, SchemeObject * expr, SchemeObject * env);

/* adds all the elements from a list (a chain of pairs ending with an empty element) */
SchemeObject * sum_wrapper(MemorySpace *ms, SchemeObject * L);

SchemeObject * make_base_environment(MemorySpace * ms);
SchemeObject * make_base_parser_frame(MemorySpace * ms);

#endif


