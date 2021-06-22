// wengwengweng

// TODO: garbage collection
// TODO: error handling
// TODO: ... to access func arguments
// TODO: 'this'
// TODO: trait
// TODO: module
// TODO: loops
// TODO: threading
// TODO: utf8
// TODO: named func
// TODO: binding helpers
// TODO: expose tokenizer
// TODO: stand alone bytecode

#ifndef D_LANG_H
#define D_LANG_H

#define DT_STACK_MAX 256

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
	DT_VAL_FUNC,
	DT_VAL_CFUNC,
	DT_VAL_CDATA,
	DT_VAL_CPTR,
} dt_val_ty;

typedef struct {
	int len;
	char *chars;
} dt_str;

struct dt_map;
struct dt_arr;
struct dt_vm;
struct dt_val;
struct dt_logic;
struct dt_func;

typedef float dt_num;
typedef bool dt_bool;

typedef struct dt_val (dt_cfunc)(struct dt_vm *vm, int nargs);

typedef struct dt_val {
	dt_val_ty type;
	union {
		dt_bool boolean;
		dt_num num;
		dt_str str;
		struct dt_arr *arr;
		struct dt_map *map;
		// TODO: remove this
		struct dt_logic *logic;
		struct dt_func *func;
		dt_cfunc *cfunc;
	} data;
} dt_val;

typedef struct dt_arr {
	int len;
	int cap;
	dt_val *values;
} dt_arr;

typedef struct {
	dt_str key;
	dt_val val;
} dt_entry;

typedef struct dt_map {
	int cnt;
	int cap;
	dt_entry **entries;
} dt_map;

typedef struct {
	int cnt;
	int cap;
	uint8_t *code;
	int *lines;
	dt_arr consts;
} dt_chunk;

typedef struct dt_logic {
	dt_chunk chunk;
	int nargs;
} dt_logic;

// TODO: separate dt_closure and dt_func
typedef struct dt_func {
	dt_logic *logic;
	dt_val **upvals;
} dt_func;

typedef struct dt_vm {
	struct dt_func *func;
	uint8_t *ip;
	dt_val stack[DT_STACK_MAX];
	dt_val *stack_top;
	int stack_offset;
	dt_map *env;
	dt_val **upvals[UINT8_MAX];
	int num_upvals;
} dt_vm;

void     dt_load_std(dt_map *map);
dt_val   dt_eval(char *src);
dt_val   dt_eval_ex(char *src, dt_map *env);
dt_val   dt_dofile(char *path);
dt_val   dt_dofile_ex(char *path, dt_map *env);

dt_val   dt_val_new_num(dt_num n);
dt_val   dt_val_new_bool(dt_bool b);
dt_val   dt_val_new_str(char *src, int len);
dt_val   dt_val_new_cfunc(dt_cfunc *func);
dt_val   dt_val_new_map(dt_map *map);
dt_val   dt_val_new_arr(dt_arr *arr);
void     dt_val_print(dt_val *val);
void     dt_val_println(dt_val *val);

dt_str   dt_str_new(char *src);
dt_str   dt_str_new_len(char *src, int len);
void     dt_str_free(dt_str *str);
dt_str   dt_str_clone(dt_str *str);
dt_val   dt_str_concat(dt_str *a, dt_str *b);
bool     dt_str_eq(dt_str *a, dt_str *b);
dt_val   dt_str_replace(dt_val src, dt_val old, dt_val new);

dt_arr   dt_arr_new();
void     dt_arr_free(dt_arr *arr);
dt_val   dt_arr_get(dt_arr *arr, int idx);
void     dt_arr_set(dt_arr *arr, int idx, dt_val val);
void     dt_arr_insert(dt_arr *arr, int idx, dt_val val);
void     dt_arr_push(dt_arr *arr, dt_val val);
dt_val   dt_arr_rm(dt_arr *arr, int idx);
void     dt_arr_print(dt_arr *arr);

dt_map   dt_map_new       ();
void     dt_map_free      (dt_map *map);
void     dt_map_set       (dt_map *map, dt_str *key, dt_val val);
void     dt_map_set_cfunc (dt_map *map, char *name, dt_cfunc *func);
void     dt_map_set_map   (dt_map *env, char *name, dt_map *map);
bool     dt_map_exists    (dt_map *map, dt_str *key);
dt_val   dt_map_get       (dt_map *map, dt_str *key);
dt_arr   dt_map_keys      (dt_map *map);
dt_arr   dt_map_vals      (dt_map *map);

dt_val*  dt_vm_get      (dt_vm *vm, int idx);
float    dt_vm_get_num  (dt_vm *vm, int idx);
bool     dt_vm_get_bool (dt_vm *vm, int idx);
char*    dt_vm_get_str  (dt_vm *vm, int idx);
dt_map*  dt_vm_get_map  (dt_vm *vm, int idx);
dt_arr*  dt_vm_get_arr  (dt_vm *vm, int idx);
dt_func* dt_vm_get_func (dt_vm *vm, int idx);
dt_val   dt_vm_call     (dt_vm *vm, dt_func *func, int nargs, ...);

void     dt_check_nargs(int expected, int actual);
void     dt_check_arg(dt_vm *vm, int pos, dt_val_ty ty);

dt_val dt_nil = (dt_val) {
	.type = DT_VAL_NIL,
	.data = {
		.num = 0,
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

#define DT_ARR_INIT_SIZE 4
#define DT_MAP_INIT_SIZE 8
#define DT_MAP_MAX_LOAD 0.75

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
	DT_OP_FUNC,
	DT_OP_MKARR,
	DT_OP_MKMAP,
	DT_OP_JMP,
	DT_OP_JMP_COND,
	DT_OP_REWIND,
	DT_OP_CLOSE,
} dt_op;

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
	DT_TOKEN_MINUS_GT, // ->
	// lit
	DT_TOKEN_IDENT,
	DT_TOKEN_STR,
	DT_TOKEN_NUM,
	// key
	DT_TOKEN_FN,
	DT_TOKEN_TRUE,
	DT_TOKEN_FALSE,
	DT_TOKEN_NIL,
	DT_TOKEN_IF,
	DT_TOKEN_ELSE,
	DT_TOKEN_LOOP,
	DT_TOKEN_RETURN,
	DT_TOKEN_BREAK,
	DT_TOKEN_CONTINUE,
	// util
	DT_TOKEN_ERR,
	DT_TOKEN_END,
} dt_token_ty;

typedef enum {
	DT_PREC_NONE,
	DT_PREC_ASSIGN,  // =
	DT_PREC_LOGICAL, // || &&
	DT_PREC_EQUAL,   // == !=
	DT_PREC_COMPARE, // < > <= >=
	DT_PREC_TERM,    // + -
	DT_PREC_FACTOR,  // * / %
	DT_PREC_POWER,  // ^
	DT_PREC_UNARY,   // ! - #
	DT_PREC_CALL,    // () [] .
	DT_PREC_PRIMARY
} dt_prec;

typedef struct {
	dt_token_ty type;
	char *start;
	int len;
	int line;
} dt_token;

typedef struct {
	dt_token prev;
	dt_token cur;
} dt_parser;

typedef struct {
	char *start;
	char *cur;
	int line;
} dt_scanner;

typedef struct {
	dt_token name;
	bool captured;
	int depth;
} dt_local;

typedef struct {
	int idx;
	bool local;
} dt_upval;

typedef struct dt_funcenv {
	struct dt_funcenv *parent;
	int depth;
	dt_local locals[UINT8_MAX];
	int num_locals;
	dt_upval upvals[UINT8_MAX];
	int num_upvals;
	dt_chunk chunk;
} dt_funcenv;

typedef struct {
	dt_scanner scanner;
	dt_parser parser;
	dt_funcenv base_env;
	dt_funcenv *env;
} dt_compiler;

typedef void (*dt_parse_fn)(dt_compiler *compiler);

typedef struct {
	dt_parse_fn prefix;
	dt_parse_fn infix;
	dt_prec prec;
} dt_parse_rule;

