// wengwengweng

#define D_IMPL
#include <d_http.h>

char *handler(const char *msg) {
	printf("%s\n", msg);
	return "HTTP/1.1 200 OK\r\n\r\nhi\n";
}

int main() {
	d_http_serve(1234, handler);
}

