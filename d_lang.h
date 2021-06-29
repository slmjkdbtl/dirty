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
// TODO: mem audit
// TODO: type
// TODO: small string / char
// TODO: string interpolation
// TODO: single ctx handle
// TODO: str add
// TODO: tail call opti
// TODO: str escape
// TODO: loop / cond return val
// TODO: switch
// TODO: allow simpler cond syntax
// TODO: str intern
// TODO: unbounded range

#ifndef D_LANG_H
#define D_LANG_H

#define DT_STACK_MAX 1024
#define DT_ARR_INIT_SIZE 4
#define DT_MAP_INIT_SIZE 8
#define DT_MAP_MAX_LOAD 0.75
#define DT_GC_GROW 2
#define DT_GC_THRESHOLD 1024 * 1024

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	DT_VAL_NIL,
	DT_VAL_BOOL,
	DT_VAL_NUM,
	DT_VAL_STR,
	DT_VAL_ARR,
	DT_VAL_MAP,
	DT_VAL_LOGIC,
	DT_VAL_RANGE,
	DT_VAL_FUNC,
	DT_VAL_CFUNC,
	DT_VAL_CDATA,
	DT_VAL_CPTR,
} dt_val_ty;

struct dt_map;
struct dt_arr;
struct dt_str;
struct dt_vm;
struct dt_val;
struct dt_logic;
struct dt_func;

typedef float dt_num;
typedef bool dt_bool;

typedef struct dt_val (dt_cfunc)(struct dt_vm* vm, int nargs);

typedef enum {
	DT_HEAPER_STR,
	DT_HEAPER_MAP,
	DT_HEAPER_ARR,
	DT_HEAPER_FUNC,
	DT_HEAPER_UPVAL,
	DT_HEAPER_CDATA,
} dt_heaper_ty;

// TODO: inline these in parent structs can save size
typedef struct dt_heaper {
	struct dt_heaper* nxt;
	dt_heaper_ty      type;
	bool              marked;
} dt_heaper;

typedef struct {
	int32_t start;
	int32_t end;
} dt_range;

typedef struct {
	dt_heaper heaper;
	size_t    size;
	void*     data;
} dt_cdata;

typedef struct dt_val {
	dt_val_ty type;
	union {
		dt_bool          boolean;
		dt_num           num;
		dt_range         range;
		struct dt_str*   str;
		struct dt_arr*   arr;
		struct dt_map*   map;
		// TODO: remove this
		struct dt_logic* logic;
		struct dt_func*  func;
		dt_cfunc*        cfunc;
		dt_cdata*        cdata;
	} data;
} dt_val;

// TODO
typedef struct {
	dt_heaper heaper;
	dt_val*   val;
} dt_upval2;

typedef struct dt_str {
	dt_heaper heaper;
	uint32_t  len;
	uint32_t  hash;
	char      chars[];
} dt_str;

typedef struct dt_arr {
	dt_heaper heaper;
	uint32_t  len;
	uint32_t  cap;
	dt_val*   values;
} dt_arr;

typedef struct {
	dt_str* key;
	dt_val  val;
} dt_entry;

typedef struct dt_map {
	dt_heaper heaper;
	uint32_t  cnt;
	uint32_t  cap;
	dt_entry* entries;
} dt_map;

typedef struct {
	int      cnt;
	int      cap;
	uint8_t* code;
	int*     lines;
	dt_arr*  consts;
} dt_chunk;

typedef struct dt_logic {
	dt_chunk chunk;
	int      nargs;
} dt_logic;

// TODO: separate dt_closure and dt_func
typedef struct dt_func {
	dt_heaper heaper;
	dt_logic* logic;
	dt_val**  upvals;
	uint8_t   num_upvals;
} dt_func;

typedef struct dt_vm {
	dt_func*   func;
	uint8_t*   ip;
	dt_val     stack[DT_STACK_MAX];
	dt_val*    stack_top;
	int        stack_offset;
	dt_map*    globals;
	dt_map*    strs;
	dt_val**   open_upvals[UINT8_MAX];
	int        num_upvals;
	dt_heaper* heaper;
	size_t     mem;
	size_t     next_gc;
} dt_vm;

void     dt_load_std      (dt_vm* vm);
dt_val   dt_eval          (dt_vm* vm, char* src);
dt_val   dt_dofile        (dt_vm* vm, char* path);

dt_val   dt_val_num       (dt_num n);
dt_val   dt_val_bool      (dt_bool b);
dt_val   dt_val_str       (dt_str* str);
dt_val   dt_val_cfunc     (dt_cfunc* func);
dt_val   dt_val_map       (dt_map* map);
dt_val   dt_val_arr       (dt_arr* arr);
void     dt_val_print     (dt_val* val);
void     dt_val_println   (dt_val* val);
char*    dt_type_name     (dt_val_ty ty);
bool     dt_is_nil        (dt_val* val);
bool     dt_is_num        (dt_val* val);
bool     dt_is_bool       (dt_val* val);
bool     dt_is_str        (dt_val* val);
bool     dt_is_map        (dt_val* val);
bool     dt_is_arr        (dt_val* val);
bool     dt_is_func       (dt_val* val);

dt_str*  dt_str_new       (dt_vm* vm, char* src);
dt_str*  dt_str_new_len   (dt_vm* vm, char* src, int len);
void     dt_str_free      (dt_vm* vm, dt_str* str);
dt_str*  dt_str_concat    (dt_vm* vm, dt_str* a, dt_str* b);
bool     dt_str_eq        (dt_str* a, dt_str* b);
dt_str*  dt_str_replace   (dt_str* src, dt_str* old, dt_str* new);
void     dt_str_print     (dt_str* str);
void     dt_str_println   (dt_str* str);

dt_arr*  dt_arr_new       (dt_vm* vm);
dt_arr*  dt_arr_new_len   (dt_vm* vm, int len);
void     dt_arr_free      (dt_vm* vm, dt_arr* arr);
dt_val   dt_arr_get       (dt_arr* arr, int idx);
void     dt_arr_set       (dt_vm* vm, dt_arr* arr, int idx, dt_val val);
void     dt_arr_insert    (dt_vm* vm, dt_arr* arr, int idx, dt_val val);
void     dt_arr_push      (dt_vm* vm, dt_arr* arr, dt_val val);
dt_val   dt_arr_rm        (dt_arr* arr, int idx);
void     dt_arr_print     (dt_arr* arr);
void     dt_arr_println   (dt_arr* arr);

dt_map*  dt_map_new        (dt_vm* vm);
dt_map*  dt_map_new_len    (dt_vm* vm, int len);
void     dt_map_free       (dt_vm* vm, dt_map* map);
void     dt_map_set        (dt_vm* vm, dt_map* map, dt_str* key, dt_val val);
void     dt_map_cset       (dt_vm* vm, dt_map* map, char* key, dt_val val);
void     dt_map_cset_cfunc (dt_vm* vm, dt_map* map, char* key, dt_cfunc* func);
void     dt_map_cset_map   (dt_vm* vm, dt_map* map, char* key, dt_map* map2);
bool     dt_map_exists     (dt_map* map, dt_str* key);
dt_val   dt_map_get        (dt_map* map, dt_str* key);
dt_arr*  dt_map_keys       (dt_vm* vm, dt_map* map);
dt_arr*  dt_map_vals       (dt_vm* vm, dt_map* map);
void     dt_map_print      (dt_map* map);
void     dt_map_println    (dt_map* map);

