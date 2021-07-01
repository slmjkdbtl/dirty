// wengwengweng

// TODO: garbage collection
// TODO: error handling
// TODO: ... to access func args
// TODO: 'this'
// TODO: trait
// TODO: module
// TODO: threading
// TODO: utf8
// TODO: named func
// TODO: binding helpers
// TODO: expose tokenizer
// TODO: standalone bytecode
// TODO: type
// TODO: string interpolation
// TODO: single ctx handle
// TODO: tail call opti
// TODO: str escape
// TODO: loop return val
// TODO: switch
// TODO: str intern
// TODO: 3 <= a < 7
// TODO: < ... > for map and make block expr?
// TODO: iter with index
// TODO: func <-> cfunc
// TODO: separate debug print / actual print
// TODO: ordered map ?
// TODO: incremental gc

#ifndef D_LANG_H
#define D_LANG_H

// vm value stack max size
#define DT_STACK_MAX 2048
// dt_arr minimum init size
#define DT_ARR_INIT_SIZE 4
// dt_map minimum init size
#define DT_MAP_INIT_SIZE 8
// dt_map expand rate (expand when cnt > cap * rate)
#define DT_MAP_MAX_LOAD 0.75
// trigger gc when memory reached this threshold
#define DT_GC_THRESHOLD 1024 * 1024
// gc threshold ^ grow rate
#define DT_GC_GROW 2

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// value types
typedef enum {
	DT_VAL_NIL,
	DT_VAL_BOOL,
	DT_VAL_NUM,
	DT_VAL_STR,
	DT_VAL_ARR,
	DT_VAL_MAP,
	DT_VAL_RANGE,
	DT_VAL_FUNC,
	DT_VAL_CFUNC,
	DT_VAL_CDATA,
	DT_VAL_CPTR,
	// TODO
	DT_VAL_UPVAL,
	// TODO
	DT_VAL_LOGIC,
} dt_val_ty;

// heap value header (define as macro for padding filling opportunity)
#define DT_HEAPER_STRUCT \
	struct dt_heaper *nxt; \
	uint8_t type; \
	bool marked;

typedef double dt_num;
typedef bool dt_bool;
struct dt_map;
struct dt_arr;
struct dt_str;
struct dt_vm;
struct dt_logic;
struct dt_func;

// TODO: should this be a value type?
// for easy loops and sub sections in lists
typedef struct {
	int16_t start;
	int16_t end;
} dt_range;

#ifdef DT_NO_NANBOX

struct dt_val;
typedef struct dt_val (dt_cfunc)(struct dt_vm* vm, int nargs);

typedef struct dt_val {
	dt_val_ty type;
	union {
		dt_num num;
		dt_bool boolean;
		dt_range range;
		struct dt_str* str;
		struct dt_arr* arr;
		struct dt_map* map;
		struct dt_func* func;
		struct dt_logic* logic;
		dt_cfunc* cfunc;
		struct dt_cdata* cdata;
		struct dt_cptr* cptr;
	} data;
} dt_val;

dt_val DT_NIL = (dt_val) {
	.type = DT_VAL_NIL,
	.data.num = 0,
};

dt_val DT_TRUE = (dt_val) {
	.type = DT_VAL_BOOL,
	.data.boolean = true,
};

dt_val DT_FALSE = (dt_val) {
	.type = DT_VAL_BOOL,
	.data.boolean = false,
};

#else

typedef uint64_t dt_val;

// IEEE 754 "double" bit segments (for nan boxing)
#define DT_SMASK_SIGN  0x8000000000000000
#define DT_SMASK_EXPO  0x7ff0000000000000
#define DT_SMASK_QUIET 0x0008000000000000
#define DT_SMASK_SIG   0xffff000000000000
#define DT_SMASK_PTR   0x0000ffffffffffff
#define DT_SMASK_RANGE 0x00000000ffffffff

// value type
#define DT_TMASK_NAN   0x0000000000000000
#define DT_TMASK_FALSE 0x0001000000000000
#define DT_TMASK_TRUE  0x0002000000000000
#define DT_TMASK_NIL   0x0003000000000000
#define DT_TMASK_RANGE 0x0004000000000000
#define DT_TMASK_HEAP  0x0005000000000000
#define DT_TMASK_LOGIC 0x0007000000000000
#define DT_TMASK_CFUNC 0x8000000000000000
#define DT_TMASK_CDATA 0x8001000000000000
#define DT_TMASK_CPTR  0x8002000000000000

// value constants
#define DT_NAN   (DT_SMASK_EXPO | DT_SMASK_QUIET)
#define DT_FALSE (DT_NAN | DT_TMASK_FALSE)
#define DT_TRUE  (DT_NAN | DT_TMASK_TRUE)
#define DT_NIL   (DT_NAN | DT_TMASK_NIL)

// value type signature
#define DT_SIG_NAN     DT_NAN
#define DT_SIG_FALSE   DT_FALSE
#define DT_SIG_TRUE    DT_TRUE
#define DT_SIG_NIL     DT_NIL
#define DT_SIG_RANGE   (DT_NAN | DT_TMASK_RANGE)
#define DT_SIG_HEAP    (DT_NAN | DT_TMASK_HEAP)
#define DT_SIG_CFUNC   (DT_NAN | DT_TMASK_CFUNC)
#define DT_SIG_LOGIC   (DT_NAN | DT_TMASK_LOGIC)

typedef dt_val (dt_cfunc)(struct dt_vm* vm, int nargs);

#endif

typedef struct dt_heaper {
	DT_HEAPER_STRUCT
} dt_heaper;

typedef struct {
	DT_HEAPER_STRUCT
	uint32_t size;
	char     data[];
} dt_cdata;

// upvalue (runtime)
// - captured: already lifted on the heap
// - not captured: still on the stack, must lift when it leaves
// TODO: naming
typedef struct {
	DT_HEAPER_STRUCT
	bool    captured;
	dt_val* val;
} dt_upval2;

// string
typedef struct dt_str {
	DT_HEAPER_STRUCT
	uint32_t  len;
	uint32_t  hash;
	char      chars[];
} dt_str;

// array
typedef struct dt_arr {
	DT_HEAPER_STRUCT
	uint32_t  len;
	uint32_t  cap;
	dt_val*   values;
} dt_arr;

// hashmap entry
typedef struct {
	dt_str* key;
	dt_val  val;
} dt_entry;

// hashmap
typedef struct dt_map {
	DT_HEAPER_STRUCT
	uint32_t  cnt;
	uint32_t  cap;
	dt_entry* entries;
} dt_map;

// TODO: store dt_logics on top level instead of consts
// holding bytecodes and constants
typedef struct {
	int      cnt;
	int      cap;
	uint8_t* code;
	int*     lines;
	dt_arr*  consts;
} dt_chunk;

// function prototype
typedef struct dt_logic {
	dt_chunk chunk;
	int      nargs;
} dt_logic;

// function
typedef struct dt_func {
	DT_HEAPER_STRUCT
	uint8_t     num_upvals;
	dt_logic*   logic;
	dt_upval2** upvals;
} dt_func;

// the runtime virtual machine
typedef struct dt_vm {
	dt_func*   func;
	uint8_t*   ip;
	dt_val     stack[DT_STACK_MAX];
	dt_val*    stack_top;
	int        stack_offset;
	dt_map*    globals;
	dt_map*    strs;
	dt_upval2* open_upvals[UINT8_MAX];
	int        num_upvals;
	dt_heaper* heaper;
	size_t     mem;
	size_t     next_gc;
} dt_vm;

// vm creation / destruction
dt_vm     dt_vm_new    ();
void      dt_vm_free   (dt_vm* vm);

// running code
dt_val    dt_eval        (dt_vm* vm, char* src);
dt_val    dt_dofile      (dt_vm* vm, char* path);

// loads standard library
void      dt_load_std    (dt_vm* vm);

// throw runtime error
void      dt_err       (dt_vm* vm, char* fmt, ...);

// for getting args in cfunc
dt_val    dt_arg       (dt_vm* vm, int idx);
dt_num    dt_arg_num   (dt_vm* vm, int idx);
dt_bool   dt_arg_bool  (dt_vm* vm, int idx);
dt_str*   dt_arg_str   (dt_vm* vm, int idx);
char*     dt_arg_cstr  (dt_vm* vm, int idx);
dt_map*   dt_arg_map   (dt_vm* vm, int idx);
dt_arr*   dt_arg_arr   (dt_vm* vm, int idx);
dt_func*  dt_arg_func  (dt_vm* vm, int idx);

// calling a dt_func
dt_val    dt_call      (dt_vm* vm, dt_func* func, int nargs, ...);
dt_val    dt_call_0    (dt_vm* vm, dt_func* func);
dt_val    dt_call_1    (dt_vm* vm, dt_func* func, dt_val a1);
dt_val    dt_call_2    (dt_vm* vm, dt_func* func, dt_val a1, dt_val a2);

// perform a full mark & sweep procedure
void      dt_gc_run    (dt_vm* vm);
// keep / unkeep a value from gc
void      dt_keep      (dt_vm*, dt_val v);
void      dt_unkeep    (dt_vm*, dt_val v);

// value conversions
dt_val    dt_to_num      (dt_num n);
dt_val    dt_to_bool     (dt_bool b);
dt_val    dt_to_range    (dt_range r);
dt_val    dt_to_str      (dt_str* str);
dt_val    dt_to_arr      (dt_arr* arr);
dt_val    dt_to_map      (dt_map* map);
dt_val    dt_to_func     (dt_func* func);
dt_val    dt_to_cfunc    (dt_cfunc* func);
dt_num    dt_as_num      (dt_val v);
dt_bool   dt_as_bool     (dt_val v);
dt_range  dt_as_range    (dt_val v);
dt_str*   dt_as_str      (dt_val v);
dt_arr*   dt_as_arr      (dt_val v);
dt_map*   dt_as_map      (dt_val v);
dt_func*  dt_as_func     (dt_val v);
dt_cfunc* dt_as_cfunc    (dt_val v);

// type checking
dt_val_ty dt_type        (dt_val v);
char*     dt_typename    (dt_val_ty ty);
bool      dt_is_nil      (dt_val v);
bool      dt_is_num      (dt_val v);
bool      dt_is_bool     (dt_val v);
bool      dt_is_str      (dt_val v);
bool      dt_is_map      (dt_val v);
bool      dt_is_arr      (dt_val v);
bool      dt_is_func     (dt_val v);

// debug print
void      dt_print   (dt_val v);
void      dt_println (dt_val v);

// TODO: think about dt_vm* passing / gc mechanisms around
// passing dt_vm* for gc managed memory, passing NULL for manual

// allocate empty str with len (need to manually fill and hash afterwards or the
// content is undefined)
dt_str*  dt_str_alloc    (dt_vm* vm, int len);
// create str from c str (until '\0')
dt_str*  dt_str_new      (dt_vm* vm, char* src);
// create str from c str with length
dt_str*  dt_str_new_len  (dt_vm* vm, char* src, int len);
// free (you shouldn't call this manually if it's managed by gc)
void     dt_str_free     (dt_vm* vm, dt_str* str);
// generate str hash
void     dt_str_hash     (dt_str* str);
// create c str (needs manual memory management)
char*    dt_str_cstr     (dt_str* str);
bool     dt_str_eq       (dt_str* a, dt_str* b);
dt_str*  dt_str_concat   (dt_vm* vm, dt_str* a, dt_str* b);
dt_str*  dt_str_replace  (dt_vm* vm, dt_str* src, dt_str* old, dt_str* new);
void     dt_str_print    (dt_str* str);
void     dt_str_println  (dt_str* str);

// create empty array
dt_arr*  dt_arr_new      (dt_vm* vm);
// create array with capacity
dt_arr*  dt_arr_new_len  (dt_vm* vm, int len);
void     dt_arr_free     (dt_vm* vm, dt_arr* arr);
dt_val   dt_arr_get      (dt_arr* arr, int idx);
void     dt_arr_set      (dt_vm* vm, dt_arr* arr, int idx, dt_val val);
void     dt_arr_insert   (dt_vm* vm, dt_arr* arr, int idx, dt_val val);
void     dt_arr_push     (dt_vm* vm, dt_arr* arr, dt_val val);
dt_val   dt_arr_rm       (dt_arr* arr, int idx);
dt_arr*  dt_arr_concat   (dt_vm* vm, dt_arr* a1, dt_arr* a2);
void     dt_arr_print    (dt_arr* arr);
void     dt_arr_println  (dt_arr* arr);

// create empty hashmap
dt_map*  dt_map_new      (dt_vm* vm);
// create hashmap with capacity
dt_map*  dt_map_new_len  (dt_vm* vm, int len);
void     dt_map_free     (dt_vm* vm, dt_map* map);
dt_val   dt_map_get      (dt_map* map, dt_str* key);
void     dt_map_set      (dt_vm* vm, dt_map* map, dt_str* key, dt_val val);
// get / set from c str
dt_val   dt_map_cget     (dt_vm* vm, dt_map* map, char* key);
void     dt_map_cset     (dt_vm* vm, dt_map* map, char* key, dt_val val);
bool     dt_map_exists   (dt_map* map, dt_str* key);
// get an array of keys
dt_arr*  dt_map_keys     (dt_vm* vm, dt_map* map);
// get an array of values
dt_arr*  dt_map_vals     (dt_vm* vm, dt_map* map);
void     dt_map_print    (dt_map* map);
void     dt_map_println  (dt_map* map);

#endif

#ifdef D_IMPL
#define D_LANG_IMPL
#endif

#ifdef D_LANG_IMPL
#ifndef D_LANG_IMPL_ONCE
#define D_LANG_IMPL_ONCE

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>

// op codes
typedef enum {
	DT_OP_STOP,
	DT_OP_CONST,
	DT_OP_NIL,
	DT_OP_TRUE,
	DT_OP_FALSE,
	DT_OP_ADD,
	DT_OP_SUB,
	DT_OP_MUL,
	DT_OP_DIV,
	DT_OP_MOD,
	DT_OP_POW,
	DT_OP_NEG,
	DT_OP_NOT,
	DT_OP_EQ,
	DT_OP_GT,
	DT_OP_GT_EQ,
	DT_OP_LT,
	DT_OP_LT_EQ,
	DT_OP_OR,
	DT_OP_AND,
	DT_OP_LEN,
	DT_OP_POP,
	DT_OP_GETG,
	DT_OP_SETG,
	DT_OP_GETL,
	DT_OP_SETL,
	DT_OP_GETU,
	DT_OP_SETU,
	DT_OP_GETI,
	DT_OP_SETI,
	DT_OP_CALL,
	DT_OP_MKSTR,
	DT_OP_MKFUNC,
	DT_OP_MKARR,
	DT_OP_MKMAP,
	DT_OP_JMP,
	DT_OP_JMP_COND,
	DT_OP_REWIND,
	DT_OP_CLOSE,
	DT_OP_SPREAD,
	DT_OP_ITER_PREP,
	DT_OP_ITER,
	DT_OP_ARGS,
} dt_op;

// tokens
typedef enum {
	// sym
	DT_TOKEN_LPAREN, // (
	DT_TOKEN_RPAREN, // )
	DT_TOKEN_LBRACE, // {
	DT_TOKEN_RBRACE, // }
	DT_TOKEN_LBRACKET, // [
	DT_TOKEN_RBRACKET, // ]
	DT_TOKEN_COMMA, // ,
	DT_TOKEN_DOT, // .
	DT_TOKEN_PLUS, // +
	DT_TOKEN_MINUS, // -
	DT_TOKEN_STAR, // *
	DT_TOKEN_SLASH, // /
	DT_TOKEN_CARET, // ^
	DT_TOKEN_GT, // >
	DT_TOKEN_GT_GT, // >>
	DT_TOKEN_LT, // <
	DT_TOKEN_LT_LT, // <<
	DT_TOKEN_EQ, // =
	DT_TOKEN_BANG, // !
	DT_TOKEN_HASH, // #
	DT_TOKEN_DOLLAR, // $
	DT_TOKEN_DOLLAR_LBRACE, // ${
	DT_TOKEN_BACKSLASH, //
	DT_TOKEN_PERCENT, // %
	DT_TOKEN_TILDE, // ~
	DT_TOKEN_COLON, // :
	DT_TOKEN_COLON_COLON, // ::
	DT_TOKEN_COLON_D, // :D
	DT_TOKEN_COLON_LPAREN, // :(
	DT_TOKEN_COLON_RPAREN, // :)
	DT_TOKEN_QUESTION, // ?
	DT_TOKEN_AND, // &
	DT_TOKEN_OR, // |
	DT_TOKEN_AT, // @
	DT_TOKEN_EQ_EQ, // ==
	DT_TOKEN_BANG_EQ, // !=
	DT_TOKEN_PLUS_EQ, // +=
	DT_TOKEN_PLUS_PLUS, // ++
	DT_TOKEN_MINUS_EQ, // -=
	DT_TOKEN_MINUS_MINUS, // --
	DT_TOKEN_STAR_EQ, // *=
	DT_TOKEN_SLASH_EQ, // /=
	DT_TOKEN_LT_EQ, // <=
	DT_TOKEN_GT_EQ, // >=
	DT_TOKEN_DOT_DOT, // ..
	DT_TOKEN_DOT_DOT_DOT, // ...
	DT_TOKEN_AND_AND, // &&
	DT_TOKEN_OR_OR, // ||
	DT_TOKEN_TILDE_GT, // ~>
	DT_TOKEN_AT_GT, // @>
	DT_TOKEN_AT_CARET, // @^
	DT_TOKEN_PERCENT_GT, // %>
	DT_TOKEN_QUOTE, // "
	// lit
	DT_TOKEN_IDENT,
	DT_TOKEN_STR,
	DT_TOKEN_NUM,
	// key
	DT_TOKEN_T,
	DT_TOKEN_F,
	// util
	DT_TOKEN_ERR,
	DT_TOKEN_END,
} dt_token_ty;

