// wengwengweng

#define D_IMPL
#include <d_http.h>

char *handler(const char *msg) {
	printf("%s\n", msg);
	return "HTTP/1.1 200 OK\r\n\r\nhi\n";
}

int main() {

// 	d_http_res res = d_http_fetch("http://example.com");

	d_http_res res = d_http_fetch(
		"example.com",
		"GET / HTTP/1.1\r\nHost: example.com\r\n\r\n"
	);

	printf("%d\n", res.status);

	for (int i = 0; i < res.num_headers; i++) {
		char *key = res.headers[i].key;
		char *val = res.headers[i].val;
		printf("%s: %s\n", key, val);
	}

	printf("%s\n", res.body);

	d_free_http_res(&res);
// 	d_http_serve(1234, handler);

}

