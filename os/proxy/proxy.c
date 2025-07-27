#include "proxy.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#include "logger.h"
#include "picohttpparser.h"

#define HTTPv1_PORT 80
#define MAX_CLIENT_QUEUE 30
#define TIMEOUT_MS 8000
#define MAX_BUF_SIZE 8192
#define READ_BUF_LEN 8192

int create_server_socket() {
    const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        log_debug("socket() failed for server", strerror(errno));
        return -1;
    }
    const int reuseaddr = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,
                   sizeof(reuseaddr)) == -1) {
        log_debug("setsockopt() failed for server", strerror(errno));
    }
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(HTTPv1_PORT);
    const int err =
            bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err == -1) {
        log_debug("bind failed for server on HTTPv1_PORT", strerror(errno));
        close(server_socket);
        return -1;
    }
    log_debug("Server socket created successfully", NULL);
    return server_socket;
}

t_proxy *proxy_create(t_thread_pool *p_thread_pool) {
    t_proxy *p_proxy = malloc(sizeof(t_proxy));
    if (!p_proxy) {
        log_debug("malloc() failed for proxy", strerror(errno));
        return NULL;
    }

    p_proxy->server_socket = create_server_socket();
    if (p_proxy->server_socket < 0) {
        free(p_proxy);
        log_debug("Failed to create server socket", NULL);
        return NULL;
    }

    p_proxy->p_thread_pool = p_thread_pool;

    log_info("Proxy successfully created", NULL);
    return p_proxy;
}

int set_socket_timeout(const int sock, const uint32_t timeout_ms) {
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;
    const int err0 = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (err0) {
        log_debug("setsockopt() failed for setting a recv timeout", strerror(errno));
        return -1;
    }
    const int err1 = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    if (err1) {
        log_debug("setsockopt() failed for setting a send timeout", strerror(errno));
        return -1;
    }
    return 0;
}

//reads and parses the request from the client; returns number of read characters on success, -1 on error
ssize_t handle_request(const int sock, char *buf, const size_t max_buf_size,
                       t_request *request) {
    ssize_t read_return;
    size_t buf_size = 0;
    while (1) {
        while ((read_return = read(sock, buf + buf_size, max_buf_size - buf_size)) ==
               -1 &&
               errno == EINTR) //in case read is interrupted
        {
        }
        if (!read_return) {
            log_debug("eof reached for socket", NULL);
            return -1;
        }
        if (read_return == -1) {
            log_debug("read() failed", strerror(errno));
            return -1;
        }
        const size_t old_buf_size = buf_size;
        buf_size += read_return;

        request->num_headers =
                sizeof(request->headers) / sizeof(request->headers[0]);
        const int parsing_request = phr_parse_request(
            buf, buf_size, &request->method, &request->method_len, &request->path,
            &request->path_len, &request->minor_version, request->headers,
            &request->num_headers, old_buf_size);
        if (parsing_request > 0) {
            log_debug("Request parsed successfully", NULL);
            break;
        }
        if (parsing_request == -1) {
            log_debug("Failed to parse request", NULL);
            return -1;
        }
        if (parsing_request != -2) {
            log_debug("Request is partial, fail", NULL);
            return -1;
        }
        if (buf_size == max_buf_size) {
            log_debug("Request is too big", NULL);
            return -1;
        }
    }
    return buf_size;
}

t_phr_header *find_header(t_phr_header headers[], const size_t headers_size,
                          const char *name, const size_t name_size) {
    for (size_t i = 0; i < headers_size; i++) {
        if (headers[i].name_len == name_size &&
            strncmp(headers[i].name, name, name_size) == 0) {
            return &headers[i];
        }
    }
    return NULL;
}

int connect_to_destination_server(const char *host_name) {
    struct addrinfo *addr_info;
    const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (!server_socket) {
        log_debug("socket() failed for connection to destination server", strerror(errno));
        return -1;
    }
    int err = set_socket_timeout(server_socket, TIMEOUT_MS);
    if (err < 0) {
        log_debug("Failed to set timeout for destination server socket", NULL);
        close(server_socket);
        return -1;
    }
    err = getaddrinfo(host_name, "http", NULL, &addr_info);
    if (err) {
        close(server_socket);
        log_debug("Failed to get address info", gai_strerror(err));
        return -1;
    }
    err = connect(server_socket, addr_info->ai_addr, addr_info->ai_addrlen);
    if (err != 0) {
        close(server_socket);
        freeaddrinfo(addr_info);
        log_debug("Failed to connect to server back", strerror(errno));
        return -1;
    }
    freeaddrinfo(addr_info);
    log_debug("Successfully connected to destination server", NULL);
    return server_socket;
}

int send_data(const int socket, const char *buf, const size_t buf_size) {
    ssize_t total_sent = 0;
    while (total_sent != buf_size) {
        const ssize_t sent = write(socket, buf + total_sent, buf_size - total_sent);
        if (sent == -1) {
            log_debug("Failed to write to socket", strerror(errno));
            return -1;
        }
        total_sent += sent;
    }
    return 0;
}

size_t get_content_size(t_phr_header headers[], const size_t headers_size) {
    t_phr_header *content_length_header =
            find_header(headers, headers_size, "Content-Length", strlen("Content-Length"));
    if (!content_length_header) {
        log_debug("Failed to find content length header", NULL);
        return 0;
    }
    char content_length[content_length_header->value_len];
    memcpy(content_length, content_length_header->value, content_length_header->value_len);
    content_length[content_length_header->value_len] = '\0';
    const long long int content_size = atoll(content_length);
    if (content_size < 0)
        return 0;
    return content_size;
}

