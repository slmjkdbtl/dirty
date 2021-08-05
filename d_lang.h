// wengwengweng

// see demo/*.dt

// TODO: ... to access func args
// TODO: 'this'
// TODO: trait
// TODO: module
// TODO: threading
// TODO: utf8
// TODO: named func
// TODO: expose tokenizer
// TODO: standalone bytecode
// TODO: type
// TODO: string interpolation
// TODO: single ctx handle
// TODO: tail call opti
// TODO: loop return val
// TODO: switch
// TODO: str intern
// TODO: chained comparison
// TODO: iter with index
// TODO: separate debug print / actual print
// TODO: ordered map ?
// TODO: disallow shadowing global var
// TODO: member diconstructing
// TODO: combine GT/LT and EQ
// TODO: '?' for cond? what for nil?
// TODO: assignment in cond
// TODO: global on root scope?
// TODO: windows fs
// TODO: load dyn lib
// TODO: hot reload
// TODO: separate compiler and vm
// TODO: don't rely on emscripten
// TODO: cli arg for log
// TODO: try func table instead of switch
// TODO: require ! postfix for mut funcs

#ifndef D_LANG_H
#define D_LANG_H

#define DT_STACK_MAX 2048
#define DT_ARR_INIT_SIZE 4
#define DT_MAP_INIT_SIZE 8
#define DT_MAP_MAX_LOAD 0.75
#define DT_PI 3.14

// #define DT_NO_NANBOX
// #define DT_VM_LOG
// #define DT_GC_LOG

#define DT_LIB_SYS
#define DT_LIB_FS
#define DT_LIB_TERM
#define DT_LIB_HTTP

#if defined(__APPLE__)
	#include <TargetConditionals.h>
	#if TARGET_OS_OSX
		#define DT_MACOS
	#elif TARGET_OS_IOS
		#define DT_IOS
	#endif
#elif defined(__EMSCRIPTEN__)
	#define DT_WEB
#elif defined(_WIN32) || defined(_WIN64)
	#define DT_WINDOWS
#elif defined(__ANDROID__)
	#define DT_ANDROID
#elif defined(__linux__) || defined(__unix__)
	#define DT_LINUX
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <poll.h>
#include <sys/stat.h>

// value types
typedef enum {
	DT_VAL_NIL,
	DT_VAL_BOOL,
	DT_VAL_NUM,
	DT_VAL_STR,
	DT_VAL_PSTR,
	DT_VAL_ARR,
	DT_VAL_MAP,
	DT_VAL_RANGE,
	DT_VAL_FUNC,
	DT_VAL_STRUCT,
	DT_VAL_BYTES,
	DT_VAL_CFUNC,
	DT_VAL_CDATA,
	DT_VAL_CPTR,
	// TODO
	DT_VAL_UPVAL,
	// TODO
	DT_VAL_LOGIC,
} dt_val_ty;

#define DT_HEAPER_STRUCT \
	uint32_t rc; \
	uint8_t type;

typedef double dt_num;
typedef bool dt_bool;
struct dt_map;
struct dt_arr;
struct dt_str;
struct dt_vm;
struct dt_logic;
struct dt_func;
struct dt_upval;
struct dt_cdata;

// for easy loops and sub sections in lists
typedef struct {
	int16_t start;
	int16_t end;
} dt_range;

// packed string
typedef struct {
	uint8_t len;
	char chars[5];
} dt_pstr;

#ifdef DT_NO_NANBOX

struct dt_val;
typedef struct dt_val (dt_cfunc)(struct dt_vm* vm);

typedef struct dt_val {
	dt_val_ty type;
	union {
		dt_num           num;
		dt_bool          boolean;
		dt_range         range;
		dt_pstr          pstr;
		struct dt_str*   str;
		struct dt_arr*   arr;
		struct dt_map*   map;
		struct dt_func*  func;
		struct dt_logic* logic;
		dt_cfunc*        cfunc;
		struct dt_cdata* cdata;
		struct dt_cptr*  cptr;
		struct dt_upval* upval;
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

// IEEE 754 "double" bit segments
#define DT_SMASK_SIGN  0x8000000000000000
#define DT_SMASK_EXPO  0x7ff0000000000000
#define DT_SMASK_QUIET 0x0008000000000000
#define DT_SMASK_SIG   0xffff000000000000
#define DT_SMASK_PTR   0x0000ffffffffffff
#define DT_SMASK_RANGE 0x00000000ffffffff
#define DT_SMASK_PSTR  0x0000ffffffffffff

// value type
#define DT_TMASK_NAN    0x0000000000000000
#define DT_TMASK_FALSE  0x0001000000000000
#define DT_TMASK_TRUE   0x0002000000000000
#define DT_TMASK_NIL    0x0003000000000000
#define DT_TMASK_RANGE  0x0004000000000000
#define DT_TMASK_PSTR   0x0005000000000000
#define DT_TMASK_HEAP   0x0006000000000000
#define DT_TMASK_STRUCT 0x0007000000000000
#define DT_TMASK_CFUNC  0x8000000000000000
#define DT_TMASK_CDATA  0x8001000000000000
#define DT_TMASK_CPTR   0x8002000000000000
#define DT_TMASK_LOGIC  0x8007000000000000

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
#define DT_SIG_PSTR    (DT_NAN | DT_TMASK_PSTR)
#define DT_SIG_HEAP    (DT_NAN | DT_TMASK_HEAP)
#define DT_SIG_LOGIC   (DT_NAN | DT_TMASK_LOGIC)
#define DT_SIG_CFUNC   (DT_NAN | DT_TMASK_CFUNC)
#define DT_SIG_CDATA   (DT_NAN | DT_TMASK_CDATA)
#define DT_SIG_CPTR    (DT_NAN | DT_TMASK_CPTR)

typedef dt_val (dt_cfunc)(struct dt_vm* vm);

#endif

// Arr<Map<Num>>
// type = DT_VAL_ARR
// idx = 0
// nargs = 1

// TODO
typedef struct dt_type {
	dt_val_ty type;
	// struct / func idx in compiler
	uint16_t idx;
} dt_type;

// struct node {
// 	bool has_child;
// 	union {
// 		struct node child;
// 		char empty;
// 	} data;
// };

dt_type DT_TYPE_NIL = (dt_type) {
	.type = DT_VAL_NIL,
};

dt_type DT_TYPE_BOOL = (dt_type) {
	.type = DT_VAL_BOOL,
};

dt_type DT_TYPE_RANGE = (dt_type) {
	.type = DT_VAL_RANGE,
};

dt_type DT_TYPE_STR = (dt_type) {
	.type = DT_VAL_STR,
};

dt_type DT_TYPE_NUM = (dt_type) {
	.type = DT_VAL_NUM,
};

typedef struct {
	dt_type type;
} dt_arr_def;

typedef struct {
	dt_type type;
} dt_map_def;

// TODO
typedef struct {
	char name[16];
	int type;
} dt_struct_mem;

typedef struct {
	char name[16];
	char mem_names[16][16];
	dt_type mems[16];
	int num_mems;
} dt_struct_def;

// TODO
typedef struct {
	char name[16];
	dt_type ret;
	dt_type args[8];
	int num_args;
} dt_func_def;

typedef struct dt_heaper {
	DT_HEAPER_STRUCT
} dt_heaper;

typedef struct dt_cdata {
	DT_HEAPER_STRUCT
	uint32_t size;
	uint8_t  data[];
} dt_cdata;

// upvalue (runtime)
// - captured: already allocated to the heap
// - not captured: points to a stack slot, must lift when it gets popped
typedef struct {
	DT_HEAPER_STRUCT
	bool    captured;
	dt_val* val;
} dt_upval;

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
} dt_map_entry;

// hashmap
typedef struct dt_map {
	DT_HEAPER_STRUCT
	uint32_t  cnt;
	uint32_t  cap;
	dt_map_entry* entries;
} dt_map;

typedef struct {
	dt_map*  map;
	uint32_t idx;
} dt_map_iter;

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
	DT_HEAPER_STRUCT
	dt_chunk chunk;
	uint16_t nargs;
} dt_logic;

// function
typedef struct dt_func {
	DT_HEAPER_STRUCT
	uint8_t     num_upvals;
	dt_logic*   logic;
	dt_upval**  upvals;
} dt_func;

typedef struct {
	int line;
	char* msg;
} dt_err;

// the runtime virtual machine
typedef struct dt_vm {
	dt_func*   func;
	uint8_t*   ip;
	dt_val     stack[DT_STACK_MAX];
	dt_val*    stack_top;
	dt_val*    stack_bot;
	dt_map*    globals;
	dt_map*    libs;
	dt_map*    map_lib;
	dt_map*    arr_lib;
	dt_map*    str_lib;
	dt_map*    func_lib;
	dt_map*    math_lib;
	dt_map*    strs;
	dt_upval*  open_upvals[UINT8_MAX];
	int        num_upvals;
	size_t     mem;
	dt_func*   trying;
	bool       throwing;
	dt_err     err;
} dt_vm;

typedef struct {
	char*  name;
	dt_val val;
} dt_map_centry;

// op codes
typedef enum {
	DT_OP_STOP,
	DT_OP_THROW,
	DT_OP_CONST,
	DT_OP_NIL,
	DT_OP_TRUE,
	DT_OP_FALSE,
	DT_OP_ADD,
	DT_OP_SUB,
	DT_OP_MUL,
	DT_OP_DIV,
	DT_OP_NEG,
	DT_OP_NOT,
	DT_OP_EQ,
	DT_OP_GT,
	DT_OP_GT_EQ,
	DT_OP_LT,
	DT_OP_LT_EQ,
	DT_OP_OR,
	DT_OP_NIL_OR,
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
	DT_OP_CALL2,
	DT_OP_TRY_PREP,
	DT_OP_TRY,
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
	DT_TOKEN_QUESTION_Q, // ??
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
	dt_type type;
} dt_c_local;

// compile time upval representation
typedef struct {
	int idx;
	bool local;
} dt_c_upval;

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
	dt_chunk           chunk;
	int                cur_depth;
	dt_block_ty        scopes[UINT8_MAX];
	dt_c_local         locals[UINT8_MAX];
	int                num_locals;
	dt_c_upval         upvals[UINT8_MAX];
	int                num_upvals;
	dt_break           breaks[UINT8_MAX];
	int                num_breaks;
} dt_funcenv;

typedef struct {
	dt_scanner scanner;
	dt_parser parser;
	dt_funcenv base_env;
	dt_funcenv* env;
} dt_compiler;

typedef dt_type (*dt_parse_fn)(dt_compiler* compiler);
typedef dt_type (*dt_parse_fn2)(dt_compiler* compiler, dt_type prev_ty);

typedef struct {
	dt_parse_fn prefix;
	dt_parse_fn2 infix;
	dt_prec prec;
} dt_parse_rule;

// vm creation / destruction
dt_vm     dt_vm_new    ();
void      dt_vm_free   (dt_vm* vm);

// running code
dt_val    dt_eval        (dt_vm* vm, char* src);
dt_val    dt_dofile      (dt_vm* vm, char* path);

// throw runtime error
void      dt_throw       (dt_vm* vm, char* fmt, ...);

// for getting args in cfunc
int       dt_nargs            (dt_vm* vm);
bool      dt_arg_exists       (dt_vm* vm, int idx);
dt_val    dt_arg              (dt_vm* vm, int idx);
dt_val    dt_arg_or           (dt_vm* vm, int idx, dt_val v);
dt_num    dt_arg_num          (dt_vm* vm, int idx);
dt_num    dt_arg_num_or       (dt_vm* vm, int idx, dt_num n);
dt_bool   dt_arg_bool         (dt_vm* vm, int idx);
dt_bool   dt_arg_bool_or      (dt_vm* vm, int idx, dt_bool b);
dt_str*   dt_arg_str          (dt_vm* vm, int idx);
dt_str*   dt_arg_str_or       (dt_vm* vm, int idx, dt_str* str);
char*     dt_arg_cstr         (dt_vm* vm, int idx);
char*     dt_arg_cstr_dup     (dt_vm* vm, int idx);
char*     dt_arg_cstr_or      (dt_vm* vm, int idx, char* str);
char*     dt_arg_cstr_or_dup  (dt_vm* vm, int idx, char* str);
dt_map*   dt_arg_map          (dt_vm* vm, int idx);
dt_arr*   dt_arg_arr          (dt_vm* vm, int idx);
dt_func*  dt_arg_func         (dt_vm* vm, int idx);
bool      dt_check_args       (dt_vm* vm, int nargs, ...);

// calling a dt_func
dt_val    dt_call      (dt_vm* vm, dt_val func, int nargs, ...);

// value conversions
dt_val    dt_to_num      (dt_num n);
dt_val    dt_to_bool     (dt_bool b);
dt_val    dt_to_range    (dt_range r);
dt_val    dt_to_str      (dt_str* str);
dt_val    dt_to_arr      (dt_arr* arr);
dt_val    dt_to_map      (dt_map* map);
dt_val    dt_to_func     (dt_func* func);
dt_val    dt_to_cfunc    (dt_cfunc* func);
dt_num    dt_as_num      (dt_vm* vm, dt_val v);
dt_num    dt_as_num_or   (dt_vm* vm, dt_val v, dt_num n);
dt_bool   dt_as_bool     (dt_vm* vm, dt_val v);
dt_bool   dt_as_bool_or  (dt_vm* vm, dt_val v, dt_bool b);
dt_range  dt_as_range    (dt_vm* vm, dt_val v);
dt_str*   dt_as_str      (dt_vm* vm, dt_val v);
dt_str*   dt_as_str_or   (dt_vm* vm, dt_val v, dt_str* new);
dt_arr*   dt_as_arr      (dt_vm* vm, dt_val v);
dt_arr*   dt_as_arr_or   (dt_vm* vm, dt_val v, dt_arr* new);
dt_map*   dt_as_map      (dt_vm* vm, dt_val v);
dt_map*   dt_as_map_or   (dt_vm* vm, dt_val v, dt_map* new);
dt_func*  dt_as_func     (dt_vm* vm, dt_val v);
dt_cfunc* dt_as_cfunc    (dt_vm* vm, dt_val v);
dt_num    dt_as_num2     (dt_val v);
dt_bool   dt_as_bool2    (dt_val v);
dt_range  dt_as_range2   (dt_val v);
dt_str*   dt_as_str2     (dt_val v);
dt_arr*   dt_as_arr2     (dt_val v);
dt_map*   dt_as_map2     (dt_val v);
dt_func*  dt_as_func2    (dt_val v);
dt_cfunc* dt_as_cfunc2   (dt_val v);

// type checking
dt_val_ty dt_typeof      (dt_val v);
char*     dt_typename    (dt_val_ty ty);
bool      dt_is_nil      (dt_val v);
bool      dt_is_num      (dt_val v);
bool      dt_is_bool     (dt_val v);
bool      dt_is_str      (dt_val v);
bool      dt_is_map      (dt_val v);
bool      dt_is_arr      (dt_val v);
bool      dt_is_func     (dt_val v);
bool      dt_is_cfunc    (dt_val v);
bool      dt_typecheck   (dt_vm* vm, dt_val v, dt_val_ty ty);
bool      dt_typecheck_opt (dt_vm* vm, dt_val v, dt_val_ty ty);

// misc value related
bool      dt_val_eq      (dt_val a, dt_val b);
bool      dt_val_lt      (dt_val a, dt_val b);
dt_val    dt_val_clone   (dt_vm* vm, dt_val v);
bool      dt_val_truthy  (dt_val v);
void      dt_val_print   (dt_val v);
void      dt_val_println (dt_val v);

// allocate empty str with len (need to manually fill and hash afterwards or the
// content is undefined)
dt_str*  dt_str_alloc    (dt_vm* vm, int len);
dt_str*  dt_str_fmt      (char* fmt, ...);
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
dt_arr*  dt_str_chars    (dt_vm* vm, dt_str* src);
dt_arr*  dt_str_split    (dt_vm* vm, dt_str* str, dt_str* sep);
dt_str*  dt_str_rep      (dt_vm* vm, dt_str* str, int times);
dt_str*  dt_str_toupper  (dt_vm* vm, dt_str* str);
dt_str*  dt_str_tolower  (dt_vm* vm, dt_str* str);
int      dt_str_find     (dt_str* str, dt_str* key);
bool     dt_str_contains (dt_str* str, dt_str* key);
dt_str*  dt_str_rev      (dt_vm* vm, dt_str* str);
dt_map*  dt_str_match    (dt_vm* vm, dt_str* str, dt_str* pat);
dt_str*  dt_str_trim     (dt_vm* vm, dt_str* str);
dt_str*  dt_str_trunc    (dt_vm* vm, dt_str* str, int len, dt_str* rest);
dt_str*  dt_str_sub      (dt_vm* vm, dt_str* str, int a, int b);
dt_str*  dt_str_get      (dt_vm* vm, dt_str* str, int idx);
int      dt_str_cmp      (dt_str* a, dt_str* b);
void     dt_str_print    (dt_str* str);
void     dt_str_println  (dt_str* str);

// create empty array
dt_arr*  dt_arr_new      (dt_vm* vm);
// create array with capacity
dt_arr*  dt_arr_new_len  (dt_vm* vm, int len);
void     dt_arr_free     (dt_vm* vm, dt_arr* arr);
void     dt_arr_free_rec (dt_vm* vm, dt_arr* arr);
dt_val   dt_arr_get      (dt_arr* arr, int idx);
void     dt_arr_set      (dt_vm* vm, dt_arr* arr, int idx, dt_val val);
void     dt_arr_insert   (dt_vm* vm, dt_arr* arr, int idx, dt_val val);
void     dt_arr_push     (dt_vm* vm, dt_arr* arr, dt_val val);
dt_val   dt_arr_rm       (dt_arr* arr, int idx);
void     dt_arr_rm_all   (dt_arr* arr, dt_val v);
int      dt_arr_find     (dt_arr* arr, dt_val v);
dt_arr*  dt_arr_map      (dt_vm* vm, dt_arr* arr, dt_val f);
dt_arr*  dt_arr_sort     (dt_vm* vm, dt_arr* arr, dt_val f);
dt_arr*  dt_arr_filter   (dt_vm* vm, dt_arr* arr, dt_val f);
void     dt_arr_each     (dt_vm* vm, dt_arr* arr, dt_val f);
dt_val   dt_arr_reduce   (dt_vm* vm, dt_arr* arr, dt_val init, dt_val f);
bool     dt_arr_contains (dt_arr* arr, dt_val v);
dt_str*  dt_arr_join     (dt_vm* vm, dt_arr* arr, dt_str* s);
dt_arr*  dt_arr_dedup    (dt_vm* vm, dt_arr* arr);
dt_arr*  dt_arr_rev      (dt_vm* vm, dt_arr* arr);
dt_val   dt_arr_pop      (dt_arr* arr);
dt_val   dt_arr_pop      (dt_arr* arr);
dt_arr*  dt_arr_sub      (dt_vm* vm, dt_arr* arr, int start, int end);
dt_arr*  dt_arr_concat   (dt_vm* vm, dt_arr* a1, dt_arr* a2);
dt_val   dt_arr_rand     (dt_arr* arr);
dt_arr*  dt_arr_clone    (dt_vm* vm, dt_arr* src);
bool     dt_arr_eq       (dt_arr* a1, dt_arr* a2);
void     dt_arr_print    (dt_arr* arr);
void     dt_arr_println  (dt_arr* arr);

// create empty hashmap
dt_map*  dt_map_new      (dt_vm* vm);
// create hashmap with capacity
dt_map*  dt_map_new_len    (dt_vm* vm, int len);
dt_map*  dt_map_new_reg    (dt_vm* vm, dt_map_centry* table);
void     dt_map_free       (dt_vm* vm, dt_map* map);
void     dt_map_free_rec   (dt_vm* vm, dt_map* map);
dt_val   dt_map_get        (dt_map* map, dt_str* key);
void     dt_map_set        (dt_vm* vm, dt_map* map, dt_str* key, dt_val val);
void     dt_map_each       (dt_vm* vm, dt_map* map, dt_val f);
// get / set from c str
dt_val   dt_map_cget       (dt_vm* vm, dt_map* map, char* key);
void     dt_map_cset       (dt_vm* vm, dt_map* map, char* key, dt_val val);
void     dt_map_reg        (dt_vm* vm, dt_map* map, dt_map_centry* table);
bool     dt_map_has        (dt_map* map, dt_str* key);
bool     dt_map_chas       (dt_vm* vm, dt_map* map, char* key);
dt_val   dt_map_rm         (dt_map* map, dt_str* key);
// get an array of keys
dt_arr*  dt_map_keys       (dt_vm* vm, dt_map* map);
// get an array of values
dt_arr*  dt_map_vals       (dt_vm* vm, dt_map* map);
dt_map*  dt_map_clone      (dt_vm* vm, dt_map* src);
bool     dt_map_eq         (dt_map* a1, dt_map* a2);
void     dt_map_print      (dt_map* map);
void     dt_map_println    (dt_map* map);

dt_map_iter   dt_map_iter_new   (dt_map* map);
dt_map_entry* dt_map_iter_nxt   (dt_map_iter* iter);

dt_cdata* dt_cdata_new     (dt_vm* vm, void* data, size_t size);
void      dt_cdata_free    (dt_vm* vm, dt_cdata* cdata);

#endif

#ifdef D_IMPL
#define D_LANG_IMPL
#endif

#ifdef D_LANG_IMPL
#ifndef D_LANG_IMPL_ONCE
#define D_LANG_IMPL_ONCE