typedef enum {
	DT_PREC_NONE,
	DT_PREC_ASSIGN,  // =
	DT_PREC_LOGIC, // || &&
	DT_PREC_EQ,   // == !=
	DT_PREC_CMP, // < > <= >=
	DT_PREC_TERM,    // + -
	DT_PREC_FACTOR,  // * /
	DT_PREC_UNARY,   // ! - #
	DT_PREC_CALL,    // () [] .
	DT_PREC_PRIMARY
} dt_prec;

typedef struct {
	dt_token_ty type;
	char* start;
	int len;
	int line;
} dt_token;

typedef struct {
	dt_token prev;
	dt_token cur;
} dt_parser;

typedef struct {
	char* start;
	char* cur;
	int line;
} dt_scanner;

// compile time local variable representation
// TODO: store type
typedef struct {
	dt_token name;
	bool captured;
	int depth;
} dt_local;

// compile time upval representation
typedef struct {
	int idx;
	bool local;
} dt_upval;

// block types
typedef enum {
	DT_BLOCK_NORMAL,
	DT_BLOCK_LOOP,
	DT_BLOCK_COND,
} dt_block_ty;

// compile time loop break point bookkeeping
typedef struct {
	int pos;
	int depth;
	bool cont;
} dt_break;

// compile time function representation
// TODO: compact
typedef struct dt_funcenv {
	struct dt_funcenv* parent;
	dt_chunk chunk;
	int cur_depth;
	dt_block_ty scopes[UINT8_MAX];
	dt_local locals[UINT8_MAX];
	int num_locals;
	dt_upval upvals[UINT8_MAX];
	int num_upvals;
	dt_break breaks[UINT8_MAX];
	int num_breaks;
} dt_funcenv;

// TODO
typedef struct {
	char name[16];
	int type;
} dt_type_mem;

// TODO
typedef struct {
	char name[16];
	dt_type_mem members[16];
	int num_members;
} dt_typedef;

// TODO
typedef struct {
	int ret;
	int args[16];
	int num_args;
} dt_funcdef_inner;

// TODO
typedef struct {
	char name[16];
	dt_funcdef_inner styles[4];
	int num_styles;
} dt_funcdef;

typedef struct {
	dt_scanner scanner;
	dt_parser parser;
	dt_funcenv base_env;
	dt_funcenv* env;
	// TODO: dynamic
	dt_typedef types[UINT8_MAX];
	int num_types;
	dt_funcdef funcs[UINT8_MAX];
	int num_funcs;
} dt_compiler;

typedef void (*dt_parse_fn)(dt_compiler* compiler);

typedef struct {
	dt_parse_fn prefix;
	dt_parse_fn infix;
	dt_prec prec;
} dt_parse_rule;

// unrecoverable error
static void dt_fail(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

// mem ops with total mem bookkeeping
static void* dt_malloc(dt_vm* vm, size_t size) {
	if (vm) {
		vm->mem += size;
	}
	return malloc(size);
}

static void* dt_realloc(dt_vm* vm, void* ptr, size_t old_size, size_t new_size) {
	if (vm) {
		vm->mem = vm->mem - old_size + new_size;
	}
	return realloc(ptr, new_size);
}

static void dt_free(dt_vm* vm, void* ptr, size_t size) {
	if (vm) {
		vm->mem -= size;
	}
	free(ptr);
}

char* dt_token_name(dt_token_ty ty) {
	switch (ty) {
		case DT_TOKEN_LPAREN: return "LPAREN";
		case DT_TOKEN_RPAREN: return "RPAREN";
		case DT_TOKEN_LBRACE: return "LBRACE";
		case DT_TOKEN_RBRACE: return "RBRACE";
		case DT_TOKEN_LBRACKET: return "LBRACKET";
		case DT_TOKEN_RBRACKET: return "RBRACKET";
		case DT_TOKEN_COMMA: return "COMMA";
		case DT_TOKEN_DOT: return "DOT";
		case DT_TOKEN_PLUS: return "PLUS";
		case DT_TOKEN_MINUS: return "MINUS";
		case DT_TOKEN_STAR: return "STAR";
		case DT_TOKEN_SLASH: return "SLASH";
		case DT_TOKEN_CARET: return "CARET";
		case DT_TOKEN_GT: return "GT";
		case DT_TOKEN_GT_GT: return "GT_GT";
		case DT_TOKEN_LT: return "LT";
		case DT_TOKEN_LT_LT: return "LT_LT";
		case DT_TOKEN_EQ: return "EQ";
		case DT_TOKEN_BANG: return "BANG";
		case DT_TOKEN_HASH: return "HASH";
		case DT_TOKEN_DOLLAR: return "DOLLAR";
		case DT_TOKEN_DOLLAR_LBRACE: return "DOLLAR_LBRACE";
		case DT_TOKEN_BACKSLASH: return "BACKSLASH";
		case DT_TOKEN_PERCENT: return "PERCENT";
		case DT_TOKEN_TILDE: return "TILDE";
		case DT_TOKEN_COLON: return "COLON";
		case DT_TOKEN_COLON_COLON: return "COLON_COLON";
		case DT_TOKEN_COLON_D: return "COLON_D";
		case DT_TOKEN_COLON_LPAREN: return "COLON_LPAREN";
		case DT_TOKEN_COLON_RPAREN: return "COLON_RPAREN";
		case DT_TOKEN_QUESTION: return "QUESTION";
		case DT_TOKEN_QUOTE: return "QUOTE";
		case DT_TOKEN_AND: return "AND";
		case DT_TOKEN_OR: return "OR";
		case DT_TOKEN_AT: return "AT";
		case DT_TOKEN_EQ_EQ: return "EQ_EQ";
		case DT_TOKEN_BANG_EQ: return "BANG_EQ";
		case DT_TOKEN_PLUS_EQ: return "PLUS_EQ";
		case DT_TOKEN_PLUS_PLUS: return "PLUS_PLUS";
		case DT_TOKEN_MINUS_EQ: return "MINUS_EQ";
		case DT_TOKEN_MINUS_MINUS: return "MINUS_MINUS";
		case DT_TOKEN_STAR_EQ: return "STAR_EQ";
		case DT_TOKEN_SLASH_EQ: return "SLASH_EQ";
		case DT_TOKEN_LT_EQ: return "LESS_EQ";
		case DT_TOKEN_GT_EQ: return "GT_EQ";
		case DT_TOKEN_DOT_DOT: return "DOT_DOT";
		case DT_TOKEN_DOT_DOT_DOT: return "DOT_DOT_DOT";
		case DT_TOKEN_AND_AND: return "AND_AND";
		case DT_TOKEN_OR_OR: return "OR_OR";
		case DT_TOKEN_TILDE_GT: return "TILDE_GT";
		case DT_TOKEN_AT_GT: return "AT_GT";
		case DT_TOKEN_AT_CARET: return "AT_CARET";
		case DT_TOKEN_PERCENT_GT: return "PERCENT_GT";
		case DT_TOKEN_IDENT: return "IDENT";
		case DT_TOKEN_STR: return "STR";
		case DT_TOKEN_NUM: return "NUM";
		case DT_TOKEN_T: return "T";
		case DT_TOKEN_F: return "F";
		case DT_TOKEN_ERR: return "ERR";
		case DT_TOKEN_END: return "END";
	}
}

static bool dt_token_eq(dt_token* t1, dt_token* t2) {
	if (t1->len != t2->len) {
		return false;
	}
	return memcmp(t1->start, t2->start, t1->len) == 0;
}

dt_val_ty dt_type(dt_val v) {

#ifdef DT_NO_NANBOX
	return v.type;
#else
	if ((~v & DT_SMASK_EXPO) != 0) {
		return DT_VAL_NUM;
	}

	switch (v & DT_SMASK_SIG) {
		case DT_SIG_NAN:   return DT_VAL_NUM;
		case DT_SIG_FALSE:
		case DT_SIG_TRUE:  return DT_VAL_BOOL;
		case DT_SIG_NIL:   return DT_VAL_NIL;
		case DT_SIG_RANGE: return DT_VAL_RANGE;
		case DT_SIG_LOGIC: return DT_VAL_LOGIC;
		case DT_SIG_CFUNC: return DT_VAL_CFUNC;
		case DT_SIG_HEAP:  return ((dt_heaper*)(v & DT_SMASK_PTR))->type;
	}

	return DT_VAL_NIL;
#endif

}

char* dt_typename(dt_val_ty ty) {
	switch (ty) {
		case DT_VAL_NIL: return "nil";
		case DT_VAL_BOOL: return "bool";
		case DT_VAL_NUM: return "num";
		case DT_VAL_STR: return "str";
		case DT_VAL_ARR: return "arr";
		case DT_VAL_MAP: return "map";
		case DT_VAL_RANGE: return "range";
		case DT_VAL_FUNC: return "func";
		case DT_VAL_CFUNC: return "cfunc";
		case DT_VAL_CDATA: return "cdata";
		case DT_VAL_CPTR: return "cptr";
		case DT_VAL_LOGIC: return "logic";
		case DT_VAL_UPVAL: return "upval";
	}
}

dt_val dt_to_num(dt_num n) {
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_NUM,
		.data.num = n,
	};
#else
	return *(dt_val*)(&n);
#endif
}

dt_num dt_as_num(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.num;
#else
	return *(dt_num*)(&v);
#endif
}

dt_val dt_to_bool(dt_bool b) {
	return b ? DT_TRUE : DT_FALSE;
}

dt_bool dt_as_bool(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.boolean;
#else
	return v == DT_TRUE ? true : false;
#endif
}

dt_val dt_to_range(dt_range r) {
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_RANGE,
		.data.range = r,
	};
#else
	return DT_SIG_RANGE | (uint32_t)(r.start << 16 | r.end);
#endif
}

dt_range dt_as_range(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.range;
#else
	uint32_t n = DT_SIG_HEAP | v;
	return (dt_range) {
		.start = n >> 16,
		.end = n & 0xffff,
	};
#endif
}

#ifndef DT_NO_NANBOX
static void* dt_as_ptr(dt_val v) {
	return (void*)(v & DT_SMASK_PTR);
}

static dt_val dt_to_heap(void* ptr) {
	if (!ptr) {
		return DT_NIL;
	}
	return DT_SIG_HEAP | (uint64_t)ptr;
}
#endif

dt_val dt_to_str(dt_str* str) {
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_STR,
		.data.str = str,
	};
#else
	return dt_to_heap(str);
#endif
}

dt_str* dt_as_str(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.str;
#else
	return (dt_str*)dt_as_ptr(v);
#endif
}

dt_val dt_to_arr(dt_arr* arr) {
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_ARR,
		.data.arr = arr,
	};
#else
	return dt_to_heap(arr);
#endif
}

dt_arr* dt_as_arr(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.arr;
#else
	return (dt_arr*)dt_as_ptr(v);
#endif
}

dt_val dt_to_map(dt_map* map) {
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_MAP,
		.data.map = map,
	};
#else
	return dt_to_heap(map);
#endif
}

dt_map* dt_as_map(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.map;
#else
	return (dt_map*)dt_as_ptr(v);
#endif
}

dt_val dt_to_func(dt_func* func) {
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_FUNC,
		.data.func = func,
	};
#else
	return dt_to_heap(func);
#endif
}

dt_func* dt_as_func(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.func;
#else
	return (dt_func*)dt_as_ptr(v);
#endif
}

dt_val dt_to_cfunc(dt_cfunc* cfunc) {
	if (!cfunc) {
		return DT_NIL;
	}
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_CFUNC,
		.data.cfunc = cfunc,
	};
#else
	return DT_SIG_CFUNC | (uint64_t)cfunc;
#endif
}

dt_cfunc* dt_as_cfunc(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.cfunc;
#else
	return (dt_cfunc*)dt_as_ptr(v);
#endif
}

dt_val dt_to_logic(dt_logic* logic) {
	if (!logic) {
		return DT_NIL;
	}
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_LOGIC,
		.data.logic = logic,
	};
#else
	return DT_SIG_LOGIC | (uint64_t)logic;
#endif
}

dt_logic* dt_as_logic(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.logic;
#else
	return (dt_logic*)dt_as_ptr(v);
#endif
}

// if the value is a heap ptr / gc managed
static bool dt_is_heap(dt_val v) {
#ifdef DT_NO_NANBOX
	dt_val_ty t = v.type;
	return
		t == DT_VAL_STR
		|| t == DT_VAL_ARR
		|| t == DT_VAL_MAP
		|| t == DT_VAL_FUNC
		|| t == DT_VAL_CDATA
		;
#else
	return (v & DT_SMASK_SIG) == DT_SIG_HEAP;
#endif
}

bool dt_is_nil(dt_val val) {
	return dt_type(val) == DT_VAL_NIL;
}

bool dt_is_num(dt_val val) {
	return dt_type(val) == DT_VAL_NUM;
}

bool dt_is_bool(dt_val val) {
	return dt_type(val) == DT_VAL_BOOL;
}

bool dt_is_str(dt_val val) {
	return dt_type(val) == DT_VAL_STR;
}

bool dt_is_map(dt_val val) {
	return dt_type(val) == DT_VAL_MAP;
}

bool dt_is_arr(dt_val val) {
	return dt_type(val) == DT_VAL_ARR;
}

bool dt_is_func(dt_val val) {
	return dt_type(val) == DT_VAL_FUNC;
}

// TODO: move around
void dt_func_print(dt_func* func);
static void dt_cdata_print(dt_cdata* cdata);

void dt_range_print(dt_range r) {
	printf("%d..%d", r.start, r.end);
}

void dt_range_println(dt_range r) {
	dt_range_print(r);
	printf("\n");
}

void dt_print(dt_val v) {
	switch (dt_type(v)) {
		case DT_VAL_NIL: printf("<nil>"); break;
		case DT_VAL_BOOL: printf(dt_as_bool(v) ? "true" : "false"); break;
		case DT_VAL_NUM: printf("%g", dt_as_num(v)); break;
		case DT_VAL_STR: dt_str_print(dt_as_str(v)); break;
		case DT_VAL_ARR: dt_arr_print(dt_as_arr(v)); break;
		case DT_VAL_MAP: dt_map_print(dt_as_map(v)); break;
		case DT_VAL_RANGE: dt_range_print(dt_as_range(v)); break;
		case DT_VAL_FUNC: dt_func_print(dt_as_func(v)); break;
		case DT_VAL_CFUNC: printf("<cfunc#%p>", (void*)dt_as_cfunc(v)); break;
// 		case DT_VAL_CDATA: dt_cdata_print(dt_as_cdata(v)); break;
		case DT_VAL_CDATA: printf("<cdata>"); break;
// 		case DT_VAL_CPTR: printf("<cptr#%p>", (void*)dt_as_cptr(v)); break;
		case DT_VAL_CPTR: printf("<cptr>"); break;
		case DT_VAL_UPVAL: printf("<upval>"); break;
		case DT_VAL_LOGIC: printf("<logic>"); break;
		default: printf("<unknown>"); break;
	}
}

void dt_println(dt_val val) {
	dt_print(val);
	printf("\n");
}

