// wengwengweng

#define D_IMPL
#include <d_http.h>

char *handler(const char *msg) {
	printf("%s\n", msg);
	return "HTTP/1.1 200 OK\r\n\r\nhi\n";
}

int main() {

	d_http_response res = d_http_fetch(
		"example.com",
		"GET / HTTP/1.1\r\nHost: example.com\r\n\r\n"
	);

// 	printf("status: %d\n", res.status);

// 	for (int i = 0; i < res.num_headers; i++) {
// 		d_http_str key = res.headers[i].key;
// 		d_http_str val = res.headers[i].val;
// 		printf(
// 			"%.*s: %.*s\n",
// 			key.end - key.start,
// 			res.msg + key.start,
// 			val.end - val.start,
// 			res.msg + val.start
// 		);
// 	}

	printf(
		"%.*s\n",
		res.body.end - res.body.start,
		res.msg + res.body.start
	);

	d_http_free_response(&res);
// 	d_http_serve(1234, handler);
}

