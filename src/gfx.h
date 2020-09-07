// wengwengweng

#ifndef D_GFX_INTERNAL_H
#define D_GFX_INTERNAL_H

#define BATCH_VERT_COUNT 65536
#define BATCH_INDEX_COUNT 65536

typedef struct {
	GLuint vbuf;
	GLuint ibuf;
	d_vertex vqueue[BATCH_VERT_COUNT];
	unsigned int iqueue[BATCH_INDEX_COUNT];
	int vcount;
	int icount;
} d_batch;

void d_gfx_init();
void d_gfx_frame_begin();
void d_gfx_frame_end();
void d_draw(GLuint, GLuint, int);
d_batch d_make_batch();
void d_batch_push(d_batch*, const d_vertex*, int, const d_index*, int);
void d_batch_flush(d_batch*);
void d_free_batch(d_batch*);

#endif

