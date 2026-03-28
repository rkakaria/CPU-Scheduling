# Computer Systems Project 3: Scheduling

**Group 1** — Jonas Bartels, Reva Kakaria, Nikhil Trivedi

## Overview

This project simulates **preemptive shortest job first** (`SJF`), **round robin** (`RR`), and **multi-level feedback** (`MLFQ`) scheduling, following the course specification and pseudocode. Jobs arrive from an input file; the simulator advances a discrete clock, handles I/O, and prints per-job statistics.

## Source layout

| Module | Role |
|--------|------|
| `queue.h` / `queue.c` | Queue as a singly linked list; `job_t` definition and queue operations |
| `scheduler.h` / `scheduler.c` | Parse input, arrivals, I/O wakeups, ready-queue integration |
| `sort.h` / `sort.c` | Merge sort of ready queues by arrival time, remaining time, etc. |
| `display.h` / `display.c` | Formatted statistics output |
| `main.c` | Outer scheduling loop, preemption hooks, simulation driver |

## Build and run

```bash
make
./main
```

- Job definitions are read from **`input_file.txt`** (same directory as the binary; format `pid:arrival:service:priority` per line).
- The active algorithm is set in **`main.c`** via `algorithm_t schedule` (`RR`, `SJF`, or `MLFQ`). Rebuild after changing it.

Sample traces under `tests/` can be copied or adapted into `input_file.txt` for manual checks.

## Simulation model

The design uses one **outer** loop (pick next runnable job from the sorted ready queue) and one **inner** loop (advance time for the current job until it stops running).

1. **Clock** — Time starts at `t = 0`. Each inner iteration begins by incrementing the clock (e.g. to `t = 1` on the first pass). The running job’s CPU slice is accounted for in terms of that tick; bookkeeping runs after the increment (see `main.c` around the inner `while`).

2. **Bookkeeping** — After each tick, every job in ready, waiting, or running state gets updated time counters according to its status. Status at the start of the tick is what drives that update; transitions happen after the current job has received its slice for that tick.

3. **Arrivals and I/O** — Jobs that arrive at the **current** clock value are enqueued to the ready queue during that iteration. They are **not** scheduled on the CPU until the **next** inner iteration (after the clock advances again). Jobs that finish an I/O burst are moved to the ready queue with the same rule.

4. **Ordering** — Once all jobs that can become ready for the next step are in the ready queue, it is sorted according to the active algorithm’s policy.

5. **Current job** — If nothing is runnable, the inner loop ends. Otherwise the simulator updates the running job: termination, preemption (algorithm-specific), or I/O request. If the job is no longer `RUNNING`, the inner loop breaks; the outer loop dequeues the next job from the already-sorted ready queue and repeats.

## Member contributions

**Everyone** — Problem framing, test cases, and integrating each other’s pieces.

| Member | Focus |
|--------|--------|
| Jonas | Multilevel feedback scheduling |
| Reva | Preemptive SJF; queue and job types; sorting; I/O and parsing glue; README |
| Nikhil | Round robin; main loop structure and shared scheduling flow |

All members iterated on shared design and logic across algorithms.