ssize_t handle_response(const int server_socket, const int client_socket) {
    const char *http_headers_closing_string = "\r\n\r\n";
    char buf[READ_BUF_LEN];
    t_response response;
    ssize_t received;
    size_t total_received = 0;
    char *headers_end;

    // find headers section's end
    do {
        if (total_received == READ_BUF_LEN) {
            log_debug("Headers section is too long", NULL);
            return -1;
        }
        received = read(server_socket, buf + total_received, READ_BUF_LEN - total_received);
        if (received == -1) {
            log_debug("read() failed when handling response", strerror(errno));
            return -1;
        }
        if (received == 0) {
            log_debug("read() error: server disconnected", NULL);
            return -1;
        }
        total_received += received;
        buf[total_received] = 0;
    } while ((headers_end = strstr(buf, http_headers_closing_string)) == NULL);
    headers_end += strlen(http_headers_closing_string);
    const size_t headers_size = headers_end - buf;

    // parse headers
    response.num_headers = sizeof(response.headers) / sizeof(response.headers[0]);
    int err = phr_parse_response(buf, headers_size, &response.minor_version, &response.status,
                                 &response.msg, &response.msg_len, response.headers,
                                 &response.num_headers, 0);
    if (err < 0) {
        log_debug("Failed to parse response", NULL);
        return -1;
    }

    // send client available data after finding headers
    const size_t content_size = get_content_size(response.headers, response.num_headers);
    err = send_data(client_socket, buf, total_received);
    if (err) {
        log_debug("Failed to send data back to client", NULL);
        return -1;
    }

    // get the rest of the data from server; send to client
    ssize_t remaining = headers_size + content_size - total_received;
    while (remaining > 0) {
        received = read(server_socket, buf, READ_BUF_LEN);
        if (received <= 0) {
            if (received == -1)
                log_debug("read failed when getting the rest of the data in handle response", strerror(errno));
            if (received == 0)
                log_debug("Receive error: server disconnected when trying to get the rest of the data", NULL);
            return -1;
        }
        remaining -= received;
        total_received += received;
        err = send_data(client_socket, buf, received);
        if (err) {
            log_debug("Failed to send data back to client", NULL);
            return -1;
        }
    }
    return total_received;
}

void handle_client(void *arg) {
    const int client_socket = *(int *) arg;
    free(arg);
    int err = set_socket_timeout(client_socket, TIMEOUT_MS);
    if (err < 0) {
        log_debug("Failed to set client socket timeouts", NULL);
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
        return;
    }

    t_request request;
    char request_buf[MAX_BUF_SIZE] = {0};
    const ssize_t request_size =
            handle_request(client_socket, request_buf, MAX_BUF_SIZE, &request);
    if (request_size < 0) {
        log_debug("Failed to handle request", NULL);
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
        return;
    }

    if (strncmp(request.method, "GET", 3) != 0) {
        log_debug("Unsupported request type", NULL);
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
        return;
    }

    t_phr_header *header_host =
            find_header(request.headers, request.num_headers, "Host", strlen("Host"));
    if (!header_host) {
        log_debug("Failed to find host", NULL);
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
        return;
    }

    char host_name[header_host->value_len];
    memcpy(host_name, header_host->value, header_host->value_len);
    host_name[header_host->value_len] = '\0';

    const int dest_server_socket = connect_to_destination_server(host_name);
    if (dest_server_socket < 0) {
        log_debug("Failed to reconnect to the server", NULL);
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
        return;
    }
    log_debug("Connected to destination server", host_name);

    err = send_data(dest_server_socket, request_buf, request_size);
    if (err < 0) {
        log_debug("Failed to send to server back", NULL);
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
        shutdown(dest_server_socket, SHUT_RDWR);
        close(dest_server_socket);
        return;
    }
    log_debug("Request sent to destination server", NULL);

    const ssize_t response_size =
            handle_response(dest_server_socket, client_socket);
    if (response_size < 0) {
        log_debug("Failed to handle response from destination server", host_name);
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
        shutdown(dest_server_socket, SHUT_RDWR);
        close(dest_server_socket);
        return;
    }
    log_info("Client handled successfully", NULL);

    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);
    shutdown(dest_server_socket, SHUT_RDWR);
    close(dest_server_socket);
}

int proxy_start(t_proxy *p_proxy) {
    const int err = listen(p_proxy->server_socket, MAX_CLIENT_QUEUE);
    if (err == -1) {
        log_debug("listen failed on server socket", strerror(errno));
        return -1;
    }

    p_proxy->running = 1;
    log_info("Proxy is running", NULL);
    while (p_proxy->running) {
        const int client_socket = accept(p_proxy->server_socket, NULL, NULL);
        if (client_socket == -1) {
            log_debug("accept failed for new connection", strerror(errno));
            continue;
        }
        log_info("New connection accepted", NULL);
        int *args = malloc(sizeof(int));
        if (!args) {
            log_debug("Failed to allocate args", strerror(errno));
            shutdown(client_socket, SHUT_RDWR);
            close(client_socket);
            continue;
        }
        *args = client_socket;
        thread_pool_submit_task(p_proxy->p_thread_pool, handle_client, args);
    }
    return 0;
}

void proxy_destroy(t_proxy *proxy) {
    if (!proxy) {
        log_debug("Proxy has already been destroyed", NULL);
        return;
    }
    proxy->running = 0;
    close(proxy->server_socket);
    free(proxy);
    log_info("Proxy successfully destroyed", NULL);
}
