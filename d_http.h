// wengwengweng

#ifndef D_HTTP_H
#define D_HTTP_H

typedef enum {
	D_HTTP_GET,
	D_HTTP_POST,
	D_HTTP_PUT,
	D_HTTP_DELETE,
	D_HTTP_HEAD,
	D_HTTP_CONNECT,
	D_HTTP_OPTIONS,
	D_HTTP_TRACE,
	D_HTTP_PATCH,
} d_http_method;

typedef struct {
	char *key;
	char *val;
} d_http_header;

typedef struct {
	int status;
	int num_headers;
	d_http_header *headers;
	char *body;
} d_http_res;

typedef struct {
	d_http_method method;
	char *path;
	int num_headers;
	d_http_header *headers;
	char *body;
} d_http_req;

typedef struct {
	int sock_fd;
} d_http_server;

typedef struct {
	int sock_fd;
} d_http_client;

typedef char *(*d_http_handler)(const char*);

void d_http_serve(int port, d_http_handler handler);
d_http_server d_make_http_server(int port);
void d_free_http_server(d_http_server *server);
void d_http_server_listen(const d_http_server *server, d_http_handler handler);

d_http_res d_http_fetch(const char *host, const char *msg);
d_http_client d_make_http_client(const char *host);
void d_free_http_client(d_http_client *client);
d_http_res d_http_client_send(const d_http_client *client, const char *req_msg);

void d_free_http_req(d_http_req *req);
void d_free_http_res(d_http_res *res);

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
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
// #include <openssl/ssl.h>

#define D_HTTP_CHUNK_SIZE 2048

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

d_http_server d_make_http_server(int port) {

	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_fd == -1) {
		fprintf(stderr, "failed to create socket\n");
	}

	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, (int[]){1}, sizeof(int));
	ioctl(sock_fd, FIONBIO, (int[]){1});

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
	}

	listen(sock_fd, 64);

	return (d_http_server) {
		.sock_fd = sock_fd,
	};

}

void d_free_http_server(d_http_server *server) {
	close(server->sock_fd);
}

#define D_HTTP_MAX_PFDS 1024

void d_http_serve(int port, d_http_handler handler) {

	d_http_server server = d_make_http_server(port);

	struct pollfd pfds[D_HTTP_MAX_PFDS] = {
		[0] = {
			.fd = STDIN_FILENO,
			.events = POLLIN,
		},
		[1] = {
			.fd = server.sock_fd,
			.events = POLLIN,
		},
	};

	int num_pfds = 2;

	while (1) {

		if (poll(pfds, num_pfds, -1) > 0) {

			if(pfds[0].revents & POLLIN) {
				// TODO
			}

			if(pfds[1].revents & POLLIN) {
				while (1) {
					int conn_fd = accept(server.sock_fd, NULL, NULL);
					if (conn_fd < 0) {
						break;
					}
					if (num_pfds + 1 >= D_HTTP_MAX_PFDS) {
						// TODO
						break;
					}
					pfds[num_pfds++] = (struct pollfd) {
						.fd = conn_fd,
						.events = POLLIN,
					};
				}
			}

			for (int i = 2; i < num_pfds; i++) {

				if (pfds[i].revents & POLLIN) {

					int conn_fd = pfds[i].fd;
					int chunk_size = D_HTTP_CHUNK_SIZE;
					char *req_msg = malloc(chunk_size);
					int iter = 0;

					while (read(conn_fd, req_msg + iter * chunk_size, chunk_size) >= chunk_size) {
						iter++;
						req_msg = realloc(req_msg, (iter + 1) * chunk_size);
					}

					char *res_msg = handler(req_msg);

					write(conn_fd, res_msg, strlen(res_msg));
					free(req_msg);
					close(conn_fd);
					pfds[i--] = pfds[--num_pfds];

				}

			}

		}

	}

	d_free_http_server(&server);

}

d_http_client d_make_http_client(const char *host) {

	struct addrinfo *res;

	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE | AI_CANONNAME,
	};

	getaddrinfo(host, "http", &hints, &res);

	int sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (sock_fd == -1) {
		fprintf(stderr, "failed to create socket\n");
	}

	connect(sock_fd, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

	return (d_http_client) {
		.sock_fd = sock_fd,
	};

}

