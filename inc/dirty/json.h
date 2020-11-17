// wengwengweng

#ifndef D_JSON_H
#define D_JSON_H

typedef struct {
	void *tmp;
} d_json;

typedef struct {
	void *tmp;
} d_json_obj;

typedef struct {
	void *tmp;
} d_json_arr;

d_json d_parse_json(const char *json);
d_json d_load_json(const char *path);
void d_free_json(d_json *doc);

d_json_obj d_json_as_obj(const d_json *doc);
d_json_arr d_json_as_arr(const d_json *doc);

d_json_obj d_json_get_obj(d_json_obj *obj, const char *name);
d_json_arr d_json_get_arr(d_json_obj *obj, const char *name);
float d_json_get_num(d_json_obj *obj, const char *name);
bool d_json_get_bool(d_json_obj *obj, const char *name);
const char *d_json_get_str(d_json_obj *obj, const char *name);

void d_json_set_obj(d_json_obj *obj, const char *name, d_json_obj *pobj);
void d_json_set_arr(d_json_obj *obj, const char *name, d_json_arr *arr);
void d_json_set_num(d_json_obj *obj, const char *name, float num);
void d_json_set_bool(d_json_obj *obj, const char *name, bool b);
void d_json_set_str(d_json_obj *obj, const char *name, const char *str);

#endif

