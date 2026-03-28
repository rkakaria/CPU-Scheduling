#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sort.h"

// split linked list into two
node_t* split(node_t* head){
    if(head == NULL || head->next == NULL){
        return head;
    }

    node_t* slow = head;
    node_t* fast = head->next;

    while(fast != NULL && fast->next != NULL){
        slow = slow->next;
        fast = fast->next->next;
    }
    
    node_t* temp = slow;
    slow = slow->next;
    temp->next = NULL;

    return slow;
}

// sort based on arrival time
node_t* merge(node_t* left, node_t* right){
    node_t* newlist = NULL;
    node_t* tail = NULL;
    node_t* newnode;

    while(left != NULL && right != NULL){
        newnode = NULL;

        if(left->data->arrival_time < right->data->arrival_time){
            newnode = left;
            left = left->next;
        } else if(right->data->arrival_time < left->data->arrival_time){
            newnode = right;
            right = right->next;
        } else { // tiebreak
            if (left->data->pid < right->data->pid){
                newnode = left;
                left = left->next;
            } else {
                newnode = right;
                right = right->next;
            }
        }

        if(tail == NULL){
            tail = newnode;
            newlist = newnode;
        } else {
            tail->next = newnode;
            tail = tail->next;
        }
    }

    if(left != NULL){
        tail->next = left;
    } else {
        tail->next = right;
    }

    return newlist;
}

// sort based on remaining job time (sjf)
node_t* merge_sjf(node_t* left, node_t* right){
    node_t* newlist = NULL;
    node_t* tail = NULL;
    node_t* newnode;

    while(left != NULL && right != NULL){
        newnode = NULL;

        if(left->data->remaining_time < right->data->remaining_time){
            newnode = left;
            left = left->next;
        } else if(right->data->remaining_time < left->data->remaining_time){
            newnode = right;
            right = right->next;
        } else { // tiebreak
            if (left->data->pid < right->data->pid){
                newnode = left;
                left = left->next;
            } else {
                newnode = right;
                right = right->next;
            }
        }

        if(tail == NULL){
            tail = newnode;
            newlist = newnode;
        } else {
            tail->next = newnode;
            tail = tail->next;
        }
    }

    if(left != NULL){
        tail->next = left;
    } else {
        tail->next = right;
    }

    return newlist;
}

// sort based on priority (mlfq)
node_t* merge_mlfq(node_t* left, node_t* right){
    node_t* newlist = NULL;
    node_t* tail = NULL;
    node_t* newnode;

    while(left != NULL && right != NULL){
        newnode = NULL;

        if(left->data->priority < right->data->priority){
            newnode = left;
            left = left->next;
        } else if(right->data->priority < left->data->priority){
            newnode = right;
            right = right->next;
        } else { // tiebreak
            if (left->data->pid < right->data->pid){
                newnode = left;
                left = left->next;
            } else {
                newnode = right;
                right = right->next;
            }
        }

        if(tail == NULL){
            tail = newnode;
            newlist = newnode;
        } else {
            tail->next = newnode;
            tail = tail->next;
        }
    }

    if(left != NULL){
        tail->next = left;
    } else {
        tail->next = right;
    }

    return newlist;
}

node_t* merge_sort(node_t* head, algorithm_t schedule){
    if(head == NULL || head->next == NULL){
        return head;
    }

    node_t* left = head;
    node_t* right = split(head);

    left = merge_sort(left, schedule);
    right = merge_sort(right, schedule);

    if(schedule == SJF){ // sort by remaining time
        return merge_sjf(left, right);
    } else if(schedule == MLFQ){ // sort by priority
        return merge_mlfq(left, right);
    }else { // sort by arrival time
        return merge(left, right);
    }
}