void d_free_http_client(d_http_client *client) {
	close(client->sock_fd);
}

void d_free_http_res(d_http_res *res) {
	free(res->body);
	res->body = NULL;
	for (int i = 0; i < res->num_headers; i++) {
		d_http_header *h = &res->headers[i];
		free(h->key);
		h->key = NULL;
		free(h->val);
		h->val = NULL;
	}
}

void d_free_http_req(d_http_req *req) {
	free(req->body);
	req->body = NULL;
	free(req->path);
	req->path = NULL;
	for (int i = 0; i < req->num_headers; i++) {
		d_http_header *h = &req->headers[i];
		free(h->key);
		h->key = NULL;
		free(h->val);
		h->val = NULL;
	}
}

static int atoin(const char *str, int n) {
	int num = 0;
	for (int i = 0; i < n; i++) {
		num = num * 10 + (str[i] - '0');
	}
	return num;
}

static char *makestr(char *src, int len) {
	char *str = malloc(len + 1);
	strncpy(str, src, len);
	str[len] = '\0';
	return str;
}

// TODO: parse url
// TODO: send_ex with headers
// TODO: https
d_http_res d_http_client_send(const d_http_client *client, const char *req_msg) {

	write(client->sock_fd, req_msg, strlen(req_msg));

	char *res_msg = malloc(D_HTTP_CHUNK_SIZE * sizeof(char));
	int bread = 0;
	int status = 0;
	int header_pos = 0;
	int body_len = 0;
	int body_pos = 0;
	int cursor = 0;
	int num_headers = 0;
	d_http_header *headers = malloc(sizeof(d_http_header));

	while (1) {

		bread += read(client->sock_fd, res_msg + bread, D_HTTP_CHUNK_SIZE);
		res_msg = realloc(res_msg, bread + D_HTTP_CHUNK_SIZE);

		if (!status) {
			char *status_pos = strchr(res_msg + cursor, ' ');
			if (status_pos) {
				char *start = status_pos + 1;
				char *end = strchr(start, ' ');
				if (end) {
					status = atoin(start, end - start);
					cursor = end - res_msg;
				}
			}
		}

		if (!status) {
			continue;
		}

		if (!header_pos) {
			char *header = strstr(res_msg + cursor, "\r\n");
			if (header) {
				header_pos = header + 2 - res_msg;
				cursor = header_pos;
			}
		}

		if (!header_pos) {
			continue;
		}

		if (!body_pos) {

			while (1) {

				if (res_msg[cursor] == '\r' && res_msg[cursor + 1] == '\n') {
					cursor += 2;
					body_pos = cursor;
					break;
				}

				char *key_end = strstr(res_msg + cursor, ": ");

				if (!key_end) {
					break;
				}

				int key_len = (key_end - res_msg) - cursor;
				char *key = makestr(res_msg + cursor, key_len);
				cursor = key_end - res_msg + 2;

				char *val_end = strstr(res_msg + cursor, "\r\n");

				if (!val_end) {
					break;
				}

				int val_len = (val_end - res_msg) - cursor;
				char *val = makestr(res_msg + cursor, val_len);
				cursor = val_end - res_msg + 2;

				headers[num_headers] = (d_http_header) {
					.key = key,
					.val = val,
				};

				num_headers++;
				headers = realloc(headers, sizeof(d_http_header) * (num_headers + 1));

				if (strcmp(key, "Content-Length") == 0) {
					body_len = atoi(val);
				}

			}

		}

		if (body_pos) {
			if (bread - body_pos >= body_len) {
				break;
			}
		}

	}

	char *body = makestr(res_msg + cursor, body_len);
	free(res_msg);

	return (d_http_res) {
		.status = status,
		.num_headers = num_headers,
		.headers = headers,
		.body = body,
	};

}

d_http_res d_http_fetch(const char *host, const char *req_msg) {
	d_http_client client = d_make_http_client(host);
	d_http_res res = d_http_client_send(&client, req_msg);
	d_free_http_client(&client);
	return res;
}

#endif
#endif

