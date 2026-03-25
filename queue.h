#ifndef QUEUE_H
#define QUEUE_H

typedef struct job{
    // from input file
    int pid;
    int arrival_time;
    int runtime;
    int priority;
    // other attributes
    int io_time;
    int ready_time;
    int total_time;
    int remaining_time;
    int status; // assign status codes?
} job_t;

typedef struct node{
    job_t* data;
    struct node* next;
} node_t;

typedef struct queue{
    node_t* head;
    int elements; // current queue size
} queue_t;

node_t* create_node(void* data);

queue_t* create_queue();

void enqueue(queue_t* queue, job_t* element);

job_t* dequeue(queue_t* queue); 

void display_queue(queue_t* q);

void destroy_queue(queue_t* queue);

#endif