dt_vm    dt_vm_new        ();
dt_val   dt_vm_get        (dt_vm* vm, int idx);
dt_num   dt_vm_get_num    (dt_vm* vm, int idx);
dt_bool  dt_vm_get_bool   (dt_vm* vm, int idx);
dt_str*  dt_vm_get_str    (dt_vm* vm, int idx);
char*    dt_vm_get_cstr   (dt_vm* vm, int idx);
dt_map*  dt_vm_get_map    (dt_vm* vm, int idx);
dt_arr*  dt_vm_get_arr    (dt_vm* vm, int idx);
dt_func* dt_vm_get_func   (dt_vm* vm, int idx);
dt_val   dt_vm_call_n     (dt_vm* vm, dt_func* func, int nargs, ...);
dt_val   dt_vm_call_0     (dt_vm* vm, dt_func* func);
dt_val   dt_vm_call_1     (dt_vm* vm, dt_func* func, dt_val a1);
dt_val   dt_vm_call_2     (dt_vm* vm, dt_func* func, dt_val a1, dt_val a2);
dt_val   dt_vm_call_3     (dt_vm* vm, dt_func* func, dt_val a1, dt_val a2, dt_val a3);
dt_val   dt_vm_call_4     (dt_vm* vm, dt_func* func, dt_val a1, dt_val a2, dt_val a3, dt_val a4);
void     dt_vm_err        (dt_vm* vm, char* fmt, ...);
void     dt_vm_gc_run     (dt_vm* vm);

dt_val dt_nil = (dt_val) {
	.type = DT_VAL_NIL,
	.data = {
		.num = 0,
	},
};

dt_val dt_true = (dt_val) {
	.type = DT_VAL_BOOL,
	.data = {
		.boolean = true,
	},
};

dt_val dt_false = (dt_val) {
	.type = DT_VAL_BOOL,
	.data = {
		.boolean = false,
	},
};

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
	DT_OP_INDEX,
	DT_OP_SETG,
	DT_OP_GETL,
	DT_OP_SETL,
	DT_OP_GETU,
	DT_OP_SETU,
	DT_OP_CALL,
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

typedef struct {
	dt_arr* arr;
	int idx;
} dt_iter;

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
	DT_TOKEN_BACKSLASH, //
	DT_TOKEN_PERCENT, // %
	DT_TOKEN_TILDE, // ~
	DT_TOKEN_COLON, // :
	DT_TOKEN_COLON_COLON, // ::
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

// TODO: store type
typedef struct {
	dt_token name;
	bool captured;
	int depth;
} dt_local;

typedef struct {
	int idx;
	bool local;
} dt_upval;

typedef enum {
	DT_BLOCK_NORMAL,
	DT_BLOCK_LOOP,
	DT_BLOCK_COND,
} dt_scope_ty;

typedef struct {
	int pos;
	int depth;
	dt_scope_ty ty;
} dt_jumper;

typedef struct dt_funcenv {
	struct dt_funcenv* parent;
	dt_chunk chunk;
	int cur_depth;
	dt_scope_ty scopes[UINT8_MAX];
	dt_local locals[UINT8_MAX];
	int num_locals;
	dt_upval upvals[UINT8_MAX];
	int num_upvals;
	dt_jumper jumpers[UINT8_MAX];
	int num_jumpers;
} dt_funcenv;

typedef struct {
	char name[16];
	int type;
} dt_type_mem;

typedef struct {
	char name[16];
	dt_type_mem members[16];
	int num_members;
} dt_typedef;

typedef struct {
	int ret;
	int args[16];
	int num_args;
} dt_funcdef_inner;

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

