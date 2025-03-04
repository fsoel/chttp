#include "list.h"

#include <stdlib.h>

// Create an empty list
list_t *list_create(void) {
    list_t *list = (list_t *)malloc(sizeof(list_t));
    if (!list) {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

// Destroy a list
void list_destroy(list_t *list) {
    list_clear(list);
    free(list);
}

// Add an element to the back of the list
void list_push_back(list_t *list, void *data) {
    list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));
    if (!node) {
        return;
    }

    node->data = data;
    node->next = NULL;

    if (list->size == 0) {
        list->head = node;
    } else {
        list->tail->next = node;
    }
    list->tail = node;
    list->size++;
}

// Add an element to the front of the list
void list_push_front(list_t *list, void *data) {
    list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));
    if (!node) {
        return;
    }

    node->data = data;
    node->next = list->head;

    if (list->size == 0) {
        list->tail = node;
    }

    list->head = node;
    list->size++;
}

// Remove and return the element at the back of the list
void *list_pop_back(list_t *list) {
    if (list->size == 0) {
        return NULL;
    }

    list_node_t *node = list->head;
    list_node_t *prev = NULL;

    while (node->next) {
        prev = node;
        node = node->next;
    }

    void *data = node->data;

    if (prev) {
        prev->next = NULL;
        list->tail = prev;
    } else {
        list->head = NULL;
        list->tail = NULL;
    }

    free(node);
    list->size--;

    return data;
}

// Remove and return the element at the front of the list
void *list_pop_front(list_t *list) {
    if (list->size == 0) {
        return NULL;
    }

    list_node_t *node = list->head;
    void *data = node->data;

    list->head = node->next;
    if (!list->head) {
        list->tail = NULL;
    }

    free(node);
    list->size--;

    return data;
}

// Return the list element at the front of the list
void *list_front(list_t *list) {
    if (list->size == 0) {
        return NULL;
    }

    return list->head;
}

// Return the list element at the back of the list
void *list_back(list_t *list) {
    if (list->size == 0) {
        return NULL;
    }

    return list->tail;
}

// Return the number of elements in the list
size_t list_size(list_t *list) {
    return list->size;
}

// Remove all elements from the list
void list_clear(list_t *list) {
    while (list->size > 0) {
        list_pop_front(list);
    }
}

// Return the next element in the list after the given node
void *list_next(void *node) {
    return ((list_node_t *)node)->next;
}
