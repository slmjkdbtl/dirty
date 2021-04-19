// wengwengweng

#ifndef D_LANG_H
#define D_LANG_H

void d_talk_eval(const char *src);

#endif

#ifdef D_IMPL
#define D_LANG_IMPL
#endif

#ifdef D_LANG_IMPL
#ifndef D_LANG_IMPL_ONCE
#define D_LANG_IMPL_ONCE

void d_talk_eval(const char *src) {
	// ..
}

#endif
#endif