static void dt_fail(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

static void dt_free(dt_vm* vm, void* ptr, size_t size) {
	if (vm) {
		vm->mem -= size;
	}
	free(ptr);
}

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
		case DT_TOKEN_BACKSLASH: return "BACKSLASH";
		case DT_TOKEN_PERCENT: return "PERCENT";
		case DT_TOKEN_TILDE: return "TILDE";
		case DT_TOKEN_COLON: return "COLON";
		case DT_TOKEN_COLON_COLON: return "COLON_COLON";
		case DT_TOKEN_QUESTION: return "QUESTION";
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

char* dt_type_name(dt_val_ty ty) {
	switch (ty) {
		case DT_VAL_NIL: return "nil";
		case DT_VAL_BOOL: return "bool";
		case DT_VAL_NUM: return "num";
		case DT_VAL_STR: return "str";
		case DT_VAL_ARR: return "arr";
		case DT_VAL_MAP: return "map";
		case DT_VAL_LOGIC: return "logic";
		case DT_VAL_RANGE: return "range";
		case DT_VAL_FUNC: return "func";
		case DT_VAL_CFUNC: return "cfunc";
		case DT_VAL_CDATA: return "cdata";
		case DT_VAL_CPTR: return "cptr";
	}
}

void dt_val_print(dt_val* val) {
	switch (val->type) {
		case DT_VAL_NIL: printf("<nil>"); break;
		case DT_VAL_BOOL: printf(val->data.boolean ? "true" : "false"); break;
		case DT_VAL_NUM: printf("%g", val->data.num); break;
		case DT_VAL_STR: dt_str_print(val->data.str); break;
		case DT_VAL_ARR: dt_arr_print(val->data.arr); break;
		case DT_VAL_MAP: dt_map_print(val->data.map); break;
		case DT_VAL_LOGIC: printf("<logic>"); break;
		case DT_VAL_RANGE: printf("%d..%d", val->data.range.start, val->data.range.end); break;
		case DT_VAL_FUNC: printf("<func>"); break;
		case DT_VAL_CFUNC: printf("<cfunc>"); break;
		case DT_VAL_CDATA: printf("cdata"); break;
		case DT_VAL_CPTR: printf("cptr"); break;
	}
}

void dt_val_println(dt_val* val) {
	dt_val_print(val);
	printf("\n");
}

dt_val dt_val_num(dt_num n) {
	return (dt_val) {
		.type = DT_VAL_NUM,
		.data = {
			.num = n,
		},
	};
}

dt_val dt_val_bool(dt_bool b) {
	return (dt_val) {
		.type = DT_VAL_BOOL,
		.data = {
			.boolean = b,
		},
	};
}

dt_val dt_val_str(dt_str* str) {
	if (!str) {
		return dt_nil;
	}
	return (dt_val) {
		.type = DT_VAL_STR,
		.data = {
			.str = str,
		},
	};
}

dt_val dt_val_cfunc(dt_cfunc* func) {
	if (!func) {
		return dt_nil;
	}
	return (dt_val) {
		.type = DT_VAL_CFUNC,
		.data = {
			.cfunc = func,
		},
	};
}

dt_val dt_val_map(dt_map* map) {
	if (!map) {
		return dt_nil;
	}
	return (dt_val) {
		.type = DT_VAL_MAP,
		.data = {
			.map = map,
		},
	};
}

dt_val dt_val_arr(dt_arr* arr) {
	if (!arr) {
		return dt_nil;
	}
	return (dt_val) {
		.type = DT_VAL_ARR,
		.data = {
			.arr = arr,
		},
	};
}

dt_val dt_val_func(dt_func* func) {
	if (!func) {
		return dt_nil;
	}
	return (dt_val) {
		.type = DT_VAL_FUNC,
		.data = {
			.func = func,
		},
	};
}

dt_val dt_val_range(dt_range range) {
	return (dt_val) {
		.type = DT_VAL_RANGE,
		.data = {
			.range = range,
		},
	};
}

static uint32_t dt_hash(char* key, int len) {
	uint32_t hash = 2166136261u;
	for (int i = 0; i < len; i++) {
		hash ^= (uint8_t)key[i];
		hash *= 16777619;
	}
	return hash;
}

static void dt_vm_manage_mem(dt_vm* vm, dt_heaper* h, dt_heaper_ty ty);

// TODO
dt_str* dt_vm_find_strn(dt_vm* vm, char* str, int len) {
	return NULL;
}

static dt_str* dt_str_alloc(dt_vm* vm, int len) {
	dt_str* str = dt_malloc(vm, sizeof(dt_str) + len + 1);
	str->len = len;
	if (vm) {
		dt_vm_manage_mem(vm, (dt_heaper*)str, DT_HEAPER_STR);
	}
	return str;
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

static void dt_str_mark(dt_str* str) {
	str->heaper.marked = true;
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

bool dt_str_eq(dt_str* a, dt_str* b) {
	return a->len == b->len && memcmp(a->chars, b->chars, a->len) == 0;
}

// TODO: replace all
// dt_str dt_str_replace(dt_str* src, dt_str* old, dt_str* new) {
// 	char* start = strstr(src->chars, old->chars);
// 	int offset = start - src->chars;
// 	if (!start) {
// 		return dt_nil;
// 	}
// 	int len = src->len - old->len + new->len;
// 	char* chars = malloc(len + 1);
// 	memcpy(chars, src->chars, offset);
// 	memcpy(chars + offset, new->chars, new->len);
// 	memcpy(
// 		chars + offset + new->len,
// 		src->chars + offset + old->len,
// 		src->len - offset - old->len
// 	);
// 	chars[len] = '\0';
// 	return dt_val_str_len(chars, len);
// }

dt_arr* dt_arr_new_len(dt_vm* vm, int len) {
	dt_arr* arr = dt_malloc(vm, sizeof(dt_arr));
	arr->len = 0;
	arr->cap = len > DT_ARR_INIT_SIZE ? len : DT_ARR_INIT_SIZE;
	arr->values = dt_malloc(vm, arr->cap * sizeof(dt_val));
	if (vm) {
		dt_vm_manage_mem(vm, (dt_heaper*)arr, DT_HEAPER_ARR);
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
		return dt_nil;
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
			arr->values[i] = dt_nil;
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
		return dt_nil;
	}
	dt_val v = arr->values[idx];
	memcpy(arr->values + idx, arr->values + idx + 1, (arr->len - idx - 1) * sizeof(dt_val));
	arr->len--;
	arr->values[arr->len] = dt_nil;
	return v;
}

void dt_arr_print(dt_arr* arr) {
	printf("[ ");
	for (int i = 0; i < arr->len; i++) {
		dt_val_print(&arr->values[i]);
		printf(", ");
	}
	printf("]");
}

void dt_arr_println(dt_arr* arr) {
	dt_arr_print(arr);
	printf("\n");
}

static void dt_val_mark(dt_val* val);

static void dt_arr_mark(dt_arr* arr) {
	arr->heaper.marked = true;
	for (int i = 0; i < arr->len; i++) {
		dt_val_mark(&arr->values[i]);
	}
}

dt_map* dt_map_new_len(dt_vm* vm, int len) {
	dt_map* map = dt_malloc(vm, sizeof(dt_map));
	map->cnt = 0;
	map->cap = len > DT_MAP_INIT_SIZE ? len : DT_MAP_INIT_SIZE;
	int entries_size = map->cap * sizeof(dt_entry);
	map->entries = dt_malloc(vm, entries_size);
	memset(map->entries, 0, entries_size);
	if (vm) {
		dt_vm_manage_mem(vm, (dt_heaper*)map, DT_HEAPER_MAP);
	}
	return map;
}

dt_map* dt_map_new(dt_vm* vm) {
	return dt_map_new_len(vm, DT_MAP_INIT_SIZE);
}


dt_arr* dt_map_keys(dt_vm* vm, dt_map* map) {
	dt_arr* arr = dt_arr_new(vm);
	for (int i = 0; i < map->cap; i++) {
		if (map->entries[i].key) {
			dt_arr_push(vm, arr, dt_val_str(map->entries[i].key));
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

void dt_map_free(dt_vm* vm, dt_map* map) {
	dt_free(vm, map->entries, map->cap * sizeof(dt_entry));
	dt_free(vm, map, sizeof(dt_map));
}

void dt_map_print(dt_map* map) {
	printf("{ ");
	for (int i = 0; i < map->cap; i++) {
		dt_entry e = map->entries[i];
		if (e.key) {
			dt_str_print(e.key);
			printf(": ");
			dt_val_print(&e.val);
			printf(", ");
		}
	}
	printf("}");
}

void dt_map_println(dt_map* map) {
	dt_map_print(map);
	printf("\n");
}

static void dt_map_mark(dt_map* map) {
	map->heaper.marked = true;
	for (int i = 0; i < map->cap; i++) {
		dt_entry e = map->entries[i];
		if (e.key) {
			dt_str_mark(e.key);
			dt_val_mark(&e.val);
		}
	}
}

bool dt_is_nil(dt_val* val) {
	return val->type == DT_VAL_NIL;
}

bool dt_is_num(dt_val* val) {
	return val->type == DT_VAL_NUM;
}

bool dt_is_bool(dt_val* val) {
	return val->type == DT_VAL_BOOL;
}

bool dt_is_str(dt_val* val) {
	return val->type == DT_VAL_STR;
}

bool dt_is_map(dt_val* val) {
	return val->type == DT_VAL_MAP;
}

bool dt_is_arr(dt_val* val) {
	return val->type == DT_VAL_ARR;
}

bool dt_is_func(dt_val* val) {
	return val->type == DT_VAL_FUNC;
}

static int dt_map_find(dt_map* map, dt_str* key) {

	uint32_t oidx = key->hash % map->cap;
	uint32_t idx = oidx;

	for (;;) {

		dt_entry e = map->entries[idx];

		if (e.key == NULL || dt_str_eq(e.key, key)) {
			return idx;
		}

		idx = (idx + 1) % map->cap;

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
		map->cap = old_cap * 2;

		dt_entry* old_entries = map->entries;
		size_t entries_size = map->cap * sizeof(dt_entry);
		map->entries = dt_malloc(vm, entries_size);
		memset(map->entries, 0, entries_size);

		for (int i = 0; i < old_cap; i++) {
			dt_entry e = old_entries[i];
			if (e.key && !dt_is_nil(&e.val)) {
				dt_map_set(vm, map, e.key, e.val);
			}
		}

		dt_free(vm, old_entries, old_cap * sizeof(dt_entry));

	}

	int idx = dt_map_find(map, key);

	if (idx == -1) {
		dt_fail("map overflow\n");
	}

	if (map->entries[idx].key) {
		map->entries[idx].val = val;
	} else {
		dt_entry* e = &map->entries[idx];
		// TODO: do we need to clone
		e->key = key;
		e->val = val;
		map->cnt++;
	}

}

void dt_map_cset(dt_vm* vm, dt_map* map, char* key, dt_val val) {
	dt_map_set(vm, map, dt_str_new(vm, key), val);
}

void dt_map_cset_cfunc(dt_vm* vm, dt_map* map, char* key, dt_cfunc* func) {
	dt_map_cset(vm, map, key, dt_val_cfunc(func));
}

void dt_map_cset_map(dt_vm* vm, dt_map* map, char* key, dt_map* map2) {
	dt_map_cset(vm, map, key, dt_val_map(map2));
}

bool dt_map_exists(dt_map* map, dt_str* key) {
	int idx = dt_map_find(map, key);
	return idx != -1 && map->entries[idx].key != NULL;
}

dt_val dt_map_get(dt_map* map, dt_str* key) {

	int idx = dt_map_find(map, key);

	if (idx == -1 || map->entries[idx].key == NULL) {
		return dt_nil;
	}

	return map->entries[idx].val;

}

dt_val dt_map_get2(dt_map* map, char* key) {
	// TODO
	return dt_map_get(map, dt_str_new(NULL, key));
}

void dt_func_print(dt_func* func) {
	printf("<func#%p>", (void*)func);
}

void dt_func_println(dt_func* func) {
	dt_func_print(func);
	printf("\n");
}

static void dt_func_mark(dt_func* func) {
	func->heaper.marked = true;
	for (int i = 0; i < func->num_upvals; i++) {
// 		func->upvals[i]->heaper.marked = true;
	}
}

static void dt_func_free(dt_vm *vm, dt_func* func) {
	dt_free(vm, func->upvals, sizeof(dt_val*) * func->num_upvals);
	dt_free(vm, func, sizeof(dt_func));
}

bool dt_val_truthiness(dt_val* val) {
	return !(
		val->type == DT_VAL_NIL
		|| (val->type == DT_VAL_BOOL && val->data.boolean == false)
	);
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
		case DT_OP_INDEX:     return "INDEX";
		case DT_OP_CALL:      return "CALL";
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
			dt_val_print(&c->consts->values[i2 << 8 | i3]);
			return idx + 3;
		}
		case DT_OP_SETG: {
			uint8_t idx2 = c->code[idx + 1];
			printf("SEFG ");
			dt_val_print(&c->consts->values[idx2]);
			return idx + 2;
		}
		case DT_OP_GETG: {
			uint8_t idx2 = c->code[idx + 1];
			printf("GETG ");
			dt_val_print(&c->consts->values[idx2]);
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
		.globals = dt_map_new(NULL),
		.strs = dt_map_new(NULL),
		.open_upvals = {0},
		.num_upvals = 0,
		.heaper = NULL,
		.mem = 0,
		.next_gc = DT_GC_THRESHOLD,
	};
	vm.stack_top = vm.stack;
	return vm;
}

void dt_vm_err(dt_vm* vm, char* fmt, ...) {
	dt_chunk* chunk = &vm->func->logic->chunk;
	int offset = vm->ip - chunk->code;
	int line = chunk->lines[offset];
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "line #%d: ", line);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

dt_val dt_vm_get(dt_vm* vm, int idx) {
	dt_val* pos = vm->stack_top - 1 + idx;
	if (pos < vm->stack) {
		return dt_nil;
	} else {
		return *pos;
	}
}

bool dt_vm_check_ty(dt_vm* vm, int idx, dt_val_ty ty) {
	dt_val v = dt_vm_get(vm, idx);
	if (v.type != ty) {
		dt_vm_err(
			vm,
			"expected a '%s' at %d, found '%s'\n",
			dt_type_name(ty),
			idx,
			dt_type_name(v.type)
		);
		return false;
	}
	return true;
}

dt_num dt_vm_get_num(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_NUM);
	return dt_vm_get(vm, idx).data.num;
}

dt_bool dt_vm_get_bool(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_BOOL);
	return dt_vm_get(vm, idx).data.boolean;
}

dt_str* dt_vm_get_str(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_STR);
	return dt_vm_get(vm, idx).data.str;
}

char* dt_vm_get_cstr(dt_vm* vm, int idx) {
	dt_str* str = dt_vm_get_str(vm, idx);
	return str->chars;
}

char* dt_vm_get_cstr_dup(dt_vm* vm, int idx) {
	dt_str* str = dt_vm_get_str(vm, idx);
	return strdup(str->chars);
}

dt_map* dt_vm_get_map(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_MAP);
	return dt_vm_get(vm, idx).data.map;
}

dt_arr* dt_vm_get_arr(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_ARR);
	return dt_vm_get(vm, idx).data.arr;
}

