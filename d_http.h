// wengwengweng

#ifndef D_HTTP_H
#define D_HTTP_H

void d_http_serve(int port, char *(*handler)(const char*));

#endif

#ifdef D_IMPL
#define D_HTTP_IMPL
#endif

#ifdef D_HTTP_IMPL
#ifndef D_HTTP_IMPL_ONCE
#define D_HTTP_IMPL_ONCE

#include <stdbool.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

static char const *status_text[] = {
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

	//100s
	"Continue", "Switching Protocols", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

	//200s
	"OK", "Created", "Accepted", "Non-Authoritative Information", "No Content",
	"Reset Content", "Partial Content", "", "", "",

	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

	//300s
	"Multiple Choices", "Moved Permanently", "Found", "See Other", "Not Modified",
	"Use Proxy", "", "Temporary Redirect", "", "",

	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

	//400s
	"Bad Request", "Unauthorized", "Payment Required", "Forbidden", "Not Found",
	"Method Not Allowed", "Not Acceptable", "Proxy Authentication Required",
	"Request Timeout", "Conflict",

	"Gone", "Length Required", "", "Payload Too Large", "", "", "", "", "", "",

	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",

	//500s
	"Internal Server Error", "Not Implemented", "Bad Gateway", "Service Unavailable",
	"Gateway Timeout", "", "", "", "", ""

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

void d_http_serve(int port, char *(*handler)(const char*)) {

	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_fd == -1) {
		fprintf(stderr, "failed to create socket\n");
		exit(EXIT_FAILURE);
	}

	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, (int[]){1}, sizeof(int));

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_addr = {
			.s_addr = INADDR_ANY,
		},
		.sin_port = htons(port),
	};

	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		switch (errno) {
			case EACCES:
				fprintf(stderr, "port %d is in protected\n", port);
				break;
			case EADDRINUSE:
				fprintf(stderr, "port %d is in use\n", port);
				break;
			default:
				fprintf(stderr, "failed to bind socket\n");
				break;
		}
		exit(EXIT_FAILURE);
	}

	listen(sock_fd, 64);

	while (1) {

		int conn_fd = accept(sock_fd, NULL, NULL);
		char *req_msg = malloc(BUF_SIZE);
		int times = 0;

		while (read(conn_fd, req_msg + times * BUF_SIZE, BUF_SIZE) >= BUF_SIZE) {
			times++;
			req_msg = realloc(req_msg, (times + 1) * BUF_SIZE);
		}

		char *res_msg = handler(req_msg);

		write(conn_fd, res_msg, strlen(res_msg));
		free(req_msg);
		close(conn_fd);

	}

	close(sock_fd);

}

#endif
#endif

