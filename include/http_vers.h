#ifndef HTTP_VERS_H
#define HTTP_VERS_H

typedef enum {
    HTTP_0_9,
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0,
    HTTP_3_0,
    HTTP_UNKNOWN
} http_version_t;

#endif // HTTP_VERS_H