dt_func* dt_vm_get_func(dt_vm* vm, int idx) {
	dt_vm_check_ty(vm, idx, DT_VAL_FUNC);
	return dt_vm_get(vm, idx).data.func;
}

static void dt_vm_push(dt_vm* vm, dt_val val) {
	*vm->stack_top = val;
	vm->stack_top++;
	if (vm->stack_top - vm->stack >= DT_STACK_MAX) {
		dt_fail("stack overflow\n");
	}
}

static dt_val dt_vm_pop(dt_vm* vm) {
	if (vm->stack_top == vm->stack) {
		dt_fail("stack underflow\n");
		return dt_nil;
	}
	vm->stack_top--;
	return *vm->stack_top;
}

static void dt_vm_pop_close(dt_vm* vm) {

	dt_val* top = vm->stack_top - 1;
	dt_val* upval = NULL;

	for (int i = 0; i < vm->num_upvals; i++) {
		if (top == *vm->open_upvals[i]) {
			if (!upval) {
				// TODO: manage upvals
				upval = dt_malloc(vm, sizeof(dt_val));
				memcpy(upval, top, sizeof(dt_val));
			}
			*vm->open_upvals[i] = upval;
			vm->open_upvals[i--] = vm->open_upvals[--vm->num_upvals];
		}
	}

	dt_vm_pop(vm);

}