// unrecoverable error
void dt_fail(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

// mem ops with total mem bookkeeping
void* dt_malloc(dt_vm* vm, size_t size) {
	if (vm) {
		vm->mem += size;
	}
	return malloc(size);
}

void* dt_realloc(dt_vm* vm, void* ptr, size_t old_size, size_t new_size) {
	if (vm) {
		vm->mem = vm->mem - old_size + new_size;
	}
	return realloc(ptr, new_size);
}

void dt_free(dt_vm* vm, void* ptr, size_t size) {
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
		case DT_TOKEN_QUESTION_Q: return "QUESTION_QUESTION";
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

bool dt_token_eq(dt_token* t1, dt_token* t2) {
	if (t1->len != t2->len) {
		return false;
	}
	return memcmp(t1->start, t2->start, t1->len) == 0;
}

dt_val_ty dt_typeof(dt_val v) {

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
		case DT_SIG_PSTR:  return DT_VAL_PSTR;
		case DT_SIG_LOGIC: return DT_VAL_LOGIC;
		case DT_SIG_CFUNC: return DT_VAL_CFUNC;
		case DT_SIG_HEAP:  return ((dt_heaper*)(v & DT_SMASK_PTR))->type;
	}

	return DT_VAL_NIL;
#endif

}

bool dt_typecheck(dt_vm* vm, dt_val v, dt_val_ty expected) {
	dt_val_ty actual = dt_typeof(v);
	if (actual != expected) {
		dt_throw(
			vm,
			"expected a '%s', found '%s'",
			dt_typename(expected),
			dt_typename(actual)
		);
		return false;
	}
	return true;
}

bool dt_typecheck_opt(dt_vm* vm, dt_val v, dt_val_ty expected) {
	dt_val_ty actual = dt_typeof(v);
	if (actual != expected && actual != DT_VAL_NIL) {
		dt_throw(
			vm,
			"expected a '%s', found '%s'",
			dt_typename(expected),
			dt_typename(actual)
		);
		return false;
	}
	return true;
}

char* dt_typename(dt_val_ty ty) {
	switch (ty) {
		case DT_VAL_NIL: return "nil";
		case DT_VAL_BOOL: return "bool";
		case DT_VAL_NUM: return "num";
		case DT_VAL_STR: return "str";
		case DT_VAL_PSTR: return "str";
		case DT_VAL_ARR: return "arr";
		case DT_VAL_MAP: return "map";
		case DT_VAL_RANGE: return "range";
		case DT_VAL_STRUCT: return "struct";
		case DT_VAL_FUNC: return "func";
		case DT_VAL_BYTES: return "bytes";
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

dt_num dt_as_num2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.num;
#else
	return *(dt_num*)(&v);
#endif
}

void dt_assert_ty(dt_vm* vm, dt_val_ty expected, dt_val_ty actual) {
	if (expected != actual) {
		dt_throw(
			vm,
			"expected '%s', found '%s'",
			dt_typename(expected),
			dt_typename(actual)
		);
	}
}

dt_num dt_as_num(dt_vm* vm, dt_val v) {
	if (dt_is_num(v)) {
		return dt_as_num2(v);
	} else {
		dt_typecheck(vm, v, DT_VAL_NUM);
		return 0;
	}
}

dt_num dt_as_num_or(dt_vm* vm, dt_val v, dt_num n) {
	if (dt_is_nil(v)) {
		return n;
	} else {
		return dt_as_num(vm, v);
	}
}

dt_bool dt_as_bool(dt_vm* vm, dt_val v) {
	if (dt_is_bool(v)) {
		return dt_as_bool2(v);
	} else {
		dt_typecheck(vm, v, DT_VAL_BOOL);
		return false;
	}
}

dt_bool dt_as_bool_or(dt_vm* vm, dt_val v, dt_bool n) {
	if (dt_is_nil(v)) {
		return n;
	} else {
		return dt_as_bool(vm, v);
	}
}

dt_str* dt_as_str(dt_vm* vm, dt_val v) {
	if (dt_is_str(v)) {
		return dt_as_str2(v);
	} else {
		dt_typecheck(vm, v, DT_VAL_STR);
		return NULL;
	}
}

dt_str* dt_as_str_or(dt_vm* vm, dt_val v, dt_str* s) {
	if (dt_is_nil(v)) {
		return s;
	} else {
		return dt_as_str(vm, v);
	}
}

dt_arr* dt_as_arr(dt_vm* vm, dt_val v) {
	if (dt_is_arr(v)) {
		return dt_as_arr2(v);
	} else {
		dt_typecheck(vm, v, DT_VAL_ARR);
		return NULL;
	}
}

dt_arr* dt_as_arr_or(dt_vm* vm, dt_val v, dt_arr* s) {
	if (dt_is_nil(v)) {
		return s;
	} else {
		return dt_as_arr(vm, v);
	}
}

dt_map* dt_as_map(dt_vm* vm, dt_val v) {
	if (dt_is_map(v)) {
		return dt_as_map2(v);
	} else {
		dt_typecheck(vm, v, DT_VAL_MAP);
		return NULL;
	}
}

dt_map* dt_as_map_or(dt_vm* vm, dt_val v, dt_map* s) {
	if (dt_is_nil(v)) {
		return s;
	} else {
		return dt_as_map(vm, v);
	}
}

dt_val dt_to_bool(dt_bool b) {
	return b ? DT_TRUE : DT_FALSE;
}

dt_bool dt_as_bool2(dt_val v) {
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

dt_range dt_as_range2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.range;
#else
	uint32_t n = v & DT_SMASK_RANGE;
	return (dt_range) {
		.start = n >> 16,
		.end = n & 0xffff,
	};
#endif
}

// bool dt_is_big() {
// 	uint32_t check_number = 0x11223344;
// 	char first_byte = *(char*)(&check_number);
// 	return first_byte == 0x11;
// }

// dt_val dt_to_pstr(dt_pstr s) {
// #ifdef DT_NO_NANBOX
// 	return (dt_val) {
// 		.type = DT_VAL_PSTR,
// 		.data.pstr = s,
// 	};
// #else
// 	dt_val v = DT_SIG_PSTR;
// 	char* buf = (char*)&v;
// 	if (dt_is_big()) {
// 		buf[2] = s.chars[0];
// 		buf[3] = s.chars[1];
// 		buf[4] = s.chars[2];
// 		buf[5] = s.chars[3];
// 		buf[6] = s.chars[4];
// 	} else {
// 		buf[0] = s.chars[0];
// 		buf[1] = s.chars[1];
// 		buf[2] = s.chars[2];
// 		buf[3] = s.chars[3];
// 		buf[4] = s.chars[4];
// 	}
// 	return v;
// #endif
// }

#ifndef DT_NO_NANBOX
void* dt_as_ptr2(dt_val v) {
	return (void*)(v & DT_SMASK_PTR);
}

dt_val dt_to_heap(void* ptr) {
	if (!ptr) {
		return DT_NIL;
	}
	return DT_SIG_HEAP | (uint64_t)ptr;
}
#endif

dt_heaper* dt_as_heaper(dt_val v) {
#ifdef DT_NO_NANBOX
	switch (dt_typeof(v)) {
		case DT_VAL_STR: return (dt_heaper*)v.data.str;
		case DT_VAL_ARR: return (dt_heaper*)v.data.arr;
		case DT_VAL_MAP: return (dt_heaper*)v.data.map;
		case DT_VAL_CDATA: return (dt_heaper*)v.data.cdata;
		case DT_VAL_FUNC: return (dt_heaper*)v.data.func;
		case DT_VAL_UPVAL: return (dt_heaper*)v.data.upval;
		case DT_VAL_LOGIC: return (dt_heaper*)v.data.logic;
		default: return NULL;
	}
#else
	return dt_as_ptr2(v);
#endif
}

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

dt_str* dt_as_str2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.str;
#else
	return (dt_str*)dt_as_ptr2(v);
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

dt_arr* dt_as_arr2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.arr;
#else
	return (dt_arr*)dt_as_ptr2(v);
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

dt_map* dt_as_map2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.map;
#else
	return (dt_map*)dt_as_ptr2(v);
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

dt_func* dt_as_func2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.func;
#else
	return (dt_func*)dt_as_ptr2(v);
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

dt_cfunc* dt_as_cfunc2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.cfunc;
#else
	return (dt_cfunc*)dt_as_ptr2(v);
#endif
}

dt_val dt_to_cdata(dt_cdata* cdata) {
	if (!cdata) {
		return DT_NIL;
	}
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_CDATA,
		.data.cdata = cdata,
	};
#else
	return DT_SIG_CDATA | (uint64_t)cdata;
#endif
}

dt_cdata* dt_as_cdata2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.cdata;
#else
	return (dt_cdata*)dt_as_ptr2(v);
#endif
}

dt_val dt_to_cptr(void* cptr) {
	if (!cptr) {
		return DT_NIL;
	}
#ifdef DT_NO_NANBOX
	return (dt_val) {
		.type = DT_VAL_CPTR,
		.data.cptr = cptr,
	};
#else
	return DT_SIG_CPTR | (uint64_t)cptr;
#endif
}

void* dt_as_cptr2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.cptr;
#else
	return (void*)dt_as_ptr2(v);
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

dt_logic* dt_as_logic2(dt_val v) {
#ifdef DT_NO_NANBOX
	return v.data.logic;
#else
	return (dt_logic*)dt_as_ptr2(v);
#endif
}

// if the value is a heap ptr / gc managed
bool dt_is_heap(dt_val v) {
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
	return dt_typeof(val) == DT_VAL_NIL;
}

bool dt_is_num(dt_val val) {
	return dt_typeof(val) == DT_VAL_NUM;
}

bool dt_is_bool(dt_val val) {
	return dt_typeof(val) == DT_VAL_BOOL;
}

bool dt_is_str(dt_val val) {
	return dt_typeof(val) == DT_VAL_STR;
}

bool dt_is_map(dt_val val) {
	return dt_typeof(val) == DT_VAL_MAP;
}

bool dt_is_arr(dt_val val) {
	return dt_typeof(val) == DT_VAL_ARR;
}

bool dt_is_func(dt_val val) {
	return dt_typeof(val) == DT_VAL_FUNC;
}

bool dt_is_cfunc(dt_val val) {
	return dt_typeof(val) == DT_VAL_CFUNC;
}

// TODO: move around
void dt_func_print(dt_func* func);
void dt_cdata_print(dt_cdata* cdata);

void dt_range_print(dt_range r) {
	printf("%d..%d", r.start, r.end);
}

void dt_range_println(dt_range r) {
	dt_range_print(r);
	printf("\n");
}

void dt_val_print(dt_val v) {
	switch (dt_typeof(v)) {
		case DT_VAL_NIL: printf("<nil>"); break;
		case DT_VAL_BOOL: printf(dt_as_bool2(v) ? "true" : "false"); break;
		case DT_VAL_NUM: printf("%.12g", dt_as_num2(v)); break;
		case DT_VAL_STR: dt_str_print(dt_as_str2(v)); break;
		case DT_VAL_ARR: dt_arr_print(dt_as_arr2(v)); break;
		case DT_VAL_MAP: dt_map_print(dt_as_map2(v)); break;
		case DT_VAL_RANGE: dt_range_print(dt_as_range2(v)); break;
		case DT_VAL_FUNC: dt_func_print(dt_as_func2(v)); break;
		case DT_VAL_CFUNC: printf("<cfunc#%p>", (void*)dt_as_cfunc2(v)); break;
		case DT_VAL_CDATA: dt_cdata_print(dt_as_cdata2(v)); break;
		case DT_VAL_CPTR: printf("<cptr#%p>", dt_as_cptr2(v)); break;
		case DT_VAL_UPVAL: printf("<upval>"); break;
		case DT_VAL_LOGIC: printf("<logic>"); break;
		default: printf("<unknown>"); break;
	}
}

void dt_val_println(dt_val val) {
	dt_val_print(val);
	printf("\n");
}

bool dt_val_eq(dt_val a, dt_val b) {
	dt_val_ty ta = dt_typeof(a);
	dt_val_ty tb = dt_typeof(b);
#ifdef DT_NO_NANBOX
	if (ta != tb) {
		return false;
	} else if (ta == DT_VAL_NUM && tb == DT_VAL_NUM) {
		return dt_as_num2(a) == dt_as_num2(b);
	} else if (ta == DT_VAL_BOOL && tb == DT_VAL_BOOL) {
		return dt_as_bool2(a) == dt_as_bool2(b);
	} else if (ta == DT_VAL_NIL && tb == DT_VAL_NIL) {
		return true;
	} else if (ta == DT_VAL_STR && tb == DT_VAL_STR) {
		return dt_str_eq(dt_as_str2(a), dt_as_str2(b));
	} else {
		return false;
	}
#else
	// TODO: no this with str interning
	if (ta == DT_VAL_STR && tb == DT_VAL_STR) {
		return dt_str_eq(dt_as_str2(a), dt_as_str2(b));
	} else {
		return a == b;
	}
#endif
}

// TODO
bool dt_val_deep_eq(dt_val a, dt_val b) {
	dt_val_ty ta = dt_typeof(a);
	dt_val_ty tb = dt_typeof(b);
#ifdef DT_NO_NANBOX
	if (ta != tb) {
		return false;
	} else if (ta == DT_VAL_NUM && tb == DT_VAL_NUM) {
		return dt_as_num2(a) == dt_as_num2(b);
	} else if (ta == DT_VAL_BOOL && tb == DT_VAL_BOOL) {
		return dt_as_bool2(a) == dt_as_bool2(b);
	} else if (ta == DT_VAL_NIL && tb == DT_VAL_NIL) {
		return true;
	} else if (ta == DT_VAL_STR && tb == DT_VAL_STR) {
		return dt_str_eq(dt_as_str2(a), dt_as_str2(b));
	} else {
		return false;
	}
#else
	// TODO: no this with str interning
	if (ta == DT_VAL_STR && tb == DT_VAL_STR) {
		return dt_str_eq(dt_as_str2(a), dt_as_str2(b));
	} else {
		return a == b;
	}
#endif
}

bool dt_val_lt(dt_val a, dt_val b) {

	dt_val_ty ta = dt_typeof(a);
	dt_val_ty tb = dt_typeof(b);

	if (ta != tb) {
		return false;
	} else if (ta == DT_VAL_NUM && tb == DT_VAL_NUM) {
		return dt_as_num2(a) < dt_as_num2(b);
	} else if (ta == DT_VAL_STR && tb == DT_VAL_STR) {
		return dt_str_cmp(dt_as_str2(a), dt_as_str2(b)) < 0;
	} else {
		return false;
	}

}

// TODO: func? cdata?
dt_val dt_val_clone(dt_vm* vm, dt_val v) {
	switch (dt_typeof(v)) {
		case DT_VAL_ARR:
			return dt_to_arr(dt_arr_clone(vm, dt_as_arr2(v)));
		case DT_VAL_MAP:
			return dt_to_map(dt_map_clone(vm, dt_as_map2(v)));
		default:
			return v;
	}
}

void dt_func_free(dt_vm *vm, dt_func* func);
void dt_func_free_rec(dt_vm *vm, dt_func* func);
void dt_upval_free(dt_vm *vm, dt_upval* val);

void dt_val_free_rec(dt_vm* vm, dt_val v) {
	switch (dt_typeof(v)) {
		case DT_VAL_ARR:
			dt_arr_free_rec(vm, dt_as_arr2(v));
			break;
		case DT_VAL_MAP:
			dt_map_free_rec(vm, dt_as_map2(v));
			break;
		case DT_VAL_STR:
			dt_str_free(vm, dt_as_str2(v));
			break;
		case DT_VAL_FUNC:
			dt_func_free_rec(vm, dt_as_func2(v));
			break;
		case DT_VAL_CDATA:
			dt_cdata_free(vm, dt_as_cdata2(v));
			break;
		default:
			break;
	}
}

void dt_rc_inc(dt_val val) {
	if (!dt_is_heap(val)) {
		return;
	}
	dt_heaper* h = dt_as_heaper(val);
	h->rc++;
#ifdef DT_GC_LOG
	printf("++ %d (%s) ", h->rc, dt_typename(dt_typeof(val)));
	dt_val_println(val);
#endif
}

void dt_func_println(dt_func* func);
void dt_cdata_println(dt_cdata* cdata);

void dt_rc_check(dt_val val) {
	if (!dt_is_heap(val)) {
		return;
	}
	dt_heaper* h = dt_as_heaper(val);
	if (h->rc < 0) {
		fprintf(stderr, "something went terribly wrong: rc < 0\n");
		exit(EXIT_FAILURE);
		return;
	}
	if (h->rc == 0) {
		// TODO: recursive dec
		switch (dt_typeof(val)) {
			case DT_VAL_STR: {
#ifdef DT_GC_LOG
				dt_str* str = (dt_str*)h;
				printf("FREE STR ");
				dt_str_println(str);
#endif
				dt_str_free(NULL, (dt_str*)h);
				break;
			}
			case DT_VAL_ARR: {
#ifdef DT_GC_LOG
				dt_arr* arr = (dt_arr*)h;
				printf("FREE ARR #%d\n", arr->len);
#endif
				dt_arr_free(NULL, (dt_arr*)h);
				break;
			}
			case DT_VAL_MAP: {
#ifdef DT_GC_LOG
				dt_map* map = (dt_map*)h;
				printf("FREE MAP #%d\n", map->cnt);
#endif
				dt_map_free(NULL, (dt_map*)h);
				break;
			}
			case DT_VAL_FUNC: {
#ifdef DT_GC_LOG
				dt_func* func = (dt_func*)h;
				printf("FREE FUNC ");
				dt_func_println(func);
#endif
				dt_func_free(NULL, (dt_func*)h);
				break;
			}
			case DT_VAL_UPVAL: {
#ifdef DT_GC_LOG
				dt_upval* upval = (dt_upval*)h;
				printf("FREE UPVAL ");
				dt_val_println(*upval->val);
#endif
				dt_upval_free(NULL, (dt_upval*)h);
				break;
			}
			case DT_VAL_CDATA: {
#ifdef DT_GC_LOG
				dt_cdata* cdata = (dt_cdata*)h;
				printf("FREE CDATA ");
				dt_cdata_println(cdata);
#endif
				dt_cdata_free(NULL, (dt_cdata*)h);
				break;
			}
			default:
				break;
		}
	}
}

void dt_rc_dec_nocheck(dt_val val) {
	if (!dt_is_heap(val)) {
		return;
	}
	dt_heaper* h = dt_as_heaper(val);
	h->rc--;
#ifdef DT_GC_LOG
	printf("-- %d (%s) ", h->rc, dt_typename(dt_typeof(val)));
	dt_val_println(val);
#endif
}

void dt_rc_dec(dt_val val) {
	dt_rc_dec_nocheck(val);
	dt_rc_check(val);
}

uint32_t dt_hash(char* key, int len) {
	uint32_t hash = 2166136261u;
	for (int i = 0; i < len; i++) {
		hash ^= (uint8_t)key[i];
		hash *= 16777619;
	}
	return hash;
}

dt_str* dt_str_alloc(dt_vm* vm, int len) {
	dt_str* str = dt_malloc(vm, sizeof(dt_str) + len + 1);
	str->len = len;
	str->type = DT_VAL_STR;
	str->chars[len] = '\0';
	str->rc = 0;
	return str;
}

void dt_str_hash(dt_str* str) {
	str->hash = dt_hash(str->chars, str->len);
}

dt_str* dt_str_fmt(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	size_t size = vsnprintf(NULL, 0, fmt, args);
	va_end(args);
	dt_str* str = dt_str_alloc(NULL, size);
	va_start(args, fmt);
	vsnprintf(str->chars, size + 1, fmt, args);
	va_end(args);
	str->hash = dt_hash(str->chars, str->len);
	return str;
}

dt_str* dt_str_new_len_escape(dt_vm* vm, char* src, int len) {
	dt_str* str = dt_str_alloc(vm, len);
	char* start = src;
	char* cur = src;
	char* buf = str->chars;
	while ((cur - src) < len) {
		if (*cur == '\\') {
			memcpy(buf, start, cur - start);
			buf += cur - start;
			start = cur + 2;
			cur++;
			// TODO: bound checks
// 			if ((cur - src) >= len) goto end;
			switch (*cur) {
				case '\\': *buf++ = '\\'; break;
				case '\"': *buf++ = '\"'; break;
				case 'n':  *buf++ = '\n'; break;
				case 'r':  *buf++ = '\r'; break;
				case 't':  *buf++ = '\t'; break;
				case 'v':  *buf++ = '\v'; break;
				case 'x':
					*buf++ = (char)strtol((char[]){cur[1], cur[2]}, NULL, 16);
					cur += 2;
					start += 2;
					break;
				case 'u':
					// TODO
					*buf++ = ' ';
					cur += 4;
					start += 4;
					break;
				case 'U':
					// TODO
					*buf++ = ' ';
					cur += 6;
					start += 6;
					break;
			}
		}
		cur++;
	}
// end:
	memcpy(buf, start, cur - start);
	buf += cur - start;
	str->len = buf - str->chars;
	str->chars[str->len] = '\0';
	str->hash = dt_hash(str->chars, str->len);
	return str;
}

