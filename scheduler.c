#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scheduler.h"
#include "sort.h"

#define CHANCE_OF_IO_REQUEST 6
#define CHANCE_OF_IO_COMPLETE 4

// ask user for which algorithm to run
algorithm_t input(){
    int schedule = 0;
    char buff[20];

    while(true){
		printf("Enter 1 for Preemptive Shortest Job First\nEnter 2 for Round Robin\nEnter 3 for Multi-Level Feedback Queue\n");
		char* input = fgets(buff, 10, stdin);
		schedule = strtol(buff, NULL, 10);
		if(input != NULL && (schedule < 1 || schedule > 3)){
            printf("Selection must be 1, 2, or 3.\n");
        }
		else if(input != NULL && schedule != 0){
			break;
		} else{
			printf("Invalid entry, try again!\n");
		}
	}

    if(schedule == 1){
        return SJF;
    } else if(schedule == 2){
        return RR;
    } else{
        return MLFQ;
    }
}

// function to sort jobs array by arrival time
int comp(const void* a, const void* b){
    job_t* arg1 = *(job_t**)a;
    job_t* arg2 = *(job_t**)b;

    if(arg1->arrival_time == arg2->arrival_time){
        if(arg1->pid < arg2->pid){
            return -1;
        } else {
            return 1;
        }
    } else if (arg1->arrival_time < arg2->arrival_time){
        return -1;
    } else {
        return 1;
    }
}

// function to sort jobs array by pid
int comp_pid(const void*a, const void* b){
    job_t* arg1 = *(job_t**)a;
    job_t* arg2 = *(job_t**)b;

    if (arg1->pid < arg2->pid){
        return -1;
    } else {
        return 1;
    }
}

// calculate number of jobs in input file
int number_jobs(){
    FILE* fptr = fopen("input_file.txt", "r");

    int pid;
    int arrival;
    int service;
    int priority;

    // count number of lines
    int num = 0;
    while (fscanf(fptr, "%d:%d:%d:%d", &pid, &arrival, &service, &priority) == 4) {
       num++;
    }
    rewind(fptr);
    fclose(fptr);

    return num;
}

// create array of jobs from input file
job_t** parse(){
    FILE* fptr = fopen("input_file.txt", "r");

    int pid;
    int arrival;
    int service;
    int priority;

    int num = number_jobs();

    // malloc array of jobs
    job_t** jobs = (job_t**)malloc(sizeof(job_t*)*num);

    if(jobs == NULL){
        return NULL;
    }
    
    // initialize all job_ts and put them in the array by parsing input
    int counter = 0;
    while (fscanf(fptr, "%d:%d:%d:%d", &pid, &arrival, &service, &priority) == 4) {
        job_t* job = (job_t*)malloc(sizeof(job_t));
        
        if(job == NULL){
            return NULL;
        }

        job->pid = pid;
        job->arrival_time = arrival;
        job->runtime = service;
        job->priority = priority;
        
        job->remaining_time = service;
        job->status = NEW;
        job->io_time = 0;
        job->ready_time = 0;
        job->total_time = 0;
        job->cpu_time = 0;

        jobs[counter] = job;
        counter++;
    }
    rewind(fptr);
    fclose(fptr);

    // sort by arrival time
    qsort(jobs, num, sizeof(jobs[0]), comp);

    return jobs;
}

// srand wrapper
void os_srand(int seed){
    srand(seed);
}

// rand wrapper
int os_rand(){ 
    return rand();
}

int IO_request(){ 
    if(os_rand() % CHANCE_OF_IO_REQUEST == 0 ){
        return 1;
    } else {
        return 0;
    }
}

int IO_complete(){
    if(os_rand() % CHANCE_OF_IO_COMPLETE == 0){
        return 1;
    } else {
        return 0;
    }
}

// add newly arriving jobs to ready queue
int add_jobs(int clock, int pointer, int num_jobs, job_t** jobs, queue_t* ready_q){
    for(int i = pointer; i < num_jobs; i++){
        if(jobs[i]->arrival_time > clock){
            pointer = i;
            break;
        }

        if(i + 1 == num_jobs){
            pointer = num_jobs;
        }

        jobs[i]->status = READY; // status as ready
        enqueue(ready_q, jobs[i]);
    }

    return pointer;
}

// check all IO jobs for completion of IO
void wake_io(queue_t* io_q, queue_t* ready_q){
    node_t* temp = io_q->head;
    node_t* prev;

    while(temp != NULL){ 
        temp->data->io_time++;
        
        if(IO_complete() == 1){
            // extract the node with the job that has completed io
            if(temp == io_q->head){ 
                temp = temp->next;
                job_t* head_job = dequeue(io_q);
                head_job->status = READY;
                enqueue(ready_q, head_job);
            } else {
                prev->next = temp->next;
                temp->data->status = READY;
                enqueue(ready_q, temp->data);
                free(temp);
                temp = prev->next;
                io_q->elements--;
            }
        } else {
            prev = temp;
            temp = prev->next;
        }
    }
}

// check for preemption
bool preemption(algorithm_t algorithm, queue_t* ready_q, job_t* current_job){
    if(ready_q->elements <= 0){
        return false;
    }

    if(algorithm == SJF){
        job_t* shortest = ready_q->head->data;

        if((shortest->remaining_time < current_job->remaining_time) || (shortest->remaining_time == current_job->remaining_time && shortest->pid < current_job->pid)){ // if there is a higher priority job
            return true;
        }
    } else if(algorithm == MLFQ){
        return true;
    }

    return false;
}
