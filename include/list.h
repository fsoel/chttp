#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct list_node {
    void *data;
    struct list_node *next;
} list_node_t;

typedef struct list {
    list_node_t *head;
    list_node_t *tail;
    size_t size;
} list_t;

list_t *list_create(void);
void list_destroy(list_t *);
void list_push_back(list_t *, void *);
void list_push_front(list_t *, void *);
void *list_pop_back(list_t *);
void *list_pop_front(list_t *);
void *list_front(list_t *);
void *list_back(list_t *);
size_t list_size(list_t *);
void list_clear(list_t *);

#endif // LIST_H
