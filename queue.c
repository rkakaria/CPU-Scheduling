#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

node_t* create_node(void* data){
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->data = data;
    node->next = NULL;

    return node;
}

queue_t* create_queue(){
    queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
    queue->head = NULL;
    queue->elements = 0;

    return queue;
}

void enqueue(queue_t *queue, job_t* element){
    if (queue == NULL || element == NULL){
        return;
    }

    // create new node with job_t as its data
    node_t* newNode = create_node(element);
    if (newNode == NULL){
        return;
    }

    if (queue->head == NULL){ // if queue is empty
        queue->head = newNode;
    } else {
        node_t* head = queue->head;

        while(head->next != NULL){
            head = head->next;
        }

        head->next = newNode;
    }

    queue->elements++;
    newNode->next = NULL;
    return;
}

job_t* dequeue(queue_t* queue){
    if (queue == NULL || queue->head == NULL || queue->elements == 0){
        return NULL;
    } else {
        node_t* node = queue->head;
        queue->head = node->next;

        job_t* data = node->data;
        queue->elements--;

        // free memory associated with a dequeued node  
        free(node);
        node = NULL;

        return data;
    }
}

/* Function to free the queue nodes and the queue.
Data held by the nodes is freed elsewhere (by freeing
the jobs array at the end of the loop in main.c.) */
void destroy_queue(queue_t* queue){
    if (queue == NULL){
        return;
    }

    node_t* head = queue->head;
    while (head != NULL){
        node_t* temp = head;
        head = head->next;

        free(temp);
    }

    free(queue);
}