static void dt_vm_stack_print(dt_vm* vm) {

	printf("[ ");

	for (dt_val* slot = vm->stack; slot < vm->stack_top; slot++) {
		dt_val_print(slot);
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

static dt_val dt_vm_call(dt_vm* vm, dt_func* func, int nargs) {

	dt_logic* logic = func->logic;

	// balance the arg stack
	if (logic->nargs > nargs) {
		for (int i = 0; i < logic->nargs - nargs; i++) {
			dt_vm_push(vm, dt_nil);
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

dt_val dt_vm_call_n(dt_vm* vm, dt_func* func, int nargs, ...) {
	va_list args;
	va_start(args, nargs);
	for (int i = 0; i < nargs; i++) {
		dt_vm_push(vm, va_arg(args, dt_val));
	}
	dt_val ret = dt_vm_call(vm, func, nargs);
	va_end(args);
	return ret;
}

dt_val dt_vm_call_0(dt_vm* vm, dt_func* func) {
	return dt_vm_call_n(vm, func, 0);
}

dt_val dt_vm_call_1(dt_vm* vm, dt_func* func, dt_val a1) {
	return dt_vm_call_n(vm, func, 1, a1);
}

dt_val dt_vm_call_2(dt_vm* vm, dt_func* func, dt_val a1, dt_val a2) {
	return dt_vm_call_n(vm, func, 2, a1, a2);
}

dt_val dt_vm_call_3(
	dt_vm* vm,
	dt_func* func,
	dt_val a1,
	dt_val a2,
	dt_val a3
) {
	return dt_vm_call_n(vm, func, 3, a1, a2, a3);
}

dt_val dt_vm_call_4(
	dt_vm* vm,
	dt_func* func,
	dt_val a1,
	dt_val a2,
	dt_val a3,
	dt_val a4
) {
	return dt_vm_call_n(vm, func, 4, a1, a2, a3, a4);
}

static void dt_val_mark(dt_val* val) {
	switch (val->type) {
		case DT_VAL_STR:
			dt_str_mark(val->data.str);
			break;
		case DT_VAL_ARR:
			dt_arr_mark(val->data.arr);
			break;
		case DT_VAL_MAP:
			dt_map_mark(val->data.map);
			break;
		case DT_VAL_FUNC:
			dt_func_mark(val->data.func);
			break;
		default:
			break;
	}
}

static void dt_vm_manage_mem(dt_vm* vm, dt_heaper* h, dt_heaper_ty ty) {
	h->type = ty;
	h->marked = false;
	h->nxt = vm->heaper;
	vm->heaper = h;
}

static void dt_vm_gc_mark(dt_vm* vm) {
	for (dt_val* val = vm->stack; val < vm->stack_top; val++) {
		dt_val_mark(val);
	}
	dt_map_mark(vm->globals);
}

// TODO: should sweep to 0 when nothing is marked
static void dt_vm_gc_sweep(dt_vm* vm) {
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
				case DT_HEAPER_STR: {
#ifdef DT_GC_LOG
					dt_str* str = (dt_str*)unreached;
					printf("FREE STR ");
					dt_str_println(str);
#endif
					dt_str_free(vm, (dt_str*)unreached);
					break;
				}
				case DT_HEAPER_ARR: {
#ifdef DT_GC_LOG
					dt_arr* arr = (dt_arr*)unreached;
					printf("FREE ARR ");
					dt_arr_println(arr);
#endif
					dt_arr_free(vm, (dt_arr*)unreached);
					break;
				}
				case DT_HEAPER_MAP: {
#ifdef DT_GC_LOG
					dt_map* map = (dt_map*)unreached;
					printf("FREE MAP ");
					dt_map_println(map);
#endif
					dt_map_free(vm, (dt_map*)unreached);
					break;
				}
				case DT_HEAPER_FUNC: {
#ifdef DT_GC_LOG
					dt_func* func = (dt_func*)unreached;
					printf("FREE FUNC ");
					dt_func_println(func);
#endif
					dt_func_free(vm, (dt_func*)unreached);
					break;
				}
				case DT_HEAPER_UPVAL: {
#ifdef DT_GC_LOG
// 					dt_func* func = (dt_func*)unreached;
					printf("FREE UPVAL ");
// 					dt_upval_println(func);
#endif
// 					dt_func_free(vm, (dt_func*)unreached);
					break;
				}
				case DT_HEAPER_CDATA: {
#ifdef DT_GC_LOG
// 					dt_func* func = (dt_func*)unreached;
					printf("FREE CDATA ");
// 					dt_upval_println(func);
#endif
// 					dt_func_free(vm, (dt_func*)unreached);
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

static void dt_vm_gc_check(dt_vm* vm) {
#ifdef DT_GC_STRESS
	dt_vm_gc_run(vm);
#else
	if (vm->mem >= vm->next_gc) {
		dt_vm_gc_run(vm);
		vm->next_gc *= DT_GC_GROW;
	}
#endif
}

// TODO: upval
// TODO: consts ?
void dt_vm_gc_run(dt_vm* vm) {
	dt_vm_gc_mark(vm);
	dt_vm_gc_sweep(vm);
}

void dt_vm_free(dt_vm* vm) {
	dt_vm_gc_sweep(vm);
	dt_map_free(NULL, vm->globals);
	dt_map_free(NULL, vm->strs);
	memset(vm, 0, sizeof(dt_vm));
}

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
				dt_vm_push(vm, dt_nil);
				break;

			case DT_OP_TRUE:
				dt_vm_push(vm, dt_true);
				break;

			case DT_OP_FALSE:
				dt_vm_push(vm, dt_false);
				break;

			case DT_OP_ADD: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_num(a.data.num + b.data.num));
				} else if (a.type == DT_VAL_STR && b.type == DT_VAL_STR) {
					dt_vm_push(vm, dt_val_str(dt_str_concat(vm, a.data.str, b.data.str)));
				} else if (a.type == DT_VAL_STR || b.type == DT_VAL_STR) {
					// TODO
// 					dt_str a_str = dt_val_to_str(&a);
// 					dt_str b_str = dt_val_to_str(&b);
// 					dt_vm_push(vm, dt_str_concat(&a_str, &b_str));
				} else {
					dt_vm_err(
						vm,
						"cannot add a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_SUB: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_num(a.data.num - b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot sub a '%s' by '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_MUL: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_num(a.data.num * b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot mul a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_DIV: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_num(a.data.num / b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot div a '%s' by '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_MOD: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_num(fmodf(a.data.num, b.data.num)));
				} else {
					dt_vm_err(
						vm,
						"cannot mod a '%s' by '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_POW: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_num(powf(a.data.num, b.data.num)));
				} else {
					dt_vm_err(
						vm,
						"cannot pow a '%s' by '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_SPREAD: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_range((dt_range) {
						.start = a.data.num,
						.end = b.data.num,
					}));
				} else {
					dt_vm_err(
						vm,
						"cannot spread a '%s' and '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_NEG: {
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_num(-a.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot negate a '%s'\n",
						dt_type_name(a.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_NOT: {
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_val_bool(!a.data.boolean));
				} else {
					dt_vm_err(
						vm,
						"cannot neg a '%s'\n",
						dt_type_name(a.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_LEN: {
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_STR) {
					dt_vm_push(vm, dt_val_num(a.data.str->len));
				} else if (a.type == DT_VAL_ARR) {
					dt_vm_push(vm, dt_val_num(a.data.arr->len));
				} else if (a.type == DT_VAL_MAP) {
					dt_vm_push(vm, dt_val_num(a.data.map->cnt));
				} else if (a.type == DT_VAL_RANGE) {
					dt_vm_push(vm, dt_val_num(abs(a.data.range.end - a.data.range.start)));
				} else {
					dt_vm_err(
						vm,
						"cannot get len of a '%s'\n",
						dt_type_name(a.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_EQ: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_false);
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_bool(a.data.num == b.data.num));
				} else if (a.type == DT_VAL_BOOL && b.type == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_val_bool(a.data.boolean == b.data.boolean));
				} else if (a.type == DT_VAL_NIL && b.type == DT_VAL_NIL) {
					dt_vm_push(vm, dt_true);
				} else if (a.type == DT_VAL_STR && b.type == DT_VAL_STR) {
					dt_vm_push(vm, dt_val_bool(dt_str_eq(a.data.str, b.data.str)));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_false);
				}
				break;
			}

			case DT_OP_GT: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_false);
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_bool(a.data.num > b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_false);
				}
				break;
			}

			case DT_OP_GT_EQ: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_false);
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_bool(a.data.num >= b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_false);
				}
				break;
			}

			case DT_OP_LT: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_false);
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_bool(a.data.num < b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_false);
				}
				break;
			}

			case DT_OP_LT_EQ: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_false);
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_bool(a.data.num <= b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_false);
				}
				break;
			}

			case DT_OP_OR: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_false);
				} else if (a.type == DT_VAL_BOOL && b.type == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_val_bool(a.data.boolean || b.data.boolean));
				} else {
					dt_vm_err(
						vm,
						"cannot or a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_false);
				}
				break;
			}

			case DT_OP_AND: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_false);
				} else if (a.type == DT_VAL_BOOL && b.type == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_val_bool(a.data.boolean && b.data.boolean));
				} else {
					dt_vm_err(
						vm,
						"cannot and a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_false);
				}
				break;
			}

			case DT_OP_POP:
				dt_vm_pop(vm);
				break;

			case DT_OP_GETG: {
				if (!vm->globals) {
					dt_vm_push(vm, dt_nil);
				}
				dt_val name = chunk->consts->values[*vm->ip++];
				if (name.type == DT_VAL_STR) {
					dt_val val = dt_map_get(vm->globals, name.data.str);
					dt_vm_push(vm, val);
				} else {
					dt_vm_err(
						vm,
						"expected var name to be 'str' found '%s'\n",
						dt_type_name(name.type)
					);
					dt_vm_push(vm, dt_nil);
				}
				break;
			}

			case DT_OP_SETG: {
				if (!vm->globals) {
					dt_vm_push(vm, dt_nil);
				}
				dt_val name = chunk->consts->values[*vm->ip++];
				if (name.type == DT_VAL_STR) {
					if (dt_map_exists(vm->globals, name.data.str)) {
						dt_map_set(vm, vm->globals, name.data.str, dt_vm_get(vm, 0));
					} else {
						dt_vm_err(vm, "'%s' is not declared\n", name.data.str->chars);
					}
				} else {
					dt_vm_err(
						vm,
						"expected var name to be 'str' found '%s'\n",
						dt_type_name(name.type)
					);
				}
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
				dt_vm_push(vm, *vm->func->upvals[*vm->ip++]);
				break;
			}

			case DT_OP_SETU: {
				*vm->func->upvals[*vm->ip++] = dt_vm_get(vm, 0);
				break;
			}

			case DT_OP_ARGS: {
				// TODO
				dt_vm_push(vm, dt_nil);
				break;
			}

			case DT_OP_INDEX: {
				dt_val key = dt_vm_pop(vm);
				dt_val val = dt_vm_pop(vm);
				switch (val.type) {
					case DT_VAL_ARR:
						if (key.type == DT_VAL_NUM) {
							dt_vm_push(vm, dt_arr_get(val.data.arr, key.data.num));
						} else if (key.type == DT_VAL_RANGE) {
							dt_range range = key.data.range;
							if (range.end < range.start) {
								int tmp = range.start;
								range.start = range.end;
								range.end = tmp;
							}
							dt_arr* arr = val.data.arr;
							if (range.start >= arr->len || range.end > arr->len) {
								dt_vm_push(vm, dt_nil);
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
								dt_vm_push(vm, dt_val_arr(arr2));
							}
						} else {
							dt_vm_err(
								vm,
								"invalid arr idx type '%s'\n",
								dt_type_name(val.type)
							);
							dt_vm_push(vm, dt_nil);
						}
						break;
					case DT_VAL_MAP:
						if (key.type == DT_VAL_STR) {
							dt_vm_push(vm, dt_map_get(val.data.map, key.data.str));
						} else {
							dt_vm_err(
								vm,
								"invalid map idx type '%s'\n",
								dt_type_name(val.type)
							);
							dt_vm_push(vm, dt_nil);
						}
						break;
					case DT_VAL_STR:
						if (key.type == DT_VAL_NUM) {
							int idx = (int)key.data.num;
							dt_str* str = val.data.str;
							if (idx >= str->len) {
								dt_vm_push(vm, dt_nil);
							} else {
								dt_vm_push(vm, dt_val_str(dt_str_new_len(
									vm,
									str->chars + idx,
									1
								)));
							}
						} else if (key.type == DT_VAL_RANGE) {
							dt_range range = key.data.range;
							if (range.end < range.start) {
								int tmp = range.start;
								range.start = range.end;
								range.end = tmp;
							}
							dt_str* str = val.data.str;
							if (range.start >= str->len || range.end > str->len) {
								dt_vm_push(vm, dt_nil);
							} else {
								dt_vm_push(vm, dt_val_str(dt_str_new_len(vm,
									str->chars + range.start,
									range.end - range.start
								)));
							}
						} else {
							dt_vm_err(
								vm,
								"invalid str idx type '%s'\n",
								dt_type_name(val.type)
							);
							dt_vm_push(vm, dt_nil);
						}
						break;
					default:
						dt_vm_err(
							vm,
							"cannot index a '%s'\n",
							dt_type_name(val.type)
						);
						dt_vm_push(vm, dt_nil);
						break;
				}
				break;
			}

			// TODO: improve
			case DT_OP_CALL: {

				int nargs = *vm->ip++;

				dt_val val = dt_vm_get(vm, -nargs);
				dt_val ret = dt_nil;

				if (val.type == DT_VAL_CFUNC) {

					ret = (val.data.cfunc)(vm, nargs);

					// pop args
					for (int i = 0; i < nargs; i++) {
						dt_vm_pop_close(vm);
					}

				} else if (val.type == DT_VAL_FUNC) {
					ret = dt_vm_call(vm, val.data.func, nargs);
				} else {
					dt_vm_err(
						vm,
						"cannot call a '%s'\n",
						dt_type_name(val.type)
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

			case DT_OP_MKARR: {
				int len = *vm->ip++;
				dt_arr* arr = dt_arr_new_len(vm, len);
				for (int i = 0; i < len; i++) {
					dt_val v = *(vm->stack_top - len + i);
					if (v.type == DT_VAL_RANGE) {
						dt_range r = v.data.range;
						if (r.end >= r.start) {
							for (int j = r.start; j < r.end; j++) {
								dt_arr_push(vm, arr, dt_val_num(j));
							}
						} else {
							for (int j = r.start; j > r.end; j--) {
								dt_arr_push(vm, arr, dt_val_num(j));
							}
						}
					} else {
						dt_arr_push(vm, arr, v);
					}
				}
				vm->stack_top -= len;
				dt_vm_push(vm, dt_val_arr(arr));
				dt_vm_gc_check(vm);
				break;
			}

			case DT_OP_MKMAP: {
				int len = *vm->ip++;
				dt_map* map = dt_map_new_len(vm, len);
				for (int i = 0; i < len; i++) {
					dt_val val = dt_vm_pop(vm);
					dt_val key = dt_vm_pop(vm);
					if (key.type == DT_VAL_STR) {
						dt_map_set(vm, map, key.data.str, val);
					} else {
						dt_vm_err(
							vm,
							"expected key to be 'str', found '%s'\n",
							dt_type_name(key.type)
						);
					}
				}
				dt_vm_push(vm, dt_val_map(map));
				dt_vm_gc_check(vm);
				break;
			}

			case DT_OP_MKFUNC: {
				dt_val val = dt_vm_pop(vm);
				uint8_t num_upvals = *vm->ip++;
				if (val.type != DT_VAL_LOGIC) {
					// unreachable
					dt_vm_err(
						vm,
						"cannot make a func out of '%s'\n",
						dt_type_name(val.type)
					);
				}
				dt_func* func = dt_malloc(vm, sizeof(dt_func));
				func->logic = val.data.logic;
				func->num_upvals = num_upvals;
				func->upvals = dt_malloc(vm, sizeof(dt_val*) * num_upvals);
				for (int i = 0; i < num_upvals; i++) {
					bool local = *vm->ip++;
					uint8_t idx = *vm->ip++;
					if (local) {
						dt_val* upval = &vm->stack[vm->stack_offset + idx];
						func->upvals[i] = upval;
						vm->open_upvals[vm->num_upvals++] = &func->upvals[i];
					} else {
						func->upvals[i] = vm->func->upvals[idx];
					}
				}
				dt_vm_push(vm, dt_val_func(func));
				dt_vm_manage_mem(vm, (dt_heaper*)func, DT_HEAPER_FUNC);
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
				if (cond.type != DT_VAL_BOOL) {
					dt_vm_err(
						vm,
						"expected cond to be 'bool', found '%s'\n",
						dt_type_name(cond.type)
					);
				} else {
					jump = !cond.data.boolean;
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
				switch (iter.type) {
					case DT_VAL_ARR:
						if (iter.data.arr->len == 0) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						dt_vm_push(vm, dt_val_num(0));
						dt_vm_push(vm, dt_arr_get(iter.data.arr, 0));
						break;
					case DT_VAL_STR:
						if (iter.data.str->len == 0) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						dt_vm_push(vm, dt_val_num(0));
						dt_vm_push(vm, dt_val_str(dt_str_new_len(
							vm,
							iter.data.str->chars,
							1
						)));
						break;
					case DT_VAL_MAP:
						if (iter.data.map->cnt == 0) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						int i = 0;
						for (; i < iter.data.map->cap; i++) {
							if (iter.data.map->entries[i].key) {
								break;
							}
						}
						dt_vm_push(vm, dt_val_num(i));
						dt_str* key = iter.data.map->entries[i].key;
						dt_vm_push(vm, dt_val_str(key));
						break;
					case DT_VAL_RANGE:
						if (iter.data.range.start == iter.data.range.end) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						dt_vm_push(vm, dt_val_num(0));
						dt_vm_push(vm, dt_val_num(iter.data.range.start));
						break;
					default:
						dt_vm_err(
							vm,
							"'%s' is not iterable\n",
							dt_type_name(iter.type)
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
				int idx = ++n->data.num;
				switch (iter.type) {
					case DT_VAL_ARR:
						if (idx < iter.data.arr->len) {
							*(vm->stack_top - 1) = dt_arr_get(iter.data.arr, idx);
							vm->ip -= dis;
						}
						break;
					case DT_VAL_STR:
						if (idx < iter.data.str->len) {
							*(vm->stack_top - 1) = dt_val_str(dt_str_new_len(
								vm,
								iter.data.str->chars + idx,
								1
							));
							vm->ip -= dis;
						}
						break;
					case DT_VAL_MAP:
						if (idx < iter.data.map->cap) {
							while (idx < iter.data.map->cap && !iter.data.map->entries[idx].key) {
								idx++;
							}
							if (idx < iter.data.map->cap) {
								dt_str* key = iter.data.map->entries[idx].key;
								*(vm->stack_top - 1) = dt_val_str(key);
								n->data.num = idx;
								vm->ip -= dis;
							}
						}
						break;
					case DT_VAL_RANGE: {
						int start = iter.data.range.start;
						int end = iter.data.range.end;
						bool done = false;
						if (end < start) {
							done = idx >= start - end;
						} else {
							done = idx >= end - start;
						}
						if (!done) {
							if (end < start) {
								*(vm->stack_top - 1) = dt_val_num(start - idx);
							} else {
								*(vm->stack_top - 1) = dt_val_num(start + idx);
							}
							vm->ip -= dis;
						}
						break;
					}
					default:
						dt_vm_err(
							vm,
							"'%s' is not iterable\n",
							dt_type_name(iter.type)
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
		case '$': return dt_scanner_make_token(s, DT_TOKEN_DOLLAR);
		case '\\': return dt_scanner_make_token(s, DT_TOKEN_BACKSLASH);
		case '?': return dt_scanner_make_token(s, DT_TOKEN_QUESTION);
		case '^': return dt_scanner_make_token(s, DT_TOKEN_CARET);
		case ':':
			return dt_scanner_make_token(s,
				dt_scanner_match(s, ':') ? DT_TOKEN_COLON_COLON : DT_TOKEN_COLON
			);
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

static void dt_c_prec(dt_compiler* c, dt_prec prec);
static void dt_c_binary(dt_compiler* c);

static void dt_c_err(dt_compiler* c, char* fmt, ...) {
	dt_token* cur = &c->parser.cur;
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "line #%d: ", cur->line);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

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

static void dt_c_emit_jmp(dt_compiler* c, dt_op op, uint16_t dis) {
	dt_c_emit3(c, op, dis >> 8, dis & 0xff);
}

static int dt_c_emit_jmp_empty(dt_compiler* c, dt_op op) {
	dt_c_emit3(c, op, 0, 0);
	return c->env->chunk.cnt;
}

static void dt_c_patch_jmp(dt_compiler* c, int pos) {

	int dis = c->env->chunk.cnt - pos;

	if (dis >= UINT16_MAX) {
		dt_c_err(c, "jump too large\n");
	}

	c->env->chunk.code[pos - 2] = dis >> 8;
	c->env->chunk.code[pos - 1] = dis & 0xff;

}

static void dt_c_push_const(dt_compiler* c, dt_val val) {
	uint16_t idx = dt_chunk_add_const(&c->env->chunk, val);
	dt_c_emit3(c, DT_OP_CONST, idx >> 8, idx & 0xff);
}

static void dt_c_nxt(dt_compiler* c) {
	c->parser.prev = c->parser.cur;
	dt_token t = dt_scanner_scan(&c->scanner);
	c->parser.cur = t;
	if (t.type == DT_TOKEN_ERR) {
		dt_c_err(c, "unexpected token\n");
	}
}

static dt_token_ty dt_c_peek(dt_compiler* c) {
	return c->parser.cur.type;
}

static bool dt_c_match(dt_compiler* c, dt_token_ty ty) {
	if (dt_c_peek(c) == ty) {
		dt_c_nxt(c);
		return true;
	}
	return false;
}

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
	dt_val num = dt_val_num(strtof(c->parser.prev.start, NULL));
	dt_c_push_const(c, num);
}

static void dt_c_str(dt_compiler* c) {
	dt_token str_t = dt_c_consume(c, DT_TOKEN_STR);
	dt_val str = dt_val_str(dt_str_new_len(
		NULL,
		str_t.start + 1,
		str_t.len - 2
	));
	dt_c_push_const(c, str);
}

static void dt_c_lit(dt_compiler* c) {
	dt_c_nxt(c);
	switch (c->parser.prev.type) {
		case DT_TOKEN_QUESTION: dt_c_emit(c, DT_OP_NIL); break;
		case DT_TOKEN_T: dt_c_emit(c, DT_OP_TRUE); break;
		case DT_TOKEN_F: dt_c_emit(c, DT_OP_FALSE); break;
		default: dt_c_err(c, "cannot process as literal\n");
	}
}

static void dt_c_expr(dt_compiler* c) {
	dt_c_prec(c, DT_PREC_ASSIGN);
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
		dt_c_push_const(c, dt_val_str(dt_str_new_len(NULL, name.start, name.len)));
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

static void dt_c_scope_begin(dt_compiler* c, dt_scope_ty ty) {
	c->env->scopes[c->env->cur_depth++] = ty;
}

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

static int dt_c_block(dt_compiler* c, dt_scope_ty ty) {

	dt_c_consume(c, DT_TOKEN_LBRACE);
	dt_c_scope_begin(c, ty);

	while (dt_c_peek(c) != DT_TOKEN_RBRACE) {
		dt_c_stmt(c);
	}

	dt_c_consume(c, DT_TOKEN_RBRACE);
	dt_c_scope_end(c);

	return c->env->cur_depth;

}

// for parsing following cond without % only |
static void dt_c_cond_inner(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_LPAREN);
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RPAREN);
	int if_start = dt_c_emit_jmp_empty(c, DT_OP_JMP_COND);
	dt_c_block(c, DT_BLOCK_COND);
	int if_dis = c->env->chunk.cnt - if_start;

	if (dt_c_match(c, DT_TOKEN_OR)) {

		// for JMP(2)
		if_dis += 3;
		int pos = dt_c_emit_jmp_empty(c, DT_OP_JMP);

		if (dt_c_peek(c) == DT_TOKEN_LPAREN) {
			dt_c_cond_inner(c);
		} else {
			dt_c_block(c, DT_BLOCK_COND);
		}

		dt_c_patch_jmp(c, pos);

	}

	if (if_dis >= UINT16_MAX) {
		dt_c_err(c, "jump too large\n");
	}

	// TODO: patchable?
	c->env->chunk.code[if_start - 2] = if_dis >> 8;
	c->env->chunk.code[if_start - 1] = if_dis & 0xff;

}

// for parsing following cond without % only |
static void dt_c_cond2_inner(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_LPAREN);
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RPAREN);
	int if_start = dt_c_emit_jmp_empty(c, DT_OP_JMP_COND);
	dt_c_expr(c);
	int if_dis = c->env->chunk.cnt - if_start;

	if (dt_c_match(c, DT_TOKEN_OR)) {

		// for JMP(2)
		if_dis += 3;
		int pos = dt_c_emit_jmp_empty(c, DT_OP_JMP);

		if (dt_c_peek(c) == DT_TOKEN_LPAREN) {
			dt_c_cond2_inner(c);
		} else {
			dt_c_expr(c);
		}

		dt_c_patch_jmp(c, pos);

	}

	if (if_dis >= UINT16_MAX) {
		dt_c_err(c, "jump too large\n");
	}

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

// conditionals
// % (<bool>) <expr> | (<bool>)? <expr>
static void dt_c_cond2(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_PERCENT);
	dt_c_cond2_inner(c);
}

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
		int depth = dt_c_block(c, DT_BLOCK_LOOP);
		int dis = c->env->chunk.cnt - pos + 3;

		if (dis >= UINT16_MAX) {
			dt_c_err(c, "jump too large\n");
		}

		dt_c_emit_jmp(c, DT_OP_ITER, dis);

		for (int i = 0; i < c->env->num_jumpers; i++) {
			dt_jumper j = c->env->jumpers[i];
			if (j.depth == depth) {
				dt_c_patch_jmp(c, j.pos);
				c->env->jumpers[i--] = c->env->jumpers[--c->env->num_jumpers];
			}
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
		int depth = dt_c_block(c, DT_BLOCK_LOOP);
		int dis = c->env->chunk.cnt - start + 3;

		if (dis >= UINT16_MAX) {
			dt_c_err(c, "jump too large\n");
		}

		dt_c_emit_jmp(c, DT_OP_REWIND, dis);

		for (int i = 0; i < c->env->num_jumpers; i++) {
			dt_jumper j = c->env->jumpers[i];
			if (j.depth == depth) {
				dt_c_patch_jmp(c, j.pos);
				c->env->jumpers[i--] = c->env->jumpers[--c->env->num_jumpers];
			}
		}

	}

	dt_c_emit(c, DT_OP_NIL);

}

static void dt_c_end_func(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_TILDE_GT);
	dt_c_expr(c);
	dt_c_emit(c, DT_OP_STOP);
}

static void dt_c_add_jumper(dt_compiler* c, dt_scope_ty ty, dt_op op) {
	int depth = -1;
	for (int i = c->env->cur_depth - 1; i >= 0; i--) {
		if (c->env->scopes[i] == ty) {
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
	int pos = dt_c_emit_jmp_empty(c, op);
	c->env->jumpers[c->env->num_jumpers++] = (dt_jumper) {
		.ty = ty,
		.pos = pos,
		.depth = depth,
	};
}

static void dt_c_end_loop(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_AT_GT);
	dt_c_add_jumper(c, DT_BLOCK_LOOP, DT_OP_JMP);
}

// TODO
static void dt_c_nxt_loop(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_AT_CARET);
	dt_c_add_jumper(c, DT_BLOCK_LOOP, DT_OP_JMP);
}

static void dt_c_stmt(dt_compiler* c) {

	switch (dt_c_peek(c)) {
		case DT_TOKEN_BANG:       dt_c_typedef(c); break;
		case DT_TOKEN_DOLLAR:     dt_c_decl(c); break;
		case DT_TOKEN_PERCENT:    dt_c_cond(c); break;
		case DT_TOKEN_LBRACE:     dt_c_block(c, DT_BLOCK_NORMAL); break;
		case DT_TOKEN_TILDE_GT:   dt_c_end_func(c); break;
		case DT_TOKEN_AT_GT:      dt_c_end_loop(c); break;
		case DT_TOKEN_AT_CARET:   dt_c_nxt_loop(c); break;
		default: {
			dt_c_expr(c);
			dt_c_emit(c, DT_OP_POP);
		};
	}

}

static void dt_c_index(dt_compiler* c) {
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RBRACKET);
	dt_c_emit(c, DT_OP_INDEX);
}

static void dt_c_index2(dt_compiler* c) {
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
	dt_c_push_const(c, dt_val_str(dt_str_new_len(NULL, name.start, name.len)));
	dt_c_emit(c, DT_OP_INDEX);
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
			idx = dt_chunk_add_const(&c->env->chunk, dt_val_str(str));
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
		.jumpers = {0},
		.num_jumpers = 0,
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

	dt_c_block(c, DT_BLOCK_NORMAL);

	dt_c_emit(c, DT_OP_NIL);
	dt_c_emit(c, DT_OP_STOP);

	c->env = prev_env;

	// TODO
	dt_logic* logic = malloc(sizeof(dt_logic));
	logic->nargs = nargs;
	logic->chunk = env.chunk;

	dt_c_push_const(c, (dt_val) {
		.type = DT_VAL_LOGIC,
		.data = {
			.logic = logic,
		},
	});

	dt_c_emit(c, DT_OP_MKFUNC);
	dt_c_emit(c, env.num_upvals);

	for (int i = 0; i < env.num_upvals; i++) {
		dt_c_emit(c, env.upvals[i].local);
		dt_c_emit(c, env.upvals[i].idx);
	}

}

static dt_parse_rule dt_rules[] = {
	// token                     // prefix   // infix     // precedence
	[DT_TOKEN_LPAREN]        = { dt_c_group, dt_c_call,   DT_PREC_CALL },
	[DT_TOKEN_RPAREN]        = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_LBRACE]        = { dt_c_map,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_RBRACE]        = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_LBRACKET]      = { dt_c_arr,   dt_c_index,  DT_PREC_CALL },
	[DT_TOKEN_RBRACKET]      = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_COMMA]         = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_DOT]           = { NULL,       dt_c_index2, DT_PREC_CALL },
	[DT_TOKEN_MINUS]         = { dt_c_unary, dt_c_binary, DT_PREC_TERM },
	[DT_TOKEN_PLUS]          = { NULL,       dt_c_binary, DT_PREC_TERM },
	[DT_TOKEN_SLASH]         = { NULL,       dt_c_binary, DT_PREC_FACTOR },
	[DT_TOKEN_STAR]          = { NULL,       dt_c_binary, DT_PREC_FACTOR },
	[DT_TOKEN_DOT_DOT]       = { NULL,       dt_c_binary, DT_PREC_UNARY },
	[DT_TOKEN_DOT_DOT_DOT]   = { dt_c_args,  NULL,        DT_PREC_NONE },
	[DT_TOKEN_HASH]          = { dt_c_unary, NULL,        DT_PREC_NONE },
	[DT_TOKEN_BANG]          = { dt_c_unary, NULL,        DT_PREC_NONE },
	[DT_TOKEN_BANG_EQ]       = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_EQ]            = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_EQ_EQ]         = { NULL,       dt_c_binary, DT_PREC_EQ },
	[DT_TOKEN_GT]            = { NULL,       dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_GT_EQ]         = { NULL,       dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_LT]            = { NULL,       dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_LT_EQ]         = { NULL,       dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_LT_LT]         = { NULL,       dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_OR_OR]         = { NULL,       dt_c_binary, DT_PREC_LOGIC },
	[DT_TOKEN_AND_AND]       = { NULL,       dt_c_binary, DT_PREC_LOGIC },
	[DT_TOKEN_IDENT]         = { dt_c_ident, NULL,        DT_PREC_NONE },
	[DT_TOKEN_STR]           = { dt_c_str,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_NUM]           = { dt_c_num,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_AND]           = { dt_c_this,  NULL,        DT_PREC_NONE },
	[DT_TOKEN_T]             = { dt_c_lit,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_F]             = { dt_c_lit,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_QUESTION]      = { dt_c_lit,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_OR]            = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_ERR]           = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_END]           = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_AT]            = { dt_c_loop,  NULL,        DT_PREC_NONE },
	[DT_TOKEN_PERCENT]       = { dt_c_cond2, NULL,        DT_PREC_NONE },
	[DT_TOKEN_TILDE]         = { dt_c_func,  NULL,        DT_PREC_NONE },
};