static void dt_fail(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

char *dt_token_name(dt_token_ty ty) {
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
		case DT_TOKEN_MINUS_GT: return "MINUS_GT";
		case DT_TOKEN_IDENT: return "IDENT";
		case DT_TOKEN_STR: return "STR";
		case DT_TOKEN_NUM: return "NUM";
		case DT_TOKEN_FN: return "FN";
		case DT_TOKEN_TRUE: return "TRUE";
		case DT_TOKEN_FALSE: return "FALSE";
		case DT_TOKEN_NIL: return "NIL";
		case DT_TOKEN_IF: return "IF";
		case DT_TOKEN_ELSE: return "ELSE";
		case DT_TOKEN_LOOP: return "LOOP";
		case DT_TOKEN_BREAK: return "BREAK";
		case DT_TOKEN_RETURN: return "RETURN";
		case DT_TOKEN_CONTINUE: return "CONTINUE";
		case DT_TOKEN_ERR: return "ERR";
		case DT_TOKEN_END: return "END";
	}
}

static bool dt_token_eq(dt_token *t1, dt_token *t2) {
	if (t1->len != t2->len) {
		return false;
	}
	return memcmp(t1->start, t2->start, t1->len) == 0;
}

char *dt_type_name(dt_val_ty ty) {
	switch (ty) {
		case DT_VAL_NIL: return "nil";
		case DT_VAL_BOOL: return "bool";
		case DT_VAL_NUM: return "num";
		case DT_VAL_STR: return "str";
		case DT_VAL_ARR: return "arr";
		case DT_VAL_MAP: return "map";
		case DT_VAL_LOGIC: return "logic";
		case DT_VAL_FUNC: return "func";
		case DT_VAL_CFUNC: return "cfunc";
		case DT_VAL_CDATA: return "cdata";
		case DT_VAL_CPTR: return "cptr";
	}
}

void dt_val_print(dt_val *val) {
	switch (val->type) {
		case DT_VAL_NIL: printf("<nil>"); break;
		case DT_VAL_BOOL:
			printf(val->data.boolean ? "true" : "false");
			break;
		case DT_VAL_NUM:
			printf("%g", val->data.num);
			break;
		case DT_VAL_STR:
			printf("%s", val->data.str.chars);
			break;
		case DT_VAL_ARR:
			printf("[ ");
			for (int i = 0; i < val->data.arr->len; i++) {
				dt_val_print(&val->data.arr->values[i]);
				printf(", ");
			}
			printf("]");
			break;
// 		case DT_VAL_MAP: printf("<map#%p>", val->map); break;
		case DT_VAL_MAP: printf("<map>"); break;
		case DT_VAL_LOGIC: printf("logic"); break;
// 		case DT_VAL_FUNC: printf("<func#%p>", val->func); break;
		case DT_VAL_FUNC: printf("<func>"); break;
// 		case DT_VAL_CFUNC: printf("<cfunc#%p>", val->cfunc); break;
		case DT_VAL_CFUNC: printf("<cfunc>"); break;
		case DT_VAL_CDATA: printf("cdata"); break;
		case DT_VAL_CPTR: printf("cptr"); break;
	}
}

void dt_val_println(dt_val *val) {
	dt_val_print(val);
	printf("\n");
}

dt_val dt_val_new_num(dt_num n) {
	return (dt_val) {
		.type = DT_VAL_NUM,
		.data = {
			.num = n,
		},
	};
}

dt_val dt_val_new_bool(dt_bool b) {
	return (dt_val) {
		.type = DT_VAL_BOOL,
		.data = {
			.boolean = b,
		},
	};
}

dt_val dt_val_new_str(char *src, int len) {
	if (!src) {
		return dt_nil;
	}
	return (dt_val) {
		.type = DT_VAL_STR,
		.data = {
			.str = dt_str_new_len(src, len),
		},
	};
}

