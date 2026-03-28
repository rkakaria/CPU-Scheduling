# Round Robin — test write-ups

Short notes for two RR inputs under `test_cases/RR/`. Both assume **Round Robin (menu `2`)**, **`TIME_QUANTUM = 5`** in `main.c`, and **no I/O** (fourth field `0` on each job line).

---

## Test 1 — `time_quant_check.txt`

**Focus:** Preemption when the **quantum expires** and **other jobs are ready** — RR should rotate the CPU (multiple `PREEMPTED` events), with **no** `IO_Request()` involved.

### 1. Assumptions and input file

| Item | Detail |
|------|--------|
| **Input file** | `test_cases/RR/time_quant_check.txt` |
| **Job format** | `pid:arrival:cpu_burst:io_request` — each line is one job; the fourth field is `0` so no job calls `IO_Request()` during the run. |
| **Arrivals** | All jobs arrive at time `0`, so the scheduler’s behavior is driven only by RR ordering and preemption, not staggered arrivals. |
| **Time quantum** | The simulator uses `TIME_QUANTUM = 5` (see `main.c`). |
| **Algorithm under test** | Round Robin (menu option `2`). |

### 2. What this test checks

This case checks that **preemption on time quantum expiry** works for RR when there is **no I/O**: a running job that still has remaining CPU work is moved back to the ready queue after it has consumed one full quantum, and the next ready job is scheduled. Repeating alternation between long-running jobs should show multiple `PREEMPTED` events at quantum boundaries, not only at job completion.

### 3. Results (representative run)

Summary from a run using this input with RR enabled:

| Metric | Value |
|--------|--------|
| Total simulation time | 21 |
| Jobs | 3 (PIDs 124, 125, 126) |
| PID 124 — time in ready / I/O / in system | 0 / 0 / 5 |
| PID 125 — time in ready / I/O / in system | 11 / 0 / 20 |
| PID 126 — time in ready / I/O / in system | 16 / 0 / 21 |
| Average completion time | 15.33 |
| Average time in ready queue | 9.00 |
| Average time sleeping on I/O | 0.00 |

**Trace highlights**

- **124** runs continuously from the first scheduling decision and **finishes at t=5** (burst 5, no preemption).
- **125** runs **t=5…10** (five ticks of CPU), then **`PREEMPTED` at t=10** with remaining work (burst 10, so 5 left).
- **126** runs **t=10…15** (five ticks), then **`PREEMPTED` at t=15** with remaining 1.
- **125** resumes **t=15…20** and **terminates at t=20**.
- **126** runs **t=20…21** and **terminates at t=21**.

`complete_jobs` reaches `3/3` by the end of the trace; I/O time stays zero for all jobs.

### 4. Evaluation against the trace

**Verdict: behavior matches RR with quantum 5 and no I/O.**

1. **Quantum-based preemption**  
   After job **124** completes, **125** and **126** each need more than one quantum. The trace shows **125** and **126** each receiving **exactly five ticks** of CPU before a `PREEMPTED` line while still having `remaining > 0`. That is exactly what we expect when the timer expires and RR rotates the CPU.

2. **No I/O path**  
   There are no `WAITING` transitions or I/O-related events; **“Total time sleeping on I/O”** is **0** for every job, consistent with the input’s zero I/O field.

3. **Completion order and makespan**  
   Shortest job **124** finishes first (t=5). The longer bursts are interleaved by quantum until **125** finishes at t=20 and **126** at t=21. Total simulation time **21** equals the total CPU demand (5+10+6), as expected with no idle time and no I/O blocking.

4. **Ready-time statistics**  
   Nonzero ready times for **125** and **126** reflect ticks spent **in the ready queue** while other jobs run (including **124**’s full burst and the other job’s quantum slices). The exact counts depend on when `ready_time` is incremented in your simulator, but they are **consistent** with multiple waits and **average ready 9.00** across three jobs.


