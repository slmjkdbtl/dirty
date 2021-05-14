// wengwengweng

#ifndef D_THREAD_H
#define D_THREAD_H

#include <pthread.h>

typedef struct {
	int num_threads;
} d_tpool;

d_tpool d_make_tpool(int num);
d_tpool d_tpool_exec(d_tpool *pool, void(*action)(void*), void *arg);
d_tpool d_tpool_wait(const d_tpool *pool);
d_tpool d_tpool_free(d_tpool *pool);
pthread_t d_texec(void*(*action)(void*), void *arg);

#endif

#ifdef D_IMPL
#define D_THREAD_IMPL
#endif

#ifdef D_THREAD_IMPL
#ifndef D_THREAD_IMPL_ONCE
#define D_THREAD_IMPL_ONCE

pthread_t d_texec(void*(*action)(void*), void *arg) {
	pthread_t id;
	int status = pthread_create(&id, NULL, action, arg);
	if (status < 0) {
		fprintf(stderr, "failed to start thread\n");
	}
	return id;
}

#endif
#endif
