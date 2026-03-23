#ifndef IO_LIST_H
#define IO_LIST_H

#include "queue.h"

/*
 * Doubly linked list with dummy head and tail sentinels.
 * Real nodes live strictly between head_sentinel and tail_sentinel.
 * Sentinels have data == NULL and must never be passed to io_list_remove_node.
 */
typedef struct io_node {
    job_t* data;
    struct io_node* prev;
    struct io_node* next;
} io_node_t;

typedef struct io_list {
    io_node_t* head_sentinel;
    io_node_t* tail_sentinel;
    int elements;
} io_list_t;

/* First real node, or tail_sentinel if the list is empty. */
static inline io_node_t* io_list_begin(const io_list_t* list) {
    return list->head_sentinel->next;
}

/* One-past-last; do not dereference except for comparison / prev. */
static inline io_node_t* io_list_end(const io_list_t* list) {
    return list->tail_sentinel;
}

io_list_t* io_list_create(void);
io_node_t* io_node_create(job_t* job);
void io_list_append(io_list_t* list, job_t* job);
void io_list_remove_node(io_list_t* list, io_node_t* node);
void io_list_destroy(io_list_t* list);

#endif
