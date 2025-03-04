#include "chttp.h"

#include <stdio.h>

int main(void) {
    int port = 8080;
    printf("Starting HTTP server on port %d\n", port);
    chttp_add_route("/", "<html><body><h1>Hello, World!</h1></body></html>");
    chttp_add_route("/about", "<html><body><h1>About</h1></body></html>");
    chttp_start_server(port);
    return 0;
}