static void dt_c_prec(dt_compiler* c, dt_prec prec) {
	dt_parse_rule* prev_rule = &dt_rules[dt_c_peek(c)];
	if (prev_rule->prefix == NULL) {
		dt_c_err(c, "expected expression\n");
		return;
	}
	prev_rule->prefix(c);
	while (prec <= dt_rules[dt_c_peek(c)].prec) {
		dt_c_nxt(c);
		dt_rules[c->parser.prev.type].infix(c);
	}
}

static void dt_c_binary(dt_compiler* c) {

	dt_token_ty ty = c->parser.prev.type;

	dt_parse_rule* rule = &dt_rules[ty];
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

static dt_val dt_f_print(dt_vm* vm, int nargs) {

	for (int i = -nargs + 1; i <= 0; i++) {
		dt_val val = dt_vm_get(vm, i);
		dt_val_print(&val);
		printf(" ");
	}

	printf("\n");

	return dt_nil;

}

// TODO
static dt_val dt_f_error(dt_vm* vm, int nargs) {
	return dt_nil;
}

static dt_val dt_f_type(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return dt_nil;
	}
	dt_val val = dt_vm_get(vm, 0);
	char* tname = dt_type_name(val.type);
	return dt_val_str(dt_str_new(vm, tname));
}

static dt_val dt_f_exit(dt_vm* vm, int nargs) {
	exit(EXIT_SUCCESS);
	return dt_nil;
}