static uint32_t dt_hash(char* key, int len) {
	uint32_t hash = 2166136261u;
	for (int i = 0; i < len; i++) {
		hash ^= (uint8_t)key[i];
		hash *= 16777619;
	}
	return hash;
}

// TODO: move around
static void dt_manage(dt_vm* vm, dt_heaper* h, uint8_t ty);

dt_str* dt_str_alloc(dt_vm* vm, int len) {
	dt_str* str = dt_malloc(vm, sizeof(dt_str) + len + 1);
	str->len = len;
	str->type = DT_VAL_STR;
	if (vm) {
		dt_manage(vm, (dt_heaper*)str, DT_VAL_STR);
	}
	return str;
}

void dt_str_hash(dt_str* str) {
	str->hash = dt_hash(str->chars, str->len);
}

dt_str* dt_str_new_len(dt_vm* vm, char* src, int len) {
	dt_str* str = dt_str_alloc(vm, len);
	memcpy(str->chars, src, len);
	str->chars[len] = '\0';
	str->hash = dt_hash(str->chars, len);
	return str;
}

dt_str* dt_str_new(dt_vm* vm, char* src) {
	return dt_str_new_len(vm, src, strlen(src));
}

void dt_str_print(dt_str* str) {
	printf("%s", str->chars);
}

void dt_str_println(dt_str* str) {
	dt_str_print(str);
	printf("\n");
}

void dt_str_free(dt_vm* vm, dt_str* str) {
	dt_free(vm, str, sizeof(dt_str) + str->len + 1);
}

// mark for gc to not collect
static void dt_str_mark(dt_str* str) {
	str->marked = true;
}

dt_str* dt_str_concat(dt_vm* vm, dt_str* a, dt_str* b) {
	int len = a->len + b->len;
	dt_str* new = dt_str_alloc(vm, len);
	memcpy(new->chars, a->chars, a->len);
	memcpy(new->chars + a->len, b->chars, b->len);
	new->chars[len] = '\0';
	new->hash = dt_hash(new->chars, new->len);
	return new;
}

bool dt_str_is(dt_str* str, char* str2) {
	return strcmp(str->chars, str2) == 0;
}

bool dt_str_eq(dt_str* a, dt_str* b) {
	return a->len == b->len && a->hash == b->hash && memcmp(a->chars, b->chars, a->len) == 0;
}

// TODO: replace all
dt_str* dt_str_replace(dt_vm* vm, dt_str* src, dt_str* old, dt_str* new) {
	char* start = strstr(src->chars, old->chars);
	if (!start) {
		return src;
	}
	int offset = start - src->chars;
	int len = src->len - old->len + new->len;
	char* chars = malloc(len + 1);
	memcpy(chars, src->chars, offset);
	memcpy(chars + offset, new->chars, new->len);
	memcpy(
		chars + offset + new->len,
		src->chars + offset + old->len,
		src->len - offset - old->len
	);
	chars[len] = '\0';
	return dt_str_new_len(vm, chars, len);
}

char* dt_str_cstr(dt_str* str) {
	char* chars = malloc(str->len + 1);
	memcpy(chars, str->chars, str->len);
	chars[str->len] = '\0';
	return chars;
}

dt_str* dt_num_to_str(dt_vm* vm, dt_num num) {
	int len = snprintf(NULL, 0, "%g", num);
	dt_str* str = dt_str_alloc(vm, len);
	sprintf(str->chars, "%g", num);
	str->hash = dt_hash(str->chars, len);
	return str;
}

dt_arr* dt_arr_new_len(dt_vm* vm, int len) {
	dt_arr* arr = dt_malloc(vm, sizeof(dt_arr));
	arr->len = 0;
	arr->cap = len > DT_ARR_INIT_SIZE ? len : DT_ARR_INIT_SIZE;
	arr->values = dt_malloc(vm, arr->cap * sizeof(dt_val));
	arr->type = DT_VAL_ARR;
	if (vm) {
		dt_manage(vm, (dt_heaper*)arr, DT_VAL_ARR);
	}
	return arr;
}

dt_arr* dt_arr_new(dt_vm* vm) {
	return dt_arr_new_len(vm, 0);
}

void dt_arr_free(dt_vm* vm, dt_arr* arr) {
	dt_free(vm, arr->values, arr->cap * sizeof(dt_val));
	dt_free(vm, arr, sizeof(dt_arr));
}

dt_val dt_arr_get(dt_arr* arr, int idx) {
	if (idx >= arr->len) {
		return DT_NIL;
	}
	return arr->values[idx];
}

void dt_arr_set(dt_vm* vm, dt_arr* arr, int idx, dt_val val) {

	// expand
	if (idx >= arr->cap) {
		size_t old_cap = arr->cap;
		while (idx >= arr->cap) {
			arr->cap *= 2;
		}
		arr->values = dt_realloc(
			vm,
			arr->values,
			old_cap * sizeof(dt_val),
			arr->cap * sizeof(dt_val)
		);
	}

	// fill with nil
	if (idx >= arr->len) {
		arr->len++;
		for (int i = arr->len - 1; i < idx; i++) {
			arr->len++;
			arr->values[i] = DT_NIL;
		}
	}

	arr->values[idx] = val;

}

void dt_arr_insert(dt_vm* vm, dt_arr* arr, int idx, dt_val val) {

	if (idx >= arr->cap) {
		dt_arr_set(vm, arr, idx, val);
		return;
	}

	if (arr->len >= arr->cap) {
		size_t old_cap = arr->cap;
		arr->cap *= 2;
		arr->values = dt_realloc(
			vm,
			arr->values,
			old_cap * sizeof(dt_val),
			arr->cap * sizeof(dt_val)
		);
	}

	arr->len++;

	// TODO
	for (int i = arr->len - 1; i > idx; i++) {
		arr->values[i] = arr->values[i - 1];
	}

	arr->values[idx] = val;

}

void dt_arr_push(dt_vm* vm, dt_arr* arr, dt_val val) {
	dt_arr_set(vm, arr, arr->len, val);
}

dt_val dt_arr_rm(dt_arr* arr, int idx) {
	if (idx >= arr->len) {
		return DT_NIL;
	}
	dt_val v = arr->values[idx];
	memcpy(arr->values + idx, arr->values + idx + 1, (arr->len - idx - 1) * sizeof(dt_val));
	arr->len--;
	arr->values[arr->len] = DT_NIL;
	return v;
}

dt_arr* dt_arr_concat(dt_vm* vm, dt_arr* a1, dt_arr* a2) {
	dt_arr* new = dt_arr_new_len(vm, a1->len + a2->len);
	for (int i = 0; i < a1->len; i++) {
		dt_arr_push(vm, new, dt_arr_get(a1, i));
	}
	for (int i = 0; i < a2->len; i++) {
		dt_arr_push(vm, new, dt_arr_get(a2, i));
	}
	return new;
}

void dt_arr_print(dt_arr* arr) {
	printf("[ ");
	for (int i = 0; i < arr->len; i++) {
		dt_print(arr->values[i]);
		printf(", ");
	}
	printf("]");
}

void dt_arr_println(dt_arr* arr) {
	dt_arr_print(arr);
	printf("\n");
}

static void dt_val_mark(dt_val val);

static void dt_arr_mark(dt_arr* arr) {
	arr->marked = true;
	for (int i = 0; i < arr->len; i++) {
		dt_val_mark(arr->values[i]);
	}
}

dt_map* dt_map_new_len(dt_vm* vm, int len) {
	dt_map* map = dt_malloc(vm, sizeof(dt_map));
	map->cnt = 0;
	map->cap = len > DT_MAP_INIT_SIZE ? len : DT_MAP_INIT_SIZE;
	int entries_size = map->cap * sizeof(dt_entry);
	map->entries = dt_malloc(vm, entries_size);
	memset(map->entries, 0, entries_size);
	map->type = DT_VAL_MAP;
	if (vm) {
		dt_manage(vm, (dt_heaper*)map, DT_VAL_MAP);
	}
	return map;
}

dt_map* dt_map_new(dt_vm* vm) {
	return dt_map_new_len(vm, DT_MAP_INIT_SIZE);
}

void dt_map_free(dt_vm* vm, dt_map* map) {
	dt_free(vm, map->entries, map->cap * sizeof(dt_entry));
	dt_free(vm, map, sizeof(dt_map));
}

static void dt_map_mark(dt_map* map) {
	map->marked = true;
	for (int i = 0; i < map->cap; i++) {
		dt_entry e = map->entries[i];
		if (e.key) {
			dt_str_mark(e.key);
			dt_val_mark(e.val);
		}
	}
}

static int dt_map_find(dt_map* map, dt_str* key) {

	uint32_t oidx = key->hash % map->cap;
// 	uint32_t oidx = key->hash & (map->cap - 1);
	uint32_t idx = oidx;

	for (;;) {

		dt_entry e = map->entries[idx];

		if (e.key == NULL || dt_str_eq(e.key, key)) {
			return idx;
		}

		idx = (idx + 1) % map->cap;
// 		idx = (idx + 1) & (map->cap - 1);

		// went through one round
		if (idx == oidx) {
			return -1;
		}

	}

}

void dt_map_set(dt_vm* vm, dt_map* map, dt_str* key, dt_val val) {

	// resize
	if (map->cnt + 1 > map->cap * DT_MAP_MAX_LOAD) {

		int old_cap = map->cap;
		dt_entry* old_entries = map->entries;
		size_t old_size = old_cap * sizeof(dt_entry);

		map->cap = old_cap * 2;
		size_t size = map->cap * sizeof(dt_entry);
		map->entries = dt_malloc(vm, size);
		memset(map->entries, 0, size);

		for (int i = 0; i < old_cap; i++) {
			dt_entry e = old_entries[i];
			if (e.key && !dt_is_nil(e.val)) {
				dt_map_set(vm, map, e.key, e.val);
			}
		}

		dt_free(vm, old_entries, old_size);

	}

	int idx = dt_map_find(map, key);

	if (idx == -1) {
		dt_err(vm, "map overflow\n");
	}

	if (map->entries[idx].key) {
		map->entries[idx].val = val;
	} else {
		dt_entry* e = &map->entries[idx];
		e->key = key;
		e->val = val;
		map->cnt++;
	}

}

bool dt_map_exists(dt_map* map, dt_str* key) {
	int idx = dt_map_find(map, key);
	return idx != -1 && map->entries[idx].key != NULL;
}

dt_val dt_map_get(dt_map* map, dt_str* key) {

	int idx = dt_map_find(map, key);

	if (idx == -1 || map->entries[idx].key == NULL) {
		return DT_NIL;
	}

	return map->entries[idx].val;

}

dt_val dt_map_cget(dt_vm* vm, dt_map* map, char* key) {
	return dt_map_get(map, dt_str_new(vm, key));
}

void dt_map_cset(dt_vm* vm, dt_map* map, char* key, dt_val val) {
	dt_map_set(vm, map, dt_str_new(vm, key), val);
}

dt_arr* dt_map_keys(dt_vm* vm, dt_map* map) {
	dt_arr* arr = dt_arr_new(vm);
	for (int i = 0; i < map->cap; i++) {
		if (map->entries[i].key) {
			dt_arr_push(vm, arr, dt_to_str(map->entries[i].key));
		}
	}
	return arr;
}

dt_arr* dt_map_vals(dt_vm* vm, dt_map* map) {
	dt_arr* arr = dt_arr_new(vm);
	for (int i = 0; i < map->cap; i++) {
		if (map->entries[i].key) {
			dt_arr_push(vm, arr, map->entries[i].val);
		}
	}
	return arr;
}

void dt_map_print(dt_map* map) {
	printf("{ ");
	for (int i = 0; i < map->cap; i++) {
		dt_entry e = map->entries[i];
		if (e.key) {
			dt_str_print(e.key);
			printf(": ");
			dt_print(e.val);
			printf(", ");
		}
	}
	printf("}");
}

void dt_map_println(dt_map* map) {
	dt_map_print(map);
	printf("\n");
}

void dt_func_print(dt_func* func) {
	printf("<func#%p>", (void*)func);
}

void dt_func_println(dt_func* func) {
	dt_func_print(func);
	printf("\n");
}

static void dt_upval_free(dt_vm* vm, dt_upval2* upval) {
	if (upval->captured) {
		dt_free(vm, upval->val, sizeof(dt_val));
	}
	dt_free(vm, upval, sizeof(dt_upval2));
}

static void dt_func_mark(dt_func* func) {
	func->marked = true;
	for (int i = 0; i < func->num_upvals; i++) {
		func->upvals[i]->marked = true;
	}
}

static void dt_func_free(dt_vm *vm, dt_func* func) {
	dt_free(vm, func->upvals, sizeof(dt_val*) * func->num_upvals);
	dt_free(vm, func, sizeof(dt_func));
}

static void dt_cdata_print(dt_cdata* cdata) {
	printf("<cdata#%p>", (void*)cdata);
}

static void dt_cdata_println(dt_cdata* cdata) {
	dt_cdata_print(cdata);
	printf("\n");
}

dt_cdata* dt_cdata_new(dt_vm* vm, void* src, uint32_t size) {
	dt_cdata* cdata = dt_malloc(vm, sizeof(dt_cdata) + size);
	memcpy(cdata->data, src, size);
	return cdata;
}

void dt_cdata_free(dt_vm* vm, dt_cdata* cdata) {
	dt_free(vm, cdata, sizeof(dt_cdata) + cdata->size);
}

bool dt_val_truthiness(dt_val val) {
#ifdef DT_NO_NANBOX
	return !(
		val.type == DT_VAL_NIL
		&& (val.type == DT_VAL_BOOL && val.data.boolean == false)
	);
#else
	return val != DT_NIL && val != DT_FALSE;
#endif
}

dt_str* dt_bool_to_str(dt_vm* vm, dt_bool b) {
	return b ? dt_str_new(vm, "true") : dt_str_new(vm, "false");
}

// TODO: don't provide this
dt_str* dt_val_to_str(dt_vm* vm, dt_val val) {
	switch (dt_type(val)) {
		case DT_VAL_STR:
			return dt_as_str(val);
		case DT_VAL_NUM:
			return dt_num_to_str(vm, dt_as_num(val));
		case DT_VAL_BOOL:
			return dt_bool_to_str(vm, dt_as_num(val));
		default:
			return NULL;
	}
}

static dt_chunk dt_chunk_new() {
	return (dt_chunk) {
		.cnt = 0,
		.cap = 0,
		.code = malloc(0),
		.lines = malloc(0),
		.consts = dt_arr_new(NULL),
	};
}

static void dt_chunk_free(dt_chunk* c) {
	free(c->code);
	free(c->lines);
	dt_arr_free(NULL, c->consts);
	memset(c, 0, sizeof(dt_chunk));
}

static char* dt_op_name(dt_op op) {
	switch (op) {
		case DT_OP_STOP:      return "STOP";
		case DT_OP_CONST:     return "CONST";
		case DT_OP_NIL:       return "NIL";
		case DT_OP_TRUE:      return "TRUE";
		case DT_OP_FALSE:     return "FALSE";
		case DT_OP_ADD:       return "ADD";
		case DT_OP_SUB:       return "SUB";
		case DT_OP_MUL:       return "MUL";
		case DT_OP_DIV:       return "DIV";
		case DT_OP_MOD:       return "MOD";
		case DT_OP_POW:       return "POW";
		case DT_OP_NEG:       return "NEG";
		case DT_OP_NOT:       return "NOT";
		case DT_OP_EQ:        return "EQ";
		case DT_OP_GT:        return "GT";
		case DT_OP_GT_EQ:     return "GT_EQ";
		case DT_OP_LT:        return "LT";
		case DT_OP_LT_EQ:     return "LT_EQ";
		case DT_OP_OR:        return "OR";
		case DT_OP_AND:       return "AND";
		case DT_OP_LEN:       return "LEN";
		case DT_OP_POP:       return "POP";
		case DT_OP_SETG:      return "SETG";
		case DT_OP_GETG:      return "GETG";
		case DT_OP_SETL:      return "SETL";
		case DT_OP_GETL:      return "GETL";
		case DT_OP_SETU:      return "SETU";
		case DT_OP_GETU:      return "GETU";
		case DT_OP_GETI:      return "GETI";
		case DT_OP_SETI:      return "SETI";
		case DT_OP_CALL:      return "CALL";
		case DT_OP_MKSTR:     return "MKSTR";
		case DT_OP_MKFUNC:    return "MKFUNC";
		case DT_OP_MKARR:     return "MKARR";
		case DT_OP_MKMAP:     return "MKMAP";
		case DT_OP_JMP:       return "JMP";
		case DT_OP_JMP_COND:  return "JMP_COND";
		case DT_OP_REWIND:    return "REWIND";
		case DT_OP_CLOSE:     return "CLOSE";
		case DT_OP_SPREAD:    return "SPREAD";
		case DT_OP_ITER_PREP: return "ITER_PREP";
		case DT_OP_ITER:      return "ITER";
		case DT_OP_ARGS:      return "ARGS";
	}
}

