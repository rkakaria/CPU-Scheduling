#ifndef SORT_H
#define SORT_H
#include "queue.h"

/* This file defines functions to merge sort 
a queue based on different characteristics of jobs. */

typedef enum {
    RR, SJF, MLFQ
} algorithm_t;

node_t* split(node_t* head);

node_t* merge(node_t* left, node_t* right);

node_t* merge_sjf(node_t* left, node_t* right);

node_t* merge_mlfq(node_t* left, node_t* right);

node_t* merge_sort(node_t* head, algorithm_t schedule);

#endif