dt_str* dt_str_new_len(dt_vm* vm, char* src, int len) {
	dt_str* str = dt_str_alloc(vm, len);
	memcpy(str->chars, src, len);
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

dt_str* dt_str_concat(dt_vm* vm, dt_str* a, dt_str* b) {
	int len = a->len + b->len;
	dt_str* new = dt_str_alloc(vm, len);
	memcpy(new->chars, a->chars, a->len);
	memcpy(new->chars + a->len, b->chars, b->len);
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

dt_arr* dt_str_chars(dt_vm* vm, dt_str* str) {
	dt_arr* arr = dt_arr_new_len(vm, str->len);
	for (int i = 0; i < str->len; i++) {
		dt_arr_push(vm, arr, dt_to_str(dt_str_new_len(vm, str->chars + i, 1)));
	}
	return arr;
}

dt_arr* dt_str_split(dt_vm* vm, dt_str* str, dt_str* sep) {
	if (sep->len == 0) {
		return dt_str_chars(vm, str);
	}
	dt_arr* arr = dt_arr_new(vm);
	int last = 0;
	int i = 0;
	for (; i < str->len; i++) {
		if (memcmp(str->chars + i, sep->chars, sep->len) == 0) {
			dt_arr_push(vm, arr, dt_to_str(dt_str_new_len(vm, str->chars + last, i - last)));
			i += sep->len - 1;
			last = i + 1;
		}
	}
	dt_arr_push(vm, arr, dt_to_str(dt_str_new_len(vm, str->chars + last, i - last)));
	return arr;
}

dt_str* dt_str_rep(dt_vm* vm, dt_str* str, int times) {
	int len = str->len * times;
	dt_str* dest = dt_str_alloc(vm, len);
	for (int i = 0; i < times; i++) {
		memcpy(dest->chars + str->len * i, str->chars, str->len);
	}
	dt_str_hash(dest);
	return dest;
}

dt_str* dt_str_toupper(dt_vm* vm, dt_str* str) {
	dt_str* dest = dt_str_alloc(vm, str->len);
	for (int i = 0; i < str->len; i++) {
		dest->chars[i] = toupper(str->chars[i]);
	}
	dt_str_hash(dest);
	return dest;
}

dt_str* dt_str_tolower(dt_vm* vm, dt_str* str) {
	dt_str* dest = dt_str_alloc(vm, str->len);
	for (int i = 0; i < str->len; i++) {
		dest->chars[i] = tolower(str->chars[i]);
	}
	dt_str_hash(dest);
	return dest;
}

int dt_str_rfind_at(dt_str* str, dt_str* key, int idx) {
	if (idx < 0 || idx >= str->len || key->len == 0) {
		return -1;
	}
	char* cur = str->chars + idx - key->len + 1;
	while (cur >= str->chars) {
		if (memcmp(cur, key->chars, key->len) == 0) {
			return cur - str->chars;
		}
		cur--;
	}
	return -1;
}

int dt_str_rfind(dt_str* str, dt_str* key) {
	return dt_str_rfind_at(str, key, str->len - 1);
}

int dt_str_find_at(dt_str* str, dt_str* key, int idx) {
	if (idx < 0 || idx >= str->len || key->len == 0) {
		return -1;
	}
	char* pos = strstr(str->chars + idx, key->chars);
	if (pos) {
		return pos - str->chars;
	} else {
		return -1;
	}
}

int dt_str_find(dt_str* str, dt_str* key) {
	return dt_str_find_at(str, key, 0);
}

bool dt_str_contains(dt_str* str, dt_str* key) {
	return dt_str_find(str, key) != -1;
}

dt_str* dt_str_rev(dt_vm* vm, dt_str* str) {
	dt_str* dest = dt_str_alloc(vm, str->len);
	for (int i = 0; i < str->len; i++) {
		dest->chars[i] = str->chars[str->len - i - 1];
	}
	dt_str_hash(dest);
	return dest;
}

// TODO
dt_map* dt_str_match(dt_vm* vm, dt_str* str, dt_str* pat) {

	char* r = pat->chars;
	char* t = str->chars;
	dt_map* res = dt_map_new(vm);

	for (;;) {
		if (*r == '\0') {
			break;
		}
		if (*r == '<') {
			char* name_start = r + 1;
			while (*r != '>') r++;
			int name_len = r - name_start;
			char* txt_start = t;
			char stop = *++r;
			while (*t != stop) {
				if (*t++ == '\0') {
					return NULL;
				}
			}
			int txt_len = t - txt_start;
			dt_map_set(
				vm,
				res,
				dt_str_new_len(vm, name_start, name_len),
				dt_to_str(dt_str_new_len(vm, txt_start, txt_len))
			);
		} else {
			if (*r != *t) {
				return NULL;
			}
			r++;
			t++;
		}
	}

	return res;

}

dt_str* dt_str_trim(dt_vm* vm, dt_str* str) {
	char* head = str->chars;
	char* tail = str->chars + str->len - 1;
	while (*head == ' ' || *head == '\t' || *head == '\n') head++;
	while (*tail == ' ' || *tail == '\t' || *tail == '\n') tail--;
	return dt_str_new_len(vm, head, tail - head + 1);
}

dt_str* dt_str_trunc(dt_vm* vm, dt_str* str, int len, dt_str* rest) {
	int rest_len = rest ? rest->len : 0;
	if (str->len + rest_len <= len) {
		return str;
	}
	dt_str* new = dt_str_alloc(vm, len);
	memcpy(new->chars, str->chars, len - rest_len);
	if (rest) {
		memcpy(new->chars + len - rest_len, rest->chars, rest_len);
	}
	dt_str_hash(new);
	return new;
}

dt_str* dt_str_get(dt_vm* vm, dt_str* str, int idx) {
	if (idx < 0) {
		idx = str->len + idx;
	}
	if (idx >= str->len || idx < 0) {
		return NULL;
	}
	return dt_str_new_len(vm, str->chars + idx, 1);
}

dt_str* dt_str_sub(dt_vm* vm, dt_str* str, int a, int b) {
	return dt_str_new_len(vm, str->chars + a, b - a);
}

int dt_str_len(dt_str* str) {
	return str->len;
}

int dt_str_cmp(dt_str* a, dt_str* b) {
	return strncmp(a->chars, b->chars, a->len < b->len ? a->len : b->len);
}

char* dt_str_cstr(dt_str* str) {
	char* chars = malloc(str->len + 1);
	memcpy(chars, str->chars, str->len);
	chars[str->len] = '\0';
	return chars;
}

dt_str* dt_num_to_str(dt_vm* vm, dt_num num) {
	int len = snprintf(NULL, 0, "%.12g", num);
	dt_str* str = dt_str_alloc(vm, len);
	sprintf(str->chars, "%.12g", num);
	str->hash = dt_hash(str->chars, len);
	return str;
}

size_t dt_nxt_pow2(size_t x) {
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}

dt_arr* dt_arr_new_len(dt_vm* vm, int len) {
	dt_arr* arr = dt_malloc(vm, sizeof(dt_arr));
	arr->len = 0;
	arr->cap = len > DT_ARR_INIT_SIZE ? dt_nxt_pow2(len) : DT_ARR_INIT_SIZE;
	arr->values = dt_malloc(vm, arr->cap * sizeof(dt_val));
	arr->type = DT_VAL_ARR;
	arr->rc = 0;
	arr->type = DT_VAL_ARR;
	return arr;
}

dt_arr* dt_arr_new(dt_vm* vm) {
	return dt_arr_new_len(vm, DT_ARR_INIT_SIZE);
}

void dt_arr_free(dt_vm* vm, dt_arr* arr) {
	for (int i = 0; i < arr->len; i++) {
		dt_rc_dec(arr->values[i]);
	}
	dt_free(vm, arr->values, arr->cap * sizeof(dt_val));
	dt_free(vm, arr, sizeof(dt_arr));
}

void dt_arr_free_rec(dt_vm* vm, dt_arr* arr) {
	for (int i = 0; i < arr->len; i++) {
		dt_val_free_rec(vm, arr->values[i]);
	}
	dt_arr_free(vm, arr);
}

dt_val dt_arr_get(dt_arr* arr, int idx) {
	if (idx < 0) {
		idx = arr->len + idx;
	}
	if (idx >= arr->len || idx < 0) {
		return DT_NIL;
	}
	return arr->values[idx];
}

void dt_arr_set_weak(dt_vm* vm, dt_arr* arr, int idx, dt_val val) {

	// expand
	if (idx >= arr->cap) {
		size_t old_cap = arr->cap;
		arr->cap = dt_nxt_pow2(arr->cap + 1);
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

// TODO: dec if overwritten
void dt_arr_set(dt_vm* vm, dt_arr* arr, int idx, dt_val val) {
	dt_arr_set_weak(vm, arr, idx, val);
	dt_rc_inc(val);
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

	memcpy(arr->values + idx + 1, arr->values + idx, sizeof(dt_val) * (arr->len - idx));
	arr->len++;
	arr->values[idx] = val;

}

void dt_arr_push_weak(dt_vm* vm, dt_arr* arr, dt_val val) {
	dt_arr_set_weak(vm, arr, arr->len, val);
}

void dt_arr_push(dt_vm* vm, dt_arr* arr, dt_val val) {
	dt_arr_set(vm, arr, arr->len, val);
}

dt_val dt_arr_rm(dt_arr* arr, int idx) {
	if (idx < 0) idx = arr->len + idx;
	if (idx >= arr->len || idx < 0) return DT_NIL;
	dt_val v = arr->values[idx];
	memcpy(arr->values + idx, arr->values + idx + 1, (arr->len - idx - 1) * sizeof(dt_val));
	arr->len--;
	arr->values[arr->len] = DT_NIL;
	dt_rc_dec(v);
	return v;
}

int dt_arr_find(dt_arr* arr, dt_val v) {
	for (int i = 0; i < arr->len; i++) {
		if (dt_val_eq(v, arr->values[i])) {
			return i;
		}
	}
	return -1;
}

void dt_arr_rm_all(dt_arr* arr, dt_val v) {
	for (int i = arr->len - 1; i >= 0; i++) {
		if (dt_val_eq(v, arr->values[i])) {
			dt_arr_rm(arr, i);
		}
	}
}

dt_arr* dt_arr_map(dt_vm* vm, dt_arr* arr, dt_val f) {
	dt_arr* new = dt_arr_new_len(vm, arr->len);
	for (int i = 0; i < arr->len; i++) {
		dt_arr_set(vm, new, i, dt_call(vm, f, 2, arr->values[i], dt_to_num(i)));
	}
	return new;
}

dt_arr* dt_arr_sort(dt_vm* vm, dt_arr* arr, dt_val f) {
	dt_arr* new = dt_arr_new_len(vm, arr->len);
	dt_arr_set(vm, new, 0, arr->values[0]);
	for (int i = 1; i < arr->len; i++) {
		dt_val cur = arr->values[i];
		bool inserted = false;
		for (int j = 0; j < i; j++) {
			dt_val res = dt_call(vm, f, 2, cur, new->values[j]);
			if (dt_val_truthy(res)) {
				dt_arr_insert(vm, new, j, cur);
				inserted = true;
				break;
			}
		}
		if (!inserted) {
			dt_arr_set(vm, new, i, cur);
		}
	}
	return new;
}

void dt_arr_each(dt_vm* vm, dt_arr* arr, dt_val f) {
	for (int i = 0; i < arr->len; i++) {
		dt_call(vm, f, 2, arr->values[i], dt_to_num(i));
	}
}

dt_arr* dt_arr_filter(dt_vm* vm, dt_arr* arr, dt_val f) {
	dt_arr* new = dt_arr_new_len(vm, arr->len);
	for (int i = 0; i < arr->len; i++) {
		dt_val res = dt_call(vm, f, 2, arr->values[i], dt_to_num(i));
		if (dt_val_truthy(res)) {
			dt_arr_push(vm, new, arr->values[i]);
		}
	}
	return new;
}

dt_val dt_arr_reduce(dt_vm* vm, dt_arr* arr, dt_val state, dt_val f) {
	for (int i = 0; i < arr->len; i++) {
		state = dt_call(vm, f, 3, state, arr->values[i], dt_to_num(i));
	}
	return state;
}

bool dt_arr_contains(dt_arr* arr, dt_val v) {
	return dt_arr_find(arr, v) != -1;
}

dt_str* dt_val_to_str(dt_vm* vm, dt_val val);

// TODO: too many alloc
dt_str* dt_arr_join(dt_vm* vm, dt_arr* arr, dt_str* sep) {
	dt_str* s = dt_str_new(vm, "");
	for (int i = 0; i < arr->len; i++) {
		s = dt_str_concat(vm, s, dt_val_to_str(vm, arr->values[i]));
		if (i < arr->len - 1) {
			s = dt_str_concat(vm, s, sep);
		}
	}
	return s;
}

dt_arr* dt_arr_dedup(dt_vm* vm, dt_arr* arr) {
	dt_arr* new = dt_arr_new_len(vm, arr->len);
	// TODO
	return new;
}

dt_arr* dt_arr_rev(dt_vm* vm, dt_arr* arr) {
	dt_arr* new = dt_arr_new_len(vm, arr->len);
	for (int i = 0; i < arr->len; i++) {
		dt_arr_set(vm, new, i, arr->values[arr->len - i - 1]);
	}
	return new;
}

dt_val dt_arr_pop(dt_arr* arr) {
	return dt_arr_rm(arr, arr->len - 1);
}

dt_arr* dt_arr_sub(dt_vm* vm, dt_arr* arr, int start, int end) {
	dt_arr* new = dt_arr_new_len(vm, end - start);
	for (int i = start; i < end; i++) {
		dt_arr_set(
			vm,
			new,
			i - start,
			dt_arr_get(arr, i)
		);
	}
	return new;
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

dt_val dt_arr_rand(dt_arr* arr) {
	if (arr->len == 0) {
		return DT_NIL;
	}
	return dt_arr_get(arr, rand() % arr->len);
}

dt_arr* dt_arr_clone(dt_vm* vm, dt_arr* src) {
	dt_arr* new = dt_arr_new_len(vm, src->len);
	for (int i = 0; i < src->len; i++) {
		dt_arr_set(vm, new, i, dt_val_clone(vm, src->values[i]));
	}
	return new;
}

bool dt_arr_eq(dt_arr* a1, dt_arr* a2) {
	if (a1->len != a2->len) return false;
	for (int i = 0; i < a1->len; i++) {
		if (!dt_val_deep_eq(a1->values[i], a2->values[i])) return false;
	}
	return true;
}

void dt_arr_print(dt_arr* arr) {
	printf("[ ");
	for (int i = 0; i < arr->len; i++) {
		dt_val_print(arr->values[i]);
		printf(", ");
	}
	printf("]");
}

void dt_arr_println(dt_arr* arr) {
	dt_arr_print(arr);
	printf("\n");
}

dt_map* dt_map_new_len(dt_vm* vm, int len) {
	dt_map* map = dt_malloc(vm, sizeof(dt_map));
	map->cnt = 0;
	map->cap = len > DT_MAP_INIT_SIZE ? dt_nxt_pow2(len) : DT_MAP_INIT_SIZE;
	int entries_size = map->cap * sizeof(dt_map_entry);
	map->entries = dt_malloc(vm, entries_size);
	memset(map->entries, 0, entries_size);
	map->type = DT_VAL_MAP;
	map->rc = 0;
	map->type = DT_VAL_MAP;
	return map;
}

dt_map* dt_map_new(dt_vm* vm) {
	return dt_map_new_len(vm, DT_MAP_INIT_SIZE);
}

dt_map* dt_map_new_reg(dt_vm* vm, dt_map_centry* table) {
	dt_map* map = dt_map_new(vm);
	dt_map_reg(vm, map, table);
	return map;
}

void dt_map_free(dt_vm* vm, dt_map* map) {
	dt_map_iter iter = dt_map_iter_new(map);
	dt_map_entry* e;
	while ((e = dt_map_iter_nxt(&iter))) {
		dt_rc_dec(dt_to_str(e->key));
		dt_rc_dec(e->val);
	}
	dt_free(vm, map->entries, map->cap * sizeof(dt_map_entry));
	dt_free(vm, map, sizeof(dt_map));
}

void dt_map_free_rec(dt_vm* vm, dt_map* map) {
	dt_map_iter iter = dt_map_iter_new(map);
	dt_map_entry* e;
	while ((e = dt_map_iter_nxt(&iter))) {
		dt_str_free(vm, e->key);
		dt_val_free_rec(vm, e->val);
	}
	dt_map_free(vm, map);
}

int dt_map_find(dt_map* map, dt_str* key) {

	uint32_t oidx = key->hash & (map->cap - 1);
	uint32_t idx = oidx;

	for (;;) {

		dt_map_entry e = map->entries[idx];

		if (e.key == NULL || dt_str_eq(e.key, key)) {
			return idx;
		}

		idx = (idx + 1) & (map->cap - 1);

		// went through one round
		if (idx == oidx) {
			return -1;
		}

	}

}

bool dt_map_set_weak(dt_vm* vm, dt_map* map, dt_str* key, dt_val val) {

	// resize
	if (map->cnt + 1 > map->cap * DT_MAP_MAX_LOAD) {

		int old_cap = map->cap;
		dt_map_entry* old_entries = map->entries;
		size_t old_size = old_cap * sizeof(dt_map_entry);

		map->cap = dt_nxt_pow2(map->cap + 1);
		size_t size = map->cap * sizeof(dt_map_entry);
		map->entries = dt_malloc(vm, size);
		memset(map->entries, 0, size);

		for (int i = 0; i < old_cap; i++) {
			dt_map_entry e = old_entries[i];
			if (e.key && !dt_is_nil(e.val)) {
				dt_map_set_weak(vm, map, e.key, e.val);
			}
		}

		dt_free(vm, old_entries, old_size);

	}

	int idx = dt_map_find(map, key);

	if (idx == -1) {
		dt_throw(vm, "map overflow");
		return false;
	}

	if (map->entries[idx].key) {
		map->entries[idx].val = val;
	} else {
		dt_map_entry* e = &map->entries[idx];
		e->key = key;
		e->val = val;
		map->cnt++;
	}

	return true;

}

// TODO: dec if overwritten
void dt_map_set(dt_vm* vm, dt_map* map, dt_str* key, dt_val val) {
	if (dt_map_set_weak(vm, map, key, val)) {
		dt_rc_inc(dt_to_str(key));
		dt_rc_inc(val);
	}
}

// TODO: review
bool dt_map_has(dt_map* map, dt_str* key) {
	int idx = dt_map_find(map, key);
	dt_map_entry e = map->entries[idx];
	return idx != -1 && e.key != NULL && !dt_is_nil(e.val);
}

bool dt_map_chas(dt_vm* vm, dt_map* map, char* key) {
	return dt_map_has(map, dt_str_new(vm, key));
}

dt_val dt_map_rm(dt_map* map, dt_str* key) {
	int idx = dt_map_find(map, key);
	dt_map_entry* e = &map->entries[idx];
	dt_val v = e->val;
	e->val = DT_NIL;
	dt_rc_dec(v);
	return v;
}

dt_val dt_map_get(dt_map* map, dt_str* key) {

	int idx = dt_map_find(map, key);

	if (idx == -1 || map->entries[idx].key == NULL) {
		return DT_NIL;
	}

	return map->entries[idx].val;

}

dt_map_iter dt_map_iter_new(dt_map* map) {
	return (dt_map_iter) {
		.map = map,
		.idx = 0,
	};
}

dt_map_entry* dt_map_iter_nxt(dt_map_iter* iter) {
	while (iter->idx < iter->map->cap) {
		dt_map_entry* e = iter->map->entries + iter->idx++;
		if (e->key && !dt_is_nil(e->val)) {
			return e;
		}
	}
	return NULL;
}

void dt_map_each(dt_vm* vm, dt_map* map, dt_val f) {
	dt_map_iter iter = dt_map_iter_new(map);
	dt_map_entry* e;
	while ((e = dt_map_iter_nxt(&iter))) {
		dt_call(vm, f, 2, dt_to_str(e->key), e->val);
	}
}

dt_val dt_map_cget(dt_vm* vm, dt_map* map, char* key) {
	return dt_map_get(map, dt_str_new(vm, key));
}

void dt_map_cset(dt_vm* vm, dt_map* map, char* key, dt_val val) {
	dt_map_set(vm, map, dt_str_new(vm, key), val);
}

void dt_map_reg(dt_vm* vm, dt_map* map, dt_map_centry* table) {
	for (dt_map_centry* entry = table; entry->name != NULL; entry++) {
		dt_map_cset(vm, map, entry->name, entry->val);
	}
}

dt_arr* dt_map_keys(dt_vm* vm, dt_map* map) {
	dt_arr* arr = dt_arr_new(vm);
	dt_map_iter iter = dt_map_iter_new(map);
	dt_map_entry* e;
	while ((e = dt_map_iter_nxt(&iter))) {
		dt_arr_push(vm, arr, dt_to_str(e->key));
	}
	return arr;
}

dt_arr* dt_map_vals(dt_vm* vm, dt_map* map) {
	dt_arr* arr = dt_arr_new(vm);
	dt_map_iter iter = dt_map_iter_new(map);
	dt_map_entry* e;
	while ((e = dt_map_iter_nxt(&iter))) {
		dt_arr_push(vm, arr, e->val);
	}
	return arr;
}

dt_map* dt_map_clone(dt_vm* vm, dt_map* src) {
	dt_map* new = dt_map_new_len(vm, src->cap);
	dt_map_iter iter = dt_map_iter_new(src);
	dt_map_entry* e;
	while ((e = dt_map_iter_nxt(&iter))) {
		dt_map_set(vm, new, e->key, dt_val_clone(vm, e->val));
	}
	return new;
}

bool dt_map_eq(dt_map* m1, dt_map* m2) {
	if (m1->cnt != m2->cnt) return false;
	dt_map_iter iter = dt_map_iter_new(m1);
	dt_map_entry* e;
	while ((e = dt_map_iter_nxt(&iter))) {
		if (!dt_val_deep_eq(e->val, dt_map_get(m2, e->key))) return false;
	}
	return true;
}

void dt_map_print(dt_map* map) {
	printf("{ ");
// 	printf("\n");
	dt_map_iter iter = dt_map_iter_new(map);
	dt_map_entry* e;
	while ((e = dt_map_iter_nxt(&iter))) {
// 		printf("    ");
		dt_str_print(e->key);
		printf(": ");
		dt_val_print(e->val);
		printf(", ");
// 		printf("\n");
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

void dt_upval_free(dt_vm* vm, dt_upval* upval) {
	if (upval->captured) {
		dt_free(vm, upval->val, sizeof(dt_val));
	}
	dt_free(vm, upval, sizeof(dt_upval));
}

void dt_func_free(dt_vm *vm, dt_func* func) {
	// TODO: dec upval
	for (int i = 0; i < func->num_upvals; i++) {
// 		dt_rc_dec(func->upvals[i]);
	}
	dt_free(vm, func->upvals, sizeof(dt_val*) * func->num_upvals);
	dt_free(vm, func, sizeof(dt_func));
}

void dt_func_free_rec(dt_vm *vm, dt_func* func) {
	for (int i = 0; i < func->num_upvals; i++) {
		dt_upval_free(vm, func->upvals[i]);
	}
	dt_func_free(vm, func);
}

void dt_cdata_print(dt_cdata* cdata) {
	printf("<cdata#%p>", (void*)cdata);
}

void dt_cdata_println(dt_cdata* cdata) {
	dt_cdata_print(cdata);
	printf("\n");
}

dt_cdata* dt_cdata_new(dt_vm* vm, void* data, size_t size) {
	dt_cdata* cdata = dt_malloc(vm, sizeof(dt_cdata) + size);
	memcpy(cdata->data, data, size);
	cdata->size = size;
	cdata->rc = 0;
	cdata->type = DT_VAL_CDATA;
	return cdata;
}

void* dt_cdata_get(dt_cdata* cdata) {
	return cdata->data;
}

void dt_cdata_free(dt_vm* vm, dt_cdata* cdata) {
	dt_free(vm, cdata, sizeof(dt_cdata) + cdata->size);
}

bool dt_val_truthy(dt_val val) {
#ifdef DT_NO_NANBOX
	return !(
		val.type == DT_VAL_NIL
		|| (val.type == DT_VAL_BOOL && val.data.boolean == false)
	);
#else
	return val != DT_NIL && val != DT_FALSE;
#endif
}

dt_str* dt_bool_to_str(dt_vm* vm, dt_bool b) {
	return b ? dt_str_new(vm, "true") : dt_str_new(vm, "false");
}

dt_str* dt_val_to_str(dt_vm* vm, dt_val val) {
	switch (dt_typeof(val)) {
		case DT_VAL_STR:
			return dt_as_str2(val);
		case DT_VAL_NUM:
			return dt_num_to_str(vm, dt_as_num2(val));
		case DT_VAL_BOOL:
			return dt_bool_to_str(vm, dt_as_num2(val));
		default:
			return NULL;
	}
}

dt_chunk dt_chunk_new() {
	return (dt_chunk) {
		.cnt = 0,
		.cap = 0,
		.code = malloc(0),
		.lines = malloc(0),
		.consts = dt_arr_new(NULL),
	};
}

// TODO: how to store heap const like str?
// chunk file layout
//  ----------------------------------
// | magic      char[10] (d1RtyChuNk) |
//  ----------------------------------
// | #opcodes   uint32                |
//  ----------------------------------
// | opcodes    uint8[#opcodes]       |
//  ----------------------------------
// | linenos    uint8[#opcodes]       |
//  ----------------------------------
// | #consts    uint32                |
//  ----------------------------------
// | consts     uint64[#consts]       |
//  ----------------------------------

uint8_t* dt_chunk_save(dt_chunk* c, size_t* osize) {
	// TODO
	return NULL;
}

void dt_chunk_free(dt_chunk* c) {
	free(c->code);
	free(c->lines);
	dt_arr_free(NULL, c->consts);
	memset(c, 0, sizeof(dt_chunk));
}

char* dt_op_name(dt_op op) {
	switch (op) {
		case DT_OP_STOP:      return "STOP";
		case DT_OP_THROW:     return "THROW";
		case DT_OP_CONST:     return "CONST";
		case DT_OP_NIL:       return "NIL";
		case DT_OP_TRUE:      return "TRUE";
		case DT_OP_FALSE:     return "FALSE";
		case DT_OP_ADD:       return "ADD";
		case DT_OP_SUB:       return "SUB";
		case DT_OP_MUL:       return "MUL";
		case DT_OP_DIV:       return "DIV";
		case DT_OP_NEG:       return "NEG";
		case DT_OP_NOT:       return "NOT";
		case DT_OP_EQ:        return "EQ";
		case DT_OP_GT:        return "GT";
		case DT_OP_GT_EQ:     return "GT_EQ";
		case DT_OP_LT:        return "LT";
		case DT_OP_LT_EQ:     return "LT_EQ";
		case DT_OP_OR:        return "OR";
		case DT_OP_NIL_OR:    return "NIL_OR";
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
		case DT_OP_CALL2:     return "CALL2";
		case DT_OP_TRY_PREP:  return "TRY_PREP";
		case DT_OP_TRY:       return "TRY";
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

int dt_chunk_peek_at(dt_chunk* c, int idx) {

	uint8_t ins = c->code[idx];

	switch (ins) {
		case DT_OP_CONST: {
			uint8_t i2 = c->code[idx + 1];
			uint8_t i3 = c->code[idx + 2];
			printf("CONST ");
			dt_val_print(c->consts->values[i2 << 8 | i3]);
			return idx + 3;
		}
		case DT_OP_SETG: {
			uint8_t idx2 = c->code[idx + 1];
			printf("SEFG ");
			dt_val_print(c->consts->values[idx2]);
			return idx + 2;
		}
		case DT_OP_GETG: {
			uint8_t idx2 = c->code[idx + 1];
			printf("GETG ");
			dt_val_print(c->consts->values[idx2]);
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
		case DT_OP_CALL2: {
			uint8_t nargs = c->code[idx + 1];
			printf("CALL2 %d", nargs);
			return idx + 2;
		}
		case DT_OP_MKFUNC: {
			uint8_t num_upvals = c->code[idx + 1];
			printf("MKFUNC %d", num_upvals);
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

void dt_chunk_push(dt_chunk* c, uint8_t byte, int line) {

	if (c->cap < c->cnt + 1) {
		c->cap = c->cap < 8 ? 8 : c->cap * 2;
		c->code = realloc(c->code, c->cap * sizeof(uint8_t));
		c->lines = realloc(c->lines, c->cap * sizeof(int));
	}

	c->code[c->cnt] = byte;
	c->lines[c->cnt] = line;
	c->cnt++;

}

int dt_chunk_add_const(dt_chunk* c, dt_val val) {
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
		.stack_bot = NULL,
		.globals = NULL,
		.libs = dt_map_new(NULL),
		.math_lib = dt_map_new(NULL),
		.str_lib = dt_map_new(NULL),
		.arr_lib = dt_map_new(NULL),
		.map_lib = dt_map_new(NULL),
		.func_lib = dt_map_new(NULL),
		.strs = dt_map_new(NULL),
		.open_upvals = {0},
		.num_upvals = 0,
		.mem = 0,
		.trying = NULL,
		.throwing = false,
	};
	vm.stack_bot = vm.stack;
	vm.stack_top = vm.stack + 1;
	vm.globals = dt_map_new(&vm);
	vm.stack_top = vm.stack;
	dt_rc_inc(dt_to_map(vm.libs));
	dt_rc_inc(dt_to_map(vm.strs));
	return vm;
}

void dt_def_catch(dt_err* err) {
	fprintf(stderr, "line #%d: %s\n", err->line, err->msg);
}

void dt_throw(dt_vm* vm, char* fmt, ...) {

	// not running
	if (!vm->func) {
		va_list args;
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		fprintf(stderr, "\n");
		va_end(args);
		exit(EXIT_FAILURE);
		return;
	}

	dt_chunk* chunk = &vm->func->logic->chunk;
	int offset = vm->ip - chunk->code;
	int line = chunk->lines[offset - 1];

	va_list args;
	va_start(args, fmt);
	size_t size = vsnprintf(NULL, 0, fmt, args);
	va_end(args);
	char* msg = malloc(size + 1);
	va_start(args, fmt);
	vsnprintf(msg, size + 1, fmt, args);
	va_end(args);

	vm->throwing = true;
	vm->err.msg = msg;
	vm->err.line = line;

	if (vm->func != vm->trying) {
		while (*vm->ip != DT_OP_STOP) vm->ip++;
	}

}

// get the last n'th value on stack
dt_val dt_vm_get(dt_vm* vm, int n) {
	dt_val* pos = vm->stack_top - 1 + n;
	if (pos < vm->stack) {
		return DT_NIL;
	} else {
		return *pos;
	}
}

dt_val dt_arg(dt_vm* vm, int idx) {
	dt_val* v = vm->stack_bot + idx;
	if (v < vm->stack_top) {
		return *v;
	} else {
		// TODO: error?
		return DT_NIL;
	}
}

int dt_nargs(dt_vm* vm) {
	return vm->stack_top - vm->stack_bot;
}

bool dt_arg_exists(dt_vm* vm, int idx) {
	return dt_nargs(vm) > idx && dt_typeof(dt_arg(vm, idx)) != DT_VAL_NIL;
}

bool dt_check_args(dt_vm* vm, int nargs, ...) {
	va_list args;
	va_start(args, nargs);
	for (int i = 0; i < nargs; i++) {
		dt_val v = dt_arg(vm, i);
		dt_val_ty actual = dt_typeof(dt_arg(vm, i));
		dt_val_ty expected = va_arg(args, dt_val_ty);
		if (actual != expected) {
			dt_throw(
				vm,
				"expected a '%s' at %d, found '%s'",
				dt_typename(expected),
				i,
				dt_typename(actual)
			);
			return false;
		}
	}
	va_end(args);
	return true;
}

dt_val dt_arg_or(dt_vm* vm, int idx, dt_val v) {
	if (dt_arg_exists(vm, idx)) {
		return dt_arg(vm, idx);
	} else {
		return v;
	}
}

dt_num dt_arg_num(dt_vm* vm, int idx) {
	return dt_as_num2(dt_arg(vm, idx));
}

dt_num dt_arg_num_or(dt_vm* vm, int idx, dt_num n) {
	if (dt_arg_exists(vm, idx)) {
		return dt_arg_num(vm, idx);
	} else {
		return n;
	}
}

dt_bool dt_arg_bool(dt_vm* vm, int idx) {
	return dt_as_bool2(dt_arg(vm, idx));
}

dt_bool dt_arg_bool_or(dt_vm* vm, int idx, dt_bool n) {
	if (dt_arg_exists(vm, idx)) {
		return dt_arg_bool(vm, idx);
	} else {
		return n;
	}
}

dt_str* dt_arg_str(dt_vm* vm, int idx) {
	return dt_as_str2(dt_arg(vm, idx));
}

dt_str* dt_arg_str_or(dt_vm* vm, int idx, dt_str* str) {
	if (dt_arg_exists(vm, idx)) {
		return dt_arg_str(vm, idx);
	} else {
		return str;
	}
}

char* dt_arg_cstr(dt_vm* vm, int idx) {
	dt_str* str = dt_arg_str(vm, idx);
	if (str) {
		return str->chars;
	} else {
		return NULL;
	}
}

char* dt_arg_cstr_dup(dt_vm* vm, int idx) {
	dt_str* str = dt_arg_str(vm, idx);
	if (str) {
		return dt_str_cstr(dt_arg_str(vm, idx));
	} else {
		return NULL;
	}
}

char* dt_arg_cstr_or(dt_vm* vm, int idx, char* str) {
	if (dt_arg_exists(vm, idx)) {
		return dt_arg_cstr(vm, idx);
	} else {
		return str;
	}
}

char* dt_arg_cstr_or_dup(dt_vm* vm, int idx, char* str) {
	if (dt_arg_exists(vm, idx)) {
		return dt_arg_cstr_dup(vm, idx);
	} else {
		return str;
	}
}

dt_map* dt_arg_map(dt_vm* vm, int idx) {
	return dt_as_map2(dt_arg(vm, idx));
}

dt_arr* dt_arg_arr(dt_vm* vm, int idx) {
	return dt_as_arr2(dt_arg(vm, idx));
}

dt_func* dt_arg_func(dt_vm* vm, int idx) {
	return dt_as_func2(dt_arg(vm, idx));
}

void dt_vm_push_noinc(dt_vm* vm, dt_val val) {
	*vm->stack_top = val;
	vm->stack_top++;
	if (vm->stack_top - vm->stack >= DT_STACK_MAX) {
		dt_fail("stack overflow\n");
		return;
	}
}

// push a value to the stack
void dt_vm_push(dt_vm* vm, dt_val val) {
	dt_vm_push_noinc(vm, val);
	dt_rc_inc(val);
}

// pop a value off the stack
dt_val dt_vm_pop_nocheck(dt_vm* vm) {
	if (vm->stack_top == vm->stack) {
		dt_fail("stack underflow\n");
		return DT_NIL;
	}
	vm->stack_top--;
	dt_rc_dec_nocheck(*vm->stack_top);
	return *vm->stack_top;
}

// pop a value off the stack
dt_val dt_vm_pop(dt_vm* vm) {
	dt_val val = dt_vm_pop_nocheck(vm);
	dt_rc_check(val);
	return val;
}

// pop a value and perform upval close check
void dt_vm_pop_close(dt_vm* vm) {

	dt_val* top = vm->stack_top - 1;
	dt_val* upval = NULL;

	for (int i = 0; i < vm->num_upvals; i++) {
		if (top == vm->open_upvals[i]->val) {
			if (!upval) {
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

void dt_vm_stack_print(dt_vm* vm) {

	printf("[ ");

	for (dt_val* slot = vm->stack; slot < vm->stack_top; slot++) {
		dt_val_print(*slot);
		printf(", ");
	}

	printf("]");

}

void dt_vm_stack_println(dt_vm* vm) {
	dt_vm_stack_print(vm);
	printf("\n");
}

void dt_vm_run(dt_vm* vm, dt_func* func);

// call a dt_func
dt_val dt_vm_call(dt_vm* vm, dt_val val, int nargs) {

	dt_val_ty ty = dt_typeof(val);
	dt_val ret = DT_NIL;

	if (ty == DT_VAL_CFUNC) {

		dt_val* prev_bot = vm->stack_bot;
		vm->stack_bot = vm->stack_top - nargs;
		ret = (dt_as_cfunc2(val))(vm);
		vm->stack_bot = prev_bot;

		// pop args
		for (int i = 0; i < nargs; i++) {
			dt_vm_pop(vm);
		}

	} else if (ty == DT_VAL_FUNC) {

		dt_func* func = dt_as_func2(val);
		dt_logic* logic = func->logic;

		// balance the arg stack
		if (logic->nargs > nargs) {
			for (int i = 0; i < logic->nargs - nargs; i++) {
				dt_vm_push(vm, DT_NIL);
			}
		// TODO: keep extra args
		} else if (logic->nargs < nargs) {
			for (int i = 0; i < nargs - logic->nargs; i++) {
				dt_vm_pop(vm);
			}
		}

		nargs = logic->nargs;

		dt_func* prev_func = vm->func;
		uint8_t* prev_ip = vm->ip;
		dt_val* prev_bot = vm->stack_bot;
		vm->stack_bot = vm->stack_top - nargs;
		dt_vm_run(vm, func);

		int locals_cnt = vm->stack_top - vm->stack_bot - 1 - nargs;
		vm->stack_bot = prev_bot;
		vm->func = prev_func;
		vm->ip = prev_ip;
		// TODO: rc audit
		ret = dt_vm_pop_nocheck(vm);

		// pop locals + args + func
		for (int i = 0; i < locals_cnt + nargs; i++) {
			dt_vm_pop_close(vm);
		}

	} else {
		dt_throw(
			vm,
			"cannot call a '%s'",
			dt_typename(ty)
		);
		for (int i = 0; i < nargs; i++) {
			dt_vm_pop(vm);
		}
	}

	if (vm->throwing) {
		// TODO: add stack trace here
		// TODO: somtimes vm->ip is NULL for single depth stuff
		if (vm->func != vm->trying) {
			while (*vm->ip != DT_OP_STOP) vm->ip++;
		}
		return DT_NIL;
	}

	return ret;

}

dt_val dt_call(dt_vm* vm, dt_val func, int nargs, ...) {
	va_list args;
	va_start(args, nargs);
	for (int i = 0; i < nargs; i++) {
		dt_vm_push(vm, va_arg(args, dt_val));
	}
	dt_val ret = dt_vm_call(vm, func, nargs);
	va_end(args);
	return ret;
}

void dt_vm_gc_check(dt_vm* vm);

// start running those opcodes
void dt_vm_run(dt_vm* vm, dt_func* func) {

	vm->func = func;
	dt_chunk* chunk = &vm->func->logic->chunk;
	vm->ip = func->logic->chunk.code;

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

		switch (*vm->ip++) {

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
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_val_ty ta = dt_typeof(a);
				dt_val_ty tb = dt_typeof(b);
				if (ta == DT_VAL_NUM && tb == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(dt_as_num2(a) + dt_as_num2(b)));
				} else if (ta == DT_VAL_STR && tb == DT_VAL_STR) {
					dt_vm_push(vm, dt_to_str(dt_str_concat(vm, dt_as_str2(a), dt_as_str2(b))));
				} else if (ta == DT_VAL_ARR && tb == DT_VAL_ARR) {
					dt_vm_push(vm, dt_to_arr(dt_arr_concat(vm, dt_as_arr2(a), dt_as_arr2(b))));
				} else if (ta == DT_VAL_STR || tb == DT_VAL_STR) {
					dt_str* a_str = dt_val_to_str(vm, a);
					dt_str* b_str = dt_val_to_str(vm, b);
					if (a_str == NULL || b_str == NULL) {
						dt_throw(
							vm,
							"cannot add a '%s' with '%s'",
							dt_typename(ta),
							dt_typename(tb)
						);
						dt_vm_push(vm, DT_NIL);
					} else {
						dt_vm_push(vm, dt_to_str(dt_str_concat(vm, a_str, b_str)));
					}
				} else {
					dt_throw(
						vm,
						"cannot add a '%s' with '%s'",
						dt_typename(ta),
						dt_typename(tb)
					);
					dt_vm_push(vm, DT_NIL);
				}
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_SUB: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_val_ty ta = dt_typeof(a);
				dt_val_ty tb = dt_typeof(b);
				if (ta == DT_VAL_NUM && tb == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(dt_as_num2(a) - dt_as_num2(b)));
				} else {
					dt_throw(
						vm,
						"cannot sub a '%s' by '%s'",
						dt_typename(ta),
						dt_typename(tb)
					);
					dt_vm_push(vm, DT_NIL);
				}
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_MUL: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_val_ty ta = dt_typeof(a);
				dt_val_ty tb = dt_typeof(b);
				if (ta == DT_VAL_NUM && tb == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(dt_as_num2(a) * dt_as_num2(b)));
				} else {
					dt_throw(
						vm,
						"cannot mul a '%s' with '%s'",
						dt_typename(ta),
						dt_typename(tb)
					);
					dt_vm_push(vm, DT_NIL);
				}
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_DIV: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_val_ty ta = dt_typeof(a);
				dt_val_ty tb = dt_typeof(b);
				if (ta == DT_VAL_NUM && tb == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(dt_as_num2(a) / dt_as_num2(b)));
				} else {
					dt_throw(
						vm,
						"cannot div a '%s' by '%s'",
						dt_typename(ta),
						dt_typename(tb)
					);
					dt_vm_push(vm, DT_NIL);
				}
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_SPREAD: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				if (dt_typeof(a) == DT_VAL_NUM && dt_typeof(b) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_range((dt_range) {
						.start = dt_as_num2(a),
						.end = dt_as_num2(b),
					}));
				} else {
					dt_throw(
						vm,
						"cannot spread a '%s' and '%s'",
						dt_typename(dt_typeof(a)),
						dt_typename(dt_typeof(b))
					);
					dt_vm_push(vm, DT_NIL);
				}
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_NEG: {
				dt_val a = dt_vm_pop_nocheck(vm);
				if (dt_typeof(a) == DT_VAL_NUM) {
					dt_vm_push(vm, dt_to_num(-dt_as_num2(a)));
				} else {
					dt_throw(
						vm,
						"cannot negate a '%s'",
						dt_typename(dt_typeof(a))
					);
					dt_vm_push(vm, DT_NIL);
				}
				dt_rc_check(a);
				break;
			}

			case DT_OP_NOT: {
				dt_val a = dt_vm_pop_nocheck(vm);
				if (dt_typeof(a) == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_to_bool(!dt_as_bool2(a)));
				} else {
					dt_throw(
						vm,
						"cannot not a '%s'",
						dt_typename(dt_typeof(a))
					);
					dt_vm_push(vm, DT_NIL);
				}
				dt_rc_check(a);
				break;
			}

			case DT_OP_LEN: {
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_val_ty t = dt_typeof(a);
				if (t == DT_VAL_STR) {
					dt_vm_push(vm, dt_to_num(dt_as_str2(a)->len));
				} else if (t == DT_VAL_ARR) {
					dt_vm_push(vm, dt_to_num(dt_as_arr2(a)->len));
				} else if (t == DT_VAL_MAP) {
					dt_vm_push(vm, dt_to_num(dt_as_map2(a)->cnt));
				} else if (t == DT_VAL_RANGE) {
					dt_range r = dt_as_range2(a);
					dt_vm_push(vm, dt_to_num(abs(r.end - r.start)));
				} else {
					dt_throw(
						vm,
						"cannot get len of a '%s'",
						dt_typename(t)
					);
					dt_vm_push(vm, DT_NIL);
				}
				dt_rc_check(a);
				break;
			}

			case DT_OP_EQ: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_vm_push(vm, dt_to_bool(dt_val_eq(a, b)));
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_GT: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_vm_push(vm, dt_to_bool(!dt_val_lt(a, b) && !dt_val_eq(a, b)));
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_GT_EQ: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_vm_push(vm, dt_to_bool(!dt_val_lt(a, b)));
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_LT: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_vm_push(vm, dt_to_bool(dt_val_lt(a, b)));
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_LT_EQ: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_vm_push(vm, dt_to_bool(dt_val_lt(a, b) || dt_val_eq(a, b)));
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_OR: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_val_ty ta = dt_typeof(a);
				dt_val_ty tb = dt_typeof(b);
				if (ta != tb) {
					dt_vm_push(vm, DT_FALSE);
				} else if (ta == DT_VAL_BOOL && tb == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_to_bool(dt_as_bool2(a) || dt_as_bool2(b)));
				} else {
					// TODO: eval to the later operand?
					dt_throw(
						vm,
						"cannot || a '%s' with '%s'",
						dt_typename(ta),
						dt_typename(tb)
					);
					dt_vm_push(vm, DT_FALSE);
				}
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_NIL_OR: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				if (dt_is_nil(a)) {
					dt_vm_push(vm, b);
				} else {
					dt_vm_push(vm, a);
				}
				dt_rc_check(a);
				dt_rc_check(b);
				break;
			}

			case DT_OP_AND: {
				dt_val b = dt_vm_pop_nocheck(vm);
				dt_val a = dt_vm_pop_nocheck(vm);
				dt_val_ty ta = dt_typeof(a);
				dt_val_ty tb = dt_typeof(b);
				if (ta != tb) {
					dt_vm_push(vm, DT_FALSE);
				} else if (ta == DT_VAL_BOOL && tb == DT_VAL_BOOL) {
					dt_vm_push(vm, dt_to_bool(dt_as_bool2(a) && dt_as_bool2(b)));
				} else {
					dt_throw(
						vm,
						"cannot && a '%s' with '%s'",
						dt_typename(ta),
						dt_typename(tb)
					);
					dt_vm_push(vm, DT_FALSE);
				}
				dt_rc_check(a);
				dt_rc_check(b);
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
				if (dt_typeof(name) == DT_VAL_STR) {
					dt_val val = dt_map_get(vm->globals, dt_as_str2(name));
					dt_vm_push(vm, val);
				} else {
					dt_throw(
						vm,
						"expected var name to be 'str' found '%s'",
						dt_typename(dt_typeof(name))
					);
					dt_vm_push(vm, DT_NIL);
				}
				break;
			}

			case DT_OP_SETG: {
				dt_val name = chunk->consts->values[*vm->ip++];
				dt_throw(vm, "cannot set global '%s'", dt_as_str2(name)->chars);
				break;
			}

			case DT_OP_GETL: {
				dt_vm_push(vm, vm->stack_bot[*vm->ip++]);
				break;
			}

			case DT_OP_SETL: {
				int pos = *vm->ip++;
				dt_rc_dec(vm->stack_bot[pos]);
				dt_rc_inc(dt_vm_get(vm, 0));
				vm->stack_bot[pos] = dt_vm_get(vm, 0);
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

			// TODO: cache
			case DT_OP_ARGS: {
				dt_arr* args = dt_arr_new(vm);
				for (int i = 0; i < vm->func->logic->nargs; i++) {
					dt_arr_push(vm, args, vm->stack_bot[i]);
				}
				dt_vm_push(vm, dt_to_arr(args));
				break;
			}

			case DT_OP_GETI: {
				dt_val key = dt_vm_pop_nocheck(vm);
				dt_val val = dt_vm_pop_nocheck(vm);
				switch (dt_typeof(val)) {
					case DT_VAL_ARR: {
						dt_arr* arr = dt_as_arr2(val);
						if (dt_typeof(key) == DT_VAL_NUM) {
							int n = dt_as_num2(key);
							if (n >= arr->len) {
								dt_vm_push(vm, DT_NIL);
								dt_throw(
									vm,
									"arr index out of bound: %d",
									n
								);
								break;
							} else {
								dt_vm_push(vm, dt_arr_get(arr, n));
							}
						} else if (dt_typeof(key) == DT_VAL_STR) {
							// TODO
							dt_vm_push(vm, DT_NIL);
						} else if (dt_typeof(key) == DT_VAL_RANGE) {
							dt_range range = dt_as_range2(key);
							if (range.end < range.start) {
								range = (dt_range) {
									.start = range.end,
									.end = range.start,
								};
							}
							if (range.start >= arr->len || range.end > arr->len) {
								dt_vm_push(vm, DT_NIL);
							} else {
								dt_vm_push(vm, dt_to_arr(dt_arr_sub(
									vm,
									arr,
									range.start,
									range.end
								)));
							}
						} else {
							dt_throw(
								vm,
								"invalid arr idx type '%s'",
								dt_typename(dt_typeof(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					}
					case DT_VAL_MAP:
						if (dt_typeof(key) == DT_VAL_STR) {
							dt_vm_push(vm, dt_map_get(dt_as_map2(val), dt_as_str2(key)));
						} else {
							dt_throw(
								vm,
								"invalid map idx type '%s'",
								dt_typename(dt_typeof(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					case DT_VAL_STR:
						if (dt_typeof(key) == DT_VAL_NUM) {
							int idx = (int)dt_as_num2(key);
							dt_str* str = dt_as_str2(val);
							dt_vm_push(vm, dt_to_str(dt_str_get(vm, str, idx)));
						} else if (dt_typeof(key) == DT_VAL_STR) {
							// TODO
							dt_vm_push(vm, DT_NIL);
						} else if (dt_typeof(key) == DT_VAL_RANGE) {
							dt_range range = dt_as_range2(key);
							if (range.end < range.start) {
								range = (dt_range) {
									.start = range.end,
									.end = range.start,
								};
							}
							dt_str* str = dt_as_str2(val);
							if (range.start > str->len || range.end > str->len) {
								dt_vm_push(vm, DT_NIL);
							} else {
								dt_vm_push(vm, dt_to_str(dt_str_sub(
									vm,
									str,
									range.start,
									range.end
								)));
							}
						} else {
							dt_throw(
								vm,
								"invalid str idx type '%s'",
								dt_typename(dt_typeof(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					case DT_VAL_NUM:
						if (dt_typeof(key) == DT_VAL_STR) {
							// TODO
							dt_vm_push(vm, DT_NIL);
						}
					default:
						dt_throw(
							vm,
							"failed to get idx for a '%s'",
							dt_typename(dt_typeof(val))
						);
						dt_vm_push(vm, DT_NIL);
						break;
				}
				dt_rc_check(key);
				dt_rc_check(val);
				break;
			}

			case DT_OP_SETI: {
				dt_val val = dt_vm_pop_nocheck(vm);
				dt_val key = dt_vm_pop_nocheck(vm);
				dt_val parent = dt_vm_pop_nocheck(vm);
				switch (dt_typeof(parent)) {
					case DT_VAL_ARR:
						if (dt_typeof(key) == DT_VAL_NUM) {
							int idx = dt_as_num2(key);
							dt_arr_set(vm, dt_as_arr2(parent), idx, val);
							dt_vm_push(vm, val);
						} else {
							dt_throw(
								vm,
								"invalid arr idx type '%s'",
								dt_typename(dt_typeof(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					case DT_VAL_MAP:
						if (dt_typeof(key) == DT_VAL_STR) {
							dt_map_set(vm, dt_as_map2(parent), dt_as_str2(key), val);
							dt_vm_push(vm, val);
						} else {
							dt_throw(
								vm,
								"invalid map idx type '%s'",
								dt_typename(dt_typeof(val))
							);
							dt_vm_push(vm, DT_NIL);
						}
						break;
					default:
						dt_throw(
							vm,
							"cannot set index a '%s'",
							dt_typename(dt_typeof(val))
						);
						dt_vm_push(vm, DT_NIL);
						break;
				}
				dt_rc_check(key);
				dt_rc_check(val);
				dt_rc_check(parent);
				break;
			}

			case DT_OP_CALL: {
				int nargs = *vm->ip++;
				dt_val func = dt_vm_get(vm, -nargs);
				dt_val ret = dt_vm_call(vm, func, nargs);
				// pop func
				dt_vm_pop(vm);
				dt_vm_push(vm, ret);
				break;
			}

			// TODO: clean
			case DT_OP_CALL2: {

				int nargs = *vm->ip++;

				dt_val name = dt_vm_pop_nocheck(vm);
				dt_val val = dt_vm_get(vm, -nargs);
				dt_map* meta = NULL;

				switch (dt_typeof(val)) {
					case DT_VAL_NUM: {
						meta = vm->math_lib;
						break;
					}
					case DT_VAL_STR: {
						meta = vm->str_lib;
						break;
					}
					case DT_VAL_ARR: {
						meta = vm->arr_lib;
						break;
					}
					case DT_VAL_MAP: {
						meta = vm->map_lib;
						break;
					}
					case DT_VAL_FUNC:
					case DT_VAL_CFUNC: {
						meta = vm->func_lib;
						break;
					}
					default:
						break;
				}

				if (meta) {
					dt_vm_push(vm, dt_vm_call(vm, dt_map_get(meta, dt_as_str2(name)), nargs + 1));
				} else {
					dt_throw(
						vm,
						"no func for '%s'",
						dt_typename(dt_typeof(val))
					);
					dt_vm_push(vm, DT_NIL);
				}

				dt_rc_check(name);
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
					dt_val v = dt_vm_pop_nocheck(vm);
					if (dt_typeof(v) == DT_VAL_RANGE) {
						dt_range r = dt_as_range2(v);
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
						dt_arr_set(vm, arr, len - 1 - i, v);
					}
					dt_rc_check(v);
				}
				dt_vm_push(vm, dt_to_arr(arr));
				break;
			}

			case DT_OP_MKMAP: {
				int len = *vm->ip++;
				dt_map* map = dt_map_new_len(vm, len);
				for (int i = 0; i < len; i++) {
					dt_val val = dt_vm_pop_nocheck(vm);
					dt_val key = dt_vm_pop_nocheck(vm);
					if (dt_typeof(key) == DT_VAL_STR) {
						dt_map_set(vm, map, dt_as_str2(key), val);
					} else {
						dt_throw(
							vm,
							"expected key to be 'str', found '%s'",
							dt_typename(dt_typeof(key))
						);
					}
					dt_rc_check(val);
					dt_rc_check(key);
				}
				dt_vm_push(vm, dt_to_map(map));
				break;
			}

			// TODO: clean
			case DT_OP_MKFUNC: {
				dt_val val = dt_vm_pop(vm);
				uint8_t num_upvals = *vm->ip++;
				if (dt_typeof(val) != DT_VAL_LOGIC) {
					// unreachable
					dt_throw(
						vm,
						"cannot make a func out of '%s'",
						dt_typename(dt_typeof(val))
					);
				}
				dt_func* func = dt_malloc(vm, sizeof(dt_func));
				func->logic = dt_as_logic2(val);
				func->num_upvals = num_upvals;
				func->upvals = dt_malloc(vm, sizeof(dt_upval*) * num_upvals);
				for (int i = 0; i < num_upvals; i++) {
					bool local = *vm->ip++;
					uint8_t idx = *vm->ip++;
					if (local) {
						bool found = false;
						for (int j = 0; j < vm->num_upvals; j++) {
							if (vm->open_upvals[j]->val == vm->stack_bot + idx) {
								found = true;
								func->upvals[i] = vm->open_upvals[j];
								break;
							}
						}
						if (!found) {
							dt_upval* upval = dt_malloc(vm, sizeof(dt_upval));
							upval->val = vm->stack_bot + idx;
							upval->captured = false;
							vm->open_upvals[vm->num_upvals++] = upval;
							upval->rc = 0;
							upval->type = DT_VAL_UPVAL;
							func->upvals[i] = upval;
						}
					} else {
						func->upvals[i] = vm->func->upvals[idx];
					}
				}
				func->rc = 0;
				func->type = DT_VAL_FUNC;
				dt_vm_push(vm, dt_to_func(func));
				break;
			}

			case DT_OP_JMP: {
				vm->ip += *vm->ip++ << 8 | *vm->ip++;
				break;
			}

			case DT_OP_JMP_COND: {
				int dis = *vm->ip++ << 8 | *vm->ip++;
				dt_val cond = dt_vm_pop_nocheck(vm);
				if (!dt_val_truthy(cond)) {
					vm->ip += dis;
				}
				dt_rc_check(cond);
				break;
			}

			case DT_OP_REWIND: {
				vm->ip -= *vm->ip++ << 8 | *vm->ip++;
				break;
			}

			case DT_OP_ITER_PREP: {
				dt_val iter = dt_vm_get(vm, 0);
				int dis = *vm->ip++ << 8 | *vm->ip++;
				switch (dt_typeof(iter)) {
					case DT_VAL_ARR:
						if (dt_as_arr2(iter)->len == 0) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						dt_vm_push(vm, dt_to_num(0));
						dt_vm_push(vm, dt_arr_get(dt_as_arr2(iter), 0));
						break;
					case DT_VAL_STR:
						if (dt_as_str2(iter)->len == 0) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						dt_vm_push(vm, dt_to_num(0));
						dt_vm_push(vm, dt_to_str(dt_str_new_len(
							vm,
							dt_as_str2(iter)->chars,
							1
						)));
						break;
					case DT_VAL_MAP:
						if (dt_as_map2(iter)->cnt == 0) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						int i = 0;
						for (; i < dt_as_map2(iter)->cap; i++) {
							if (dt_as_map2(iter)->entries[i].key) {
								break;
							}
						}
						dt_vm_push(vm, dt_to_num(i));
						dt_str* key = dt_as_map2(iter)->entries[i].key;
						dt_vm_push(vm, dt_to_str(key));
						break;
					case DT_VAL_RANGE:
						if (dt_as_range2(iter).start == dt_as_range2(iter).end) {
							dt_vm_pop(vm);
							vm->ip += dis;
							break;
						}
						dt_vm_push(vm, dt_to_num(0));
						dt_vm_push(vm, dt_to_num(dt_as_range2(iter).start));
						break;
					default:
						dt_throw(
							vm,
							"'%s' is not iterable",
							dt_typename(dt_typeof(iter))
						);
						dt_vm_pop(vm);
						vm->ip += dis;
						break;
				}
				break;
			}

			case DT_OP_ITER: {
				int dis = *vm->ip++ << 8 | *vm->ip++;
				dt_val iter = dt_vm_get(vm, -2);
				dt_val* n = vm->stack_top - 2;
				int idx = dt_as_num2(*n);
				idx++;
				*n = dt_to_num(idx);
				switch (dt_typeof(iter)) {
					case DT_VAL_ARR:
						if (idx < dt_as_arr2(iter)->len) {
							dt_vm_pop(vm);
							dt_vm_push(vm, dt_arr_get(dt_as_arr2(iter), idx));
							vm->ip -= dis;
						}
						break;
					case DT_VAL_STR:
						if (idx < dt_as_str2(iter)->len) {
							dt_vm_pop(vm);
							dt_vm_push(vm, dt_to_str(dt_str_new_len(
								vm,
								dt_as_str2(iter)->chars + idx,
								1
							)));
							vm->ip -= dis;
						}
						break;
					case DT_VAL_MAP:
						if (idx < dt_as_map2(iter)->cap) {
							while (idx < dt_as_map2(iter)->cap && !dt_as_map2(iter)->entries[idx].key) {
								idx++;
							}
							if (idx < dt_as_map2(iter)->cap) {
								dt_str* key = dt_as_map2(iter)->entries[idx].key;
								dt_vm_pop(vm);
								dt_vm_push(vm, dt_to_str(key));
								*n = dt_to_num(idx);
								vm->ip -= dis;
							}
						}
						break;
					case DT_VAL_RANGE: {
						int start = dt_as_range2(iter).start;
						int end = dt_as_range2(iter).end;
						bool done = false;
						if (end < start) {
							done = idx >= start - end;
						} else {
							done = idx >= end - start;
						}
						if (!done) {
							dt_vm_pop(vm);
							if (end < start) {
								dt_vm_push(vm, dt_to_num(start - idx));
							} else {
								dt_vm_push(vm, dt_to_num(start + idx));
							}
							vm->ip -= dis;
						}
						break;
					}
					default:
						dt_throw(
							vm,
							"'%s' is not iterable",
							dt_typename(dt_typeof(iter))
						);
						// unreachable
						break;
				}
				break;
			}

			case DT_OP_TRY_PREP: {
				vm->trying = vm->func;
				break;
			}

			case DT_OP_TRY: {
				uint8_t* prev_ip = vm->ip;
				dt_val catch = dt_vm_pop_nocheck(vm);
				dt_val try = dt_vm_pop_nocheck(vm);
				if (vm->throwing) {
					vm->throwing = false;
					// TODO: stack trace
					// TODO: file
					// TODO: deal with errors in this call
					dt_vm_push(vm, dt_call(
						vm,
						catch,
						2,
						dt_to_str(dt_str_new(vm, vm->err.msg)),
						dt_to_num(vm->err.line)
					));
					vm->ip = prev_ip;
					// TODO: restore previous
					vm->trying = NULL;
				} else {
					dt_vm_push(vm, try);
				}
				dt_rc_check(catch);
				dt_rc_check(try);
				break;
			}

			case DT_OP_THROW: {
				dt_val v = dt_vm_get(vm, 0);
				dt_val_ty vt = dt_typeof(v);
				if (dt_typeof(v) != DT_VAL_STR) {
					dt_throw(vm, "can only throw a str, found '%s'", dt_typename(vt));
					break;
				}
				dt_str* msg = dt_as_str(vm, v);
				dt_throw(vm, "%s", msg->chars);
				break;
			}

			case DT_OP_STOP: {
				return;
			}

		}

	}

}

void dt_vm_free(dt_vm* vm) {
// 	dt_gc_sweep(vm);
	dt_rc_dec(dt_to_map(vm->libs));
	dt_rc_dec(dt_to_map(vm->strs));
	memset(vm, 0, sizeof(dt_vm));
	if (vm->throwing) {
		free(vm->err.msg);
	}
}

dt_scanner dt_scanner_new(char* src) {
	return (dt_scanner) {
		.start = src,
		.cur = src,
		.line = 1,
	};
}

bool dt_scanner_ended(dt_scanner* s) {
  return *s->cur == '\0';
}

dt_token dt_scanner_make_token(dt_scanner* s, dt_token_ty type) {
	return (dt_token) {
		.type = type,
		.start = s->start,
		.len = (int)(s->cur - s->start),
		.line = s->line,
	};
}

bool dt_scanner_match(dt_scanner* s, char expected) {
	if (dt_scanner_ended(s)) {
		return false;
	}
	if (*s->cur != expected) {
		return false;
	}
	s->cur++;
	return true;
}

char dt_scanner_nxt(dt_scanner* s) {
	s->cur++;
	return s->cur[-1];
}

void dt_scanner_skip(dt_scanner* s, int n) {
	s->cur += n;
}

char dt_scanner_peek(dt_scanner* s) {
	return *s->cur;
}

char dt_scanner_peek_nxt(dt_scanner* s) {
	if (dt_scanner_ended(s)) {
		return '\0';
	}
	return s->cur[1];
}

char dt_scanner_peek_nxt_nxt(dt_scanner* s) {
	if (dt_scanner_ended(s)) {
		return '\0';
	}
	return s->cur[2];
}

void dt_scanner_skip_ws(dt_scanner* s) {
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

bool dt_is_digit(char c) {
	return c >= '0' && c <= '9';
}

bool dt_is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

dt_token dt_scanner_scan_str(dt_scanner* s) {

	char last = dt_scanner_peek(s);

	for (;;) {
		if (dt_scanner_ended(s)) {
			break;
		}
		char cur = dt_scanner_peek(s);
		if (cur == '"') {
			if (last != '\\') {
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

dt_token dt_scanner_scan_num(dt_scanner* s) {

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

bool dt_scanner_check_key(dt_scanner* s, char* key) {
	int len = strlen(key);
	return s->cur - s->start == len && memcmp(s->start, key, len) == 0;
}

dt_token_ty dt_scanner_ident_type(dt_scanner* s) {
	if        (dt_scanner_check_key(s, "T")) {
		return DT_TOKEN_T;
	} else if (dt_scanner_check_key(s, "F")) {
		return DT_TOKEN_F;
	} else {
		return DT_TOKEN_IDENT;
	}
}

dt_token dt_scanner_scan_ident(dt_scanner* s) {

	while (dt_is_alpha(dt_scanner_peek(s)) || dt_is_digit(dt_scanner_peek(s))) {
		dt_scanner_nxt(s);
	}

	return dt_scanner_make_token(s, dt_scanner_ident_type(s));

}

dt_token dt_scanner_scan(dt_scanner* s) {

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
		case '^': return dt_scanner_make_token(s, DT_TOKEN_CARET);
		case '$': return dt_scanner_make_token(s, DT_TOKEN_DOLLAR);
		case '?':
			if (dt_scanner_match(s, '?')) {
				return dt_scanner_make_token(s, DT_TOKEN_QUESTION_Q);
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_QUESTION);
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
			if (dt_scanner_match(s, '.')) {
				if (dt_scanner_match(s, '.')) {
					return dt_scanner_make_token(s, DT_TOKEN_DOT_DOT_DOT);
				} else {
					return dt_scanner_make_token(s, DT_TOKEN_DOT_DOT);
				}
			} else {
				return dt_scanner_make_token(s, DT_TOKEN_DOT);
			}
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

dt_funcenv dt_funcenv_new() {
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

void dt_funcenv_free(dt_funcenv* env) {
	dt_chunk_free(&env->chunk);
	memset(env, 0, sizeof(dt_funcenv));
}

dt_compiler dt_compiler_new(char* code) {

	dt_compiler c = (dt_compiler) {
		.scanner = dt_scanner_new(code),
		.parser = {0},
		.base_env = dt_funcenv_new(),
		.env = NULL,
	};

	c.env = &c.base_env;

	return c;

}

void dt_compiler_free(dt_compiler* c) {
	dt_funcenv_free(&c->base_env);
	memset(c, 0, sizeof(dt_compiler));
}

// TODO: don't exit
// compile time error
void dt_c_err(dt_compiler* c, char* fmt, ...) {
	dt_token* cur = &c->parser.cur;
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "line #%d: ", cur->line);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

// emit opcode
void dt_c_emit(dt_compiler* c, dt_op op) {
	dt_chunk_push(&c->env->chunk, op, c->parser.prev.line);
}

void dt_c_emit2(dt_compiler* c, dt_op op, uint8_t a1) {
	dt_c_emit(c, op);
	dt_c_emit(c, a1);
}

void dt_c_emit3(dt_compiler* c, dt_op op, uint8_t a1, uint8_t a2) {
	dt_c_emit(c, op);
	dt_c_emit(c, a1);
	dt_c_emit(c, a2);
}

// emit an opcode with distance operands
void dt_c_emit_jmp(dt_compiler* c, dt_op op, uint16_t dis) {
	dt_c_emit3(c, op, dis >> 8, dis & 0xff);
}

// emit an opcode for patching later with dt_c_patch_jmp()
int dt_c_emit_jmp_empty(dt_compiler* c, dt_op op) {
	dt_c_emit3(c, op, 0, 0);
	return c->env->chunk.cnt;
}

// patch an earlier emitted opcode with distance operands
void dt_c_patch_jmp(dt_compiler* c, int pos) {

	int dis = c->env->chunk.cnt - pos;

	if (dis >= UINT16_MAX) {
		dt_c_err(c, "jump too large\n");
	}

	c->env->chunk.code[pos - 2] = dis >> 8;
	c->env->chunk.code[pos - 1] = dis & 0xff;

}

// add a const value
void dt_c_push_const(dt_compiler* c, dt_val val) {
	uint16_t idx = dt_chunk_add_const(&c->env->chunk, val);
	dt_c_emit3(c, DT_OP_CONST, idx >> 8, idx & 0xff);
}

// advance to next token
dt_token dt_c_nxt(dt_compiler* c) {
	c->parser.prev = c->parser.cur;
	dt_token t = dt_scanner_scan(&c->scanner);
	c->parser.cur = t;
	if (t.type == DT_TOKEN_ERR) {
		dt_c_err(c, "unexpected token\n");
	}
	return c->parser.prev;
}

// check next token type
dt_token_ty dt_c_peek(dt_compiler* c) {
	return c->parser.cur.type;
}

// check next token type and proceed
bool dt_c_match(dt_compiler* c, dt_token_ty ty) {
	if (dt_c_peek(c) == ty) {
		dt_c_nxt(c);
		return true;
	}
	return false;
}

// compile error if next token is not *
void dt_c_expect(dt_compiler* c, dt_token_ty ty) {
	if (dt_c_peek(c) != ty) {
		dt_c_err(c, "expected token '%s'\n", dt_token_name(ty));
	}
}

// expect and proceed
dt_token dt_c_consume(dt_compiler* c, dt_token_ty ty) {
	dt_c_expect(c, ty);
	dt_c_nxt(c);
	return c->parser.prev;
}

// TODO
dt_type dt_c_this(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_AND);
	dt_c_emit(c, DT_OP_NIL);
	return DT_TYPE_NIL;
}

dt_type dt_c_num(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_NUM);
	dt_val num = dt_to_num(strtof(c->parser.prev.start, NULL));
	dt_c_push_const(c, num);
	return DT_TYPE_NUM;
}

dt_type dt_c_prec(dt_compiler* c, dt_prec prec);
dt_type dt_c_binary(dt_compiler* c, dt_type ty);

dt_type dt_c_expr(dt_compiler* c) {
	return dt_c_prec(c, DT_PREC_ASSIGN);
}

dt_type dt_c_str(dt_compiler* c) {
	dt_token str_t = dt_c_consume(c, DT_TOKEN_STR);
	dt_val str = dt_to_str(dt_str_new_len_escape(
		NULL,
		str_t.start + 1,
		str_t.len - 2
	));
	dt_c_push_const(c, str);
	return DT_TYPE_STR;
}

dt_type dt_c_lit(dt_compiler* c) {
	switch (dt_c_nxt(c).type) {
		case DT_TOKEN_QUESTION: dt_c_emit(c, DT_OP_NIL); return DT_TYPE_NIL;
		case DT_TOKEN_T: dt_c_emit(c, DT_OP_TRUE); return DT_TYPE_BOOL;
		case DT_TOKEN_F: dt_c_emit(c, DT_OP_FALSE); return DT_TYPE_BOOL;
		default: dt_c_err(c, "cannot process as literal\n"); return DT_TYPE_NIL;
	}
}

bool dt_type_eq(dt_type* t1, dt_type* t2) {
	return t1->type == t2->type && t1->idx == t2->idx;
}

// TODO: [ ..arr ] spread syntax
// TODO: store arr inner type
dt_type dt_c_arr(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_LBRACKET);

	int len = 0;
	dt_type inner = DT_TYPE_NIL;

	while (dt_c_peek(c) != DT_TOKEN_RBRACKET) {
		dt_type t = dt_c_expr(c);
// 		if (!dt_type_eq(&inner, &t) && len > 0) {
// 			dt_c_err(
// 				c,
// 				"inconsistent arr type: %s vs %s\n",
// 				dt_typename(inner.type),
// 				dt_typename(t.type)
// 			);
// 		}
		inner = t;
		len++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RBRACKET);
	dt_c_emit2(c, DT_OP_MKARR, len);

	return (dt_type) {
		.type = DT_VAL_ARR,
	};

}

// TODO: store map inner type
dt_type dt_c_map(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_LBRACE);

	int len = 0;
	dt_type inner = DT_TYPE_NIL;

	while (dt_c_peek(c) != DT_TOKEN_RBRACE) {
		if (dt_c_peek(c) == DT_TOKEN_IDENT) {
			dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
			// TODO
			dt_c_push_const(c, dt_to_str(dt_str_new_len(NULL, name.start, name.len)));
		} else if (dt_c_peek(c) == DT_TOKEN_STR) {
			dt_c_str(c);
		}
		dt_c_consume(c, DT_TOKEN_COLON);
		dt_type t = dt_c_expr(c);
// 		if (!dt_type_eq(&inner, &t) && len > 0) {
// 			dt_c_err(
// 				c,
// 				"inconsistent arr type: %s vs %s\n",
// 				dt_typename(inner.type),
// 				dt_typename(t.type)
// 			);
// 		}
		inner = t;
		len++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RBRACE);
	dt_c_emit2(c, DT_OP_MKMAP, len);

	return (dt_type) {
		.type = DT_VAL_MAP,
	};

}

int dt_c_find_local(dt_compiler* c, dt_token* name) {

	dt_funcenv* e = c->env;

	for (int i = e->num_locals - 1; i >= 0; i--) {
		dt_c_local* val = &e->locals[i];
		if (dt_token_eq(name, &val->name)) {
			return i;
		}
	}

	return -1;

}

int dt_c_add_upval(dt_compiler* c, int idx, bool local) {

	dt_funcenv* e = c->env;

	// find existing upval
	for (int i = 0; i < e->num_upvals; i++) {
		dt_c_upval* val = &e->upvals[i];
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

int dt_c_find_upval(dt_compiler* c, dt_token* name) {

	dt_funcenv* cur_env = c->env;

	if (!cur_env->parent) {
		return -1;
	}

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

dt_c_local* dt_c_add_local(dt_compiler* c, dt_token name, dt_type type) {
	if (c->env->num_locals >= UINT8_MAX) {
		dt_c_err(c, "too many local variables in one scope\n");
		return NULL;
	}
	if (
		dt_c_find_local(c, &name) != -1
		|| dt_c_find_upval(c, &name) != -1)
	{
		dt_c_err(c, "duplicate decl '%.*s'\n", name.len, name.start);
	}
	dt_c_local* l = &c->env->locals[c->env->num_locals++];
	l->name = name;
	l->depth = c->env->cur_depth;
	l->captured = false;
	l->type = type;
	return l;
}

void dt_c_skip_local(dt_compiler* c) {
	if (c->env->num_locals >= UINT8_MAX) {
		dt_c_err(c, "too many local variables in one scope\n");
		return;
	}
	dt_c_local* l = &c->env->locals[c->env->num_locals++];
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
void dt_c_typesig(dt_compiler* c) {
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
// 	printf("%.*s\n", name.len, name.start);
}

// TODO
void dt_c_struct(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_BANG);
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);

	dt_c_consume(c, DT_TOKEN_LBRACE);

	dt_struct_def ty = {0};

	strncpy(ty.name, name.start, name.len);

	while (dt_c_peek(c) != DT_TOKEN_RBRACE) {
		int mem_idx = ty.num_mems++;
		dt_token mem_name = dt_c_consume(c, DT_TOKEN_IDENT);
		dt_c_consume(c, DT_TOKEN_COLON);
		dt_c_typesig(c);
		dt_c_match(c, DT_TOKEN_COMMA);
		strncpy(ty.mem_names[mem_idx], mem_name.start, mem_name.len);
	}

	dt_c_consume(c, DT_TOKEN_RBRACE);

}

void dt_c_decl(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_DOLLAR);
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
	if (dt_c_match(c, DT_TOKEN_COLON)) {
		dt_c_typesig(c);
	}
	dt_c_consume(c, DT_TOKEN_EQ);
	dt_type t = dt_c_expr(c);
	dt_c_local* l = dt_c_add_local(c, name, DT_TYPE_NIL);
	l->type = t;
}

void dt_c_scope_begin(dt_compiler* c, dt_block_ty ty) {
	c->env->scopes[c->env->cur_depth++] = ty;
}

// end a scope and pop / close all locals off the stack
void dt_c_scope_end(dt_compiler* c) {

	dt_funcenv* e = c->env;

	e->cur_depth--;

	while (e->num_locals > 0) {
		dt_c_local* l = &e->locals[e->num_locals - 1];
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

void dt_c_stmt(dt_compiler* c);

int dt_c_block(dt_compiler* c, dt_block_ty ty) {

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
dt_type dt_c_cond_inner(dt_compiler* c) {

	// TODO: don't require paren
	dt_c_consume(c, DT_TOKEN_LPAREN);
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RPAREN);

	dt_type type = DT_TYPE_NIL;
	int if_start = dt_c_emit_jmp_empty(c, DT_OP_JMP_COND);

	if (dt_c_peek(c) == DT_TOKEN_LBRACE) {
		dt_c_block(c, DT_BLOCK_COND);
		dt_c_emit(c, DT_OP_NIL);
	} else {
		type = dt_c_expr(c);
	}

	int if_dis = c->env->chunk.cnt - if_start + 3;

	if (dt_c_match(c, DT_TOKEN_OR)) {

		int pos = dt_c_emit_jmp_empty(c, DT_OP_JMP);
		dt_type type2 = DT_TYPE_NIL;

		if (dt_c_peek(c) == DT_TOKEN_LPAREN) {
			type2 = dt_c_cond_inner(c);
		} else {
			if (dt_c_peek(c) == DT_TOKEN_LBRACE) {
				dt_c_block(c, DT_BLOCK_COND);
				dt_c_emit(c, DT_OP_NIL);
			} else {
				type2 = dt_c_expr(c);
			}
		}

// 		if (!dt_type_eq(&type, &type2)) {
// 			dt_c_err(
// 				c,
// 				"unmatched type in branches: %s vs %s\n",
// 				dt_typename(type.type),
// 				dt_typename(type2.type)
// 			);
// 		}

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

	return type;

}

// conditionals
// % (<bool>) <block> | (<bool>)? <block>
dt_type dt_c_cond(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_PERCENT);
	return dt_c_cond_inner(c);
}

void dt_c_add_break(dt_compiler* c, bool cont) {
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
		dt_c_local l = c->env->locals[i];
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

dt_type dt_c_break(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_AT_GT);
	dt_c_add_break(c, false);
	dt_c_emit(c, DT_OP_NIL);
	return DT_TYPE_NIL;
}

dt_type dt_c_continue(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_AT_CARET);
	dt_c_add_break(c, true);
	dt_c_emit(c, DT_OP_NIL);
	return DT_TYPE_NIL;
}

// patch previous loop breaks
void dt_c_patch_breaks(dt_compiler* c, int depth, bool cont) {
	for (int i = 0; i < c->env->num_breaks; i++) {
		dt_break b = c->env->breaks[i];
		if (b.cont == cont && b.depth == depth) {
			dt_c_patch_jmp(c, b.pos);
			c->env->breaks[i--] = c->env->breaks[--c->env->num_breaks];
		}
	}
}

// TODO: accept expr
dt_type dt_c_loop(dt_compiler* c) {

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
		dt_c_add_local(c, name, DT_TYPE_NIL);
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

	return DT_TYPE_NIL;

}

dt_type dt_c_return(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_TILDE_GT);
	dt_type type = dt_c_expr(c);
	dt_c_emit(c, DT_OP_STOP);
	return type;
}

dt_type dt_c_throw(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_COLON_LPAREN);
	dt_type type = dt_c_expr(c);
	dt_c_emit(c, DT_OP_THROW);
	return type;
}

dt_type dt_c_try(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_COLON_RPAREN);
	dt_c_emit(c, DT_OP_TRY_PREP);
	dt_type type = dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_OR);
	dt_c_expr(c);
	dt_c_emit(c, DT_OP_TRY);
	return type;
}

dt_type dt_c_func2(dt_compiler* c);

void dt_c_stmt(dt_compiler* c) {
	switch (dt_c_peek(c)) {
		case DT_TOKEN_BANG:   dt_c_struct(c); break;
		case DT_TOKEN_DOLLAR: dt_c_decl(c); break;
		case DT_TOKEN_LBRACE: dt_c_block(c, DT_BLOCK_NORMAL); break;
		case DT_TOKEN_TILDE: dt_c_func2(c); break;
		default: {
			dt_c_expr(c);
			dt_c_emit(c, DT_OP_POP);
		};
	}
}

dt_type dt_c_index(dt_compiler* c, dt_type ty) {
	dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RBRACKET);
	if (dt_c_match(c, DT_TOKEN_EQ)) {
		dt_c_expr(c);
		dt_c_emit(c, DT_OP_SETI);
	} else {
		dt_c_emit(c, DT_OP_GETI);
	}
	return DT_TYPE_NIL;
}

dt_type dt_c_index2(dt_compiler* c, dt_type ty) {
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
	// TODO
	dt_c_push_const(c, dt_to_str(dt_str_new_len(NULL, name.start, name.len)));
	if (dt_c_match(c, DT_TOKEN_EQ)) {
		dt_c_expr(c);
		dt_c_emit(c, DT_OP_SETI);
	} else {
		dt_c_emit(c, DT_OP_GETI);
	}
	return DT_TYPE_NIL;
}

dt_type dt_c_index3(dt_compiler* c, dt_type ty) {

	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
	dt_c_consume(c, DT_TOKEN_LPAREN);

	int nargs = 0;

	while (dt_c_peek(c) != DT_TOKEN_RPAREN) {
		dt_c_expr(c);
		nargs++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RPAREN);
	dt_c_push_const(c, dt_to_str(dt_str_new_len(NULL, name.start, name.len)));
	dt_c_emit2(c, DT_OP_CALL2, nargs);

	return DT_TYPE_NIL;

}

dt_type dt_c_call(dt_compiler* c, dt_type ty) {

	int nargs = 0;

	while (dt_c_peek(c) != DT_TOKEN_RPAREN) {
		dt_c_expr(c);
		nargs++;
		dt_c_match(c, DT_TOKEN_COMMA);
	}

	dt_c_consume(c, DT_TOKEN_RPAREN);
	dt_c_emit2(c, DT_OP_CALL, nargs);

	return DT_TYPE_NIL;

}

dt_type dt_c_args(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_DOT_DOT_DOT);
	dt_c_emit(c, DT_OP_ARGS);
	return DT_TYPE_NIL;
}

dt_type dt_c_ident(dt_compiler* c) {

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

		// assign
		if (dt_c_match(c, DT_TOKEN_EQ)) {

			if (set_op == DT_OP_SETG) {
				dt_c_err(c, "cannot set global\n");
			}

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

	return DT_TYPE_NIL;

}

dt_type dt_c_group(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_LPAREN);
	dt_type type = dt_c_expr(c);
	dt_c_consume(c, DT_TOKEN_RPAREN);
	return type;
}

dt_type dt_c_unary(dt_compiler* c) {
	dt_c_nxt(c);
	dt_token_ty ty = c->parser.prev.type;
	dt_c_prec(c, DT_PREC_UNARY);
	switch (ty) {
		case DT_TOKEN_MINUS:
			dt_c_emit(c, DT_OP_NEG);
			return DT_TYPE_NUM;
		case DT_TOKEN_BANG:
			dt_c_emit(c, DT_OP_NOT);
			return DT_TYPE_BOOL;
		case DT_TOKEN_HASH:
			dt_c_emit(c, DT_OP_LEN);
			return DT_TYPE_NUM;
		default:
			return DT_TYPE_NIL;
	}
}

dt_type dt_c_func_inner(dt_compiler* c) {

	dt_c_consume(c, DT_TOKEN_LPAREN);

	dt_funcenv env = dt_funcenv_new();

	dt_funcenv* prev_env = c->env;
	c->env = &env;
	c->env->parent = prev_env;

	int nargs = 0;

	while (dt_c_peek(c) != DT_TOKEN_RPAREN) {

		dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
		dt_c_add_local(c, name, DT_TYPE_NIL);
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

	dt_logic* logic = dt_malloc(NULL, sizeof(dt_logic));
	logic->nargs = nargs;
	logic->chunk = env.chunk;
	logic->rc = 0;
	logic->type = DT_VAL_LOGIC;

	dt_c_push_const(c, dt_to_logic(logic));

	dt_c_emit(c, DT_OP_MKFUNC);
	dt_c_emit(c, env.num_upvals);

	for (int i = 0; i < env.num_upvals; i++) {
		dt_c_emit(c, env.upvals[i].local);
		dt_c_emit(c, env.upvals[i].idx);
	}

	// TODO
	return (dt_type) {
		.type = DT_VAL_FUNC,
		.idx = -1,
	};

}

dt_type dt_c_func(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_TILDE);
	return dt_c_func_inner(c);
}

// TODO: store name
dt_type dt_c_func2(dt_compiler* c) {
	dt_c_consume(c, DT_TOKEN_TILDE);
	dt_token name = dt_c_consume(c, DT_TOKEN_IDENT);
	dt_c_add_local(c, name, DT_TYPE_NIL);
	return dt_c_func_inner(c);
}

dt_parse_rule dt_expr_rules[] = {
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
	[DT_TOKEN_BANG_EQ]       = { NULL,          dt_c_binary, DT_PREC_CMP },
	[DT_TOKEN_EQ]            = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_EQ_EQ]         = { NULL,          dt_c_binary, DT_PREC_CMP },
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
	[DT_TOKEN_QUESTION_Q]    = { NULL,          dt_c_binary, DT_PREC_LOGIC },
	[DT_TOKEN_ERR]           = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_END]           = { NULL,          NULL,        DT_PREC_NONE },
	[DT_TOKEN_AT]            = { dt_c_loop,     NULL,        DT_PREC_NONE },
	[DT_TOKEN_PERCENT]       = { dt_c_cond,     NULL,        DT_PREC_NONE },
	[DT_TOKEN_AT_GT]         = { dt_c_break,    NULL,        DT_PREC_NONE },
	[DT_TOKEN_AT_CARET]      = { dt_c_continue, NULL,        DT_PREC_NONE },
	[DT_TOKEN_TILDE]         = { dt_c_func,     NULL,        DT_PREC_NONE },
	[DT_TOKEN_COLON]         = { NULL,          dt_c_index3, DT_PREC_CALL },
	[DT_TOKEN_COLON_LPAREN]  = { dt_c_throw,    NULL,        DT_PREC_NONE },
	[DT_TOKEN_TILDE_GT]      = { dt_c_return,   NULL,        DT_PREC_NONE },
	[DT_TOKEN_COLON_RPAREN]  = { dt_c_try,      NULL,        DT_PREC_NONE } ,
};

dt_type dt_c_prec(dt_compiler* c, dt_prec prec) {
	dt_parse_rule* prev_rule = &dt_expr_rules[dt_c_peek(c)];
	if (prev_rule->prefix == NULL) {
		dt_c_err(c, "expected expression\n");
		return DT_TYPE_NIL;
	}
	dt_type ty = prev_rule->prefix(c);
	while (prec <= dt_expr_rules[dt_c_peek(c)].prec) {
		dt_c_nxt(c);
		ty = dt_expr_rules[c->parser.prev.type].infix(c, ty);
	}
	return ty;
}

dt_type dt_c_binary(dt_compiler* c, dt_type t1) {

	dt_token_ty ty = c->parser.prev.type;
	dt_parse_rule* rule = &dt_expr_rules[ty];
	dt_type t2 = dt_c_prec(c, rule->prec + 1);

	switch (ty) {
		case DT_TOKEN_PLUS:
			dt_c_emit(c, DT_OP_ADD);
			if (t1.type == DT_VAL_NUM && t2.type == DT_VAL_NUM) {
				return DT_TYPE_NUM;
			} else if (t1.type == DT_VAL_STR && t2.type == DT_VAL_STR) {
				return DT_TYPE_STR;
			} else {
// 				dt_c_err(
// 					c,
// 					"cannot add a %s and a %s\n",
// 					dt_typename(t1),
// 					dt_typename(t2)
// 				);
				return DT_TYPE_NIL;
			}
			break;
		case DT_TOKEN_MINUS:
			dt_c_emit(c, DT_OP_SUB);
			if (t1.type == DT_VAL_NUM && t2.type == DT_VAL_NUM) {
				return DT_TYPE_NUM;
			} else {
// 				dt_c_err(
// 					c,
// 					"cannot sub a %s and a %s\n",
// 					dt_typename(t1),
// 					dt_typename(t2)
// 				);
				return DT_TYPE_NIL;
			}
			break;
		case DT_TOKEN_STAR:
			dt_c_emit(c, DT_OP_MUL);
			if (t1.type == DT_VAL_NUM && t2.type == DT_VAL_NUM) {
				return DT_TYPE_NUM;
			} else {
// 				dt_c_err(
// 					c,
// 					"cannot mul a %s and a %s\n",
// 					dt_typename(t1),
// 					dt_typename(t2)
// 				);
				return DT_TYPE_NIL;
			}
			break;
		case DT_TOKEN_SLASH:
			dt_c_emit(c, DT_OP_DIV);
			if (t1.type == DT_VAL_NUM && t2.type == DT_VAL_NUM) {
				return DT_TYPE_NUM;
			} else {
// 				dt_c_err(
// 					c,
// 					"cannot div a %s and a %s\n",
// 					dt_typename(t1),
// 					dt_typename(t2)
// 				);
				return DT_TYPE_NIL;
			}
			break;
		case DT_TOKEN_QUESTION_Q:
			dt_c_emit(c, DT_OP_NIL_OR);
			return DT_TYPE_NIL;
		case DT_TOKEN_DOT_DOT:
			dt_c_emit(c, DT_OP_SPREAD);
			return DT_TYPE_RANGE;
		case DT_TOKEN_EQ_EQ:
			dt_c_emit(c, DT_OP_EQ);
			return DT_TYPE_BOOL;
		case DT_TOKEN_BANG_EQ:
			dt_c_emit(c, DT_OP_EQ);
			dt_c_emit(c, DT_OP_NOT);
			return DT_TYPE_BOOL;
		case DT_TOKEN_GT:
			dt_c_emit(c, DT_OP_GT);
			return DT_TYPE_BOOL;
		case DT_TOKEN_GT_EQ:
			dt_c_emit(c, DT_OP_GT_EQ);
			return DT_TYPE_BOOL;
		case DT_TOKEN_LT:
			dt_c_emit(c, DT_OP_LT);
			return DT_TYPE_BOOL;
		case DT_TOKEN_LT_EQ:
			dt_c_emit(c, DT_OP_LT_EQ);
			return DT_TYPE_BOOL;
		case DT_TOKEN_OR_OR:
			dt_c_emit(c, DT_OP_OR);
			return DT_TYPE_BOOL;
		case DT_TOKEN_AND_AND:
			dt_c_emit(c, DT_OP_AND);
			return DT_TYPE_BOOL;
		default:
			return DT_TYPE_NIL;
	}

}

char* dt_read_file(char* path, size_t* osize) {

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

dt_val dt_f_print(dt_vm* vm) {

	int nargs = dt_nargs(vm);

	for (int i = 0; i < nargs; i++) {
		dt_val_print(dt_arg(vm, i));
		printf(" ");
	}

	printf("\n");

	return DT_NIL;

}

dt_val dt_f_typeof(dt_vm* vm) {
	dt_val val = dt_arg(vm, 0);
	char* tname = dt_typename(dt_typeof(val));
	return dt_to_str(dt_str_new(vm, tname));
}

dt_val dt_f_import(dt_vm* vm) {
	if (!dt_check_args(vm, 1, DT_VAL_STR)) return DT_NIL;
	dt_str* name = dt_arg_str(vm, 0);
	dt_val lib = dt_map_get(vm->libs, name);
	if (dt_is_nil(lib)) {
		return dt_dofile(vm, dt_arg_cstr(vm, 0));
	}
	return lib;
}

dt_val dt_f_assert(dt_vm* vm) {
	dt_val v = dt_arg(vm, 0);
	dt_str* msg = dt_arg_str(vm, 1);
	if (!dt_val_truthy(v)) {
		dt_throw(vm, "%s", msg->chars);
	}
	return DT_NIL;
}

// TODO: compiletime error as runtime error here
dt_val dt_f_eval(dt_vm* vm) {
	if (!dt_check_args(vm, 1, DT_VAL_STR)) return DT_NIL;
	return dt_eval(vm, dt_arg_cstr(vm, 0));
}

dt_val dt_f_dofile(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	return dt_dofile(vm, dt_arg_cstr(vm, 0));
}

dt_val dt_f_str_replace(dt_vm* vm) {
	if (!dt_check_args(vm,
		3,
		DT_VAL_STR,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* old = dt_arg_str(vm, 1);
	dt_str* new = dt_arg_str(vm, 2);
	return dt_to_str(dt_str_replace(vm, str, old, new));
}

dt_val dt_f_str_replace_at(dt_vm* vm) {
	// TODO: interface to variable arg type
	if (!dt_check_args(vm,
		3,
		DT_VAL_STR,
		DT_VAL_NUM,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	// TODO: or range
	dt_val pos = dt_arg(vm, 1);
	dt_str* new = dt_arg_str(vm, 2);
	int len = str->len - 1 + new->len;
	// TODO
	return dt_to_str(str);
}

dt_val dt_f_str_tonum(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	return dt_to_num(atoi(str->chars));
}

dt_val dt_f_str_code(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	return dt_to_num((int)str->chars[0]);
}

dt_val dt_f_str_split(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* sep = dt_arg_str(vm, 1);
	return dt_to_arr(dt_str_split(vm, str, sep));
}

dt_val dt_f_str_chars(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	return dt_to_arr(dt_str_split(vm, str, dt_str_new(vm, "")));
}

dt_val dt_f_str_starts(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* str2 = dt_arg_str(vm, 1);
	return dt_to_bool(strncmp(str->chars, str2->chars, str2->len) == 0);
}


dt_val dt_f_str_ends(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* str2 = dt_arg_str(vm, 1);
	return dt_to_bool(strncmp(
		str->chars + str->len - str2->len,
		str2->chars,
		str2->len
	) == 0);
}

dt_val dt_f_str_rep(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_num n = dt_arg_num(vm, 1);
	return dt_to_str(dt_str_rep(vm, str, n));
}

dt_val dt_f_str_toupper(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	return dt_to_str(dt_str_toupper(vm, str));
}

dt_val dt_f_str_tolower(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	return dt_to_str(dt_str_tolower(vm, str));
}

dt_val dt_f_str_find(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* key = dt_arg_str(vm, 1);
	return dt_to_num(dt_str_find(str, key));
}

dt_val dt_f_str_find_at(dt_vm* vm) {
	if (!dt_check_args(vm,
		3,
		DT_VAL_STR,
		DT_VAL_STR,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* key = dt_arg_str(vm, 1);
	int idx = dt_arg_num(vm, 2);
	return dt_to_num(dt_str_find_at(str, key, idx));
}

dt_val dt_f_str_rfind(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* key = dt_arg_str(vm, 1);
	return dt_to_num(dt_str_rfind(str, key));
}

dt_val dt_f_str_rfind_at(dt_vm* vm) {
	if (!dt_check_args(vm,
		3,
		DT_VAL_STR,
		DT_VAL_STR,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* key = dt_arg_str(vm, 1);
	int idx = dt_arg_num(vm, 2);
	return dt_to_num(dt_str_rfind_at(str, key, idx));
}

dt_val dt_f_str_contains(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* key = dt_arg_str(vm, 1);
	return dt_to_bool(dt_str_contains(str, key));
}

dt_val dt_f_str_rev(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	return dt_to_str(dt_str_rev(vm, str));
}

dt_val dt_f_str_match(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_str* pat = dt_arg_str(vm, 1);
	return dt_to_map(dt_str_match(vm, str, pat));
}

dt_val dt_f_str_trim(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	return dt_to_str(dt_str_trim(vm, str));
}

dt_val dt_f_str_trunc(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_num len = dt_arg_num(vm, 1);
	dt_str* rest = dt_arg_str_or(vm, 2, NULL);
	return dt_to_str(dt_str_trunc(vm, str, len, rest));
}

// TODO: make third optional
dt_val dt_f_str_sub(dt_vm* vm) {
	if (!dt_check_args(vm,
		3,
		DT_VAL_STR,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	dt_num a = dt_arg_num(vm, 1);
	dt_num b = dt_arg_num(vm, 2);
	return dt_to_str(dt_str_sub(vm, str, a, b));
}

dt_val dt_f_str_len(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	return dt_to_num(str->len);
}

dt_val dt_f_str_hash(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	dt_str* str = dt_arg_str(vm, 0);
	return dt_to_num(str->hash);
}

dt_val dt_f_arr_push(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	int nargs = dt_nargs(vm);
	for (int i = 0; i < nargs - 1; i++) {
		dt_arr_push(vm, arr, dt_arg(vm, i + 1));
	}
	return dt_to_arr(arr);
}

dt_val dt_f_arr_insert(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_num idx = dt_arg_num(vm, 1);
	dt_val item = dt_arg(vm, 2);
	dt_arr_insert(vm, arr, idx, item);
	return dt_to_arr(arr);
}

dt_val dt_f_arr_rm(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_ARR,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_num idx = dt_arg_num(vm, 1);
	return dt_arr_rm(arr, idx);
}

dt_val dt_f_arr_rm_all(dt_vm* vm) {
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_val v = dt_arg(vm, 1);
	dt_arr_rm_all(arr, v);
	return DT_NIL;
}

dt_val dt_f_arr_find(dt_vm* vm) {
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_val v = dt_arg(vm, 1);
	int idx = dt_arr_find(arr, v);
	if (idx == -1) {
		return DT_NIL;
	} else {
		return dt_to_num(idx);
	}
}

dt_val dt_f_arr_map(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_val f = dt_arg(vm, 1);
	return dt_to_arr(dt_arr_map(vm, arr, f));
}

dt_val dt_f_arr_sort_def(dt_vm* vm) {
	dt_val v1 = dt_arg(vm, 0);
	dt_val v2 = dt_arg(vm, 1);
	return dt_to_bool(dt_val_lt(v1, v2));
}

dt_val dt_f_arr_sort(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_val f = dt_arg_or(vm, 1, dt_to_cfunc(dt_f_arr_sort_def));
	return dt_to_arr(dt_arr_sort(vm, arr, f));
}

dt_val dt_f_arr_each(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_val f = dt_arg(vm, 1);
	dt_arr_each(vm, arr, f);
	return DT_NIL;
}

dt_val dt_f_arr_filter(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_val f = dt_arg(vm, 1);
	return dt_to_arr(dt_arr_filter(vm, arr, f));
}

dt_val dt_f_arr_reduce(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_val init = dt_arg(vm, 1);
	dt_val f = dt_arg(vm, 2);
	return dt_arr_reduce(vm, arr, init, f);
}

dt_val dt_f_arr_contains(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_val v = dt_arg(vm, 1);
	return dt_to_bool(dt_arr_contains(arr, v));
}

dt_val dt_f_arr_join(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_ARR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_str* s = dt_arg_str(vm, 1);
	return dt_to_str(dt_arr_join(vm, arr, s));
}

dt_val dt_f_arr_rev(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	return dt_to_arr(dt_arr_rev(vm, arr));
}

dt_val dt_f_arr_pop(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	return dt_arr_pop(arr);
}

// TODO: make 3rd arg optional
dt_val dt_f_arr_sub(dt_vm* vm) {
	if (!dt_check_args(vm,
		3,
		DT_VAL_ARR,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	dt_num start = dt_arg_num(vm, 1);
	dt_num end = dt_arg_num(vm, 2);
	return dt_to_arr(dt_arr_sub(vm, arr, start, end));
}

dt_val dt_f_arr_concat(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_ARR,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* a1 = dt_arg_arr(vm, 0);
	dt_arr* a2 = dt_arg_arr(vm, 1);
	return dt_to_arr(dt_arr_concat(vm, a1, a2));
}

dt_val dt_f_arr_rand(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	return dt_arr_rand(arr);
}

dt_val dt_f_arr_clone(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	return dt_to_arr(dt_arr_clone(vm, arr));
}

dt_val dt_f_arr_eq(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_ARR,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* a1 = dt_arg_arr(vm, 0);
	dt_arr* a2 = dt_arg_arr(vm, 1);
	return dt_to_bool(dt_arr_eq(a1, a2));
}

dt_val dt_f_arr_len(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_ARR
	)) return DT_NIL;
	dt_arr* arr = dt_arg_arr(vm, 0);
	return dt_to_num(arr->len);
}

dt_val dt_f_map_keys(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_MAP
	)) return DT_NIL;
	dt_map* map = dt_arg_map(vm, 0);
	return dt_to_arr(dt_map_keys(vm, map));
}

dt_val dt_f_map_vals(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_MAP
	)) return DT_NIL;
	dt_map* map = dt_arg_map(vm, 0);
	return dt_to_arr(dt_map_vals(vm, map));
}

dt_val dt_f_map_each(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_MAP
	)) return DT_NIL;
	dt_map* map = dt_arg_map(vm, 0);
	dt_val f = dt_arg(vm, 1);
	dt_map_each(vm, map, f);
	return DT_NIL;
}

dt_val dt_f_map_rm(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_MAP,
		DT_VAL_STR
	)) return DT_NIL;
	dt_map* map = dt_arg_map(vm, 0);
	dt_str* key = dt_arg_str(vm, 1);
	return dt_map_rm(map, key);
}

dt_val dt_f_map_has(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_MAP,
		DT_VAL_STR
	)) return DT_NIL;
	dt_map* map = dt_arg_map(vm, 0);
	dt_str* key = dt_arg_str(vm, 1);
	return dt_to_bool(dt_map_has(map, key));
}

dt_val dt_f_map_eq(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_MAP,
		DT_VAL_MAP
	)) return DT_NIL;
	dt_map* m1 = dt_arg_map(vm, 0);
	dt_map* m2 = dt_arg_map(vm, 1);
	return dt_to_bool(dt_map_eq(m1, m2));
}

dt_val dt_f_map_clone(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_MAP
	)) return DT_NIL;
	dt_map* map = dt_arg_map(vm, 0);
	return dt_to_map(dt_map_clone(vm, map));
}

dt_val dt_f_func_call(dt_vm* vm) {
	dt_val func = dt_arg(vm, 0);
	dt_arr* args = dt_arg_arr(vm, 1);
	for (int i = 0; i < args->len; i++) {
		dt_vm_push(vm, args->values[i]);
	}
	return dt_vm_call(vm, func, args->len);
}

// TODO
dt_val dt_f_func_bind(dt_vm* vm) {
	dt_val func = dt_arg(vm, 0);
	return func;
}

dt_val dt_f_math_sin(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(sin(n));
}

dt_val dt_f_math_cos(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(cos(n));
}

dt_val dt_f_math_tan(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(tan(n));
}

dt_val dt_f_math_asin(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(asin(n));
}

dt_val dt_f_math_acos(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(acos(n));
}

dt_val dt_f_math_atan(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(atan(n));
}

dt_val dt_f_math_atan2(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	dt_num n2 = dt_arg_num(vm, 0);
	return dt_to_num(atan2(n, n2));
}

dt_val dt_f_math_isnan(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(isnan(n));
}

dt_val dt_f_math_abs(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(fabs(n));
}

dt_val dt_f_math_mod(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	dt_num n2 = dt_arg_num(vm, 1);
	return dt_to_num(fmod(n, n2));
}

dt_val dt_f_math_sqrt(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(sqrt(n));
}

dt_val dt_f_math_pow(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	dt_num p = dt_arg_num_or(vm, 1, 2);
	return dt_to_num(pow(n, p));
}

dt_val dt_f_math_log(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	if (dt_arg_exists(vm, 1)) {
		dt_num base = dt_arg_num(vm, 1);
		return dt_to_num(log(n) / log(base));
	} else {
		return dt_to_num(log(n));
	}
}

dt_val dt_f_math_round(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	int f = dt_arg_num_or(vm, 1, 0);
	int p = pow(10, f);
	return dt_to_num(round(n * p) / p);
}

dt_val dt_f_math_floor(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(floor(n));
}

dt_val dt_f_math_ceil(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(ceil(n));
}

dt_val dt_f_math_fract(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(n - floor(n));
}

dt_val dt_f_math_max(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num a = dt_arg_num(vm, 0);
	dt_num b = dt_arg_num(vm, 1);
	return dt_to_num(fmax(a, b));
}

dt_val dt_f_math_min(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num a = dt_arg_num(vm, 0);
	dt_num b = dt_arg_num(vm, 1);
	return dt_to_num(fmin(a, b));
}

dt_val dt_f_math_clamp(dt_vm* vm) {
	if (!dt_check_args(vm,
		3,
		DT_VAL_NUM,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num v = dt_arg_num(vm, 0);
	dt_num lo = dt_arg_num(vm, 1);
	dt_num hi = dt_arg_num(vm, 2);
	return dt_to_num(fmax(lo, fmin(hi, v)));
}

dt_val dt_f_math_map(dt_vm* vm) {
	if (!dt_check_args(vm,
		5,
		DT_VAL_NUM,
		DT_VAL_NUM,
		DT_VAL_NUM,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num v = dt_arg_num(vm, 0);
	dt_num a1 = dt_arg_num(vm, 1);
	dt_num b1 = dt_arg_num(vm, 2);
	dt_num a2 = dt_arg_num(vm, 3);
	dt_num b2 = dt_arg_num(vm, 4);
	return dt_to_num(a2 + (v - a1) / (b1 - a1) * (b2 - a2));
}

dt_val dt_f_math_mapc(dt_vm* vm) {
	if (!dt_check_args(vm,
		5,
		DT_VAL_NUM,
		DT_VAL_NUM,
		DT_VAL_NUM,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num v = dt_arg_num(vm, 0);
	dt_num a1 = dt_arg_num(vm, 1);
	dt_num b1 = dt_arg_num(vm, 2);
	dt_num a2 = dt_arg_num(vm, 3);
	dt_num b2 = dt_arg_num(vm, 4);
	dt_num v2 = a2 + (v - a1) / (b1 - a1) * (b2 - a2);
	return dt_to_num(fmax(a2, fmin(b2, v2)));
}

dt_val dt_f_math_deg(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(n * (180.0 / DT_PI));
}

dt_val dt_f_math_rad(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	return dt_to_num(n / (180.0 / DT_PI));
}

dt_val dt_f_math_sign(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	if (n > 0) {
		return dt_to_num(1);
	} else if (n < 0) {
		return dt_to_num(-1);
	} else {
		return dt_to_num(0);
	}
}

double dt_randf(double low, double hi) {
	return low + (double)rand() / (double)RAND_MAX * (hi - low);
}

dt_val dt_f_math_rand(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	switch (dt_nargs(vm)) {
		case 0:
			return dt_to_num(dt_randf(0, 1));
		case 1:
			return dt_to_num(floor(dt_randf(0, dt_arg_num(vm, 0))));
		case 2:
		default:
			return dt_to_num(floor(dt_randf(dt_arg_num(vm, 0), dt_arg_num(vm, 1))));
	}
}

// TODO: get current seed
dt_val dt_f_math_srand(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;
	dt_num n = dt_arg_num(vm, 0);
	srand(n);
	return DT_NIL;
}

#ifdef DT_LIB_SYS

dt_val dt_f_sys_exit(dt_vm* vm) {
	dt_num code = dt_arg_num_or(vm, 0, EXIT_SUCCESS);
	exit(code);
	return DT_NIL;
}

dt_val dt_f_sys_print(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* str = dt_arg_cstr(vm, 0);
	fprintf(stdout, "%s", str);
	return DT_NIL;
}

dt_val dt_f_sys_cwd(dt_vm* vm) {
	char* path = getcwd(NULL, 0);
	dt_str* str = dt_str_new(vm, path);
	free(path);
	return dt_to_str(str);
}

dt_val dt_f_sys_chdir(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	chdir(path);
	return DT_NIL;
}

dt_val dt_f_sys_fork(dt_vm* vm) {
	int pid = fork();
	return dt_to_num(pid);
}

#define DT_POPEN_SIZE 128

#if !defined(DT_IOS)
dt_val dt_f_sys_exec(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* cmd = dt_arg_cstr(vm, 0);
	system(cmd);
	return DT_NIL;
// 	char buf[DT_POPEN_SIZE];
// 	FILE* pipe = popen(cmd, "r");
// 	while (fgets(buf, DT_POPEN_SIZE, pipe) != NULL) {}
// 	pclose(pipe);
// 	return dt_to_str(dt_str_new(vm, buf));
}
#endif

dt_val dt_f_sys_getenv(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* var = dt_arg_cstr(vm, 0);
	char* val = getenv(var);
	if (val) {
		return dt_to_str(dt_str_new(vm, val));
	} else {
		return DT_NIL;
	}
}

dt_val dt_f_sys_setenv(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	char* var = dt_arg_cstr(vm, 0);
	char* val = dt_arg_cstr(vm, 1);
	setenv(var, val, 1);
	return DT_NIL;
}

#define DT_GETLINE_BUFSIZE 1024

dt_val dt_f_sys_getchar(dt_vm* vm) {
	char c = getchar();
	if (c == EOF) {
		return DT_NIL;
	}
	return dt_to_str(dt_str_new_len(vm, &c, 1));
}

dt_val dt_f_sys_getline(dt_vm* vm) {
	static char buf[DT_GETLINE_BUFSIZE];
	char c;
	int pos = 0;
	for (;;) {
		c = getchar();
		if (c == EOF || c == '\n') {
			buf[pos++] = '\0';
			break;
		}
		buf[pos++] = c;
	}
	return dt_to_str(dt_str_new_len(vm, buf, pos - 1));
}

void dt_f_sys_sigint_handler(int dummy) {
	// TODO
}

dt_val dt_f_sys_signal(dt_vm* vm) {
	dt_val f = dt_arg(vm, 0);
	signal(SIGINT, dt_f_sys_sigint_handler);
	return DT_NIL;
}

dt_val dt_f_sys_time(dt_vm* vm) {
	return dt_to_num(time(NULL));
}

dt_val dt_f_sys_date(dt_vm* vm) {
	time_t t = dt_arg_num_or(vm, 0, time(NULL));
	struct tm tm = *localtime(&t);
	dt_map* date = dt_map_new(vm);
	dt_map_cset(vm, date, "year", dt_to_num(tm.tm_year + 1900));
	dt_map_cset(vm, date, "month", dt_to_num(tm.tm_mon + 1));
	dt_map_cset(vm, date, "day", dt_to_num(tm.tm_mday));
	dt_map_cset(vm, date, "hour", dt_to_num(tm.tm_hour));
	dt_map_cset(vm, date, "min", dt_to_num(tm.tm_min));
	dt_map_cset(vm, date, "sec", dt_to_num(tm.tm_sec));
	return dt_to_map(date);
}

#endif

#ifdef DT_LIB_FS
dt_val dt_f_fs_readdir(dt_vm* vm) {

	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;

	char* path = dt_arg_cstr(vm, 0);
	DIR* dir = opendir(path);

	if (!dir) {
		dt_throw(vm, "failed to read dir '%s'", path);
		return DT_NIL;
	}

	struct dirent* entry;
	dt_arr* list = dt_arr_new(vm);

	while ((entry = readdir(dir))) {
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			dt_arr_push(vm, list, dt_to_str(dt_str_new(vm, entry->d_name)));
		}
	}

	return dt_to_arr(list);

}

// TODO: read bytes?
dt_val dt_f_fs_read(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	FILE* file = fopen(path, "r");
	if (!file) {
		dt_throw(vm, "failed to read file '%s'", path);
		return DT_NIL;
	}
	fseek(file, 0L, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	dt_str* content = dt_str_alloc(vm, size);
	size_t size_read = fread(content->chars, 1, size, file);
	content->chars[size_read] = '\0';
	fclose(file);
	dt_str_hash(content);
	return dt_to_str(content);
}

dt_val dt_f_fs_write(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	dt_str* content = dt_arg_str(vm, 1);
	FILE* file = fopen(path, "w");
	if (!file) {
		dt_throw(vm, "failed to write to file '%s'", path);
		return DT_NIL;
	}
	fwrite(content->chars, 1, content->len, file);
	fclose(file);
	return DT_NIL;
}

dt_val dt_f_fs_exists(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	struct stat st;
	return dt_to_bool(stat(path, &st) == 0);
}

dt_val dt_f_fs_isfile(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	struct stat st;
	return dt_to_bool(stat(path, &st) == 0 && S_ISREG(st.st_mode));
}

dt_val dt_f_fs_isdir(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	struct stat st;
	return dt_to_bool(stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

dt_val dt_f_fs_size(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	struct stat st;
    if (stat(path, &st) != 0) {
		dt_throw(vm, "failed to get size of file '%s'", path);
		return DT_NIL;
	}
	return dt_to_num(st.st_size);
}

dt_val dt_f_fs_lastmod(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	struct stat st;
    if (stat(path, &st) != 0) {
		dt_throw(vm, "failed to get lastmod of file '%s'", path);
		return DT_NIL;
	}
	return dt_to_num(st.st_mtime);
}

// TODO: let user pass perm with default 755?
dt_val dt_f_fs_mkdir(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	int status = mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if (status == -1) {
		dt_throw(vm, "failed to mkdir '%s'", path);
	}
	return DT_NIL;
}

// TODO: copy perm
dt_val dt_f_fs_copy(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	char* from = dt_arg_cstr(vm, 0);
	char* to = dt_arg_cstr(vm, 1);
	FILE* f1 = fopen(from, "rb");
	if (!f1) {
		dt_throw(vm, "failed to read file '%s'", from);
		return DT_NIL;
	}
	fseek(f1, 0L, SEEK_END);
	size_t size = ftell(f1);
	fseek(f1, 0, SEEK_SET);
	uint8_t* bytes = malloc(size);
	fread(bytes, 1, size, f1);
	fclose(f1);
	FILE* f2 = fopen(to, "wb");
	if (!f2) {
		dt_throw(vm, "failed to write to file '%s'", to);
		return DT_NIL;
	}
	fwrite(bytes, 1, size, f2);
	fclose(f2);
	return DT_NIL;
}

// TODO: remove dir
dt_val dt_f_fs_remove(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	int status = remove(path);
	if (status == -1) {
		dt_throw(vm, "failed to remove '%s'", path);
	}
	return DT_NIL;
}

dt_val dt_f_fs_move(dt_vm* vm) {
	if (!dt_check_args(vm,
		2,
		DT_VAL_STR,
		DT_VAL_STR
	)) return DT_NIL;
	dt_call(vm, dt_to_cfunc(dt_f_fs_copy), 2, dt_arg(vm, 0), dt_arg(vm, 1));
	dt_call(vm, dt_to_cfunc(dt_f_fs_remove), 1, dt_arg(vm, 0));
	return DT_NIL;
}

dt_val dt_f_fs_ext(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	char* dot = strrchr(path, '.');
	if (!dot) {
		return dt_to_str(dt_str_new(vm, ""));
	}
	return dt_to_str(dt_str_new(vm, dot + 1));
}

dt_val dt_f_fs_base(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	char* dot = strrchr(path, '.');
	char* slash = strrchr(path, '/');
	dot = dot ? dot : path + strlen(path);
	slash = slash ? slash + 1 : path;
	return dt_to_str(dt_str_new_len(vm, slash, dot - slash));
}

#if !defined(DT_WEB)
dt_val dt_f_fs_realpath(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	char* real = realpath(path, NULL);
	dt_str* str = dt_str_new(vm, real);
	free(real);
	return dt_to_str(str);
}
#endif

#if defined(__APPLE__)
#import <Foundation/Foundation.h>
#endif

dt_val dt_f_fs_resdir(dt_vm* vm) {
#if defined(__APPLE__)
	@autoreleasepool {
		const char* dir = [[[NSBundle mainBundle] resourcePath] UTF8String];
		return dt_to_str(dt_str_new(vm, (char*)dir));
	}
#endif
	return dt_to_str(dt_str_new(vm, ""));
}

// TODO
dt_val dt_f_fs_datadir(dt_vm* vm) {
	return dt_to_str(dt_str_new(vm, ""));
}

// TODO
dt_val dt_f_fs_watch(dt_vm* vm) {
	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;
	char* path = dt_arg_cstr(vm, 0);
	dt_val cb = dt_arg(vm, 1);
	return DT_NIL;
}

#endif

#ifdef DT_LIB_TERM

#define DT_GEN_ANSI_WRAP(name, code) \
	dt_val dt_f_term_##name(dt_vm* vm) { \
		if (!dt_check_args(vm, 1, DT_VAL_STR)) return DT_NIL; \
		char* str = dt_arg_cstr(vm, 0); \
		return dt_to_str(dt_str_fmt("\x1b[%dm%s\x1b[0m", code, str)); \
	}

DT_GEN_ANSI_WRAP(black,     30)
DT_GEN_ANSI_WRAP(red,       31)
DT_GEN_ANSI_WRAP(green,     32)
DT_GEN_ANSI_WRAP(yellow,    33)
DT_GEN_ANSI_WRAP(blue,      34)
DT_GEN_ANSI_WRAP(magenta,   35)
DT_GEN_ANSI_WRAP(cyan,      36)
DT_GEN_ANSI_WRAP(white,     37)
DT_GEN_ANSI_WRAP(blackbg,   40)
DT_GEN_ANSI_WRAP(redbg,     41)
DT_GEN_ANSI_WRAP(greenbg,   42)
DT_GEN_ANSI_WRAP(yellowbg,  43)
DT_GEN_ANSI_WRAP(bluebg,    44)
DT_GEN_ANSI_WRAP(magentabg, 45)
DT_GEN_ANSI_WRAP(cyanbg,    46)
DT_GEN_ANSI_WRAP(whitebg,   47)
DT_GEN_ANSI_WRAP(bold,      1)
DT_GEN_ANSI_WRAP(dim,       2)
DT_GEN_ANSI_WRAP(italic,    3)
DT_GEN_ANSI_WRAP(underline, 4)

dt_val dt_f_term_rgb(dt_vm* vm) {
	if (!dt_check_args(vm,
		4,
		DT_VAL_STR,
		DT_VAL_NUM,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	char* str = dt_arg_cstr(vm, 0);
	int r = dt_arg_num(vm, 1);
	int g = dt_arg_num(vm, 2);
	int b = dt_arg_num(vm, 3);
	return dt_to_str(dt_str_fmt("\x1b[38;2;%d;%d;%dm%s\x1b[0m", r, g, b, str));
}

dt_val dt_f_term_rgbbg(dt_vm* vm) {
	if (!dt_check_args(vm,
		4,
		DT_VAL_STR,
		DT_VAL_NUM,
		DT_VAL_NUM,
		DT_VAL_NUM
	)) return DT_NIL;
	char* str = dt_arg_cstr(vm, 0);
	int r = dt_arg_num(vm, 1);
	int g = dt_arg_num(vm, 2);
	int b = dt_arg_num(vm, 3);
	return dt_to_str(dt_str_fmt("\x1b[48;2;%d;%d;%dm%s\x1b[0m", r, g, b, str));
}

dt_val dt_f_term_enter_alt(dt_vm* vm) {
	return dt_to_str(dt_str_new(vm, "\x1b[?1049h"));
}

dt_val dt_f_term_exit_alt(dt_vm* vm) {
	return dt_to_str(dt_str_new(vm, "\x1b[?1049l"));
}

dt_val dt_f_term_move_cursor(dt_vm* vm) {
	int col = dt_arg_num_or(vm, 0, 1);
	int row = dt_arg_num_or(vm, 1, 1);
	return dt_to_str(dt_str_fmt("\x1b[%d;%dH", row, col));
}

dt_val dt_f_term_move_cursor_col(dt_vm* vm) {
	int col = dt_arg_num_or(vm, 0, 1);
	return dt_to_str(dt_str_fmt("\x1b[%dG", col));
}

dt_val dt_f_term_save_cursor(dt_vm* vm) {
	return dt_to_str(dt_str_new(vm, "\x1b[s"));
}

dt_val dt_f_term_restore_cursor(dt_vm* vm) {
	return dt_to_str(dt_str_new(vm, "\x1b[u"));
}

dt_val dt_f_term_hide_cursor(dt_vm* vm) {
	return dt_to_str(dt_str_new(vm, "\x1b[?25l"));
}

dt_val dt_f_term_show_cursor(dt_vm* vm) {
	return dt_to_str(dt_str_new(vm, "\x1b[?25h"));
}

dt_val dt_f_term_clear_line(dt_vm* vm) {
	int n = dt_arg_num_or(vm, 0, 2);
	return dt_to_str(dt_str_fmt("\x1b[%dK", n));
}

dt_val dt_f_term_clear_screen(dt_vm* vm) {
	int n = dt_arg_num_or(vm, 0, 2);
	return dt_to_str(dt_str_fmt("\x1b[%dJ", n));
}

#include <termios.h>

dt_val dt_f_term_no_echo(dt_vm* vm) {
	struct termios attrs;
	tcgetattr(STDIN_FILENO, &attrs);
	attrs.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &attrs);
	return DT_NIL;
}
#endif

#ifdef DT_LIB_HTTP

#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#define DT_HTTP_CHUNK_SIZE 1024
#define DT_HTTP_MAX_PFDS 256
#define DT_DATEBUF_SIZE 32

char* dt_read_all(int fd, size_t chunk_size, size_t* osize) {

	char* data = malloc(chunk_size);
	int total_size = 0;

	for (;;) {
		int size = read(fd, data + total_size, chunk_size);
		total_size += size;
		if (size < chunk_size) {
			break;
		}
		data = realloc(data, total_size + chunk_size);
	}

	if (osize) {
		*osize = total_size;
	}

	return data;

}

void dt_write_fmt(int fd, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	size_t size = vsnprintf(NULL, 0, fmt, args);
	va_end(args);
	char* buf = malloc(size + 1);
	va_start(args, fmt);
	vsnprintf(buf, size + 1, fmt, args);
	va_end(args);
	write(fd, buf, size);
	free(buf);
}

char* dt_http_status_txt[] = {

	// 000
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

	// 100
	"Continue",
	"Switching Protocols",
	"",
	"Early Hints",
	"", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

	// 200
	"OK",
	"Created",
	"Accepted",
	"Non-Authoritative Information",
	"No Content",
	"Reset Content",
	"Partial Content",
	"", "","",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

	// 300
	"Multiple Choices",
	"Moved Permanently",
	"Found",
	"See Other",
	"Not Modified",
	"Use Proxy",
	"",
	"Temporary Redirect",
	"Permanent Redirect",
	"",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

	// 400
	"Bad Request",
	"Unauthorized",
	"Payment Required",
	"Forbidden",
	"Not Found",
	"Method Not Allowed",
	"Not Acceptable",
	"Proxy Authentication Required",
	"Request Timeout",
	"Conflict",
	"Gone",
	"Length Required",
	"Precondition Failed",
	"Payload Too Large",
	"URI Too Long",
	"Unsupported Media Type",
	"Range Not Satisfiable",
	"Expectation Failed",
	"I'm a teapot",
	"",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

	// 500
	"Internal Server Error",
	"Not Implemented",
	"Bad Gateway",
	"Service Unavailable",
	"Gateway Timeout",
	"HTTP Version Not Supported",
	"Variant Also Negotiates",
	"Insufficient Storage",
	"Loop Detected",
	"",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

};

dt_val dt_f_http_serve(dt_vm* vm) {

	if (!dt_check_args(vm,
		1,
		DT_VAL_NUM
	)) return DT_NIL;

	int port = dt_arg_num(vm, 0);
	dt_val cb = dt_arg(vm, 1);
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_fd == -1) {
		dt_throw(vm, "failed to create socket");
		return DT_NIL;
	}

	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, (int[]){1}, sizeof(int));
	ioctl(sock_fd, FIONBIO, (int[]){1});

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_addr = {
			.s_addr = INADDR_ANY,
		},
		.sin_port = htons(port),
	};

	if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		switch (errno) {
			case EACCES:
				dt_throw(vm, "port %d is in protected", port);
				return DT_NIL;
			case EADDRINUSE:
				dt_throw(vm, "port %d is in use", port);
				return DT_NIL;
			default:
				dt_throw(vm, "failed to bind socket");
				return DT_NIL;
		}
	}

	listen(sock_fd, 64);

	struct pollfd pfds[DT_HTTP_MAX_PFDS] = {
		[0] = {
			.fd = sock_fd,
			.events = POLLIN,
		},
	};

	int num_pfds = 1;

	for (;;) {

		// TODO: (macos) sometimes this blocks on actual pending requests
		int poll_res = poll(pfds, num_pfds, -1);

		if (poll_res <= 0) {
			dt_throw(vm, "poll err");
			continue;
		}

		if(pfds[0].revents & POLLIN) {
			for (;;) {
				int conn_fd = accept(sock_fd, NULL, NULL);
				if (conn_fd < 0) {
					break;
				}
				if (num_pfds + 1 >= DT_HTTP_MAX_PFDS) {
					// TODO: grow list
					break;
				}
				pfds[num_pfds++] = (struct pollfd) {
					.fd = conn_fd,
					.events = POLLIN,
				};
			}
		}

		for (int i = 1; i < num_pfds; i++) {

			if (pfds[i].revents & POLLIN) {

				int conn_fd = pfds[i].fd;
				size_t size;

				dt_map* req = dt_map_new(vm);
				dt_map* req_headers = dt_map_new(vm);
				dt_map_cset(vm, req, "headers", dt_to_map(req_headers));
				char* req_msg = dt_read_all(conn_fd, DT_HTTP_CHUNK_SIZE, &size);
				char* cursor = req_msg;
				char* method = strchr(cursor, ' ');

				if (!method) {
					dt_throw(vm, "bad http request");
					goto fail;
				}

				dt_map_cset(
					vm,
					req,
					"method",
					dt_to_str(dt_str_new_len(vm, cursor, method - cursor))
				);

				cursor = method + 1;
				char* path = strchr(cursor, ' ');

				if (!path) {
					dt_throw(vm, "bad http request");
					goto fail;
				}

				dt_map_cset(
					vm,
					req,
					"path",
					dt_to_str(dt_str_new_len(vm, cursor, path - cursor))
				);

				cursor = path + 1;

				char* version = strstr(cursor, "\r\n");

				if (!version) {
					dt_throw(vm, "bad http request");
					goto fail;
				}

				dt_map_cset(
					vm,
					req,
					"version",
					dt_to_str(dt_str_new_len(vm, cursor, version - cursor))
				);

				cursor = version + 2;

				for (;;) {
					if (cursor[0] == '\r' && cursor[1] == '\n') {
						cursor += 2;
						break;
					}
					char* key = strstr(cursor, ": ");
					if (!key) {
						dt_throw(vm, "bad http request");
						goto fail;
					}
					dt_str* dkey = dt_str_new_len(vm, cursor, key - cursor);
					cursor = key + 2;
					char* val = strstr(cursor, "\r\n");
					if (!val) {
						dt_throw(vm, "bad http request");
						goto fail;
					}
					dt_str* dval = dt_str_new_len(vm, cursor, val - cursor);
					cursor = val + 2;
					dt_map_set(vm, req_headers, dkey, dt_to_str(dval));
				}

				dt_str* body = dt_str_new_len(vm, cursor, req_msg + size - cursor);
				dt_map_cset(vm, req, "body", dt_to_str(body));

				dt_val res_val = dt_call(
					vm,
					cb,
					1,
					dt_to_map(req)
				);

				if (vm->throwing) {
					dt_def_catch(&vm->err);
					vm->throwing = false;
					goto fail;
				}

				// TODO: accept binary
				if (!dt_is_nil(res_val)) {

					dt_map* res = dt_as_map(vm, res_val);
					dt_val status_val = dt_map_cget(vm, res, "status");
					dt_val body_val = dt_map_cget(vm, res, "body");
					dt_val headers_val = dt_map_cget(vm, res, "headers");
					if (!dt_typecheck_opt(vm, status_val, DT_VAL_NUM)) goto fail;
					if (!dt_typecheck_opt(vm, body_val, DT_VAL_STR)) goto fail;
					if (!dt_typecheck_opt(vm, headers_val, DT_VAL_MAP)) goto fail;

					int status = dt_as_num_or(vm, status_val, 200);

					if (status >= 600) {
						dt_throw(vm, "invalid status code: %d", status);
						goto fail;
					}

					dt_str* body = dt_as_str_or(vm, body_val, dt_str_new(vm, ""));
					dt_map* headers = dt_as_map_or(vm, headers_val, dt_map_new(vm));

					// get date
					time_t rawtime;
					time(&rawtime);
					struct tm* timeinfo = gmtime(&rawtime);
					char datebuf[DT_DATEBUF_SIZE];

					size_t datebuf_len = strftime(
						datebuf,
						DT_DATEBUF_SIZE,
						"%a, %d %b %Y %T GMT",
						timeinfo
					);

					// status line
					dt_write_fmt(conn_fd, "HTTP/1.1 %d %s\r\n", status, dt_http_status_txt[status]);

					// default fields
					if (!dt_map_chas(vm, headers, "Connection")) {
						dt_write_fmt(conn_fd, "Connection: %s\r\n", "keep-alive");
					}
					if (!dt_map_chas(vm, headers, "Content-Length")) {
						dt_write_fmt(conn_fd, "Content-Length: %d\r\n", body->len);
					}
					if (!dt_map_chas(vm, headers, "Date")) {
						dt_write_fmt(conn_fd, "Date: %.*s\r\n", datebuf_len, datebuf);
					}

					// write headers
					dt_map_iter iter = dt_map_iter_new(headers);
					dt_map_entry* e;

					while ((e = dt_map_iter_nxt(&iter))) {
						dt_str* val = dt_as_str(vm, e->val);
						if (val) {
							dt_write_fmt(
								conn_fd,
								"%.*s: %.*s\r\n",
								e->key->len,
								e->key->chars,
								val->len,
								val->chars
							);
						}
					}

					// write body
					write(conn_fd, "\r\n", 2);
					write(conn_fd, body->chars, body->len);

				}

fail:
				free(req_msg);
				close(conn_fd);
				pfds[i--] = pfds[--num_pfds];

			}

		}

	}

	close(sock_fd);

}

dt_val dt_f_http_fetch(dt_vm* vm) {

	if (!dt_check_args(vm,
		1,
		DT_VAL_STR
	)) return DT_NIL;

	char* host = dt_arg_cstr(vm, 0);
	dt_map* req = dt_arg_map(vm, 1);
	struct addrinfo* res;

	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE | AI_CANONNAME,
	};

	getaddrinfo(host, "http", &hints, &res);

	int sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (sock_fd == -1) {
		dt_throw(vm, "failed to create socket");
		return DT_NIL;
	}

	connect(sock_fd, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

// 	SSL_CTX* sctx = 0;
// 	SSL* ssl = SSL_new(sctx);

	close(sock_fd);

// 	write(client->sock_fd, req_msg, strlen(req_msg));

	return DT_NIL;

}

#endif

void dt_load_libs(dt_vm* vm) {

	dt_map_reg(vm, vm->globals, (dt_map_centry[]) {
		{ "typeof", dt_to_cfunc(dt_f_typeof), },
		{ "eval", dt_to_cfunc(dt_f_eval), },
		{ "dofile", dt_to_cfunc(dt_f_dofile), },
		{ "print", dt_to_cfunc(dt_f_print), },
		{ "import", dt_to_cfunc(dt_f_import), },
		{ "assert", dt_to_cfunc(dt_f_assert), },
		{ NULL, DT_NIL, },
	});

	dt_map_centry str_lib[] = {
		{ "split", dt_to_cfunc(dt_f_str_split), },
		{ "chars", dt_to_cfunc(dt_f_str_chars), },
		{ "starts", dt_to_cfunc(dt_f_str_starts), },
		{ "ends", dt_to_cfunc(dt_f_str_ends), },
		{ "rep", dt_to_cfunc(dt_f_str_rep), },
		{ "toupper", dt_to_cfunc(dt_f_str_toupper), },
		{ "tolower", dt_to_cfunc(dt_f_str_tolower), },
		{ "find", dt_to_cfunc(dt_f_str_find), },
		{ "find_at", dt_to_cfunc(dt_f_str_find_at), },
		{ "rfind", dt_to_cfunc(dt_f_str_rfind), },
		{ "rfind_at", dt_to_cfunc(dt_f_str_rfind_at), },
		{ "contains", dt_to_cfunc(dt_f_str_contains), },
		{ "replace", dt_to_cfunc(dt_f_str_replace), },
		{ "replace_at", dt_to_cfunc(dt_f_str_replace_at), },
		{ "rev", dt_to_cfunc(dt_f_str_rev), },
		{ "match", dt_to_cfunc(dt_f_str_match), },
		{ "trim", dt_to_cfunc(dt_f_str_trim), },
		{ "trunc", dt_to_cfunc(dt_f_str_trunc), },
		{ "sub", dt_to_cfunc(dt_f_str_sub), },
		{ "len", dt_to_cfunc(dt_f_str_len), },
		{ "hash", dt_to_cfunc(dt_f_str_hash), },
		{ "tonum", dt_to_cfunc(dt_f_str_tonum), },
		{ "code", dt_to_cfunc(dt_f_str_code), },
		{ NULL, DT_NIL, },
	};

	dt_map_reg(NULL, vm->str_lib, str_lib);
	dt_map_cset(NULL, vm->libs, "str", dt_to_map(vm->str_lib));

	dt_map_centry arr_lib[] = {
		{ "push", dt_to_cfunc(dt_f_arr_push), },
// 		{ "push!", dt_to_cfunc(dt_f_arr_push), },
		{ "insert", dt_to_cfunc(dt_f_arr_insert), },
// 		{ "insert!", dt_to_cfunc(dt_f_arr_insert), },
		{ "rm", dt_to_cfunc(dt_f_arr_rm), },
// 		{ "rm!", dt_to_cfunc(dt_f_arr_rm), },
		{ "rm_all", dt_to_cfunc(dt_f_arr_rm_all), },
// 		{ "rm_all!", dt_to_cfunc(dt_f_arr_rm_all), },
		{ "find", dt_to_cfunc(dt_f_arr_find), },
		{ "map", dt_to_cfunc(dt_f_arr_map), },
		{ "sort", dt_to_cfunc(dt_f_arr_sort), },
		{ "filter", dt_to_cfunc(dt_f_arr_filter), },
		{ "each", dt_to_cfunc(dt_f_arr_each), },
		{ "contains", dt_to_cfunc(dt_f_arr_contains), },
		{ "join", dt_to_cfunc(dt_f_arr_join), },
		{ "rev", dt_to_cfunc(dt_f_arr_rev), },
		{ "pop", dt_to_cfunc(dt_f_arr_pop), },
// 		{ "pop!", dt_to_cfunc(dt_f_arr_pop), },
		{ "sub", dt_to_cfunc(dt_f_arr_sub), },
		{ "reduce", dt_to_cfunc(dt_f_arr_reduce), },
		{ "concat", dt_to_cfunc(dt_f_arr_concat), },
		{ "rand", dt_to_cfunc(dt_f_arr_rand), },
		{ "clone", dt_to_cfunc(dt_f_arr_clone), },
		{ "eq", dt_to_cfunc(dt_f_arr_eq), },
		{ "len", dt_to_cfunc(dt_f_arr_len), },
		{ NULL, DT_NIL, },
	};

	dt_map_reg(NULL, vm->arr_lib, arr_lib);
	dt_map_cset(NULL, vm->libs, "arr", dt_to_map(vm->arr_lib));

	dt_map_centry map_lib[] = {
		{ "keys", dt_to_cfunc(dt_f_map_keys), },
		{ "vals", dt_to_cfunc(dt_f_map_vals), },
		{ "each", dt_to_cfunc(dt_f_map_each), },
		{ "rm", dt_to_cfunc(dt_f_map_rm), },
// 		{ "rm!", dt_to_cfunc(dt_f_map_rm), },
		{ "has", dt_to_cfunc(dt_f_map_has), },
		{ "eq", dt_to_cfunc(dt_f_map_eq), },
		{ "clone", dt_to_cfunc(dt_f_map_clone), },
		{ NULL, DT_NIL, },
	};

	dt_map_reg(NULL, vm->map_lib, map_lib);
	dt_map_cset(NULL, vm->libs, "map", dt_to_map(vm->map_lib));

	dt_map_centry func_lib[] = {
		{ "call", dt_to_cfunc(dt_f_func_call), },
		{ "bind", dt_to_cfunc(dt_f_func_bind), },
		{ NULL, DT_NIL, },
	};

	dt_map_reg(NULL, vm->func_lib, func_lib);
	dt_map_cset(NULL, vm->libs, "func", dt_to_map(vm->func_lib));

	dt_map_centry math_lib[] = {
		{ "sin", dt_to_cfunc(dt_f_math_sin), },
		{ "cos", dt_to_cfunc(dt_f_math_cos), },
		{ "tan", dt_to_cfunc(dt_f_math_tan), },
		{ "asin", dt_to_cfunc(dt_f_math_asin), },
		{ "acos", dt_to_cfunc(dt_f_math_acos), },
		{ "atan", dt_to_cfunc(dt_f_math_atan), },
		{ "atan2", dt_to_cfunc(dt_f_math_atan2), },
		{ "isnan", dt_to_cfunc(dt_f_math_isnan), },
		{ "abs", dt_to_cfunc(dt_f_math_abs), },
		{ "mod", dt_to_cfunc(dt_f_math_mod), },
		{ "sqrt", dt_to_cfunc(dt_f_math_sqrt), },
		{ "pow", dt_to_cfunc(dt_f_math_pow), },
		{ "log", dt_to_cfunc(dt_f_math_log), },
		{ "round", dt_to_cfunc(dt_f_math_round), },
		{ "floor", dt_to_cfunc(dt_f_math_floor), },
		{ "ceil", dt_to_cfunc(dt_f_math_ceil), },
		{ "fract", dt_to_cfunc(dt_f_math_fract), },
		{ "max", dt_to_cfunc(dt_f_math_max), },
		{ "min", dt_to_cfunc(dt_f_math_min), },
		{ "clamp", dt_to_cfunc(dt_f_math_clamp), },
		{ "map", dt_to_cfunc(dt_f_math_map), },
		{ "mapc", dt_to_cfunc(dt_f_math_mapc), },
		{ "deg", dt_to_cfunc(dt_f_math_deg), },
		{ "rad", dt_to_cfunc(dt_f_math_rad), },
		{ "sign", dt_to_cfunc(dt_f_math_sign), },
		{ "rand", dt_to_cfunc(dt_f_math_rand), },
		{ "srand", dt_to_cfunc(dt_f_math_srand), },
		{ "PI", dt_to_num(DT_PI), },
		{ NULL, DT_NIL, },
	};

	dt_map_reg(NULL, vm->math_lib, math_lib);
	dt_map_cset(NULL, vm->libs, "math", dt_to_map(vm->math_lib));

#ifdef DT_LIB_SYS
	dt_val os = dt_to_str(dt_str_new(
		NULL,
#if defined(DT_MACOS)
		"macos"
#elif defined(DT_IOS)
		"ios"
#elif defined(DT_WEB)
		"web"
#elif defined(DT_WINDOWS)
		"windows"
#elif defined(DT_ANDROID)
		"android"
#elif defined(DT_LINUX)
		"linux"
#endif
	));
	dt_map_cset(
		NULL,
		vm->libs,
		"sys",
		dt_to_map(dt_map_new_reg(NULL, (dt_map_centry[]) {
			{ "OS",  os, },
			{ "getchar", dt_to_cfunc(dt_f_sys_getchar), },
			{ "getline", dt_to_cfunc(dt_f_sys_getline), },
			{ "signal", dt_to_cfunc(dt_f_sys_signal), },
			{ "time", dt_to_cfunc(dt_f_sys_time), },
			{ "date", dt_to_cfunc(dt_f_sys_date), },
			{ "setenv", dt_to_cfunc(dt_f_sys_setenv), },
			{ "getenv", dt_to_cfunc(dt_f_sys_getenv), },
			{ "exit", dt_to_cfunc(dt_f_sys_exit), },
			{ "print", dt_to_cfunc(dt_f_sys_print), },
			{ "cwd", dt_to_cfunc(dt_f_sys_cwd), },
			{ "chdir", dt_to_cfunc(dt_f_sys_chdir), },
			{ "fork", dt_to_cfunc(dt_f_sys_fork), },
#if !defined(DT_IOS)
			{ "exec", dt_to_cfunc(dt_f_sys_exec), },
#endif
			{ NULL, DT_NIL, },
		}))
	);
#endif

#ifdef DT_LIB_FS
	dt_map_cset(
		NULL,
		vm->libs,
		"fs",
		dt_to_map(dt_map_new_reg(NULL, (dt_map_centry[]) {
			{ "read", dt_to_cfunc(dt_f_fs_read), },
			{ "write", dt_to_cfunc(dt_f_fs_write), },
			{ "readdir", dt_to_cfunc(dt_f_fs_readdir), },
			{ "exists", dt_to_cfunc(dt_f_fs_exists), },
			{ "mkdir", dt_to_cfunc(dt_f_fs_mkdir), },
			{ "copy", dt_to_cfunc(dt_f_fs_copy), },
			{ "move", dt_to_cfunc(dt_f_fs_move), },
			{ "remove", dt_to_cfunc(dt_f_fs_remove), },
			{ "isfile", dt_to_cfunc(dt_f_fs_isfile), },
			{ "isdir", dt_to_cfunc(dt_f_fs_isdir), },
			{ "size", dt_to_cfunc(dt_f_fs_size), },
			{ "lastmod", dt_to_cfunc(dt_f_fs_lastmod), },
			{ "ext", dt_to_cfunc(dt_f_fs_ext), },
			{ "base", dt_to_cfunc(dt_f_fs_base), },
			{ "resdir", dt_to_cfunc(dt_f_fs_resdir), },
			{ "datadir", dt_to_cfunc(dt_f_fs_datadir), },
			{ "watch", dt_to_cfunc(dt_f_fs_watch), },
#if !defined(DT_WEB)
			{ "realpath", dt_to_cfunc(dt_f_fs_realpath), },
#endif
			{ NULL, DT_NIL, },
		}))
	);
#endif

#ifdef DT_LIB_TERM
	dt_map_cset(
		NULL,
		vm->libs,
		"term",
		dt_to_map(dt_map_new_reg(NULL, (dt_map_centry[]) {
			{ "black", dt_to_cfunc(dt_f_term_black), },
			{ "red", dt_to_cfunc(dt_f_term_red), },
			{ "green", dt_to_cfunc(dt_f_term_green), },
			{ "yellow", dt_to_cfunc(dt_f_term_yellow), },
			{ "blue", dt_to_cfunc(dt_f_term_blue), },
			{ "magenta", dt_to_cfunc(dt_f_term_magenta), },
			{ "cyan", dt_to_cfunc(dt_f_term_cyan), },
			{ "white", dt_to_cfunc(dt_f_term_white), },
			{ "blackbg", dt_to_cfunc(dt_f_term_blackbg), },
			{ "redbg", dt_to_cfunc(dt_f_term_redbg), },
			{ "greenbg", dt_to_cfunc(dt_f_term_greenbg), },
			{ "yellowbg", dt_to_cfunc(dt_f_term_yellowbg), },
			{ "bluebg", dt_to_cfunc(dt_f_term_bluebg), },
			{ "magentabg", dt_to_cfunc(dt_f_term_magentabg), },
			{ "cyanbg", dt_to_cfunc(dt_f_term_cyanbg), },
			{ "whitebg", dt_to_cfunc(dt_f_term_whitebg), },
			{ "bold", dt_to_cfunc(dt_f_term_bold), },
			{ "dim", dt_to_cfunc(dt_f_term_dim), },
			{ "italic", dt_to_cfunc(dt_f_term_italic), },
			{ "underline", dt_to_cfunc(dt_f_term_underline), },
			{ "rgb", dt_to_cfunc(dt_f_term_rgb), },
			{ "rgbbg", dt_to_cfunc(dt_f_term_rgbbg), },
			{ "enter_alt", dt_to_cfunc(dt_f_term_enter_alt), },
			{ "exit_alt", dt_to_cfunc(dt_f_term_exit_alt), },
			{ "move_cursor", dt_to_cfunc(dt_f_term_move_cursor), },
			{ "move_cursor_col", dt_to_cfunc(dt_f_term_move_cursor_col), },
			{ "hide_cursor", dt_to_cfunc(dt_f_term_hide_cursor), },
			{ "show_cursor", dt_to_cfunc(dt_f_term_show_cursor), },
			{ "save_cursor", dt_to_cfunc(dt_f_term_save_cursor), },
			{ "restore_cursor", dt_to_cfunc(dt_f_term_restore_cursor), },
			{ "clear_line", dt_to_cfunc(dt_f_term_clear_line), },
			{ "clear_screen", dt_to_cfunc(dt_f_term_clear_screen), },
			{ "no_echo", dt_to_cfunc(dt_f_term_no_echo), },
			{ NULL, DT_NIL, },
		}))
	);
#endif

#ifdef DT_LIB_HTTP
	dt_map_cset(
		NULL,
		vm->libs,
		"http",
		dt_to_map(dt_map_new_reg(NULL, (dt_map_centry[]) {
			{ "serve", dt_to_cfunc(dt_f_http_serve), },
			{ "fetch", dt_to_cfunc(dt_f_http_fetch), },
			{ NULL, DT_NIL, },
		}))
	);
#endif

}

// TODO: pass args
dt_val dt_eval(dt_vm* vm, char* code) {

	dt_compiler c = dt_compiler_new(code);
	dt_c_nxt(&c);

	while (c.parser.cur.type != DT_TOKEN_END) {
		dt_c_stmt(&c);
	}

	dt_c_emit(&c, DT_OP_NIL);
	dt_c_emit(&c, DT_OP_STOP);

	dt_func func = (dt_func) {
		.type = DT_VAL_FUNC,
		.logic = &(dt_logic) {
			.chunk = c.env->chunk,
			.nargs = 0,
		},
		.upvals = NULL,
		.num_upvals = 0,
	};

	dt_val ret = dt_call(vm, dt_to_func(&func), 0);

	dt_compiler_free(&c);

	return ret;

}

dt_val dt_dofile(dt_vm* vm, char* path) {

	char* code = dt_read_file(path, NULL);
	char* start = code;

	if (!code) {
		dt_throw(vm, "failed to read file '%s'", path);
		return DT_NIL;
	}

	// shebang
	if (start[0] == '#' && start[1] == '!') {
		while (*start != '\n') start++;
	}

	dt_val ret = dt_eval(vm, start);
	free(code);

	return ret;

}

#endif
#endif
