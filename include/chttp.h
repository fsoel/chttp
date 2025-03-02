#ifndef CHTTP_H
#define CHTTP_H

typedef struct {
    const char* path;
    const char* response;
} chttp_route_t;

void chttp_add_route(const char*, const char*);
void chttp_start_server(int);

#endif // CHTTP_H