static int dt_chunk_peek_at(dt_chunk* c, int idx) {

	uint8_t ins = c->code[idx];

	switch (ins) {
		case DT_OP_CONST: {
			uint8_t i2 = c->code[idx + 1];
			uint8_t i3 = c->code[idx + 2];
			printf("CONST ");
			dt_print(c->consts->values[i2 << 8 | i3]);
			return idx + 3;
		}
		case DT_OP_SETG: {
			uint8_t idx2 = c->code[idx + 1];
			printf("SEFG ");
			dt_print(c->consts->values[idx2]);
			return idx + 2;
		}
		case DT_OP_GETG: {
			uint8_t idx2 = c->code[idx + 1];
			printf("GETG ");
			dt_print(c->consts->values[idx2]);
			return idx + 2;
		}
		case DT_OP_SETL: {
			uint8_t idx2 = c->code[idx + 1];
			printf("SETL %d", idx2);
			return idx + 2;
		}
		case DT_OP_GETL: {
			uint8_t idx2 = c->code[idx + 1];
			printf("GETL %d", idx2);
			return idx + 2;
		}
		case DT_OP_SETU: {
			uint8_t idx2 = c->code[idx + 1];
			printf("SETU %d", idx2);
			return idx + 2;
		}
		case DT_OP_GETU: {
			uint8_t idx2 = c->code[idx + 1];
			printf("GETU %d", idx2);
			return idx + 2;
		}
		case DT_OP_CALL: {
			uint8_t nargs = c->code[idx + 1];
			printf("CALL %d", nargs);
			return idx + 2;
		}
		case DT_OP_MKSTR: {
			uint8_t len = c->code[idx + 1];
			printf("MKSTR %d", len);
			return idx + 2;
		}
		case DT_OP_MKFUNC: {
			uint8_t num_upvals = c->code[idx + 1];
			printf("FUNC %d", num_upvals);
			return idx + 2 + num_upvals * 2;
		}
		case DT_OP_MKARR: {
			uint8_t len = c->code[idx + 1];
			printf("MKARR %d", len);
			return idx + 2;
		}
		case DT_OP_MKMAP: {
			uint8_t len = c->code[idx + 1];
			printf("MKMAP %d", len);
			return idx + 2;
		}
		case DT_OP_JMP: {
			uint8_t d1 = c->code[idx + 1];
			uint8_t d2 = c->code[idx + 2];
			printf("JMP %d", d1 << 8 | d2);
			return idx + 3;
		}
		case DT_OP_JMP_COND: {
			uint8_t d1 = c->code[idx + 1];
			uint8_t d2 = c->code[idx + 2];
			printf("JMP_COND %d", d1 << 8 | d2);
			return idx + 3;
		}
		case DT_OP_REWIND: {
			uint8_t d1 = c->code[idx + 1];
			uint8_t d2 = c->code[idx + 2];
			printf("REWIND %d", d1 << 8 | d2);
			return idx + 3;
		}
		case DT_OP_ITER_PREP: {
			uint8_t d1 = c->code[idx + 1];
			uint8_t d2 = c->code[idx + 2];
			printf("ITER_PREP %d", d1 << 8 | d2);
			return idx + 3;
		}
		case DT_OP_ITER: {
			uint8_t d1 = c->code[idx + 1];
			uint8_t d2 = c->code[idx + 2];
			printf("ITER %d", d1 << 8 | d2);
			return idx + 2;
		}
		default:
			printf("%s", dt_op_name(ins));
			return idx + 1;
	}
}

static void dt_chunk_push(dt_chunk* c, uint8_t byte, int line) {

	if (c->cap < c->cnt + 1) {
		c->cap = c->cap < 8 ? 8 : c->cap * 2;
		c->code = realloc(c->code, c->cap * sizeof(uint8_t));
		c->lines = realloc(c->lines, c->cap * sizeof(int));
	}

	c->code[c->cnt] = byte;
	c->lines[c->cnt] = line;
	c->cnt++;

}

static int dt_chunk_add_const(dt_chunk* c, dt_val val) {
	if (c->consts->len >= UINT16_MAX) {
		// TODO: use dt_c_err()
		dt_fail("constant overflow\n");
	}
	dt_arr_push(NULL, c->consts, val);
	return c->consts->len - 1;
}

dt_vm dt_vm_new() {
	dt_vm vm = (dt_vm) {
		.func = NULL,
		.ip = NULL,
		.stack = {0},
		.stack_top = NULL,
		.stack_offset = 0,
		.globals = NULL,
		.strs = dt_map_new(NULL),
		.open_upvals = {0},
		.num_upvals = 0,
		.heaper = NULL,
		.mem = 0,
		.next_gc = DT_GC_THRESHOLD,
	};
	vm.globals = dt_map_new(&vm);
	vm.stack_top = vm.stack;
	return vm;
}

// TODO: throw mechanism
void dt_err(dt_vm* vm, char* fmt, ...) {
	dt_chunk* chunk = &vm->func->logic->chunk;
	int offset = vm->ip - chunk->code;
	int line = chunk->lines[offset];
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "line #%d: ", line);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

// get the last n'th value on stack
static dt_val dt_vm_get(dt_vm* vm, int n) {
	dt_val* pos = vm->stack_top - 1 + n;
	if (pos < vm->stack) {
		return DT_NIL;
	} else {
		return *pos;
	}
}

dt_val dt_arg(dt_vm* vm, int idx) {
	dt_val* v = vm->stack + vm->stack_offset + idx;
	if (v < vm->stack_top) {
		return *v;
	} else {
		return DT_NIL;
	}
}

bool dt_vm_check_ty(dt_vm* vm, int idx, dt_val_ty ty) {
	dt_val v = dt_vm_get(vm, idx);
	if (dt_type(v) != ty) {
		dt_err(
			vm,
			"expected a '%s' at %d, found '%s'\n",
			dt_typename(ty),
			idx,
			dt_typename(dt_type(v))
		);
		return false;
	}
	return true;
}

dt_num dt_arg_num(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_NUM);
	return dt_as_num(dt_arg(vm, idx));
}

dt_bool dt_arg_bool(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_BOOL);
	return dt_as_bool(dt_arg(vm, idx));
}

dt_str* dt_arg_str(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_STR);
	return dt_as_str(dt_arg(vm, idx));
}

char* dt_arg_cstr(dt_vm* vm, int idx) {
	dt_str* str = dt_arg_str(vm, idx);
	return str->chars;
}

char* dt_arg_cstr_dup(dt_vm* vm, int idx) {
	return dt_str_cstr(dt_arg_str(vm, idx));
}

dt_map* dt_arg_map(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_MAP);
	return dt_as_map(dt_arg(vm, idx));
}

dt_arr* dt_arg_arr(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_ARR);
	return dt_as_arr(dt_arg(vm, idx));
}

dt_func* dt_arg_func(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_FUNC);
	return dt_as_func(dt_arg(vm, idx));
}

// push a value to the stack
static void dt_vm_push(dt_vm* vm, dt_val val) {
	*vm->stack_top = val;
	vm->stack_top++;
	if (vm->stack_top - vm->stack >= DT_STACK_MAX) {
		dt_fail("stack overflow\n");
	}
}

// pop a value off the stack
static dt_val dt_vm_pop(dt_vm* vm) {
	if (vm->stack_top == vm->stack) {
		dt_fail("stack underflow\n");
		return DT_NIL;
	}
	vm->stack_top--;
	return *vm->stack_top;
}

// pop a value and perform upval close check
static void dt_vm_pop_close(dt_vm* vm) {

	dt_val* top = vm->stack_top - 1;
	dt_val* upval = NULL;

	for (int i = 0; i < vm->num_upvals; i++) {
		if (top == vm->open_upvals[i]->val) {
			if (!upval) {
				// TODO: manage upvals
				upval = dt_malloc(vm, sizeof(dt_val));
				memcpy(upval, top, sizeof(dt_val));
			}
			vm->open_upvals[i]->val = upval;
			vm->open_upvals[i]->captured = true;
			vm->open_upvals[i--] = vm->open_upvals[--vm->num_upvals];
		}
	}

	dt_vm_pop(vm);

}

static void dt_vm_stack_print(dt_vm* vm) {

	printf("[ ");

	for (dt_val* slot = vm->stack; slot < vm->stack_top; slot++) {
		dt_print(*slot);
		printf(", ");
	}

	printf("]");

}

static void dt_vm_stack_println(dt_vm* vm) {
	dt_vm_stack_print(vm);
	printf("\n");
}

// TODO: combine dt_vm_run and dt_vm_call
static void dt_vm_run(dt_vm* vm, dt_func* func);

// call a dt_func
static dt_val dt_vm_call(dt_vm* vm, dt_func* func, int nargs) {

	dt_logic* logic = func->logic;

	// balance the arg stack
	if (logic->nargs > nargs) {
		for (int i = 0; i < logic->nargs - nargs; i++) {
			dt_vm_push(vm, DT_NIL);
		}
	} else if (logic->nargs < nargs) {
		for (int i = 0; i < nargs - logic->nargs; i++) {
			dt_vm_pop(vm);
		}
	}

	nargs = logic->nargs;

	dt_func* prev_func = vm->func;
	uint8_t* prev_ip = vm->ip;
	int prev_offset = vm->stack_offset;
	vm->stack_offset = vm->stack_top - vm->stack - nargs;
	dt_vm_run(vm, func);
	int locals_cnt = vm->stack_top - vm->stack - vm->stack_offset - 1 - nargs;
	vm->stack_offset = prev_offset;
	vm->func = prev_func;
	vm->ip = prev_ip;
	dt_val ret = dt_vm_pop(vm);

	// pop locals + args + func
	for (int i = 0; i < locals_cnt + nargs; i++) {
		dt_vm_pop_close(vm);
	}

	return ret;

}

dt_val dt_call(dt_vm* vm, dt_func* func, int nargs, ...) {
	va_list args;
	va_start(args, nargs);
	for (int i = 0; i < nargs; i++) {
		dt_vm_push(vm, va_arg(args, dt_val));
	}
	dt_val ret = dt_vm_call(vm, func, nargs);
	va_end(args);
	return ret;
}

dt_val dt_call_0(dt_vm* vm, dt_func* func) {
	return dt_call(vm, func, 0);
}

dt_val dt_call_1(dt_vm* vm, dt_func* func, dt_val a1) {
	return dt_call(vm, func, 1, a1);
}

dt_val dt_call_2(dt_vm* vm, dt_func* func, dt_val a1, dt_val a2) {
	return dt_call(vm, func, 2, a1, a2);
}

dt_val dt_call_3(
	dt_vm* vm,
	dt_func* func,
	dt_val a1,
	dt_val a2,
	dt_val a3
) {
	return dt_call(vm, func, 3, a1, a2, a3);
}

dt_val dt_call_4(
	dt_vm* vm,
	dt_func* func,
	dt_val a1,
	dt_val a2,
	dt_val a3,
	dt_val a4
) {
	return dt_call(vm, func, 4, a1, a2, a3, a4);
}

// mark a value for not gc this turn
static void dt_val_mark(dt_val val) {
	switch (dt_type(val)) {
		case DT_VAL_STR:
			dt_str_mark(dt_as_str(val));
			break;
		case DT_VAL_ARR:
			dt_arr_mark(dt_as_arr(val));
			break;
		case DT_VAL_MAP:
			dt_map_mark(dt_as_map(val));
			break;
		case DT_VAL_FUNC:
			dt_func_mark(dt_as_func(val));
			break;
		default:
			break;
	}
}

// add a heap value to vm gc list
static void dt_manage(dt_vm* vm, dt_heaper* h, uint8_t ty) {
	h->type = ty;
	h->marked = false;
	h->nxt = vm->heaper;
	vm->heaper = h;
}

// mark everything on root
static void dt_gc_mark(dt_vm* vm) {
	for (dt_val* val = vm->stack; val < vm->stack_top; val++) {
		dt_val_mark(*val);
	}
	dt_func_mark(vm->func);
	// TODO: don't traverse global everytime if it won't change
	dt_map_mark(vm->globals);
}

// free all unmarked heap values
static void dt_gc_sweep(dt_vm* vm) {
#ifdef DT_GC_LOG
	size_t start_mem = vm->mem;
	printf("--- GC START\n");
#endif
	dt_heaper* heaper = vm->heaper;
	dt_heaper* prev = NULL;
	while (heaper) {
		if (!heaper->marked) {
			dt_heaper* unreached = heaper;
			heaper = heaper->nxt;
			if (prev != NULL) {
				prev->nxt = heaper;
			} else {
				vm->heaper = heaper;
			}
			switch (unreached->type) {
				case DT_VAL_STR: {
#ifdef DT_GC_LOG
					dt_str* str = (dt_str*)unreached;
					printf("FREE STR ");
					dt_str_println(str);
#endif
					dt_str_free(vm, (dt_str*)unreached);
					break;
				}
				case DT_VAL_ARR: {
#ifdef DT_GC_LOG
					dt_arr* arr = (dt_arr*)unreached;
					printf("FREE ARR #%d\n", arr->len);
#endif
					dt_arr_free(vm, (dt_arr*)unreached);
					break;
				}
				case DT_VAL_MAP: {
#ifdef DT_GC_LOG
					dt_map* map = (dt_map*)unreached;
					printf("FREE MAP #%d\n", map->cnt);
#endif
					dt_map_free(vm, (dt_map*)unreached);
					break;
				}
				case DT_VAL_FUNC: {
#ifdef DT_GC_LOG
					dt_func* func = (dt_func*)unreached;
					printf("FREE FUNC ");
					dt_func_println(func);
#endif
					dt_func_free(vm, (dt_func*)unreached);
					break;
				}
				case DT_VAL_UPVAL: {
#ifdef DT_GC_LOG
					dt_upval2* upval = (dt_upval2*)unreached;
					printf("FREE UPVAL ");
					dt_println(*upval->val);
#endif
					dt_upval_free(vm, (dt_upval2*)unreached);
					break;
				}
				case DT_VAL_CDATA: {
#ifdef DT_GC_LOG
					dt_cdata* cdata = (dt_cdata*)unreached;
					printf("FREE CDATA ");
					dt_cdata_println(cdata);
#endif
					dt_cdata_free(vm, (dt_cdata*)unreached);
					break;
				}
			}
		} else {
			heaper->marked = false;
			prev = heaper;
			heaper = heaper->nxt;
		}
	}
#ifdef DT_GC_LOG
	printf("--- GC END");
	if (start_mem != vm->mem) {
		printf(" (%zu -> %zu)\n", start_mem, vm->mem);
	} else {
		printf(" (%zu)\n", vm->mem);
	}
#endif
}

// mark a value to not get collected by gc unless explicitly unreg
void dt_keep(dt_vm* vm, dt_val v) {
	if (!dt_is_heap(v)) {
		return;
	}
	// TODO
}

// mark a value to get collected by gc
void dt_unkeep(dt_vm* vm, dt_val v) {
	if (!dt_is_heap(v)) {
		return;
	}
	// TODO
}

// run gc if needed
static void dt_vm_gc_check(dt_vm* vm) {
#ifdef DT_GC_STRESS
	dt_gc_run(vm);
#else
	if (vm->mem >= vm->next_gc) {
		dt_gc_run(vm);
		vm->next_gc *= DT_GC_GROW;
	}
#endif
}

// manually start a mark & sweep process
void dt_gc_run(dt_vm* vm) {
	dt_gc_mark(vm);
	dt_gc_sweep(vm);
}

void dt_vm_free(dt_vm* vm) {
	dt_gc_sweep(vm);
	dt_map_free(NULL, vm->strs);
	memset(vm, 0, sizeof(dt_vm));
}

