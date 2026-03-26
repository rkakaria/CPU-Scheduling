#include <stdio.h>
#include "display.h"

void print_header(){
    printf("\n%35s\n\n", "RESULTS");
    printf("%10s %12s %5s %12s %5s %12s\n", "|", "Total time", "|", "Total time", "|", "Total time");
    printf("%7s %2s %13s %2s %12s %3s %12s\n", "Job #", "|", "in ready to run", "|", "in sleeping on", "|", "in system");
    printf("%10s %10s %7s %12s %5s\n", "|", "state", "|", "I/O state", "|");
    printf("=========+==================+==================+================+\n");
}

void print_data(int clock, job_t** jobs, int num_jobs){
    float average_completion = 0;
    float average_ready = 0;
    float average_io = 0;
    int shortest = jobs[0]->total_time;
    int longest = shortest;

    for(int i = 0; i < num_jobs; i++){
        printf("PID: %d %1s %8d %9s %9d %8s %9d\n", jobs[i]->pid, "|", jobs[i]->ready_time, "|", jobs[i]->io_time, "|", jobs[i]->total_time);

        if(jobs[i]->total_time < shortest){
            shortest = jobs[i]->total_time;
        }

        if(jobs[i]->total_time > longest){
            longest = jobs[i]->total_time;
        }

        average_completion += jobs[i]->total_time;
        average_ready += jobs[i]->ready_time;
        average_io += jobs[i]->io_time;

    }
    printf("Total simulation time: %d\n", clock);
    printf("Total number of jobs: %d\n", num_jobs);
    printf("Shortest job completion time: %d\n", shortest);
    printf("Longest job completion time: %d\n", longest);
    printf("Average completion time: %0.2f\n", average_completion/num_jobs);
    printf("Average time in ready queue: %0.2f\n", average_ready/num_jobs);
    printf("Average time sleeping on I/O state: %0.2f\n\n", average_io/num_jobs);
}