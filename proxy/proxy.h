#ifndef PROXY_H
#define PROXY_H
#include "picohttpparser.h"
#include "thread_pool.h"

#define MAX_HEADERS_NUM 32

typedef struct proxy {
    int server_socket;
    int running;
    t_thread_pool *p_thread_pool;
} t_proxy;

typedef struct phr_header t_phr_header;

typedef struct {
    const char *method;
    const char *path;
    t_phr_header headers[MAX_HEADERS_NUM];
    size_t method_len;
    size_t path_len;
    size_t num_headers;
    int minor_version;
} t_request;

typedef struct {
    int minor_version;
    int status;
    const char *msg;
    size_t msg_len;
    t_phr_header headers[MAX_HEADERS_NUM];
    size_t num_headers;
} t_response;

t_proxy *proxy_create(t_thread_pool *p_thread_pool);

int proxy_start(t_proxy *p_proxy);

void proxy_destroy(t_proxy *proxy);

#endif // PROXY_H