// start the runtime and run a dt_func
static void dt_vm_run(dt_vm* vm, dt_func* func) {

	vm->func = func;
	dt_chunk* chunk = &func->logic->chunk;
	vm->ip = chunk->code;

	for (;;) {

#ifdef DT_VM_LOG

		int offset = vm->ip - chunk->code;
		int line = chunk->lines[offset];

		if (offset == 0 || line != chunk->lines[offset - 1]) {
			printf("%6d ", line);
		} else {
			printf("       ");
		}

		dt_vm_stack_println(vm);

		printf("          -> ");
		dt_chunk_peek_at(chunk, offset);
		printf("\n");

#endif

		uint8_t ins = *vm->ip++;

		switch (ins) {

			case DT_OP_CONST: {
				dt_vm_push(
					vm,
					chunk->consts->values[*vm->ip++ << 8 | *vm->ip++]
				);
				break;
			}

			case DT_OP_NIL:
				dt_vm_push(vm, DT_NIL);
				break;

			case DT_OP_TRUE:
				dt_vm_push(vm, DT_TRUE);
				break;

			case DT_OP_FALSE:
				dt_vm_push(vm, DT_FALSE);
				break;

			case DT_OP_ADD: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(dt_as_num(a) + dt_as_num(b)));
				} else if (dt_type(a) == DT_VAL_STR && dt_type(b) == DT_VAL_STR) {
					dt_vm_push(vm, dt_to_str(dt_str_concat(vm, dt_as_str(a), dt_as_str(b))));
				} else if (dt_type(a) == DT_VAL_ARR && dt_type(b) == DT_VAL_ARR) {
					dt_vm_push(vm, dt_to_arr(dt_arr_concat(vm, dt_as_arr(a), dt_as_arr(b))));
				} else if (dt_type(a) == DT_VAL_STR || dt_type(b) == DT_VAL_STR) {
					dt_str* a_str = dt_val_to_str(vm, a);
					dt_str* b_str = dt_val_to_str(vm, b);
					if (a_str == NULL || b_str == NULL) {
						dt_err(
							vm,
							"cannot add a '%s' with '%s'\n",
							dt_typename(dt_type(a)),
							dt_typename(dt_type(b))
						);
						dt_vm_push(vm, DT_NIL);
					} else {
						dt_vm_push(vm, dt_to_str(dt_str_concat(vm, a_str, b_str)));
					}
				} else {
					dt_err(
						vm,
						"cannot add a '%s' with '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_SUB: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(dt_as_num(a) - dt_as_num(b)));
				} else {
					dt_err(
						vm,
						"cannot sub a '%s' by '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_MUL: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(dt_as_num(a) * dt_as_num(b)));
				} else {
					dt_err(
						vm,
						"cannot mul a '%s' with '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_DIV: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(dt_as_num(a) / dt_as_num(b)));
				} else {
					dt_err(
						vm,
						"cannot div a '%s' by '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_MOD: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(fmodf(dt_as_num(a), dt_as_num(b))));
				} else {
					dt_err(
						vm,
						"cannot mod a '%s' by '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_POW: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(powf(dt_as_num(a), dt_as_num(b))));
				} else {
					dt_err(
						vm,
						"cannot pow a '%s' by '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_SPREAD: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_range((dt_range) {
						.start = dt_as_num(a),
						.end = dt_as_num(b),
					}));
				} else {
					dt_err(
						vm,
						"cannot spread a '%s' and '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_NEG: {
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(-dt_as_num(a)));
				} else {
					dt_err(
						vm,
						"cannot negate a '%s'\n",
						dt_typename(dt_type(a))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_NOT: {
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_to_bool(!dt_as_bool(a)));
				} else {
					dt_err(
						vm,
						"cannot neg a '%s'\n",
						dt_typename(dt_type(a))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_LEN: {
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) == DT_VAL_STR) {
					dt_vm_push(vm, dt_to_num(dt_as_str(a)->len));
				} else if (dt_type(a) == DT_VAL_ARR) {
					dt_vm_push(vm, dt_to_num(dt_as_arr(a)->len));
				} else if (dt_type(a) == DT_VAL_MAP) {
					dt_vm_push(vm, dt_to_num(dt_as_map(a)->cnt));
				} else if (dt_type(a) == DT_VAL_RANGE) {
					dt_vm_push(vm, dt_to_num(abs(dt_as_range(a).end - dt_as_range(a).start)));
				} else {
					dt_err(
						vm,
						"cannot get len of a '%s'\n",
						dt_typename(dt_type(a))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_EQ: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) != dt_type(b)) {
					dt_vm_push(vm, DT_FALSE);
				} else if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_bool(dt_as_num(a) == dt_as_num(b)));
				} else if (dt_type(a) == DT_VAL_BOOL && dt_type(b) == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_to_bool(dt_as_bool(a) == dt_as_bool(b)));
				} else if (dt_type(a) == DT_VAL_NIL && dt_type(b) == DT_VAL_NIL) {
					dt_vm_push(vm, DT_TRUE);
				} else if (dt_type(a) == DT_VAL_STR && dt_type(b) == DT_VAL_STR) {
					dt_vm_push(vm, dt_to_bool(dt_str_eq(dt_as_str(a), dt_as_str(b))));
				} else {
					dt_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_FALSE);
				}
				break;
			}

			case DT_OP_GT: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) != dt_type(b)) {
					dt_vm_push(vm, DT_FALSE);
				} else if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_bool(dt_as_num(a) > dt_as_num(b)));
				} else {
					dt_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_FALSE);
				}
				break;
			}

			case DT_OP_GT_EQ: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) != dt_type(b)) {
					dt_vm_push(vm, DT_FALSE);
				} else if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_bool(dt_as_num(a) >= dt_as_num(b)));
				} else {
					dt_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_FALSE);
				}
				break;
			}

			case DT_OP_LT: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) != dt_type(b)) {
					dt_vm_push(vm, DT_FALSE);
				} else if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_bool(dt_as_num(a) < dt_as_num(b)));
				} else {
					dt_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_FALSE);
				}
				break;
			}

			case DT_OP_LT_EQ: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) != dt_type(b)) {
					dt_vm_push(vm, DT_FALSE);
				} else if (dt_type(a) == DT_VAL_NUM && dt_type(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_bool(dt_as_num(a) <= dt_as_num(b)));
				} else {
					dt_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_FALSE);
				}
				break;
			}

			case DT_OP_OR: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) != dt_type(b)) {
					dt_vm_push(vm, DT_FALSE);
				} else if (dt_type(a) == DT_VAL_BOOL && dt_type(b) == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_to_bool(dt_as_bool(a) || dt_as_bool(b)));
				} else {
					dt_err(
						vm,
						"cannot or a '%s' with '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_FALSE);
				}
				break;
			}

			case DT_OP_AND: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (dt_type(a) != dt_type(b)) {
					dt_vm_push(vm, DT_FALSE);
				} else if (dt_type(a) == DT_VAL_BOOL && dt_type(b) == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_to_bool(dt_as_bool(a) && dt_as_bool(b)));
				} else {
					dt_err(
						vm,
						"cannot and a '%s' with '%s'\n",
						dt_typename(dt_type(a)),
						dt_typename(dt_type(b))
					);
					dt_vm_push(vm, DT_FALSE);
				}
				break;
			}

			case DT_OP_POP:
				dt_vm_pop(vm);
				break;

			case DT_OP_GETG: {
				if (!vm->globals) {
					dt_vm_push(vm, DT_NIL);
				}
				dt_val name = chunk->consts->values[*vm->ip++];
				if (dt_type(name) == DT_VAL_STR) {
					dt_val val = dt_map_get(vm->globals, dt_as_str(name));
					dt_vm_push(vm, val);
				} else {
					dt_err(
						vm,
						"expected var name to be 'str' found '%s'\n",
						dt_typename(dt_type(name))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_SETG: {
				dt_val name = chunk->consts->values[*vm->ip++];
				dt_err(vm, "cannot set global '%s'\n", dt_as_str(name)->chars);
				break;
			}

			case DT_OP_GETL: {
				dt_vm_push(vm, vm->stack[*vm->ip++ + vm->stack_offset]);
				break;
			}

			case DT_OP_SETL: {
				vm->stack[*vm->ip++ + vm->stack_offset] = dt_vm_get(vm, 0);
				break;
			}

			case DT_OP_GETU: {
				dt_vm_push(vm, *vm->func->upvals[*vm->ip++]->val);
				break;
			}

			case DT_OP_SETU: {
				*vm->func->upvals[*vm->ip++]->val = dt_vm_get(vm, 0);
				break;
			}

			case DT_OP_ARGS: {
				// TODO
				dt_vm_push(vm, DT_NIL);
				break;
			}

			case DT_OP_GETI: {
				dt_val key = dt_vm_pop(vm);
				dt_val val = dt_vm_pop(vm);
				switch (dt_type(val)) {
					case DT_VAL_ARR:
						if (dt_type(key) == DT_VAL_NUM) {
							dt_vm_push(vm, dt_arr_get(dt_as_arr(val), dt_as_num(key)));
						} else if (dt_type(key) == DT_VAL_STR) {
							// TODO
							dt_vm_push(vm, DT_NIL);
						} else if (dt_type(key) == DT_VAL_RANGE) {
							dt_range range = dt_as_range(key);
							if (range.end < range.start) {
								int tmp = range.start;
								range.start = range.end;
								range.end = tmp;
							}
							dt_arr* arr = dt_as_arr(val);
							if (range.start >= arr->len || range.end > arr->len) {
								dt_vm_push(vm, DT_NIL);
							} else {
								dt_arr* arr2 = dt_arr_new_len(vm, range.end - range.start);
								for (int i = range.start; i < range.end; i++) {
									dt_arr_set(
										vm,
										arr2,
										i - range.start,
										dt_arr_get(arr, i)
									);
								}
								dt_vm_push(vm, dt_to_arr(arr2));
							}
						} else {
							dt_err(
								vm,
								"invalid arr idx type '%s'\n",
								dt_typename(dt_type(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					case DT_VAL_MAP:
						if (dt_type(key) == DT_VAL_STR) {
							dt_vm_push(vm, dt_map_get(dt_as_map(val), dt_as_str(key)));
						} else {
							dt_err(
								vm,
								"invalid map idx type '%s'\n",
								dt_typename(dt_type(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					case DT_VAL_STR:
						if (dt_type(key) == DT_VAL_NUM) {
							int idx = (int)dt_as_num(key);
							dt_str* str = dt_as_str(val);
							if (idx >= str->len) {
								dt_vm_push(vm, DT_NIL);
							} else {
								dt_vm_push(vm, dt_to_str(dt_str_new_len(
									vm,
									str->chars + idx,
									1
								)));
							}
						} else if (dt_type(key) == DT_VAL_STR) {
							// TODO
							dt_vm_push(vm, DT_NIL);
						} else if (dt_type(key) == DT_VAL_RANGE) {
							dt_range range = dt_as_range(key);
							if (range.end < range.start) {
								int tmp = range.start;
								range.start = range.end;
								range.end = tmp;
							}
							dt_str* str = dt_as_str(val);
							if (range.start >= str->len || range.end > str->len) {
								dt_vm_push(vm, DT_NIL);
							} else {
								dt_vm_push(vm, dt_to_str(dt_str_new_len(vm,
									str->chars + range.start,
									range.end - range.start
								)));
							}
						} else {
							dt_err(
								vm,
								"invalid str idx type '%s'\n",
								dt_typename(dt_type(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					case DT_VAL_NUM:
						if (dt_type(key) == DT_VAL_STR) {
							// TODO
							dt_vm_push(vm, DT_NIL);
						}
					default:
						dt_err(
							vm,
							"cannot index a '%s'\n",
							dt_typename(dt_type(val))
						);
						dt_vm_push(vm, DT_NIL);
						break;
				}
				break;
			}

			case DT_OP_SETI: {
				dt_val val = dt_vm_pop(vm);
				dt_val key = dt_vm_pop(vm);
				dt_val parent = dt_vm_pop(vm);
				switch (dt_type(parent)) {
					case DT_VAL_ARR:
						if (dt_type(key) == DT_VAL_NUM) {
							int idx = dt_as_num(key);
							dt_arr_set(vm, dt_as_arr(parent), idx, val);
							dt_vm_push(vm, val);
						} else {
							dt_err(
								vm,
								"invalid arr idx type '%s'\n",
								dt_typename(dt_type(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					case DT_VAL_MAP:
						if (dt_type(key) == DT_VAL_STR) {
							dt_map_set(vm, dt_as_map(parent), dt_as_str(key), val);
							dt_vm_push(vm, val);
						} else {
							dt_err(
								vm,
								"invalid map idx type '%s'\n",
								dt_typename(dt_type(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					default:
						dt_err(
							vm,
							"cannot set index a '%s'\n",
							dt_typename(dt_type(val))
						);
						dt_vm_push(vm, DT_NIL);
						break;
				}
				break;
			}

			// TODO: improve
			case DT_OP_CALL: {

				int nargs = *vm->ip++;

				dt_val val = dt_vm_get(vm, -nargs);
				dt_val ret = DT_NIL;

				if (dt_type(val) == DT_VAL_CFUNC) {

					int prev_offset = vm->stack_offset;
					vm->stack_offset = vm->stack_top - vm->stack - nargs;
					ret = (dt_as_cfunc(val))(vm, nargs);
					vm->stack_offset = prev_offset;

					// pop args
					for (int i = 0; i < nargs; i++) {
						dt_vm_pop_close(vm);
					}

				} else if (dt_type(val) == DT_VAL_FUNC) {
					ret = dt_vm_call(vm, dt_as_func(val), nargs);
				} else {
					dt_err(
						vm,
						"cannot call a '%s'\n",
						dt_typename(dt_type(val))
					);
					for (int i = 0; i < nargs; i++) {
						dt_vm_pop_close(vm);
					}
				}

				// pop func
				dt_vm_pop(vm);
				dt_vm_push(vm, ret);

				break;

			}

			case DT_OP_CLOSE: {
				dt_vm_pop_close(vm);
				break;
			}

			case DT_OP_MKSTR: {
				int len = *vm->ip++;
				dt_str* str = dt_str_new_len(vm, "", 0);
				for (int i = 0; i < len; i++) {
					dt_val v = *(vm->stack_top - len + i);
					// TODO
				}
				vm->stack_top -= len;
				dt_vm_push(vm, dt_to_str(str));
				dt_vm_gc_check(vm);
				break;
			}

			case DT_OP_MKARR: {
				int len = *vm->ip++;
				dt_arr* arr = dt_arr_new_len(vm, len);
				for (int i = 0; i < len; i++) {
					dt_val v = *(vm->stack_top - len + i);
					if (dt_type(v) == DT_VAL_RANGE) {
						dt_range r = dt_as_range(v);
						if (r.end >= r.start) {
							for (int j = r.start; j < r.end; j++) {
								dt_arr_push(vm, arr, dt_to_num(j));
							}
						} else {
							for (int j = r.start; j > r.end; j--) {
								dt_arr_push(vm, arr, dt_to_num(j));
							}
						}
					} else {
						dt_arr_push(vm, arr, v);
					}
				}
				vm->stack_top -= len;
				dt_vm_push(vm, dt_to_arr(arr));
				dt_vm_gc_check(vm);
				break;
			}

			case DT_OP_MKMAP: {
				int len = *vm->ip++;
				dt_map* map = dt_map_new_len(vm, len);
				for (int i = 0; i < len; i++) {
					dt_val val = dt_vm_pop(vm);
					dt_val key = dt_vm_pop(vm);
					if (dt_type(key) == DT_VAL_STR) {
						dt_map_set(vm, map, dt_as_str(key), val);
					} else {
						dt_err(
							vm,
							"expected key to be 'str', found '%s'\n",
							dt_typename(dt_type(key))
						);
					}
				}
				dt_vm_push(vm, dt_to_map(map));
				dt_vm_gc_check(vm);
				break;
			}

			// TODO: clean
			case DT_OP_MKFUNC: {
				dt_val val = dt_vm_pop(vm);
				uint8_t num_upvals = *vm->ip++;
				if (dt_type(val) != DT_VAL_LOGIC) {
					// unreachable
					dt_err(
						vm,
						"cannot make a func out of '%s'\n",
						dt_typename(dt_type(val))
					);
				}
				dt_func* func = dt_malloc(vm, sizeof(dt_func));
				func->logic = dt_as_logic(val);
				func->num_upvals = num_upvals;
				func->upvals = dt_malloc(vm, sizeof(dt_upval2*) * num_upvals);
				for (int i = 0; i < num_upvals; i++) {
					bool local = *vm->ip++;
					uint8_t idx = *vm->ip++;
					if (local) {
						bool found = false;
						for (int j = 0; j < vm->num_upvals; j++) {
							if (vm->open_upvals[j]->val == vm->stack + vm->stack_offset + idx) {
								found = true;
								func->upvals[i] = vm->open_upvals[j];
								break;
							}
						}
						if (!found) {
							dt_upval2* upval = dt_malloc(vm, sizeof(dt_upval2));
							upval->val = vm->stack + vm->stack_offset + idx;
							upval->captured = false;
							vm->open_upvals[vm->num_upvals++] = upval;
							dt_manage(vm, (dt_heaper*)upval, DT_VAL_UPVAL);
							func->upvals[i] = upval;
						}
					} else {
						func->upvals[i] = vm->func->upvals[idx];
					}
				}
				dt_vm_push(vm, dt_to_func(func));
				dt_manage(vm, (dt_heaper*)func, DT_VAL_FUNC);
				dt_vm_gc_check(vm);
				break;
			}

			case DT_OP_JMP: {
				vm->ip += *vm->ip++ << 8 | *vm->ip++;
				break;
			}

			case DT_OP_JMP_COND: {
				int dis = *vm->ip++ << 8 | *vm->ip++;
				dt_val cond = dt_vm_pop(vm);
				bool jump = true;
				if (dt_type(cond) != DT_VAL_BOOL) {
					dt_err(
						vm,
						"expected cond to be 'bool', found '%s'\n",
						dt_typename(dt_type(cond))
					);
				} else {
					jump = !dt_as_bool(cond);
				}
				if (jump) {
					vm->ip += dis;
				}
				break;
			}

			case DT_OP_REWIND: {
				vm->ip -= *vm->ip++ << 8 | *vm->ip++;
				break;
			}

			// TODO: clean
			case DT_OP_ITER_PREP: {
				dt_val iter = dt_vm_get(vm, 0);
				int dis = *vm->ip++ << 8 | *vm->ip++;
				switch (dt_type(iter)) {
					case DT_VAL_ARR:
						if (dt_as_arr(iter)->len == 0) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						dt_vm_push(vm, dt_to_num(0));
						dt_vm_push(vm, dt_arr_get(dt_as_arr(iter), 0));
						break;
					case DT_VAL_STR:
						if (dt_as_str(iter)->len == 0) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						dt_vm_push(vm, dt_to_num(0));
						dt_vm_push(vm, dt_to_str(dt_str_new_len(
							vm,
							dt_as_str(iter)->chars,
							1
						)));
						break;
					case DT_VAL_MAP:
						if (dt_as_map(iter)->cnt == 0) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						int i = 0;
						for (; i < dt_as_map(iter)->cap; i++) {
							if (dt_as_map(iter)->entries[i].key) {
								break;
							}
						}
						dt_vm_push(vm, dt_to_num(i));
						dt_str* key = dt_as_map(iter)->entries[i].key;
						dt_vm_push(vm, dt_to_str(key));
						break;
					case DT_VAL_RANGE:
						if (dt_as_range(iter).start == dt_as_range(iter).end) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						dt_vm_push(vm, dt_to_num(0));
						dt_vm_push(vm, dt_to_num(dt_as_range(iter).start));
						break;
					default:
						dt_err(
							vm,
							"'%s' is not iterable\n",
							dt_typename(dt_type(iter))
						);
						dt_vm_pop(vm);
						vm->ip += dis;
						break;
				}
				break;
			}

			// TODO: clean
			case DT_OP_ITER: {
				int dis = *vm->ip++ << 8 | *vm->ip++;
				dt_val iter = dt_vm_get(vm, -2);
				dt_val* n = vm->stack_top - 2;
				int idx = dt_as_num(*n);
				idx++;
				*n = dt_to_num(idx);
				switch (dt_type(iter)) {
					case DT_VAL_ARR:
						if (idx < dt_as_arr(iter)->len) {
							*(vm->stack_top - 1) = dt_arr_get(dt_as_arr(iter), idx);
							vm->ip -= dis;
						}
						break;
					case DT_VAL_STR:
						if (idx < dt_as_str(iter)->len) {
							*(vm->stack_top - 1) = dt_to_str(dt_str_new_len(
								vm,
								dt_as_str(iter)->chars + idx,
								1
							));
							vm->ip -= dis;
						}
						break;
					case DT_VAL_MAP:
						if (idx < dt_as_map(iter)->cap) {
							while (idx < dt_as_map(iter)->cap && !dt_as_map(iter)->entries[idx].key) {
								idx++;
							}
							if (idx < dt_as_map(iter)->cap) {
								dt_str* key = dt_as_map(iter)->entries[idx].key;
								*(vm->stack_top - 1) = dt_to_str(key);
								*n = dt_to_num(idx);
								vm->ip -= dis;
							}
						}
						break;
					case DT_VAL_RANGE: {
						int start = dt_as_range(iter).start;
						int end = dt_as_range(iter).end;
						bool done = false;
						if (end < start) {
							done = idx >= start - end;
						} else {
							done = idx >= end - start;
						}
						if (!done) {
							if (end < start) {
								*(vm->stack_top - 1) = dt_to_num(start - idx);
							} else {
								*(vm->stack_top - 1) = dt_to_num(start + idx);
							}
							vm->ip -= dis;
						}
						break;
					}
					default:
						dt_err(
							vm,
							"'%s' is not iterable\n",
							dt_typename(dt_type(iter))
						);
						// unreachable
						break;
				}
				break;
			}

			case DT_OP_STOP: {
				return;
			}

		}

	}

}

static dt_scanner dt_scanner_new(char* src) {
	return (dt_scanner) {
		.start = src,
		.cur = src,
		.line = 1,
	};
}

static bool dt_scanner_ended(dt_scanner* s) {
  return *s->cur == '\0';
}

static dt_token dt_scanner_make_token(dt_scanner* s, dt_token_ty type) {
	return (dt_token) {
		.type = type,
		.start = s->start,
		.len = (int)(s->cur - s->start),
		.line = s->line,
	};
}

static bool dt_scanner_match(dt_scanner* s, char expected) {
	if (dt_scanner_ended(s)) {
		return false;
	}
	if (*s->cur != expected) {
		return false;
	}
	s->cur++;
	return true;
}

static char dt_scanner_nxt(dt_scanner* s) {
	s->cur++;
	return s->cur[-1];
}

static void dt_scanner_skip(dt_scanner* s, int n) {
	s->cur += n;
}

static char dt_scanner_peek(dt_scanner* s) {
	return *s->cur;
}

static char dt_scanner_peek_nxt(dt_scanner* s) {
	if (dt_scanner_ended(s)) {
		return '\0';
	}
	return s->cur[1];
}

static char dt_scanner_peek_nxt_nxt(dt_scanner* s) {
	if (dt_scanner_ended(s)) {
		return '\0';
	}
	return s->cur[2];
}

static void dt_scanner_skip_ws(dt_scanner* s) {
	for (;;) {
		char c = *s->cur;
		switch (c) {
			case ' ':
			case '\t':
				dt_scanner_nxt(s);
				break;
			case '\n':
				s->line++;
				dt_scanner_nxt(s);
				break;
			case '-':
				if (dt_scanner_peek_nxt(s) == '-') {
					// TODO
					if (dt_scanner_peek_nxt_nxt(s) == '-') {
						dt_scanner_skip(s, 3);
						for (;;) {
							if (dt_scanner_peek(s) == '\n') {
								s->line++;
							}
							if (strncmp(s->cur, "---", 3) == 0) {
								dt_scanner_skip(s, 3);
								break;
							}
							if (dt_scanner_ended(s)) {
								break;
							}
							dt_scanner_nxt(s);
						}
					} else {
						while (*s->cur != '\n' && !dt_scanner_ended(s)) {
							dt_scanner_nxt(s);
						}
					}
				} else {
					return;
				}
				break;
			default:
				return;
		}
	}
}

static bool dt_is_digit(char c) {
	return c >= '0' && c <= '9';
}

static bool dt_is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static dt_token dt_scanner_scan_str(dt_scanner* s) {

	char last = dt_scanner_peek(s);

	for (;;) {
		if (dt_scanner_ended(s)) {
			break;
		}
		char cur = dt_scanner_peek(s);
		if (cur == '"') {
			if (last == '\\') {
				// TODO
			} else {
				break;
			}
		}
		if (cur == '\n') {
			s->line++;
		}
		last = dt_scanner_nxt(s);
	}

	if (dt_scanner_ended(s)) {
		return dt_scanner_make_token(s, DT_TOKEN_ERR);
	}

	// closing "
	dt_scanner_nxt(s);

	return dt_scanner_make_token(s, DT_TOKEN_STR);

}

static dt_token dt_scanner_scan_num(dt_scanner* s) {

	while (dt_is_digit(dt_scanner_peek(s))) {
		dt_scanner_nxt(s);
	}

	// look for '.'
	if (dt_scanner_peek(s) == '.' && dt_is_digit(dt_scanner_peek_nxt(s))) {
		// consume '.'
		dt_scanner_nxt(s);

		while (dt_is_digit(dt_scanner_peek(s))) {
			dt_scanner_nxt(s);
		}
	}

	return dt_scanner_make_token(s, DT_TOKEN_NUM);

}

static bool dt_scanner_check_key(dt_scanner* s, char* key) {
	int len = strlen(key);
	return s->cur - s->start == len && memcmp(s->start, key, len) == 0;
}

static dt_token_ty dt_scanner_ident_type(dt_scanner* s) {
	if        (dt_scanner_check_key(s, "T")) {
		return DT_TOKEN_T;
	} else if (dt_scanner_check_key(s, "F")) {
		return DT_TOKEN_F;
	} else {
		return DT_TOKEN_IDENT;
	}
}

static dt_token dt_scanner_scan_ident(dt_scanner* s) {

	while (dt_is_alpha(dt_scanner_peek(s)) || dt_is_digit(dt_scanner_peek(s))) {
		dt_scanner_nxt(s);
	}

	return dt_scanner_make_token(s, dt_scanner_ident_type(s));

}

static dt_token dt_scanner_scan(dt_scanner* s) {

	dt_scanner_skip_ws(s);
	s->start = s->cur;

	if (dt_scanner_ended(s)) {
		return dt_scanner_make_token(s, DT_TOKEN_END);
	}

	char c = dt_scanner_nxt(s);

	if (dt_is_digit(c)) {
		return dt_scanner_scan_num(s);
	}

	if (dt_is_alpha(c)) {
		return dt_scanner_scan_ident(s);
	}

	switch (c) {
		case '(': return dt_scanner_make_token(s, DT_TOKEN_LPAREN);
		case ')': return dt_scanner_make_token(s, DT_TOKEN_RPAREN);
		case '{': return dt_scanner_make_token(s, DT_TOKEN_LBRACE);
		case '}': return dt_scanner_make_token(s, DT_TOKEN_RBRACE);
		case '[': return dt_scanner_make_token(s, DT_TOKEN_LBRACKET);
		case ']': return dt_scanner_make_token(s, DT_TOKEN_RBRACKET);
		case ',': return dt_scanner_make_token(s, DT_TOKEN_COMMA);
		case '#': return dt_scanner_make_token(s, DT_TOKEN_HASH);
		case '\\': return dt_scanner_make_token(s, DT_TOKEN_BACKSLASH);
		case '?': return dt_scanner_make_token(s, DT_TOKEN_QUESTION);
		case '^': return dt_scanner_make_token(s, DT_TOKEN_CARET);
		case '$':
			if (dt_scanner_match(s, '{')) {
				return dt_scanner_make_token(s, DT_TOKEN_DOLLAR_LBRACE);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_DOLLAR);
			}
		case ':':
			if (dt_scanner_match(s, ':')) {
				return dt_scanner_make_token(s, DT_TOKEN_COLON_COLON);
			} else if (dt_scanner_match(s, 'D')) {
				return dt_scanner_make_token(s, DT_TOKEN_COLON_D);
			} else if (dt_scanner_match(s, '(')) {
				return dt_scanner_make_token(s, DT_TOKEN_COLON_LPAREN);
			} else if (dt_scanner_match(s, ')')) {
				return dt_scanner_make_token(s, DT_TOKEN_COLON_RPAREN);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_COLON);
			}
		case '&':
			return dt_scanner_make_token(s,
				dt_scanner_match(s, '&') ? DT_TOKEN_AND_AND : DT_TOKEN_AND
			);
		case '|':
			return dt_scanner_make_token(s,
				dt_scanner_match(s, '|') ? DT_TOKEN_OR_OR : DT_TOKEN_OR
			);
		case '.':
			return dt_scanner_make_token(s,
				dt_scanner_match(s, '.') ? DT_TOKEN_DOT_DOT : DT_TOKEN_DOT
			);
		case '+':
			if (dt_scanner_match(s, '=')) {
				return dt_scanner_make_token(s, DT_TOKEN_PLUS_EQ);
			} else if (dt_scanner_match(s, '+')) {
				return dt_scanner_make_token(s, DT_TOKEN_PLUS_PLUS);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_PLUS);
			}
		case '-':
			if (dt_scanner_match(s, '=')) {
				return dt_scanner_make_token(s, DT_TOKEN_MINUS_EQ);
			} else if (dt_scanner_match(s, '-')) {
				return dt_scanner_make_token(s, DT_TOKEN_MINUS_MINUS);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_MINUS);
			}
		case '*':
			return dt_scanner_make_token(s,
				dt_scanner_match(s, '=') ? DT_TOKEN_STAR_EQ : DT_TOKEN_STAR
			);
		case '/':
			return dt_scanner_make_token(s,
				dt_scanner_match(s, '=') ? DT_TOKEN_SLASH_EQ : DT_TOKEN_SLASH
			);
		case '!':
			return dt_scanner_make_token(s,
				dt_scanner_match(s, '=') ? DT_TOKEN_BANG_EQ : DT_TOKEN_BANG
			);
		case '=':
			return dt_scanner_make_token(s,
				dt_scanner_match(s, '=') ? DT_TOKEN_EQ_EQ : DT_TOKEN_EQ
			);
		case '<':
			if (dt_scanner_match(s, '=')) {
				return dt_scanner_make_token(s, DT_TOKEN_LT_EQ);
			} else if (dt_scanner_match(s, '<')) {
				return dt_scanner_make_token(s, DT_TOKEN_LT_LT);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_LT);
			}
		case '>':
			if (dt_scanner_match(s, '=')) {
				return dt_scanner_make_token(s, DT_TOKEN_GT_EQ);
			} else if (dt_scanner_match(s, '>')) {
				return dt_scanner_make_token(s, DT_TOKEN_GT_GT);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_GT);
			}
		case '~':
			if (dt_scanner_match(s, '>')) {
				return dt_scanner_make_token(s, DT_TOKEN_TILDE_GT);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_TILDE);
			}
		case '@':
			if (dt_scanner_match(s, '>')) {
				return dt_scanner_make_token(s, DT_TOKEN_AT_GT);
			} else if (dt_scanner_match(s, '^')) {
				return dt_scanner_make_token(s, DT_TOKEN_AT_CARET);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_AT);
			}
		case '%':
			if (dt_scanner_match(s, '>')) {
				return dt_scanner_make_token(s, DT_TOKEN_PERCENT_GT);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_PERCENT);
			}
		case '"': return dt_scanner_scan_str(s);
	}

	return dt_scanner_make_token(s, DT_TOKEN_ERR);

}

static dt_funcenv dt_funcenv_new() {
	return (dt_funcenv) {
		.parent = NULL,
		.chunk = dt_chunk_new(),
		.scopes = {0},
		.cur_depth = 0,
		.locals = {0},
		.num_locals = 0,
		.upvals = {0},
		.num_upvals = 0,
		.breaks = {0},
		.num_breaks = 0,
	};
}

static void dt_funcenv_free(dt_funcenv* env) {
	dt_chunk_free(&env->chunk);
	memset(env, 0, sizeof(dt_funcenv));
}

static dt_compiler dt_compiler_new(char* code) {

	dt_compiler c = (dt_compiler) {
		.scanner = dt_scanner_new(code),
		.parser = {0},
		.base_env = dt_funcenv_new(),
		.env = NULL,
		.types = {0},
		.num_types = 0,
		.funcs = {0},
		.num_funcs = 0,
	};

	c.env = &c.base_env;

	return c;

}

static void dt_compiler_free(dt_compiler* c) {
	dt_funcenv_free(&c->base_env);
	memset(c, 0, sizeof(dt_compiler));
}

static void dt_c_prec(dt_compiler* c, dt_prec prec);
static void dt_c_binary(dt_compiler* c);

// compile time error
static void dt_c_err(dt_compiler* c, char* fmt, ...) {
	dt_token* cur = &c->parser.cur;
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "line #%d: ", cur->line);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

// emit opcode
static void dt_c_emit(dt_compiler* c, dt_op op) {
	dt_chunk_push(&c->env->chunk, op, c->parser.prev.line);
}

static void dt_c_emit2(dt_compiler* c, dt_op op, uint8_t a1) {
	dt_c_emit(c, op);
	dt_c_emit(c, a1);
}

static void dt_c_emit3(dt_compiler* c, dt_op op, uint8_t a1, uint8_t a2) {
	dt_c_emit(c, op);
	dt_c_emit(c, a1);
	dt_c_emit(c, a2);
}

// emit an opcode with distance operands
static void dt_c_emit_jmp(dt_compiler* c, dt_op op, uint16_t dis) {
	dt_c_emit3(c, op, dis >> 8, dis & 0xff);
}

// emit an opcode for patching later with dt_c_patch_jmp()
static int dt_c_emit_jmp_empty(dt_compiler* c, dt_op op) {
	dt_c_emit3(c, op, 0, 0);
	return c->env->chunk.cnt;
}

// patch an earlier emitted opcode with distance operands
static void dt_c_patch_jmp(dt_compiler* c, int pos) {

	int dis = c->env->chunk.cnt - pos;

	if (dis >= UINT16_MAX) {
		dt_c_err(c, "jump too large\n");
	}

	c->env->chunk.code[pos - 2] = dis >> 8;
	c->env->chunk.code[pos - 1] = dis & 0xff;

}

// add a const value
static void dt_c_push_const(dt_compiler* c, dt_val val) {
	uint16_t idx = dt_chunk_add_const(&c->env->chunk, val);
	dt_c_emit3(c, DT_OP_CONST, idx >> 8, idx & 0xff);
}

// advance to next token
static dt_token dt_c_nxt(dt_compiler* c) {
	c->parser.prev = c->parser.cur;
	dt_token t = dt_scanner_scan(&c->scanner);
	c->parser.cur = t;
	if (t.type == DT_TOKEN_ERR) {
		dt_c_err(c, "unexpected token\n");
	}
	return c->parser.prev;
}

// check next token type
static dt_token_ty dt_c_peek(dt_compiler* c) {
	return c->parser.cur.type;
}

// check next token type and proceed
static bool dt_c_match(dt_compiler* c, dt_token_ty ty) {
	if (dt_c_peek(c) == ty) {
		dt_c_nxt(c);
		return true;
	}
	return false;
}

// compile error if next token is not *
static dt_token dt_c_consume(dt_compiler* c, dt_token_ty ty) {
	if (dt_c_peek(c) != ty) {
		dt_c_err(c, "expected token '%s'\n", dt_token_name(ty));
		return c->parser.cur;
	}
	dt_c_nxt(c);
	return c->parser.prev;
}

// TODO
static void dt_c_this(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_AND);
	dt_c_emit(c, DT_OP_NIL);
}

static void dt_c_num(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_NUM);
	dt_val num = dt_to_num(strtof(c->parser.prev.start, NULL));
	dt_c_push_const(c, num);
}

static void dt_c_expr(dt_compiler* c) {
	dt_c_prec(c, DT_PREC_ASSIGN);
}

static void dt_c_str(dt_compiler* c) {
	dt_token str_t = dt_c_consume(c, DT_TOKEN_STR);
	// TODO: gc manage?
	dt_val str = dt_to_str(dt_str_new_len(
		NULL,
		str_t.start + 1,
		str_t.len - 2
	));
	dt_c_push_const(c, str);
}

// template string
static void dt_c_str2(dt_compiler* c) {
	int n = 0;
	dt_c_consume(c, DT_TOKEN_QUOTE);
	while (dt_c_peek(c) != DT_TOKEN_QUOTE) {
		if (dt_c_match(c, DT_TOKEN_DOLLAR_LBRACE)) {
			dt_c_expr(c);
			dt_c_consume(c, DT_TOKEN_RBRACE);
			n++;
		} else {
			dt_token str_t = dt_c_consume(c, DT_TOKEN_STR);
			dt_val str = dt_to_str(dt_str_new_len(
				NULL,
				str_t.start,
				str_t.len
			));
			dt_c_push_const(c, str);
			n++;
		}
	}
	dt_c_consume(c, DT_TOKEN_QUOTE);
	dt_c_emit2(c, DT_OP_MKSTR, n);
}

static void dt_c_lit(dt_compiler* c) {
	switch (dt_c_nxt(c).type) {
		case DT_TOKEN_QUESTION: dt_c_emit(c, DT_OP_NIL); break;
		case DT_TOKEN_T: dt_c_emit(c, DT_OP_TRUE); break;
		case DT_TOKEN_F: dt_c_emit(c, DT_OP_FALSE); break;
		default: dt_c_err(c, "cannot process as literal\n");
	}
}

static void dt_c_arr(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_LBRACKET);

	int len = 0;

	while (dt_c_peek(c) != DT_TOKEN_RBRACKET) {
		dt_c_expr(c);
		len++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RBRACKET);
	dt_c_emit2(c, DT_OP_MKARR, len);

}

static void dt_c_map(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_LBRACE);

	int len = 0;

	while (dt_c_peek(c) != DT_TOKEN_RBRACE) {
		dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
		dt_c_push_const(c, dt_to_str(dt_str_new_len(NULL, name.start, name.len)));
		dt_c_consume(c, DT_TOKEN_COLON);
		dt_c_expr(c);
		len++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RBRACE);
	dt_c_emit2(c, DT_OP_MKMAP, len);

}

static int dt_c_find_local(dt_compiler* c, dt_token* name) {

	dt_funcenv* e = c->env;

	for (int i = e->num_locals - 1; i >= 0; i--) {
		dt_local* val = &e->locals[i];
		if (dt_token_eq(name, &val->name)) {
			return i;
		}
	}

	return -1;

}

static int dt_c_add_upval(dt_compiler* c, int idx, bool local) {

	dt_funcenv* e = c->env;

	// find existing upval
	for (int i = 0; i < e->num_upvals; i++) {
		dt_upval* val = &e->upvals[i];
		if (val->idx == idx && val->local == local) {
			return i;
		}
	}

	if (e->num_upvals >= UINT8_MAX) {
		dt_c_err(c, "too many upvalues in one function\n");
		return 0;
	}

	// add new if not found
	e->upvals[e->num_upvals].idx = idx;
	e->upvals[e->num_upvals].local = local;

	return e->num_upvals++;

}

static int dt_c_find_upval(dt_compiler* c, dt_token* name) {

	dt_funcenv* cur_env = c->env;

	if (!cur_env->parent) {
		return -1;
	}

	// TODO: clean
	c->env = cur_env->parent;
	int local = dt_c_find_local(c, name);
	c->env = cur_env;

	if (local != -1) {
		c->env->parent->locals[local].captured = true;
		return dt_c_add_upval(c, local, true);
	}

	// recursively find / add upvals from parent functions
	c->env = cur_env->parent;
	int upval = dt_c_find_upval(c, name);
	c->env = cur_env;

	if (upval != -1) {
		return dt_c_add_upval(c, upval, false);
	}

	return -1;

}

static void dt_c_add_local(dt_compiler* c, dt_token name) {
	if (c->env->num_locals >= UINT8_MAX) {
		dt_c_err(c, "too many local variables in one scope\n");
		return;
	}
	if (
		dt_c_find_local(c, &name) != -1
		|| dt_c_find_upval(c, &name) != -1)
	{
		dt_c_err(c, "duplicate decl '%.*s'\n", name.len, name.start);
	}
	dt_local* l = &c->env->locals[c->env->num_locals++];
	l->name = name;
	l->depth = c->env->cur_depth;
	l->captured = false;
}

static void dt_c_skip_local(dt_compiler* c) {
	if (c->env->num_locals >= UINT8_MAX) {
		dt_c_err(c, "too many local variables in one scope\n");
		return;
	}
	dt_local* l = &c->env->locals[c->env->num_locals++];
	l->name = (dt_token) {
		.type = DT_TOKEN_IDENT,
		.start = NULL,
		.len = 0,
		.line = 0,
	};
	l->depth = c->env->cur_depth;
	l->captured = false;
}

// TODO: return sig
static void dt_c_typesig(dt_compiler* c) {
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
	if (dt_c_match(c, DT_TOKEN_LT)) {
		while (dt_c_peek(c) != DT_TOKEN_GT) {
			dt_token arg = dt_c_consume(c, DT_TOKEN_IDENT);
			dt_c_match(c, DT_TOKEN_COMMA);
		}
		dt_c_consume(c, DT_TOKEN_GT);
	}
}

// TODO
static void dt_c_typedef(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_BANG);
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);

	dt_c_consume(c, DT_TOKEN_LBRACE);

	dt_typedef ty = {0};

	strncpy(ty.name, name.start, name.len);

	while (dt_c_peek(c) != DT_TOKEN_RBRACE) {
		dt_type_mem* mem = &ty.members[ty.num_members++];
		dt_token memname = dt_c_consume(c, DT_TOKEN_IDENT);
		dt_c_consume(c, DT_TOKEN_COLON);
		dt_token memtype = dt_c_consume(c, DT_TOKEN_IDENT);
		dt_c_match(c, DT_TOKEN_COMMA);
		strncpy(mem->name, memname.start, memname.len);
	}

	dt_c_consume(c, DT_TOKEN_RBRACE);

}

static void dt_c_decl(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_DOLLAR);
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
	dt_c_add_local(c, name);
	if (dt_c_match(c, DT_TOKEN_COLON)) {
		dt_c_typesig(c);
	}
	dt_c_consume(c, DT_TOKEN_EQ);
	dt_c_expr(c);
}

static void dt_c_scope_begin(dt_compiler* c, dt_block_ty ty) {
	c->env->scopes[c->env->cur_depth++] = ty;
}

// end a scope and pop / close all locals off the stack
static void dt_c_scope_end(dt_compiler* c) {

	dt_funcenv* e = c->env;

	e->cur_depth--;

	while (e->num_locals > 0) {
		dt_local* l = &e->locals[e->num_locals - 1];
		if (l->depth <= e->cur_depth) {
			break;
		}
		if (l->captured) {
			dt_c_emit(c, DT_OP_CLOSE);
		} else {
			dt_c_emit(c, DT_OP_POP);
		}
		e->num_locals--;
	}

}

static void dt_c_stmt(dt_compiler* c);

static int dt_c_block(dt_compiler* c, dt_block_ty ty) {

	dt_c_consume(c, DT_TOKEN_LBRACE);
	dt_c_scope_begin(c, ty);

	while (dt_c_peek(c) != DT_TOKEN_RBRACE) {
		dt_c_stmt(c);
	}

	dt_c_consume(c, DT_TOKEN_RBRACE);
	dt_c_scope_end(c);

	return c->env->cur_depth;

}

// TODO: emit nil for expr
// for parsing following cond without % only |
static void dt_c_cond_inner(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_LPAREN);
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RPAREN);

	int if_start = dt_c_emit_jmp_empty(c, DT_OP_JMP_COND);

	if (dt_c_peek(c) == DT_TOKEN_LBRACE) {
		dt_c_block(c, DT_BLOCK_COND);
		dt_c_emit(c, DT_OP_NIL);
	} else {
		dt_c_expr(c);
	}

	int if_dis = c->env->chunk.cnt - if_start + 3;

	if (dt_c_match(c, DT_TOKEN_OR)) {

		int pos = dt_c_emit_jmp_empty(c, DT_OP_JMP);

		if (dt_c_peek(c) == DT_TOKEN_LPAREN) {
			dt_c_cond_inner(c);
		} else {
			if (dt_c_peek(c) == DT_TOKEN_LBRACE) {
				dt_c_block(c, DT_BLOCK_COND);
				dt_c_emit(c, DT_OP_NIL);
			} else {
				dt_c_expr(c);
			}
		}

		dt_c_patch_jmp(c, pos);

	}

	if (if_dis >= UINT16_MAX) {
		dt_c_err(c, "jump too large\n");
	}

	int nil_pos = dt_c_emit_jmp_empty(c, DT_OP_JMP);
	dt_c_emit(c, DT_OP_NIL);
	dt_c_patch_jmp(c, nil_pos);

	// TODO: patchable?
	c->env->chunk.code[if_start - 2] = if_dis >> 8;
	c->env->chunk.code[if_start - 1] = if_dis & 0xff;

}

// conditionals
// % (<bool>) <block> | (<bool>)? <block>
static void dt_c_cond(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_PERCENT);
	dt_c_cond_inner(c);
}

static void dt_c_add_break(dt_compiler* c, bool cont) {
	int depth = -1;
	for (int i = c->env->cur_depth - 1; i >= 0; i--) {
		if (c->env->scopes[i] == DT_BLOCK_LOOP) {
			depth = i;
			break;
		}
	}
	if (depth == -1) {
		dt_c_err(c, "cannot jump here\n");
		return;
	}
	for (int i = c->env->num_locals - 1; i >= 0; i--) {
		dt_local l = c->env->locals[i];
		if (l.depth <= depth) {
			break;
		}
		if (l.captured) {
			dt_c_emit(c, DT_OP_CLOSE);
		} else {
			dt_c_emit(c, DT_OP_POP);
		}
	}
	int pos = dt_c_emit_jmp_empty(c, DT_OP_JMP);
	c->env->breaks[c->env->num_breaks++] = (dt_break) {
		.pos = pos,
		.depth = depth,
		.cont = cont,
	};
}

static void dt_c_break(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_AT_GT);
	dt_c_add_break(c, false);
	dt_c_emit(c, DT_OP_NIL);
}