// TODO: return stdout
static dt_val dt_f_exec(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return dt_nil;
	}
	char* cmd = dt_vm_get_cstr(vm, 0);
	system(cmd);
	return dt_nil;
}

static dt_val dt_f_getenv(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return dt_nil;
	}
	char* var = dt_vm_get_cstr(vm, 0);
	char* val = getenv(var);
	if (val) {
		return dt_val_str(dt_str_new(vm, val));
	} else {
		return dt_nil;
	}
}

static dt_val dt_f_time(dt_vm* vm, int nargs) {
	return dt_val_num(time(NULL));
}

static dt_val dt_f_eval(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return dt_nil;
	}
	char* code = dt_vm_get_cstr(vm, 0);
	// TODO
	return dt_eval(vm, code);
}

static dt_val dt_f_dofile(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return dt_nil;
	}
	char* path = dt_vm_get_cstr(vm, 0);
	// TODO
	return dt_dofile(vm, path);
}

static dt_val dt_f_fread(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return dt_nil;
	}
	char* path = dt_vm_get_cstr(vm, 0);
	size_t size;
	char* content = dt_read_file(path, &size);
	free(path);
	dt_str* str = dt_str_new_len(vm, content, size);
	free(content);
	return dt_val_str(str);
}

void dt_load_std(dt_vm* vm) {
	// TODO: namespacing
	dt_map_cset_cfunc(NULL, vm->globals, "type", dt_f_type);
	dt_map_cset_cfunc(NULL, vm->globals, "eval", dt_f_eval);
	dt_map_cset_cfunc(NULL, vm->globals, "dofile", dt_f_dofile);
	dt_map_cset_cfunc(NULL, vm->globals, "error", dt_f_error);
	dt_map_cset_cfunc(NULL, vm->globals, "print", dt_f_print);
	dt_map_cset_cfunc(NULL, vm->globals, "exit", dt_f_exit);
	dt_map_cset_cfunc(NULL, vm->globals, "exec", dt_f_exec);
	dt_map_cset_cfunc(NULL, vm->globals, "time", dt_f_time);
	dt_map_cset_cfunc(NULL, vm->globals, "getenv", dt_f_getenv);
	dt_map_cset_cfunc(NULL, vm->globals, "fread", dt_f_fread);
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
		return dt_nil;
	}

	dt_val ret = dt_eval(vm, code);
	free(code);

	return ret;

}

#endif
#endif