dt_val dt_val_new_cfunc(dt_cfunc *func) {
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

dt_val dt_val_new_map(dt_map *map) {
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

dt_val dt_val_new_arr(dt_arr *arr) {
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

dt_val dt_val_new_func(dt_func *func) {
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

static uint32_t dt_hash(char *key, int len) {
	uint32_t hash = 2166136261u;
	for (int i = 0; i < len; i++) {
		hash ^= (uint8_t)key[i];
		hash *= 16777619;
	}
	return hash;
}

dt_str dt_str_new_len(char *src, int len) {
	char *chars = malloc(len + 1);
	memcpy(chars, src, len);
	chars[len] = '\0';
	return (dt_str) {
		.len = len,
		.chars = chars,
	};
}

dt_str dt_str_new(char *src) {
	return dt_str_new_len(src, strlen(src));
}

dt_str dt_str_clone(dt_str *str) {
	return dt_str_new_len(str->chars, str->len);
}

void dt_str_free(dt_str *str) {
	free(str->chars);
	memset(str, 0, sizeof(dt_str));
}

dt_val dt_str_concat(dt_str *a, dt_str *b) {
	int len = a->len + b->len;
	char *chars = malloc(len + 1);
	memcpy(chars, a->chars, a->len);
	memcpy(chars + a->len, b->chars, b->len);
	chars[len] = '\0';
	return dt_val_new_str(chars, len);
}

bool dt_str_eq(dt_str *a, dt_str *b) {
	return a->len == b->len && memcmp(a->chars, b->chars, a->len) == 0;
}

// TODO: replace all
dt_val dt_str_replace(dt_val src, dt_val old, dt_val new) {
	char *start = strstr(src.data.str.chars, old.data.str.chars);
	int offset = start - src.data.str.chars;
	if (!start) {
		return dt_nil;
	}
	int len = src.data.str.len - old.data.str.len + new.data.str.len;
	char *chars = malloc(len + 1);
	memcpy(chars, src.data.str.chars, offset);
	memcpy(chars + offset, new.data.str.chars, new.data.str.len);
	memcpy(
		chars + offset + new.data.str.len,
		src.data.str.chars + offset + old.data.str.len,
		src.data.str.len - offset - old.data.str.len
	);
	chars[len] = '\0';
	return dt_val_new_str(chars, len);
}

dt_arr dt_arr_new() {
	return (dt_arr) {
		.len = 0,
		.cap = DT_ARR_INIT_SIZE,
		.values = malloc(DT_ARR_INIT_SIZE * sizeof(dt_val)),
	};
}

void dt_arr_free(dt_arr *arr) {
	free(arr->values);
	memset(arr, 0, sizeof(dt_arr));
}

dt_val dt_arr_get(dt_arr *arr, int idx) {
	if (idx >= arr->len) {
		return dt_nil;
	}
	return arr->values[idx];
}

void dt_arr_set(dt_arr *arr, int idx, dt_val val) {

	// expand
	if (idx >= arr->cap) {
		while (idx >= arr->cap) {
			arr->cap *= 2;
		}
		arr->values = realloc(arr->values, arr->cap * sizeof(dt_val));
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

void dt_arr_insert(dt_arr *arr, int idx, dt_val val) {

	if (idx >= arr->cap) {
		dt_arr_set(arr, idx, val);
		return;
	}

	if (arr->len >= arr->cap) {
		arr->cap *= 2;
		arr->values = realloc(arr->values, arr->cap * sizeof(dt_val));
	}

	// TODO: move & insert

}

void dt_arr_push(dt_arr *arr, dt_val val) {
	dt_arr_set(arr, arr->len, val);
}

dt_val dt_arr_rm(dt_arr *arr, int idx) {
	if (idx >= arr->len) {
		return dt_nil;
	}
	dt_val v = arr->values[idx];
	memcpy(arr->values + idx, arr->values + idx + 1, (arr->len - idx - 1) * sizeof(dt_val));
	arr->len--;
	arr->values[arr->len] = dt_nil;
	return v;
}

void dt_arr_print(dt_arr *arr) {
	printf("[ ");
	for (int i = 0; i < arr->len; i++) {
		dt_val_print(&arr->values[i]);
	}
	printf(" ]\n");
}

dt_map dt_map_new() {
	return (dt_map) {
		.cnt = 0,
		.cap = DT_MAP_INIT_SIZE,
		.entries = calloc(DT_MAP_INIT_SIZE, sizeof(dt_entry*)),
	};
}

dt_arr dt_map_keys(dt_map *map) {
	dt_arr arr = dt_arr_new();
	for (int i = 0; i < map->cap; i++) {
		if (map->entries[i]) {
			dt_arr_push(&arr, (dt_val) {
				.type = DT_VAL_STR,
				.data = {
					.str = map->entries[i]->key,
				},
			});
		}
	}
	return arr;
}

dt_arr dt_map_vals(dt_map *map) {
	dt_arr arr = dt_arr_new();
	for (int i = 0; i < map->cap; i++) {
		if (map->entries[i]) {
			dt_arr_push(&arr, map->entries[i]->val);
		}
	}
	return arr;
}

void dt_map_free(dt_map *map) {

	for (int i = 0; i < map->cap; i++) {
		if (map->entries[i]) {
			dt_str_free(&map->entries[i]->key);
			free(map->entries[i]);
		}
	}

	free(map->entries);
	memset(map, 0, sizeof(dt_map));

}

bool dt_is_nil(dt_val *val) {
	return val->type == DT_VAL_NIL;
}

static int dt_map_find(dt_map *map, dt_str *key) {

	uint32_t hash = dt_hash(key->chars, key->len);
	uint32_t oidx = hash % map->cap;
	uint32_t idx = oidx;

	for (;;) {

		dt_entry *e = map->entries[idx];

		if (e == NULL || dt_str_eq(&e->key, key)) {
			return idx;
		}

		idx = (idx + 1) % map->cap;

		// went through one round
		if (idx == oidx) {
			return -1;
		}

	}

}

void dt_map_set(dt_map *map, dt_str *key, dt_val val) {

	// resize
	if (map->cnt + 1 > map->cap * DT_MAP_MAX_LOAD) {

		int old_cap = map->cap;
		int new_cap = old_cap * 2;

		dt_entry **old_entries = map->entries;
		map->cap = new_cap;
		map->entries = calloc(new_cap, sizeof(dt_entry*));

		for (int i = 0; i < old_cap; i++) {
			dt_entry *e = old_entries[i];
			if (e && !dt_is_nil(&e->val)) {
				dt_map_set(map, &e->key, e->val);
			}
		}

		free(old_entries);

	}

	int idx = dt_map_find(map, key);

	if (idx == -1) {
		dt_fail("map overflow\n");
	}

	if (map->entries[idx]) {
		map->entries[idx]->val = val;
	} else {
		dt_entry *e = malloc(sizeof(dt_entry));
		e->key = dt_str_clone(key);
		e->val = val;
		map->entries[idx] = e;
		map->cnt++;
	}

}

void dt_map_set_cfunc(dt_map *env, char *name, dt_cfunc *func) {
	dt_str key = dt_str_new(name);
	dt_map_set(env, &key, dt_val_new_cfunc(func));
}

void dt_map_set_map(dt_map *env, char *name, dt_map *map) {
	dt_str key = dt_str_new(name);
	dt_map_set(env, &key, dt_val_new_map(map));
}

bool dt_map_exists(dt_map *map, dt_str *key) {
	int idx = dt_map_find(map, key);
	return idx != -1 && map->entries[idx] != NULL;
}

dt_val dt_map_get(dt_map *map, dt_str *key) {

	int idx = dt_map_find(map, key);

	if (idx == -1 || map->entries[idx] == NULL) {
		return dt_nil;
	}

	return map->entries[idx]->val;

}

void dt_func_free(dt_func *func) {
	free(func->upvals);
	memset(func, 0, sizeof(dt_func));
}

// TODO
void dt_val_free(dt_val val) {
	switch (val.type) {
		case DT_VAL_STR:
			dt_str_free(&val.data.str);
			break;
		default:
			break;
	}
}

bool dt_val_truthiness(dt_val *val) {
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
		.consts = dt_arr_new(),
	};
}

static void dt_chunk_free(dt_chunk *c) {
	free(c->code);
	free(c->lines);
	dt_arr_free(&c->consts);
	memset(c, 0, sizeof(dt_chunk));
}

char *dt_op_name(dt_op op) {
	switch (op) {
		case DT_OP_STOP:     return "STOP";
		case DT_OP_CONST:    return "CONST";
		case DT_OP_NIL:      return "NIL";
		case DT_OP_TRUE:     return "TRUE";
		case DT_OP_FALSE:    return "FALSE";
		case DT_OP_ADD:      return "ADD";
		case DT_OP_SUB:      return "SUB";
		case DT_OP_MUL:      return "MUL";
		case DT_OP_DIV:      return "DIV";
		case DT_OP_MOD:      return "MOD";
		case DT_OP_POW:      return "POW";
		case DT_OP_NEG:      return "NEG";
		case DT_OP_NOT:      return "NOT";
		case DT_OP_EQ:       return "EQ";
		case DT_OP_GT:       return "GT";
		case DT_OP_GT_EQ:    return "GT_EQ";
		case DT_OP_LT:       return "LT";
		case DT_OP_LT_EQ:    return "LT_EQ";
		case DT_OP_OR:       return "OR";
		case DT_OP_AND:      return "AND";
		case DT_OP_LEN:      return "LEN";
		case DT_OP_POP:      return "POP";
		case DT_OP_SETG:     return "SETG";
		case DT_OP_GETG:     return "GETG";
		case DT_OP_SETL:     return "SETL";
		case DT_OP_GETL:     return "GETL";
		case DT_OP_SETU:     return "SETU";
		case DT_OP_GETU:     return "GETU";
		case DT_OP_INDEX:    return "INDEX";
		case DT_OP_CALL:     return "CALL";
		case DT_OP_FUNC:     return "FUNC";
		case DT_OP_MKARR:    return "MKARR";
		case DT_OP_MKMAP:    return "MKMAP";
		case DT_OP_JMP:      return "JMP";
		case DT_OP_JMP_COND: return "JMP_COND";
		case DT_OP_REWIND:   return "REWIND";
		case DT_OP_CLOSE:    return "CLOSE";
	}
}

int dt_chunk_peek_at(dt_chunk *c, int idx) {

	uint8_t ins = c->code[idx];

	switch (ins) {
		case DT_OP_CONST: {
			uint8_t idx2 = c->code[idx + 1];
			printf("CONST ");
			dt_val_print(&c->consts.values[idx2]);
			return idx + 2;
		}
		case DT_OP_SETG: {
			uint8_t idx2 = c->code[idx + 1];
			printf("SEFG ");
			dt_val_print(&c->consts.values[idx2]);
			return idx + 2;
		}
		case DT_OP_GETG: {
			uint8_t idx2 = c->code[idx + 1];
			printf("GETG ");
			dt_val_print(&c->consts.values[idx2]);
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
		case DT_OP_FUNC: {
			uint8_t idx2 = c->code[idx + 1];
			uint8_t num_upvals = c->code[idx + 2];
			printf("FUNC %d %d", idx2, num_upvals);
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
			uint8_t dis = c->code[idx + 1];
			printf("JMP %d", dis);
			return idx + 2;
		}
		case DT_OP_JMP_COND: {
			uint8_t dis = c->code[idx + 1];
			printf("JMP_COND %d", dis);
			return idx + 2;
		}
		case DT_OP_REWIND: {
			uint8_t dis = c->code[idx + 1];
			printf("BACK %d", dis);
			return idx + 2;
		}
		default:
			printf("%s", dt_op_name(ins));
			return idx + 1;
	}
}

void dt_chunk_push(dt_chunk *c, uint8_t byte, int line) {

	if (c->cap < c->cnt + 1) {
		c->cap = c->cap < 8 ? 8 : c->cap * 2;
		c->code = realloc(c->code, c->cap * sizeof(uint8_t));
		c->lines = realloc(c->lines, c->cap * sizeof(int));
	}

	c->code[c->cnt] = byte;
	c->lines[c->cnt] = line;
	c->cnt++;

}

void dt_chunk_push2(dt_chunk *c, uint8_t b1, uint8_t b2, int line) {
	dt_chunk_push(c, b1, line);
	dt_chunk_push(c, b2, line);
}

int dt_chunk_add_const(dt_chunk *c, dt_val val) {
	if (c->consts.len >= UINT8_MAX) {
		dt_fail("constant overflow\n");
	}
	dt_arr_push(&c->consts, val);
	return c->consts.len - 1;
}

static dt_vm dt_vm_new() {
	dt_vm vm = (dt_vm) {
		.func = NULL,
		.ip = NULL,
		.stack = {0},
		.stack_top = NULL,
		.stack_offset = 0,
		.env = NULL,
		.upvals = {0},
		.num_upvals = 0,
	};
	vm.stack_top = vm.stack;
	return vm;
}

static void dt_vm_push(dt_vm *vm, dt_val val) {
	*vm->stack_top = val;
	vm->stack_top++;
	if (vm->stack_top - vm->stack >= DT_STACK_MAX) {
		dt_fail("stack overflow\n");
	}
}

static dt_val dt_vm_pop(dt_vm *vm) {
	if (vm->stack_top == vm->stack) {
		dt_fail("stack underflow\n");
		return dt_nil;
	}
	vm->stack_top--;
	return *vm->stack_top;
}

// TODO: return NULL when out
dt_val *dt_vm_get(dt_vm *vm, int idx) {
	return vm->stack_top + idx - 1;
}

// TODO
dt_num dt_vm_get_num(dt_vm *vm, int idx) {
	return 0;
}

static void dt_vm_err(dt_vm *vm, char *fmt, ...) {
	dt_chunk *chunk = &vm->func->logic->chunk;
	int offset = vm->ip - chunk->code;
	int line = chunk->lines[offset];
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "line #%d: ", line);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

static void dt_vm_pop_close(dt_vm *vm) {

	dt_val *top = dt_vm_get(vm, 0);
	dt_val *upval = NULL;

	for (int i = 0; i < vm->num_upvals; i++) {
		if (top == *vm->upvals[i]) {
			if (!upval) {
				upval = malloc(sizeof(dt_val));
				memcpy(upval, top, sizeof(dt_val));
			}
			*vm->upvals[i] = upval;
			vm->upvals[i--] = vm->upvals[--vm->num_upvals];
		}
	}

	dt_vm_pop(vm);

}

static void dt_vm_stack_print(dt_vm *vm) {

	printf("[ ");

	for (dt_val *slot = vm->stack; slot < vm->stack_top; slot++) {
		dt_val_print(slot);
		printf(", ");
	}

	printf("]");

}

static void dt_vm_stack_println(dt_vm *vm) {
	dt_vm_stack_print(vm);
	printf("\n");
}

static void dt_vm_run(dt_vm *vm, dt_func *func);

static dt_val dt_vm_call_n(dt_vm *vm, dt_func *func, int nargs) {

	dt_logic *logic = func->logic;

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

	dt_func *prev_func = vm->func;
	uint8_t *prev_ip = vm->ip;
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

dt_val dt_vm_call(dt_vm *vm, dt_func *func, int nargs, ...) {
	va_list args;
	va_start(args, nargs);
	for (int i = 0; i < nargs; i++) {
		dt_vm_push(vm, va_arg(args, dt_val));
	}
	dt_val ret = dt_vm_call_n(vm, func, 0);
	va_end(args);
	return ret;
}

static void dt_vm_run(dt_vm *vm, dt_func *func) {

	vm->func = func;
	dt_chunk *chunk = &func->logic->chunk;
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
				dt_val constant = chunk->consts.values[*vm->ip++];
				dt_vm_push(vm, constant);
				break;
			}
			case DT_OP_NIL:
				dt_vm_push(vm, dt_nil);
				break;
			case DT_OP_TRUE:
				dt_vm_push(vm, dt_val_new_bool(true));
				break;
			case DT_OP_FALSE:
				dt_vm_push(vm, dt_val_new_bool(false));
				break;
			case DT_OP_ADD: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_new_num(a.data.num + b.data.num));
				} else if (a.type == DT_VAL_STR && b.type == DT_VAL_STR) {
					dt_vm_push(vm, dt_str_concat(&a.data.str, &b.data.str));
				// TODO: str + *
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
					dt_vm_push(vm, dt_val_new_num(a.data.num - b.data.num));
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
					dt_vm_push(vm, dt_val_new_num(a.data.num * b.data.num));
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
					dt_vm_push(vm, dt_val_new_num(a.data.num / b.data.num));
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
					dt_vm_push(vm, dt_val_new_num(fmodf(a.data.num, b.data.num)));
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
					dt_vm_push(vm, dt_val_new_num(powf(a.data.num, b.data.num)));
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
			case DT_OP_NEG: {
				dt_val a = dt_vm_pop(vm);
				if (a.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_new_num(-a.data.num));
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
					dt_vm_push(vm, dt_val_new_bool(!a.data.boolean));
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
					dt_vm_push(vm, dt_val_new_num(a.data.str.len));
				} else if (a.type == DT_VAL_ARR) {
					dt_vm_push(vm, dt_val_new_num(a.data.arr->len));
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
					dt_vm_push(vm, dt_val_new_bool(false));
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_new_bool(a.data.num == b.data.num));
				} else if (a.type == DT_VAL_BOOL && b.type == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_val_new_bool(a.data.boolean == b.data.boolean));
				} else if (a.type == DT_VAL_NIL && b.type == DT_VAL_NIL) {
					dt_vm_push(vm, dt_val_new_bool(true));
				} else if (a.type == DT_VAL_STR && b.type == DT_VAL_STR) {
					dt_vm_push(vm, dt_val_new_bool(dt_str_eq(&a.data.str, &b.data.str)));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_val_new_bool(false));
				}
				break;
			}
			case DT_OP_GT: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_val_new_bool(false));
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_new_bool(a.data.num > b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_val_new_bool(false));
				}
				break;
			}
			case DT_OP_GT_EQ: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_val_new_bool(false));
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_new_bool(a.data.num >= b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_val_new_bool(false));
				}
				break;
			}
			case DT_OP_LT: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_val_new_bool(false));
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_new_bool(a.data.num < b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_val_new_bool(false));
				}
				break;
			}
			case DT_OP_LT_EQ: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_val_new_bool(false));
				} else if (a.type == DT_VAL_NUM && b.type == DT_VAL_NUM) {
					dt_vm_push(vm, dt_val_new_bool(a.data.num <= b.data.num));
				} else {
					dt_vm_err(
						vm,
						"cannot compare a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_val_new_bool(false));
				}
				break;
			}
			case DT_OP_OR: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_val_new_bool(false));
				} else if (a.type == DT_VAL_BOOL && b.type == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_val_new_bool(a.data.boolean || b.data.boolean));
				} else {
					dt_vm_err(
						vm,
						"cannot or a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_val_new_bool(false));
				}
				break;
			}
			case DT_OP_AND: {
				dt_val b = dt_vm_pop(vm);
				dt_val a = dt_vm_pop(vm);
				if (a.type != b.type) {
					dt_vm_push(vm, dt_val_new_bool(false));
				} else if (a.type == DT_VAL_BOOL && b.type == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_val_new_bool(a.data.boolean && b.data.boolean));
				} else {
					dt_vm_err(
						vm,
						"cannot and a '%s' with '%s'\n",
						dt_type_name(a.type),
						dt_type_name(b.type)
					);
					dt_vm_push(vm, dt_val_new_bool(false));
				}
				break;
			}
			case DT_OP_POP:
				dt_vm_pop(vm);
				break;
			case DT_OP_GETG: {
				if (!vm->env) {
					dt_vm_push(vm, dt_nil);
				}
				dt_val name = chunk->consts.values[*vm->ip++];
				if (name.type == DT_VAL_STR) {
					dt_val val = dt_map_get(vm->env, &name.data.str);
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
				if (!vm->env) {
					dt_vm_push(vm, dt_nil);
				}
				dt_val name = chunk->consts.values[*vm->ip++];
				if (name.type == DT_VAL_STR) {
					if (dt_map_exists(vm->env, &name.data.str)) {
						dt_val *val = dt_vm_get(vm, 0);
						dt_map_set(vm->env, &name.data.str, *val);
					} else {
						dt_vm_err(vm, "'%s' is not declared\n", name.data.str.chars);
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
				vm->stack[*vm->ip++ + vm->stack_offset] = *dt_vm_get(vm, 0);
				break;
			}
			case DT_OP_GETU: {
				dt_vm_push(vm, *vm->func->upvals[*vm->ip++]);
				break;
			}
			case DT_OP_SETU: {
				*vm->func->upvals[*vm->ip++] = *dt_vm_get(vm, 0);
				break;
			}
			case DT_OP_INDEX: {
				dt_val key = dt_vm_pop(vm);
				dt_val val = dt_vm_pop(vm);
				switch (val.type) {
					case DT_VAL_ARR:
						if (key.type == DT_VAL_NUM) {
							dt_vm_push(vm, dt_arr_get(val.data.arr, key.data.num));
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
							dt_vm_push(vm, dt_map_get(val.data.map, &key.data.str));
						} else {
							dt_vm_err(
								vm,
								"invalid map idx type '%s'\n",
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

			case DT_OP_FUNC: {
				dt_val *val = &chunk->consts.values[*vm->ip++];
				int num_upvals = *vm->ip++;
				if (val->type != DT_VAL_LOGIC) {
					dt_vm_err(
						vm,
						"cannot make a func out of '%s'\n",
						dt_type_name(val->type)
					);
				}
				dt_func *func = malloc(sizeof(dt_func));
				func->logic = val->data.logic;
				func->upvals = malloc(sizeof(dt_val*) * num_upvals);
				for (int i = 0; i < num_upvals; i++) {
					bool local = *vm->ip++;
					uint8_t idx = *vm->ip++;
					if (local) {
						dt_val *upval = &vm->stack[vm->stack_offset + idx];
						func->upvals[i] = upval;
						vm->upvals[vm->num_upvals++] = &func->upvals[i];
					} else {
						func->upvals[i] = vm->func->upvals[idx];
					}
				}
				dt_vm_push(vm, dt_val_new_func(func));
				break;
			}

			// TODO: improve
			case DT_OP_CALL: {

				int nargs = *vm->ip++;

				dt_val *val = dt_vm_get(vm, -nargs);
				dt_val ret = dt_nil;

				if (val->type == DT_VAL_CFUNC) {

					ret = (val->data.cfunc)(vm, nargs);

					// pop args
					for (int i = 0; i < nargs; i++) {
						dt_vm_pop_close(vm);
					}

				} else if (val->type == DT_VAL_FUNC) {
					ret = dt_vm_call_n(vm, val->data.func, nargs);
				} else {
					dt_vm_err(
						vm,
						"cannot call a '%s'\n",
						dt_type_name(val->type)
					);
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
				dt_arr arr = dt_arr_new();
				for (int i = 0; i < len; i++) {
					dt_arr_set(&arr, len - i - 1, dt_vm_pop(vm));
				}
				// TODO
				dt_arr *arrm = malloc(sizeof(dt_arr));
				memcpy(arrm, &arr, sizeof(dt_arr));
				// TODO
				dt_vm_push(vm, dt_val_new_arr(arrm));
				break;
			}
			case DT_OP_MKMAP: {
				int len = *vm->ip++;
				dt_map map = dt_map_new();
				for (int i = 0; i < len; i++) {
					dt_val val = dt_vm_pop(vm);
					dt_val key = dt_vm_pop(vm);
					if (key.type == DT_VAL_STR) {
						dt_map_set(&map, &key.data.str, val);
					} else {
						dt_vm_err(
							vm,
							"expected key to be 'str' found '%s'\n",
							dt_type_name(key.type)
						);
					}
				}
				dt_map *mapm = malloc(sizeof(dt_map));
				memcpy(mapm, &map, sizeof(dt_map));
				// TODO
				dt_vm_push(vm, dt_val_new_map(mapm));
				break;
			}
			case DT_OP_JMP: {
				int dis = *vm->ip++;
				vm->ip += dis;
				break;
			}
			case DT_OP_JMP_COND: {
				int dis = *vm->ip++;
				dt_val cond = dt_vm_pop(vm);
				bool jump = true;
				if (cond.type != DT_VAL_BOOL) {
					dt_vm_err(
						vm,
						"expected cond to be 'bool' found '%s'\n",
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
				int dis = *vm->ip++;
				vm->ip -= dis;
				break;
			}
			case DT_OP_STOP: {
				return;
			}
		}

	}

}

static dt_scanner dt_scanner_new(char *src) {
	return (dt_scanner) {
		.start = src,
		.cur = src,
		.line = 1,
	};
}

static bool dt_scanner_ended(dt_scanner *s) {
  return *s->cur == '\0';
}

static dt_token dt_scanner_make_token(dt_scanner *s, dt_token_ty type) {
	return (dt_token) {
		.type = type,
		.start = s->start,
		.len = (int)(s->cur - s->start),
		.line = s->line,
	};
}

static bool dt_scanner_match(dt_scanner *s, char expected) {
	if (dt_scanner_ended(s)) {
		return false;
	}
	if (*s->cur != expected) {
		return false;
	}
	s->cur++;
	return true;
}

static char dt_scanner_nxt(dt_scanner *s) {
	s->cur++;
	return s->cur[-1];
}

static char dt_scanner_peek(dt_scanner *s) {
	return *s->cur;
}

static char dt_scanner_peek_nxt(dt_scanner *s) {
	if (dt_scanner_ended(s)) {
		return '\0';
	}
	return s->cur[1];
}

static void dt_scanner_skip_ws(dt_scanner *s) {
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
				// TODO: comment block with ---
				if (dt_scanner_peek_nxt(s) == '-') {
					while (*s->cur != '\n' && !dt_scanner_ended(s)) {
						dt_scanner_nxt(s);
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

static dt_token dt_scanner_scan_str(dt_scanner *s) {

	while (dt_scanner_peek(s) != '"' && !dt_scanner_ended(s)) {
		if (dt_scanner_peek(s) == '\n') {
			s->line++;
		}
		dt_scanner_nxt(s);
	}

	if (dt_scanner_ended(s)) {
		return dt_scanner_make_token(s, DT_TOKEN_ERR);
	}

	// closing "
	dt_scanner_nxt(s);

	return dt_scanner_make_token(s, DT_TOKEN_STR);

}

static dt_token dt_scanner_scan_num(dt_scanner *s) {

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

static bool dt_scanner_check_key(dt_scanner *s, char *key) {
	int len = strlen(key);
	return s->cur - s->start == len && memcmp(s->start, key, len) == 0;
}

static dt_token_ty dt_scanner_ident_type(dt_scanner *s) {
	if        (dt_scanner_check_key(s, "fn")) {
		return DT_TOKEN_FN;
	} else if (dt_scanner_check_key(s, "true")) {
		return DT_TOKEN_TRUE;
	} else if (dt_scanner_check_key(s, "false")) {
		return DT_TOKEN_FALSE;
	} else if (dt_scanner_check_key(s, "false")) {
		return DT_TOKEN_FALSE;
	} else if (dt_scanner_check_key(s, "if")) {
		return DT_TOKEN_IF;
	} else if (dt_scanner_check_key(s, "else")) {
		return DT_TOKEN_ELSE;
	} else if (dt_scanner_check_key(s, "loop")) {
		return DT_TOKEN_LOOP;
	} else if (dt_scanner_check_key(s, "nil")) {
		return DT_TOKEN_NIL;
	} else if (dt_scanner_check_key(s, "return")) {
		return DT_TOKEN_RETURN;
	} else if (dt_scanner_check_key(s, "break")) {
		return DT_TOKEN_BREAK;
	} else if (dt_scanner_check_key(s, "continue")) {
		return DT_TOKEN_CONTINUE;
	} else {
		return DT_TOKEN_IDENT;
	}
}

static dt_token dt_scanner_scan_ident(dt_scanner *s) {

	while (dt_is_alpha(dt_scanner_peek(s)) || dt_is_digit(dt_scanner_peek(s))) {
		dt_scanner_nxt(s);
	}

	return dt_scanner_make_token(s, dt_scanner_ident_type(s));

}

static dt_token dt_scanner_scan(dt_scanner *s) {

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
		case '%': return dt_scanner_make_token(s, DT_TOKEN_PERCENT);
		case '~': return dt_scanner_make_token(s, DT_TOKEN_TILDE);
		case ':': return dt_scanner_make_token(s, DT_TOKEN_COLON);
		case '?': return dt_scanner_make_token(s, DT_TOKEN_QUESTION);
		case '@': return dt_scanner_make_token(s, DT_TOKEN_AT);
		case '^': return dt_scanner_make_token(s, DT_TOKEN_CARET);
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
			if (dt_scanner_match(s, '>')) {
				return dt_scanner_make_token(s, DT_TOKEN_MINUS_GT);
			} else if (dt_scanner_match(s, '=')) {
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
		case '"': return dt_scanner_scan_str(s);
	}

	return dt_scanner_make_token(s, DT_TOKEN_ERR);

}

static char *dt_read_file(char *path, size_t *osize) {

	FILE *file = fopen(path, "r");

	if (!file) {
		return NULL;
	}

	fseek(file, 0L, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buf = malloc(size + 1);
	size_t size_read = fread(buf, sizeof(char), size, file);
	buf[size_read] = '\0';

	if (osize) {
		*osize = size_read;
	}

	fclose(file);

	return buf;

}

static void dt_c_prec(dt_compiler *c, dt_prec prec);
static void dt_c_binary(dt_compiler *c);

static void dt_c_err(dt_compiler *c, char *fmt, ...) {
	dt_token *cur = &c->parser.cur;
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "line #%d: ", cur->line);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

static void dt_c_emit(dt_compiler *c, uint8_t byte) {
	dt_chunk_push(&c->env->chunk, byte, c->parser.prev.line);
}

static void dt_c_emit2(dt_compiler *c, uint8_t b1, uint8_t b2) {
	dt_c_emit(c, b1);
	dt_c_emit(c, b2);
}

static void dt_c_push_const(dt_compiler *c, dt_val val) {
	int idx = dt_chunk_add_const(&c->env->chunk, val);
	dt_c_emit2(c, DT_OP_CONST, idx);
}

static void dt_c_nxt(dt_compiler *c) {
	c->parser.prev = c->parser.cur;
	dt_token t = dt_scanner_scan(&c->scanner);
	c->parser.cur = t;
	if (t.type == DT_TOKEN_ERR) {
		dt_c_err(c, "unexpected token\n");
	}
}

static bool dt_c_match(dt_compiler *c, dt_token_ty ty) {
	if (c->parser.cur.type == ty) {
		dt_c_nxt(c);
		return true;
	}
	return false;
}

static void dt_c_consume(dt_compiler *c, dt_token_ty ty) {
	if (c->parser.cur.type != ty) {
		dt_c_err(c, "expected token '%s'\n", dt_token_name(ty));
		return;
	}
	dt_c_nxt(c);
}

static void dt_c_num(dt_compiler *c) {
	dt_c_consume(c, DT_TOKEN_NUM);
	dt_val num = dt_val_new_num(strtof(c->parser.prev.start, NULL));
	dt_c_push_const(c, num);
}

static void dt_c_str(dt_compiler *c) {
	dt_c_consume(c, DT_TOKEN_STR);
	dt_val str = dt_val_new_str(c->parser.prev.start + 1, c->parser.prev.len - 2);
	dt_c_push_const(c, str);
}

static void dt_c_lit(dt_compiler *c) {
	dt_c_nxt(c);
	switch (c->parser.prev.type) {
		case DT_TOKEN_NIL: dt_c_emit(c, DT_OP_NIL); break;
		case DT_TOKEN_TRUE: dt_c_emit(c, DT_OP_TRUE); break;
		case DT_TOKEN_FALSE: dt_c_emit(c, DT_OP_FALSE); break;
		default: dt_c_err(c, "cannot process as literal\n");
	}
}

static void dt_c_expr(dt_compiler *c) {
	dt_c_prec(c, DT_PREC_ASSIGN);
}

static void dt_c_arr(dt_compiler *c) {

	dt_c_consume(c, DT_TOKEN_LBRACKET);

	int len = 0;

	while (c->parser.cur.type != DT_TOKEN_RBRACKET) {
		dt_c_expr(c);
		len++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RBRACKET);
	dt_c_emit2(c, DT_OP_MKARR, len);

}

static void dt_c_map(dt_compiler *c) {

	dt_c_consume(c, DT_TOKEN_LBRACE);

	int len = 0;

	while (c->parser.cur.type != DT_TOKEN_RBRACE) {
		dt_c_consume(c, DT_TOKEN_IDENT);
		dt_val name = dt_val_new_str(c->parser.prev.start, c->parser.prev.len);
		dt_c_push_const(c, name);
		dt_c_consume(c, DT_TOKEN_COLON);
		dt_c_expr(c);
		len++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RBRACE);
	dt_c_emit2(c, DT_OP_MKMAP, len);

}

static int dt_c_find_local(dt_compiler *c, dt_token *name) {

	dt_funcenv *e = c->env;

	for (int i = e->num_locals - 1; i >= 0; i--) {
		dt_local *val = &e->locals[i];
		if (dt_token_eq(name, &val->name)) {
			return i;
		}
	}

	return -1;

}

static int dt_c_add_upval(dt_compiler *c, int idx, bool local) {

	dt_funcenv *e = c->env;

	// find existing upval
	for (int i = 0; i < e->num_upvals; i++) {
		dt_upval *val = &e->upvals[i];
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

static int dt_c_find_upval(dt_compiler *c, dt_token *name) {

	dt_funcenv *cur_env = c->env;

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

static void dt_c_add_local(dt_compiler *c, dt_token name) {
	if (c->env->num_locals >= UINT8_MAX) {
		dt_c_err(c, "too many local variables in one scope\n");
		return;
	}
	dt_local *l = &c->env->locals[c->env->num_locals++];
	l->name = name;
	l->depth = c->env->depth;
	l->captured = false;
}

static void dt_c_decl(dt_compiler *c) {
	dt_c_consume(c, DT_TOKEN_DOLLAR);
	dt_c_consume(c, DT_TOKEN_IDENT);
	dt_token namet = c->parser.prev;
	if (
		dt_c_find_local(c, &namet) != -1
		|| dt_c_find_upval(c, &namet) != -1)
	{
		dt_c_err(c, "duplicate decl '%.*s'\n", namet.len, namet.start);
	}
	dt_c_consume(c, DT_TOKEN_EQ);
	dt_c_add_local(c, namet);
	dt_c_expr(c);
}

static void dt_c_scope_begin(dt_compiler *c) {
	c->env->depth++;
}

static void dt_c_scope_end(dt_compiler *c) {

	dt_funcenv *e = c->env;

	e->depth--;

	while (e->num_locals > 0) {
		dt_local *l = &e->locals[e->num_locals - 1];
		if (l->depth <= e->depth) {
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

static void dt_c_stmt(dt_compiler *c);

static void dt_c_block(dt_compiler *c) {

	dt_c_consume(c, DT_TOKEN_LBRACE);
	dt_c_scope_begin(c);

	while (c->parser.cur.type != DT_TOKEN_RBRACE) {
		dt_c_stmt(c);
	}

	dt_c_consume(c, DT_TOKEN_RBRACE);
	dt_c_scope_end(c);

}

static void dt_c_cond(dt_compiler *c) {

	dt_c_consume(c, DT_TOKEN_IF);
	dt_c_consume(c, DT_TOKEN_LPAREN);
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RPAREN);
	dt_c_emit2(c, DT_OP_JMP_COND, 0);
	int if_start = c->env->chunk.cnt;
	dt_c_block(c);
	int if_dis = c->env->chunk.cnt - if_start;

	if (dt_c_match(c, DT_TOKEN_ELSE)) {

		// for JMP(1)
		if_dis += 2;
		dt_c_emit2(c, DT_OP_JMP, 0);
		int else_start = c->env->chunk.cnt;

		if (c->parser.cur.type == DT_TOKEN_IF) {
			dt_c_cond(c);
		} else {
			dt_c_block(c);
		}

		int else_dis = c->env->chunk.cnt - else_start;

		if (else_dis >= UINT8_MAX) {
			dt_c_err(c, "jump too large\n");
		}

		c->env->chunk.code[else_start - 1] = else_dis;

	}

	if (if_dis >= UINT8_MAX) {
		dt_c_err(c, "jump too large\n");
	}

	c->env->chunk.code[if_start - 1] = if_dis;

}

// TODO
static void dt_c_loop(dt_compiler *c) {

	dt_c_consume(c, DT_TOKEN_LOOP);
	int start = c->env->chunk.cnt;
	dt_c_block(c);

	int dis = c->env->chunk.cnt - start + 2;

	if (dis >= UINT8_MAX) {
		dt_c_err(c, "jump too large\n");
	}

	dt_c_emit2(c, DT_OP_REWIND, dis);

}

// TODO: DT_OP_JMP after DT_OP_REWIND
static void dt_c_break(dt_compiler *c) {
	dt_c_consume(c, DT_TOKEN_BREAK);
}

// TODO: DT_OP_JMP to DT_OP_REWIND
static void dt_c_continue(dt_compiler *c) {
	dt_c_consume(c, DT_TOKEN_CONTINUE);
}

static void dt_c_return(dt_compiler *c) {
	dt_c_consume(c, DT_TOKEN_MINUS_GT);
	dt_c_expr(c);
	dt_c_emit(c, DT_OP_STOP);
}

static void dt_c_stmt(dt_compiler *c) {

	dt_token_ty t = c->parser.cur.type;

	switch (t) {
		case DT_TOKEN_DOLLAR:   dt_c_decl(c); break;
		case DT_TOKEN_LBRACE:   dt_c_block(c); break;
		case DT_TOKEN_IF:       dt_c_cond(c); break;
		case DT_TOKEN_LOOP:     dt_c_loop(c); break;
		case DT_TOKEN_BREAK:    dt_c_break(c); break;
		case DT_TOKEN_CONTINUE: dt_c_break(c); break;
		case DT_TOKEN_MINUS_GT: dt_c_return(c); break;
		default: {
			dt_c_expr(c);
			dt_c_emit(c, DT_OP_POP);
		};
	}

}

static void dt_c_index(dt_compiler *c) {
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RBRACKET);
	dt_c_emit(c, DT_OP_INDEX);
}

static void dt_c_index2(dt_compiler *c) {
	dt_c_consume(c, DT_TOKEN_IDENT);
	dt_val name = dt_val_new_str(c->parser.prev.start, c->parser.prev.len);
	dt_c_push_const(c, name);
	dt_c_emit(c, DT_OP_INDEX);
}

static void dt_c_call(dt_compiler *c) {

	int nargs = 0;

	while (c->parser.cur.type != DT_TOKEN_RPAREN) {
		dt_c_expr(c);
		nargs++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RPAREN);
	dt_c_emit2(c, DT_OP_CALL, nargs);

}

static void dt_c_ident(dt_compiler *c) {

	dt_c_consume(c, DT_TOKEN_IDENT);
	dt_op set_op;
	dt_op get_op;
	int idx;

	if ((idx = dt_c_find_local(c, &c->parser.prev)) != -1) {
		get_op = DT_OP_GETL;
		set_op = DT_OP_SETL;
	} else if ((idx = dt_c_find_upval(c, &c->parser.prev)) != -1) {
		get_op = DT_OP_GETU;
		set_op = DT_OP_SETU;
	} else {
		get_op = DT_OP_GETG;
		set_op = DT_OP_SETG;
		dt_val name = dt_val_new_str(c->parser.prev.start, c->parser.prev.len);
		idx = dt_chunk_add_const(&c->env->chunk, name);
	}

	// assign
	if (dt_c_match(c, DT_TOKEN_EQ)) {

		dt_c_expr(c);
		dt_c_emit2(c, set_op, idx);

	} else {

		// get
		dt_c_emit2(c, get_op, idx);

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

static void dt_c_group(dt_compiler *c) {
	dt_c_consume(c, DT_TOKEN_LPAREN);
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RPAREN);
}

static void dt_c_unary(dt_compiler *c) {
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
		.depth = 0,
		.locals = {0},
		.num_locals = 0,
		.upvals = {0},
		.num_upvals = 0,
		.chunk = dt_chunk_new(),
	};
}

static void dt_funcenv_free(dt_funcenv *env) {
	dt_chunk_free(&env->chunk);
	memset(env, 0, sizeof(dt_funcenv));
}

dt_compiler dt_compiler_new(char *code) {
	dt_compiler c = (dt_compiler) {
		.scanner = dt_scanner_new(code),
		.parser = {0},
		.base_env = dt_funcenv_new(),
		.env = NULL,
	};
	c.env = &c.base_env;
	return c;
}

void dt_compiler_free(dt_compiler *c) {
	dt_funcenv_free(&c->base_env);
	memset(c, 0, sizeof(dt_compiler));
}

static void dt_c_func(dt_compiler *c) {

	dt_c_consume(c, DT_TOKEN_FN);
	dt_c_consume(c, DT_TOKEN_LPAREN);

	dt_funcenv env = dt_funcenv_new();

	dt_funcenv *prev_env = c->env;
	c->env = &env;
	c->env->parent = prev_env;

	int nargs = 0;

	while (c->parser.cur.type != DT_TOKEN_RPAREN) {
		dt_c_consume(c, DT_TOKEN_IDENT);
		dt_token namet = c->parser.prev;
		dt_c_add_local(c, namet);
		nargs++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RPAREN);
	dt_c_consume(c, DT_TOKEN_LBRACE);

	while (c->parser.cur.type != DT_TOKEN_RBRACE) {
		dt_c_stmt(c);
	}

	dt_c_consume(c, DT_TOKEN_RBRACE);

	dt_c_emit(c, DT_OP_NIL);
	dt_c_emit(c, DT_OP_STOP);

	c->env = prev_env;

	dt_logic *logic = malloc(sizeof(dt_logic));
	logic->nargs = nargs;
	logic->chunk = env.chunk;

	int idx = dt_chunk_add_const(&c->env->chunk, (dt_val) {
		.type = DT_VAL_LOGIC,
		.data = {
			.logic = logic,
		},
	});

	dt_c_emit(c, DT_OP_FUNC);
	dt_c_emit(c, idx);
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
	[DT_TOKEN_PERCENT]       = { NULL,       dt_c_binary, DT_PREC_FACTOR },
	[DT_TOKEN_CARET]         = { NULL,       dt_c_binary, DT_PREC_POWER },
	[DT_TOKEN_HASH]          = { dt_c_unary, NULL,        DT_PREC_NONE },
	[DT_TOKEN_BANG]          = { dt_c_unary, NULL,        DT_PREC_NONE },
	[DT_TOKEN_BANG_EQ]       = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_EQ]            = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_EQ_EQ]         = { NULL,       dt_c_binary, DT_PREC_EQUAL },
	[DT_TOKEN_GT]            = { NULL,       dt_c_binary, DT_PREC_COMPARE },
	[DT_TOKEN_GT_EQ]         = { NULL,       dt_c_binary, DT_PREC_COMPARE },
	[DT_TOKEN_LT]            = { NULL,       dt_c_binary, DT_PREC_COMPARE },
	[DT_TOKEN_LT_EQ]         = { NULL,       dt_c_binary, DT_PREC_COMPARE },
	[DT_TOKEN_LT_LT]         = { NULL,       dt_c_binary, DT_PREC_COMPARE },
	[DT_TOKEN_OR_OR]         = { NULL,       dt_c_binary, DT_PREC_LOGICAL },
	[DT_TOKEN_AND_AND]       = { NULL,       dt_c_binary, DT_PREC_LOGICAL },
	[DT_TOKEN_IDENT]         = { dt_c_ident, NULL,        DT_PREC_NONE },
	[DT_TOKEN_STR]           = { dt_c_str,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_NUM]           = { dt_c_num,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_AND]           = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_TRUE]          = { dt_c_lit,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_FALSE]         = { dt_c_lit,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_NIL]           = { dt_c_lit,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_QUESTION]      = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_OR]            = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_ERR]           = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_END]           = { NULL,       NULL,        DT_PREC_NONE },
	[DT_TOKEN_FN]            = { dt_c_func,  NULL,        DT_PREC_NONE },
};

static void dt_c_prec(dt_compiler *c, dt_prec prec) {
	dt_parse_rule *prev_rule = &dt_rules[c->parser.cur.type];
	if (prev_rule->prefix == NULL) {
		dt_c_err(c, "expected expression\n");
		return;
	}
	prev_rule->prefix(c);
	while (prec <= dt_rules[c->parser.cur.type].prec) {
		dt_c_nxt(c);
		dt_rules[c->parser.prev.type].infix(c);
	}
}

static void dt_c_binary(dt_compiler *c) {

	dt_token_ty ty = c->parser.prev.type;

	dt_parse_rule *rule = &dt_rules[ty];
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
		case DT_TOKEN_PERCENT:
			dt_c_emit(c, DT_OP_MOD);
			break;
		case DT_TOKEN_CARET:
			dt_c_emit(c, DT_OP_POW);
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

void dt_check_nargs(int expected, int actual) {
	if (expected != actual) {
		dt_fail("expects %d args, found %d\n", expected, actual);
	}
}

void dt_check_arg(dt_vm *vm, int pos, dt_val_ty ty) {
	dt_val *val = dt_vm_get(vm, -pos);
	if (val->type != DT_VAL_STR) {
		dt_fail(
			"expected %s at arg %d, found %s",
			dt_type_name(ty),
			pos,
			dt_type_name(val->type)
		);
	}
}

static dt_val dt_f_print(dt_vm *vm, int nargs) {

	for (int i = -nargs + 1; i <= 0; i++) {
		dt_val *val = dt_vm_get(vm, i);
		dt_val_print(val);
		printf(" ");
	}

	printf("\n");

	return dt_nil;

}

// TODO
static dt_val dt_f_error(dt_vm *vm, int nargs) {
	return dt_nil;
}

static dt_val dt_f_type(dt_vm *vm, int nargs) {
	dt_check_nargs(1, nargs);
	dt_val *val = dt_vm_get(vm, 0);
	char *tname = dt_type_name(val->type);
	return dt_val_new_str(tname, strlen(tname));
}

static dt_val dt_f_exit(dt_vm *vm, int nargs) {
	exit(EXIT_SUCCESS);
	return dt_nil;
}

// TODO: return stdout
static dt_val dt_f_exec(dt_vm *vm, int nargs) {
	dt_check_nargs(1, nargs);
	dt_check_arg(vm, 0, DT_VAL_STR);
	char *cmd = dt_vm_get(vm, 0)->data.str.chars;
	system(cmd);
	return dt_nil;
}

static dt_val dt_f_getenv(dt_vm *vm, int nargs) {
	dt_check_nargs(1, nargs);
	dt_check_arg(vm, 0, DT_VAL_STR);
	char *var = dt_vm_get(vm, 0)->data.str.chars;
	char *val = getenv(var);
	if (val) {
		return dt_val_new_str(val, strlen(val));
	} else {
		return dt_nil;
	}
}

static dt_val dt_f_time(dt_vm *vm, int nargs) {
	dt_check_nargs(0, nargs);
	return dt_val_new_num(time(NULL));
}

static dt_val dt_f_eval(dt_vm *vm, int nargs) {
	dt_check_nargs(1, nargs);
	dt_check_arg(vm, 0, DT_VAL_STR);
	char *code = dt_vm_get(vm, 0)->data.str.chars;
	dt_val ret = dt_eval_ex(code, vm->env);
	return ret;
}

static dt_val dt_f_dofile(dt_vm *vm, int nargs) {
	dt_check_nargs(1, nargs);
	dt_check_arg(vm, 0, DT_VAL_STR);
	char *path = dt_vm_get(vm, 0)->data.str.chars;
	dt_val ret = dt_dofile_ex(path, vm->env);
	return ret;
}

static dt_val dt_f_fread(dt_vm *vm, int nargs) {
	dt_check_nargs(1, nargs);
	dt_check_arg(vm, 0, DT_VAL_STR);
	char *path = dt_vm_get(vm, 0)->data.str.chars;
	size_t size;
	char *content = dt_read_file(path, &size);
	return dt_val_new_str(content, size);
}

// TODO
static dt_val dt_f_mod(dt_vm *vm, int nargs) {
	return dt_nil;
}

void dt_load_std(dt_map *env) {
	// TODO: namespacing
	dt_map_set_cfunc(env, "type", dt_f_type);
	dt_map_set_cfunc(env, "eval", dt_f_eval);
	dt_map_set_cfunc(env, "dofile", dt_f_dofile);
	dt_map_set_cfunc(env, "error", dt_f_error);
	dt_map_set_cfunc(env, "mod", dt_f_mod);
	dt_map_set_cfunc(env, "print", dt_f_print);
	dt_map_set_cfunc(env, "exit", dt_f_exit);
	dt_map_set_cfunc(env, "exec", dt_f_exec);
	dt_map_set_cfunc(env, "time", dt_f_time);
	dt_map_set_cfunc(env, "getenv", dt_f_getenv);
	dt_map_set_cfunc(env, "fread", dt_f_fread);
}

dt_val dt_eval_ex(char *code, dt_map *env) {

	dt_vm vm = dt_vm_new();
	vm.env = env;
	dt_compiler c = dt_compiler_new(code);
	dt_c_nxt(&c);

	while (c.parser.cur.type != DT_TOKEN_END) {
		dt_c_stmt(&c);
	}

	dt_c_emit(&c, DT_OP_NIL);
	dt_c_emit(&c, DT_OP_STOP);

	// TODO: clean?
	dt_func func = (dt_func) {
		.logic = &(dt_logic) {
			.chunk = c.env->chunk,
			.nargs = 0,
		},
		.upvals = NULL,
	};

	dt_vm_run(&vm, &func);

	return *dt_vm_get(&vm, 0);

}

dt_val dt_eval(char *code) {
	dt_map env = dt_map_new();
	dt_load_std(&env);
	dt_val ret = dt_eval_ex(code, &env);
	dt_map_free(&env);
	return ret;
}

dt_val dt_dofile_ex(char *path, dt_map *env) {

	char *code = dt_read_file(path, NULL);

	if (!code) {
		fprintf(stderr, "failed to read '%s'\n", path);
		return dt_nil;
	}

	dt_val ret = dt_eval_ex(code, env);
	free(code);

	return ret;

}

dt_val dt_dofile(char *path) {
	dt_map env = dt_map_new();
	dt_load_std(&env);
	dt_val ret = dt_dofile_ex(path, &env);
	dt_map_free(&env);
	return ret;
}

#endif
#endif