static void dt_c_continue(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_AT_CARET);
	dt_c_add_break(c, true);
	dt_c_emit(c, DT_OP_NIL);
}

// patch previous loop breaks
static void dt_c_patch_breaks(dt_compiler* c, int depth, bool cont) {
	for (int i = 0; i < c->env->num_breaks; i++) {
		dt_break b = c->env->breaks[i];
		if (b.cont == cont && b.depth == depth) {
			dt_c_patch_jmp(c, b.pos);
			c->env->breaks[i--] = c->env->breaks[--c->env->num_breaks];
		}
	}
}

// TODO: accept expr
static void dt_c_loop(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_AT);

	if (dt_c_match(c, DT_TOKEN_LPAREN)) {

		// list loop
		// @ (<item> \ <iter>) <block>

		// stack during this loop:
		// [
		//   ...
		//   iter (internal)
		//   idx (internal)
		//   item (user)
		//   ...
		// ]

		dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
		dt_c_skip_local(c);
		dt_c_skip_local(c);
		dt_c_add_local(c, name);
		dt_c_consume(c, DT_TOKEN_BACKSLASH);
		dt_c_expr(c);
		dt_c_consume(c, DT_TOKEN_RPAREN);
		int pos = dt_c_emit_jmp_empty(c, DT_OP_ITER_PREP);
		int depth = -1;

		if (dt_c_peek(c) == DT_TOKEN_LBRACE) {
			depth = dt_c_block(c, DT_BLOCK_LOOP);
		} else {
			dt_c_expr(c);
			dt_c_emit(c, DT_OP_POP);
		}

		int dis = c->env->chunk.cnt - pos + 3;

		if (dis >= UINT16_MAX) {
			dt_c_err(c, "jump too large\n");
		}

		// TODO: support break in expr loop
		// TODO: reduce dup code
		if (depth != -1) {
			dt_c_patch_breaks(c, depth, true);
		}

		dt_c_emit_jmp(c, DT_OP_ITER, dis);

		if (depth != -1) {
			dt_c_patch_breaks(c, depth, false);
		}

		c->env->num_locals--;
		c->env->num_locals--;
		c->env->num_locals--;
		dt_c_emit(c, DT_OP_POP);
		dt_c_emit(c, DT_OP_POP);
		dt_c_emit(c, DT_OP_POP);
		dt_c_patch_jmp(c, pos);

	} else {

		// infinite loop
		// @ <block>

		int start = c->env->chunk.cnt;
		int depth = -1;

		if (dt_c_peek(c) == DT_TOKEN_LBRACE) {
			depth = dt_c_block(c, DT_BLOCK_LOOP);
		} else {
			dt_c_expr(c);
			dt_c_emit(c, DT_OP_POP);
		}

		int dis = c->env->chunk.cnt - start + 3;

		if (dis >= UINT16_MAX) {
			dt_c_err(c, "jump too large\n");
		}

		if (depth != -1) {
			dt_c_patch_breaks(c, depth, true);
		}

		dt_c_emit_jmp(c, DT_OP_REWIND, dis);

		if (depth != -1) {
			dt_c_patch_breaks(c, depth, false);
		}

	}

	dt_c_emit(c, DT_OP_NIL);

}

