// wengwengweng

#define D_IMPL
#include <d_http.h>

char *handler(const char *msg) {
	printf("%s\n", msg);
	return "HTTP/1.1 200 OK\r\n\r\nhi\n";
}

int main() {
// 	char *msg = d_http_fetch(
// 		"space55.xyz",
// 		"GET / HTTP/1.1\r\nHost: space55.xyz\r\n\r\n"
// 	);

// 	printf("%s\n", msg);
// 	free(msg);
	d_http_serve(1234, handler);
}

