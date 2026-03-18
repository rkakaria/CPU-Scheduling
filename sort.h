#ifndef SORT_H
#define SORT_H
#include "queue.h"

node_t* split(node_t* head);

node_t* merge(node_t* left, node_t* right);

node_t* merge_sjf(node_t* left, node_t* right);

node_t* merge_mlfq(node_t* left, node_t* right);

node_t* merge_sort(node_t* head, char* schedule);

#endif