## Computer Systems Project 3: Scheduling

Group 1
<br>
Jonas Bartels, Reva Kakaria, Nikhil Trivedi

### Overview
This project implements preemptive shortest job first, round robin, and multi-level feedback scheduling, based on the provided specifications and pseudocode. 

### Code files
<u>Queue.h/Queue.c</u>: Implements queue structure and functions as a singly-linked list. Also defines a job struct.<br>
<u>Scheduler.h/Scheduler.c</u>: Implements scheduling functions such as initializing jobs from input file, checking for IO, and adding newly ready jobs to the ready queue.<br>
<u>Sort.h/Sort.c</u>: Implements merge sorting of a queue of jobs based on various attributes of a job (i.e. arrival time, remaining time, etc.).<br>
<u>Display.h/Display.c</u>: Implements the output formatting

### How to Compile and Test

Run the following commands:

```make```<br>
```./main```

Enter in either 1, 2, or 3 as prompted to select a scheduling algorithm.

### Assumptions

To demonstrate our assumptions, the following is a high-level example of how the first iteration of our program works:

Time starts at t = 0. We increment the clock to t = 1 at the beginning of the first iteration of the inner while loop; this is to represent that the first current job, if any, receives CPU time from t = 0 to to = 1. Therefore, CPU work time is treated as complete by line 45 of main.c, where the clock increments.

As such, we update bookkeeping immediately after line 45. Every ready, waiting, or running job's total time increments, and status-specific time metrics increment. The status of each job is "fixed" at the beginning of the loop, and cannot change until after the current job has received its time slice on the CPU. 

The array of all jobs is then checked to determine whether any jobs are arriving at t = 1. These jobs are added to the ready queue. We assume that jobs arriving at t = 1 cannot be scheduled on the CPU until the next iteration of the inner loop, at the beginning of which t = 2. We then check whether any waiting jobs have completed their I/O bursts; if so, they are added to the ready queue with the same assumption as the newly arrived jobs.

At this time, all jobs that could possibly be ready in the next iteration have been added to the ready queue, so we sort the queue by whatever priority is needed for the specific algorithm. 

We then determine what the state of the current job will be on the next iteration. If there is no current job because there were no ready jobs to choose from, the loop breaks. This is our implementation of an "idle" process, for the case where no process is running and we do not need to track statistics for this pseudo-process. Otherwise, we update the statistics of the current job, and then we check whether the current job is complete; if not, we check if will be preempted by any of the ready jobs; if not, we check if requests I/O. In each of these cases, the status that the current job should start with on the next iteration is updated.

If the status of the current job is no longer RUNNING (i.e. it is TERMINATED, WAITING, or READY), the inner loop breaks. We return to the outer loop, where we choose a new current job from the ready queue, which has already been sorted by priority. Then we enter the inner loop at t = 2 and repeat the process.

Additional assumptions:
- The priority of jobs is >= 0.
- The PID of jobs is >= 0.
- The arrival time of jobs is >= 0.
- The service time of jobs is > 0.
- The number of total arriving jobs is >= 0.
- There are no duplicate PIDs.
- All lines in the input file are in the ```Process Id:Arrival time:Service time:Priority``` format, with no header line.

### Test Cases
In order to assess the correct implementation of preemption for the shortest jot first and multilevel feedback queue algorithms, we tested our program both with and without the possibility of jobs entering an I/O burst. This included varying number of jobs, arrival times, service times, and priorities. Several test input files are included in the folder ```test_cases```.

### Results

### Member Involvement

All group members:
- Brainstorming and approach conception
- Test cases

| Member | Focus |
|--------|--------|
| Jonas | Multilevel feedback scheduling |
| Reva | Preemptive SJF; queue and job types; sorting; I/O and parsing glue; README |
| Nikhil | Round robin; main loop structure and shared scheduling flow |

All group members built upon each others' work and actively worked together to create an approach and logic to apply towards each scheduling algorithm.