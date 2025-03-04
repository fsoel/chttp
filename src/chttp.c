#include "chttp.h"
#include "http_vers.h"
#include "list.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define CLOSESOCKET closesocket
#define SOCKET_TYPE SOCKET
#define SOCKADDR_TYPE struct sockaddr
#define SOCKADDR_IN_TYPE struct sockaddr_in
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define CLOSESOCKET close
#define SOCKET_TYPE int
#define SOCKADDR_TYPE struct sockaddr
#define SOCKADDR_IN_TYPE struct sockaddr_in
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static http_version_t detect_http_version(const char *);
static void handle_http_09(int, const char *);
static void http_handle_client(int);
static int network_init(void);
static void network_cleanup(void);

static list_t *route_list = NULL;
static int route_count = 0;

/* Detects the HTTP version of the request */
static http_version_t detect_http_version(const char *request_line) {
    const char *http_sub = strstr(request_line, "HTTP/");
    if (!http_sub) {
        return HTTP_0_9;
    }
    if (strncmp(http_sub, "HTTP/1.0", 8) == 0) {
        return HTTP_1_0;
    } else if (strncmp(http_sub, "HTTP/1.1", 8) == 0) {
        return HTTP_1_1;
    } else if (strncmp(http_sub, "HTTP/2.0", 8) == 0) {
        return HTTP_2_0;
    } else if (strncmp(http_sub, "HTTP/3.0", 8) == 0) {
        return HTTP_3_0;
    }
    return HTTP_UNKNOWN;
}

static void handle_http_09(int client_sock, const char *request) {
    const char *first_space = strchr(request, ' ');
    if (!first_space) {
        first_space = request + strlen(request);
    }

    // Calculate length of the path token
    size_t path_len = (size_t)(first_space - request);
    if (path_len == 0) {
        send(client_sock, "400 Bad Request\n", 16, 0);
        return;
    }
    if (path_len > 127) {
        send(client_sock, "414 URI Too Long\n", 17, 0);
        return;
    }

    char path[128];
    strncpy(path, request, path_len);
    path[path_len] = '\0';

    // Trim any trailing newline or carriage return characters
    while (path_len > 0 && (path[path_len - 1] == '\n' || path[path_len - 1] == '\r')) {
        path[--path_len] = '\0';
    }

    // Compare with registered routes
    for (int i = 0; i < route_count; i++) {
        chttp_route_t *route = (chttp_route_t *)list_front(route_list);
        if (strcmp(path, route->path) == 0) {
            send(client_sock, route->response, strlen(route->response),
                 0);
            return;
        }
    }

    send(client_sock, "404 Not Found\n", 14, 0);
}

static void http_handle_client(int client_sock) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int n = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        CLOSESOCKET(client_sock);
        return;
    }

    if (strncmp(buffer, "GET ", 4) == 0) {
        http_version_t ver = detect_http_version(buffer);
        switch (ver) {
        case HTTP_0_9:
            handle_http_09(client_sock, buffer + 4);
            break;
        default:
            printf("Unsupported HTTP version\n");
            break;
        }
    }
    CLOSESOCKET(client_sock);
}

/* Initialize the network library */
static int network_init(void) {
#ifdef _WIN32
    WSADATA wsa_data;
    int wsa_err = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (wsa_err != 0) {
        fprintf(stderr, "WSAStartup failed with error %d\n", wsa_err);
        return -1;
    }
#endif
    return 0;
}

/* Cleanup the network library */
static void network_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

/* Start a HTTP server on the given port */
void chttp_start_server(int port) {
    if (network_init() != 0) {
        exit(EXIT_FAILURE);
    }

    SOCKET_TYPE server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if ((int)server_sock < 0) {
        perror("socket");
        network_cleanup();
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval,
                   sizeof(optval)) < 0) {
        perror("setsockopt");
        CLOSESOCKET(server_sock);
        network_cleanup();
        exit(EXIT_FAILURE);
    }

    SOCKADDR_IN_TYPE server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (SOCKADDR_TYPE *)&server_addr, sizeof(server_addr)) <
        0) {
        perror("bind");
        CLOSESOCKET(server_sock);
        network_cleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) < 0) {
        perror("listen");
        CLOSESOCKET(server_sock);
        network_cleanup();
        exit(EXIT_FAILURE);
    }

    printf("Simple HTTP server listening on port %d...\n", port);

    while (1) {
        SOCKADDR_IN_TYPE client_addr;
        socklen_t client_len = sizeof(client_addr);
        SOCKET_TYPE client_sock =
            accept(server_sock, (SOCKADDR_TYPE *)&client_addr, &client_len);
        if ((int)client_sock < 0) {
            perror("accept");
            break;
        }
        http_handle_client(client_sock);
    }

    CLOSESOCKET(server_sock);
    network_cleanup();
    list_destroy(route_list);
}

/* Add a route for HTTP */
void chttp_add_route(const char *path, const char *response) {
    if (route_list == NULL) {
        route_list = list_create();
    }

    chttp_route_t *route = (chttp_route_t *)malloc(sizeof(chttp_route_t));
    if (!route) {
        return;
    }

    route->path = path;
    route->response = response;

    list_push_back(route_list, route);
    route_count++;
}