static void dt_c_return(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_TILDE_GT);
	dt_c_expr(c);
	dt_c_emit(c, DT_OP_STOP);
}

// TODO
static void dt_c_throw(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_COLON_LPAREN);
	dt_c_expr(c);
	dt_c_emit(c, DT_OP_STOP);
}

static void dt_c_stmt(dt_compiler* c) {
	switch (dt_c_peek(c)) {
		case DT_TOKEN_BANG:   dt_c_typedef(c); break;
		case DT_TOKEN_DOLLAR: dt_c_decl(c); break;
		case DT_TOKEN_LBRACE: dt_c_block(c, DT_BLOCK_NORMAL); break;
		default: {
			dt_c_expr(c);
			dt_c_emit(c, DT_OP_POP);
		};
	}
}

static void dt_c_index(dt_compiler* c) {
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RBRACKET);
	dt_c_emit(c, DT_OP_GETI);
}

static void dt_c_index2(dt_compiler* c) {
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
	dt_c_push_const(c, dt_to_str(dt_str_new_len(NULL, name.start, name.len)));
	dt_c_emit(c, DT_OP_GETI);
}

static void dt_c_call(dt_compiler* c) {

	int nargs = 0;

	while (dt_c_peek(c) != DT_TOKEN_RPAREN) {
		dt_c_expr(c);
		nargs++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RPAREN);
	dt_c_emit2(c, DT_OP_CALL, nargs);

}