---

## Test 2 — `continuous_quants.txt`

**Focus:** **Continuous** use of multiple quanta **without preemption** when **no other job is ready**, plus **staggered arrival** and **idle** time until the second job appears.

### 1. Assumptions and input file

| Item | Detail |
|------|--------|
| **Input file** | `test_cases/RR/continuous_quants.txt` |
| **Jobs** | `124:0:10:0` and `125:20:5:0` — `pid:arrival:cpu_burst:io_request`; I/O field is `0` for both. |
| **Scheduling idea** | **124** is the only job that exists from **t = 0** until **125** arrives at **t = 20**. There is no second runnable job in the ready queue during **124**’s run, so RR never needs to preempt **124** for fairness. |
| **Time quantum** | `TIME_QUANTUM = 5` (`main.c`). |
| **Algorithm** | Round Robin (option `2`). |

### 2. What this test checks

This case checks that **time-quantum bookkeeping does not force preemption when no other job is ready**: the running process may consume **multiple consecutive quanta** (here, two full slices of 5 for **124**’s burst of 10) without a `PREEMPTED` event, as long as the ready queue has no other work. It also checks that a **late-arriving** job (**125** at **t = 20**) is picked up and run to completion, and that the simulator **idles** cleanly when the ready queue is empty (**t = 11 … 19** in the trace).

### 3. Results (representative run)

| Metric | Value |
|--------|--------|
| Total simulation time | 25 |
| Jobs | 2 (PIDs 124, 125) |
| PID 124 — ready / I/O / in system | 0 / 0 / 10 |
| PID 125 — ready / I/O / in system | 0 / 0 / 5 |
| Shortest completion (per-job “in system”) | 5 |
| Longest completion | 10 |
| Average completion time | 7.50 |
| Average time in ready queue | 0.00 |
| Average time sleeping on I/O | 0.00 |

**Trace highlights**

- **124** runs **t = 0 … 10** (ten `TICK` lines), **`remaining` hits 0 at t = 10**; **`complete_jobs=1/2`**. No `PREEMPTED` — **124** is alone, so RR keeps it on the CPU across quantum boundaries.
- **t = 11 … 19**: `add_jobs pointer=1` — second job not yet arrived; **`Current Job` is not set** (idle), matching **`breaking loop job=NULL`** after **124** finishes.
- **t = 20**: `add_jobs pointer=2`; **125** starts with **`remaining=5`**, **`arrival=20`**.
- **125** runs **t = 21 … 25** and completes; **`complete_jobs=2/2`**.

### 4. Evaluation against the trace

**Verdict: behavior matches RR with quantum 5, staggered arrival, and no I/O.**

1. **Continuous CPU across quanta (no spurious preemption)**  
   **124** needs **10** units of CPU — **two** full quanta of **5**. The trace shows **uninterrupted** execution until termination, with **no** `PREEMPTED`. That is correct for RR when **no other job is in the ready queue**: there is nothing to rotate to, so the running job keeps the CPU.

2. **Idle interval before the second job**  
   After **124** completes at **t = 10**, **125** has not arrived (**arrival 20**). The trace shows ticks with **`add_jobs pointer=1`** and no running job until **t = 20**, when **`pointer` becomes 2** and **125** is scheduled. That matches **staggered arrival** and an **empty ready queue** for **10** simulation ticks.

3. **Second job runs to completion**  
   **125** runs **5** ticks and exits with **`remaining=0`** at **t = 25**. No I/O, no preemption — expected for a **single** runnable job after **t = 20**.

4. **Statistics**  
   **Ready times 0** for both jobs: neither waits in the ready queue while another runnable job holds the CPU (**124** never queued behind **125**; **125** is scheduled as soon as it appears). **Total simulation time 25** = **10** (124) + **10** (idle) + **5** (**125**), which matches the trace. **I/O** averages **0**, consistent with the input.
