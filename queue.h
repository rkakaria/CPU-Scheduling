#ifndef QUEUE_H
#define QUEUE_H

enum process_state {
    NEW, READY, RUNNING, WAITING, TERMINATED
};


typedef struct job{
    // from input file
    int pid;
    int arrival_time;
    int runtime;
    int priority;
    // other attributes
    int io_time; // time spent on I/O
    int ready_time; // time spent in ready queue
    int cpu_time; // time spent on CPU
    int total_time; // total time spent in system
    int remaining_time;
    enum process_state status;
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

// void display_queue(queue_t* q);

void destroy_queue(queue_t* queue);

#endif