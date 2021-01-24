// wengwengweng

#ifndef D_TERM_H
#define D_TERM_H

#define D_ANSI_BLACK   30
#define D_ANSI_RED     31
#define D_ANSI_GREEN   32
#define D_ANSI_YELLOW  33
#define D_ANSI_BLUE    34
#define D_ANSI_MAGENTA 35
#define D_ANSI_CYAN    36
#define D_ANSI_WHITE   37

#define D_ANSI_BG_BLACK   30
#define D_ANSI_BG_RED     31
#define D_ANSI_BG_GREEN   32
#define D_ANSI_BG_YELLOW  33
#define D_ANSI_BG_BLUE    34
#define D_ANSI_BG_MAGENTA 35
#define D_ANSI_BG_CYAN    36
#define D_ANSI_BG_WHITE   37

#define D_ANSI_RESET     0
#define D_ANSI_BOLD      1
#define D_ANSI_DIM       2
#define D_ANSI_ITALIC    3
#define D_ANSI_UNDERLINE 4
#define D_ANSI_BLINK     5

void d_cprintf(int color, const char *fmt, ...);
void d_cfprintf(FILE *stream, int color, const char *fmt, ...);
void d_term_ansi(int style);
void d_term_ansi_f(FILE *stream, int style);
void d_term_ansi_reset();
void d_term_ansi_reset_f(FILE *stream);
void d_term_set_raw(bool b);

#endif

#ifdef D_IMPL
#define D_TERM_IMPL
#endif

#ifdef D_TERM_IMPL
#ifndef D_TERM_IMPL_ONCE
#define D_TERM_IMPL_ONCE

#include <termios.h>
#include <unistd.h>

void d_cprintf(int style, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	d_term_ansi(style);
	vprintf(fmt, args);
	d_term_ansi_reset();
	va_end(args);
}

void d_cfprintf(FILE *stream, int style, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	d_term_ansi_f(stream, style);
	vfprintf(stream, fmt, args);
	d_term_ansi_reset_f(stream);
	va_end(args);
}

void d_term_ansi(int style) {
	printf("\x1b[%dm", style);
}

void d_term_ansi_f(FILE *stream, int style) {
	fprintf(stream, "\x1b[%dm", style);
}

void d_term_ansi_reset() {
	d_term_ansi(D_ANSI_RESET);
}

void d_term_ansi_reset_f(FILE *stream) {
	d_term_ansi_f(stream, D_ANSI_RESET);
}

typedef struct {
	struct termios orig_termios;
} d_term_ctx;

static d_term_ctx d_term;

void d_term_set_raw(bool b) {
	if (b) {
		tcgetattr(STDIN_FILENO, &d_term.orig_termios);
		struct termios raw = d_term.orig_termios;
		raw.c_lflag &= ~(ECHO);
	} else {
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &d_term.orig_termios);
	}
}

#endif
#endif

