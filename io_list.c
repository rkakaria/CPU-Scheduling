#include <stdlib.h>
#include "io_list.h"

static io_node_t* sentinel_create(void) {
    io_node_t* s = (io_node_t*)malloc(sizeof(io_node_t));
    if (s == NULL) {
        return NULL;
    }
    s->data = NULL;
    s->prev = NULL;
    s->next = NULL;
    return s;
}

io_list_t* io_list_create(void) {
    io_list_t* list = (io_list_t*)malloc(sizeof(io_list_t));
    if (list == NULL) {
        return NULL;
    }

    list->head_sentinel = sentinel_create();
    list->tail_sentinel = sentinel_create();
    if (list->head_sentinel == NULL || list->tail_sentinel == NULL) {
        free(list->head_sentinel);
        free(list->tail_sentinel);
        free(list);
        return NULL;
    }

    list->head_sentinel->next = list->tail_sentinel;
    list->head_sentinel->prev = NULL;
    list->tail_sentinel->prev = list->head_sentinel;
    list->tail_sentinel->next = NULL;
    list->elements = 0;

    return list;
}

io_node_t* io_node_create(job_t* job) {
    if (job == NULL) {
        return NULL;
    }

    io_node_t* node = (io_node_t*)malloc(sizeof(io_node_t));
    if (node == NULL) {
        return NULL;
    }

    node->data = job;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void io_list_append(io_list_t* list, job_t* job) {
    if (list == NULL || job == NULL) {
        return;
    }

    io_node_t* node = io_node_create(job);
    if (node == NULL) {
        return;
    }

    /* Insert immediately before tail_sentinel (uniform; no empty-list branch). */
    io_node_t* before_tail = list->tail_sentinel->prev;
    node->next = list->tail_sentinel;
    node->prev = before_tail;
    before_tail->next = node;
    list->tail_sentinel->prev = node;

    list->elements++;
}

void io_list_remove_node(io_list_t* list, io_node_t* node) {
    if (list == NULL || node == NULL) {
        return;
    }
    /* Never unlink sentinels. */
    if (node == list->head_sentinel || node == list->tail_sentinel) {
        return;
    }

    node->prev->next = node->next;
    node->next->prev = node->prev;

    if (list->elements > 0) {
        list->elements--;
    }

    free(node);
}

void io_list_destroy(io_list_t* list) {
    if (list == NULL) {
        return;
    }

    io_node_t* current = list->head_sentinel->next;
    while (current != list->tail_sentinel) {
        io_node_t* next = current->next;
        free(current);
        current = next;
    }

    free(list->head_sentinel);
    free(list->tail_sentinel);
    free(list);
}
