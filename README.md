# chttp

A lightweight HTTP server library in C supporting minimal HTTP/0.9 requests with simple route registration. It uses conditional compilation to work on both Windows (Winsock) and Unix-like systems (BSD sockets).

## Building

Use the provided Makefile.

```sh
make
```

## Usage

Include the header and register your routes:

```c
#include "chttp.h"

int main(void) {
    chttp_add_route("/", "<html><body><h1>Hello, World!</h1></body></html>");
    chttp_start_server(8080);
    return 0;
}
```

## Limitations

- Only HTTP/0.9 is fully implemented.