static void dt_c_args(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_DOT_DOT_DOT);
	dt_c_emit(c, DT_OP_ARGS);
}

static void dt_c_ident(dt_compiler* c) {

	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);

	if (dt_c_match(c, DT_TOKEN_LBRACE)) {

		// TODO
		// typed init

		while (dt_c_peek(c) != DT_TOKEN_RBRACE) {
			dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
			dt_c_consume(c, DT_TOKEN_COLON);
			dt_c_expr(c);
			dt_c_match(c, DT_TOKEN_COMMA);
		}

		dt_c_consume(c, DT_TOKEN_RBRACE);

	} else {

		// variable

		dt_op set_op;
		dt_op get_op;
		int idx;

		if ((idx = dt_c_find_local(c, &name)) != -1) {
			get_op = DT_OP_GETL;
			set_op = DT_OP_SETL;
		} else if ((idx = dt_c_find_upval(c, &name)) != -1) {
			get_op = DT_OP_GETU;
			set_op = DT_OP_SETU;
		} else {
			get_op = DT_OP_GETG;
			set_op = DT_OP_SETG;
			dt_str* str = dt_str_new_len(NULL, name.start, name.len);
			idx = dt_chunk_add_const(&c->env->chunk, dt_to_str(str));
		}

		// TODO: member assign
		// assign
		if (dt_c_match(c, DT_TOKEN_EQ)) {

			dt_c_expr(c);
			dt_c_emit2(c, set_op, idx);

		} else {

			// get
			dt_c_emit2(c, get_op, idx);

			// TODO: check index assign

			// op assign
			if (dt_c_match(c, DT_TOKEN_PLUS_EQ)) {
				dt_c_expr(c);
				dt_c_emit(c, DT_OP_ADD);
				dt_c_emit2(c, set_op, idx);
			} else if (dt_c_match(c, DT_TOKEN_MINUS_EQ)) {
				dt_c_expr(c);
				dt_c_emit(c, DT_OP_SUB);
				dt_c_emit2(c, set_op, idx);
			} else if (dt_c_match(c, DT_TOKEN_STAR_EQ)) {
				dt_c_expr(c);
				dt_c_emit(c, DT_OP_MUL);
				dt_c_emit2(c, set_op, idx);
			} else if (dt_c_match(c, DT_TOKEN_SLASH_EQ)) {
				dt_c_expr(c);
				dt_c_emit(c, DT_OP_DIV);
				dt_c_emit2(c, set_op, idx);
			}

		}

	}

}

static void dt_c_group(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_LPAREN);
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RPAREN);
}

static void dt_c_unary(dt_compiler* c) {
	dt_c_nxt(c);
	dt_token_ty ty = c->parser.prev.type;
	dt_c_prec(c, DT_PREC_UNARY);
	switch (ty) {
		case DT_TOKEN_MINUS:
			dt_c_emit(c, DT_OP_NEG);
			break;
		case DT_TOKEN_BANG:
			dt_c_emit(c, DT_OP_NOT);
			break;
		case DT_TOKEN_HASH:
			dt_c_emit(c, DT_OP_LEN);
			break;
		default:
			return;
	}
}

static void dt_c_func(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_TILDE);

	if (dt_c_peek(c) == DT_TOKEN_IDENT) {
		dt_token name1 = dt_c_consume(c, DT_TOKEN_IDENT);
		if (dt_c_peek(c) == DT_TOKEN_IDENT) {
			dt_token name2 = dt_c_consume(c, DT_TOKEN_IDENT);
		} else {
			// ...
		}
		// TODO
// 		dt_c_add_local(c, namet);
	}

	dt_c_consume(c, DT_TOKEN_LPAREN);

	dt_funcenv env = dt_funcenv_new();

	dt_funcenv* prev_env = c->env;
	c->env = &env;
	c->env->parent = prev_env;

	int nargs = 0;

	while (dt_c_peek(c) != DT_TOKEN_RPAREN) {

		dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
		dt_c_add_local(c, name);
		nargs++;

		// arg type
		if (dt_c_match(c, DT_TOKEN_COLON)) {
			dt_c_typesig(c);
			// TODO
		}

		dt_c_match(c, DT_TOKEN_COMMA);

	}

	dt_c_consume(c, DT_TOKEN_RPAREN);

	// return type
	if (dt_c_match(c, DT_TOKEN_COLON)) {
		dt_c_typesig(c);
		// TODO
	}

	if (dt_c_peek(c) == DT_TOKEN_LBRACE) {
		dt_c_block(c, DT_BLOCK_NORMAL);
		dt_c_emit(c, DT_OP_NIL);
	} else {
		dt_c_expr(c);
	}

	dt_c_emit(c, DT_OP_STOP);

	c->env = prev_env;

	// TODO
	dt_logic* logic = malloc(sizeof(dt_logic));
	logic->nargs = nargs;
	logic->chunk = env.chunk;

	dt_c_push_const(c, dt_to_logic(logic));

	dt_c_emit(c, DT_OP_MKFUNC);
	dt_c_emit(c, env.num_upvals);

	for (int i = 0; i < env.num_upvals; i++) {
		dt_c_emit(c, env.upvals[i].local);
		dt_c_emit(c, env.upvals[i].idx);
	}

}

static dt_parse_rule dt_expr_rules[] = {
	// token                     // prefix      // infix     // precedence
	[DT_TOKEN_LPAREN]        = { dt_c_group,    dt_c_call,   DT_PREC_CALL },
	[DT_TOKEN_RPAREN]        = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_LBRACE]        = { dt_c_map,      NULL,        DT_PREC_NONE },
	[DT_TOKEN_RBRACE]        = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_LBRACKET]      = { dt_c_arr,      dt_c_index,  DT_PREC_CALL },
	[DT_TOKEN_RBRACKET]      = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_COMMA]         = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_DOT]           = { NULL,          dt_c_index2, DT_PREC_CALL },
	[DT_TOKEN_MINUS]         = { dt_c_unary,    dt_c_binary, DT_PREC_TERM },
	[DT_TOKEN_PLUS]          = { NULL,          dt_c_binary, DT_PREC_TERM },
	[DT_TOKEN_SLASH]         = { NULL,          dt_c_binary, DT_PREC_FACTOR },
	[DT_TOKEN_STAR]          = { NULL,          dt_c_binary, DT_PREC_FACTOR },
	[DT_TOKEN_DOT_DOT]       = { NULL,          dt_c_binary, DT_PREC_UNARY },
	[DT_TOKEN_DOT_DOT_DOT]   = { dt_c_args,     NULL,        DT_PREC_NONE },
	[DT_TOKEN_HASH]          = { dt_c_unary,    NULL,        DT_PREC_NONE },
	[DT_TOKEN_BANG]          = { dt_c_unary,    NULL,        DT_PREC_NONE },
	[DT_TOKEN_BANG_EQ]       = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_EQ]            = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_EQ_EQ]         = { NULL,          dt_c_binary, DT_PREC_EQ },
	[DT_TOKEN_GT]            = { NULL,          dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_GT_EQ]         = { NULL,          dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_LT]            = { NULL,          dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_LT_EQ]         = { NULL,          dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_LT_LT]         = { NULL,          dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_OR_OR]         = { NULL,          dt_c_binary, DT_PREC_LOGIC },
	[DT_TOKEN_AND_AND]       = { NULL,          dt_c_binary, DT_PREC_LOGIC },
	[DT_TOKEN_IDENT]         = { dt_c_ident,    NULL,        DT_PREC_NONE },
	[DT_TOKEN_STR]           = { dt_c_str,      NULL,        DT_PREC_NONE },
	[DT_TOKEN_NUM]           = { dt_c_num,      NULL,        DT_PREC_NONE },
	[DT_TOKEN_AND]           = { dt_c_this,     NULL,        DT_PREC_NONE },
	[DT_TOKEN_T]             = { dt_c_lit,      NULL,        DT_PREC_NONE },
	[DT_TOKEN_F]             = { dt_c_lit,      NULL,        DT_PREC_NONE },
	[DT_TOKEN_QUESTION]      = { dt_c_lit,      NULL,        DT_PREC_NONE },
	[DT_TOKEN_OR]            = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_ERR]           = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_END]           = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_AT]            = { dt_c_loop,     NULL,        DT_PREC_NONE },
	[DT_TOKEN_PERCENT]       = { dt_c_cond,     NULL,        DT_PREC_NONE },
	[DT_TOKEN_AT_GT]         = { dt_c_break,    NULL,        DT_PREC_NONE },
	[DT_TOKEN_AT_CARET]      = { dt_c_continue, NULL,        DT_PREC_NONE },
	[DT_TOKEN_TILDE]         = { dt_c_func,     NULL,        DT_PREC_NONE },
	[DT_TOKEN_COLON_LPAREN]  = { dt_c_throw,    NULL,        DT_PREC_NONE },
	[DT_TOKEN_TILDE_GT]      = { dt_c_return,   NULL,        DT_PREC_NONE },
};

static void dt_c_prec(dt_compiler* c, dt_prec prec) {
	dt_parse_rule* prev_rule = &dt_expr_rules[dt_c_peek(c)];
	if (prev_rule->prefix == NULL) {
		dt_c_err(c, "expected expression\n");
		return;
	}
	prev_rule->prefix(c);
	while (prec <= dt_expr_rules[dt_c_peek(c)].prec) {
		dt_c_nxt(c);
		dt_expr_rules[c->parser.prev.type].infix(c);
	}
}

static void dt_c_binary(dt_compiler* c) {

	dt_token_ty ty = c->parser.prev.type;

	dt_parse_rule* rule = &dt_expr_rules[ty];
	dt_c_prec(c, rule->prec + 1);

	switch (ty) {
		case DT_TOKEN_PLUS:
			dt_c_emit(c, DT_OP_ADD);
			break;
		case DT_TOKEN_MINUS:
			dt_c_emit(c, DT_OP_SUB);
			break;
		case DT_TOKEN_STAR:
			dt_c_emit(c, DT_OP_MUL);
			break;
		case DT_TOKEN_SLASH:
			dt_c_emit(c, DT_OP_DIV);
			break;
		case DT_TOKEN_DOT_DOT:
			dt_c_emit(c, DT_OP_SPREAD);
			break;
		case DT_TOKEN_EQ_EQ:
			dt_c_emit(c, DT_OP_EQ);
			break;
		case DT_TOKEN_GT:
			dt_c_emit(c, DT_OP_GT);
			break;
		case DT_TOKEN_GT_EQ:
			dt_c_emit(c, DT_OP_GT_EQ);
			break;
		case DT_TOKEN_LT:
			dt_c_emit(c, DT_OP_LT);
			break;
		case DT_TOKEN_LT_EQ:
			dt_c_emit(c, DT_OP_LT_EQ);
			break;
		case DT_TOKEN_OR_OR:
			dt_c_emit(c, DT_OP_OR);
			break;
		case DT_TOKEN_AND_AND:
			dt_c_emit(c, DT_OP_AND);
			break;
		default:
			return;
	}

}

static char* dt_read_file(char* path, size_t* osize) {

	FILE* file = fopen(path, "r");

	if (!file) {
		return NULL;
	}

	fseek(file, 0L, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buf = malloc(size + 1);
	size_t size_read = fread(buf, sizeof(char), size, file);
	buf[size_read] = '\0';

	if (osize) {
		*osize = size_read;
	}

	fclose(file);

	return buf;

}

static dt_val dt_f_print(dt_vm* vm, int nargs) {

	for (int i = 0; i < nargs; i++) {
		dt_print(dt_arg(vm, i));
		printf(" ");
	}

	printf("\n");

	return DT_NIL;

}

// TODO
static dt_val dt_f_error(dt_vm* vm, int nargs) {
	return DT_NIL;
}

static dt_val dt_f_type(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return DT_NIL;
	}
	dt_val val = dt_arg(vm, 0);
	char* tname = dt_typename(dt_type(val));
	return dt_to_str(dt_str_new(vm, tname));
}

static dt_val dt_f_exit(dt_vm* vm, int nargs) {
	exit(EXIT_SUCCESS);
	return DT_NIL;
}

// TODO: return stdout
static dt_val dt_f_exec(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return DT_NIL;
	}
	char* cmd = dt_arg_cstr(vm, 0);
	system(cmd);
	return DT_NIL;
}

static dt_val dt_f_getenv(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return DT_NIL;
	}
	char* var = dt_arg_cstr(vm, 0);
	char* val = getenv(var);
	if (val) {
		return dt_to_str(dt_str_new(vm, val));
	} else {
		return DT_NIL;
	}
}

static dt_val dt_f_time(dt_vm* vm, int nargs) {
	return dt_to_num(time(NULL));
}

static dt_val dt_f_eval(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return DT_NIL;
	}
	char* code = dt_arg_cstr(vm, 0);
	// TODO
	return dt_eval(vm, code);
}

static dt_val dt_f_dofile(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return DT_NIL;
	}
	char* path = dt_arg_cstr(vm, 0);
	// TODO
	return dt_dofile(vm, path);
}

static dt_val dt_f_fread(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return DT_NIL;
	}
	char* path = dt_arg_cstr(vm, 0);
	size_t size;
	char* content = dt_read_file(path, &size);
	dt_str* str = dt_str_new_len(vm, content, size);
	free(content);
	return dt_to_str(str);
}

void dt_load_std(dt_vm* vm) {
	// TODO: namespacing
	dt_map_cset(vm, vm->globals, "type", dt_to_cfunc(dt_f_type));
	dt_map_cset(vm, vm->globals, "eval", dt_to_cfunc(dt_f_eval));
	dt_map_cset(vm, vm->globals, "dofile", dt_to_cfunc(dt_f_dofile));
	dt_map_cset(vm, vm->globals, "error", dt_to_cfunc(dt_f_error));
	dt_map_cset(vm, vm->globals, "print", dt_to_cfunc(dt_f_print));
	dt_map_cset(vm, vm->globals, "exit", dt_to_cfunc(dt_f_exit));
	dt_map_cset(vm, vm->globals, "exec", dt_to_cfunc(dt_f_exec));
	dt_map_cset(vm, vm->globals, "time", dt_to_cfunc(dt_f_time));
	dt_map_cset(vm, vm->globals, "getenv", dt_to_cfunc(dt_f_getenv));
	dt_map_cset(vm, vm->globals, "fread", dt_to_cfunc(dt_f_fread));
}

dt_val dt_eval(dt_vm* vm, char* code) {

	dt_compiler c = dt_compiler_new(code);
	dt_c_nxt(&c);

	while (c.parser.cur.type != DT_TOKEN_END) {
		dt_c_stmt(&c);
	}

	dt_c_emit(&c, DT_OP_NIL);
	dt_c_emit(&c, DT_OP_STOP);

	dt_func func = (dt_func) {
		.logic = &(dt_logic) {
			.chunk = c.env->chunk,
			.nargs = 0,
		},
		.upvals = NULL,
	};

	dt_vm_run(vm, &func);

	return dt_vm_get(vm, 0);

}

dt_val dt_dofile(dt_vm* vm, char* path) {

	char* code = dt_read_file(path, NULL);

	if (!code) {
		fprintf(stderr, "failed to read '%s'\n", path);
		return DT_NIL;
	}

	dt_val ret = dt_eval(vm, code);
	free(code);

	return ret;

}

#endif
#endif
