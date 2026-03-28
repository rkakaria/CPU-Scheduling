# Round Robin simulation trace review

This document replays the `./main` trace output (full run, total simulation time **85**) so you can verify **who holds the CPU each timestep**, **remaining CPU work after each tick**, and **preemption / I/O / termination** events.

**Legend**

| Symbol | Meaning |
|--------|---------|
| **Run** | PID that executed a CPU `tick` (`RUNNING`) |
| **rem** | `remaining` CPU time **after** that tick (from trace) |
| **cpu** | Cumulative CPU time for that job after the tick |
| **P→** | Time quantum exhausted: job moved to `READY`, next job dispatched (trace: `PREEMPTED`) |
| **IO** | Job issued I/O: moved to `WAITING` / I/O queue (trace: `move current job to io queue`) |
| **DONE** | Job finished (`remaining=0`, trace: `job_terminated`) |
| **→ next** | Next `Current Job` dispatched in the same simulation step (after the event) |

**Implied quantum:** Every `RUNNING` job gets **2 consecutive CPU ticks** before a `PREEMPTED` event, unless I/O or termination ends the burst early.

**Trace quirk:** After I/O completion the log still prints `PREEMPTED` when the job becomes `READY` (e.g. **t=81**). That is **I/O wakeup**, not a time-slice preemption.

---

## Per-timestep CPU timeline (t = 1 … 85)

Each row is one global simulation time **t** as labeled in `[TRACE t=…]`.

| t | Run | rem | cpu | Events in this timestep |
|---|-----|-----|-----|-------------------------|
| 1 | 123 | 9 | 1 | — |
| 2 | 123 | 8 | 2 | **P→** dispatch **130** (rem 11) |
| 3 | 130 | 10 | 1 | — |
| 4 | 130 | 9 | 2 | **P→** dispatch **131** (rem 1) |
| 5 | 131 | 0 | 1 | **DONE** (1/10); dispatch **124** (rem 20) |
| 6 | 124 | 19 | 1 | — |
| 7 | 124 | 18 | 2 | **P→** dispatch **126** (rem 13) |
| 8 | 126 | 12 | 1 | — |
| 9 | 126 | 11 | 2 | **P→** dispatch **127** (rem 2) |
| 10 | 127 | 1 | 1 | **IO** **127**; dispatch **129** (rem 10) |
| 11 | 129 | 9 | 1 | — |
| 12 | 129 | 8 | 2 | **P→** dispatch **132** (rem 2) |
| 13 | 132 | 1 | 1 | — |
| 14 | 132 | 0 | 2 | **DONE** (2/10); dispatch **125** (rem 13) |
| 15 | 125 | 12 | 1 | — |
| 16 | 125 | 11 | 2 | **P→** dispatch **128** (rem 3) |
| 17 | 128 | 2 | 1 | — |
| 18 | 128 | 1 | 2 | **P→** dispatch **123** (rem 8) |
| 19 | 123 | 7 | 3 | **IO** **123**; dispatch **130** (rem 9) |
| 20 | 130 | 8 | 3 | — |
| 21 | 130 | 7 | 4 | **P→** dispatch **124** (rem 18) |
| 22 | 124 | 17 | 3 | — |
| 23 | 124 | 16 | 4 | **P→** dispatch **126** (rem 11) |
| 24 | 126 | 10 | 3 | **IO** **126**; dispatch **127** (rem 1) |
| 25 | 127 | 0 | 2 | **DONE** (3/10); dispatch **129** (rem 8) |
| 26 | 129 | 7 | 3 | — |
| 27 | 129 | 6 | 4 | **P→** dispatch **125** (rem 11) |
| 28 | 125 | 10 | 3 | — |
| 29 | 125 | 9 | 4 | **P→** dispatch **128** (rem 1) |
| 30 | 128 | 0 | 3 | **DONE** (4/10); dispatch **130** (rem 7) |
| 31 | 130 | 6 | 5 | — |
| 32 | 130 | 5 | 6 | **P→** dispatch **124** (rem 16) |
| 33 | 124 | 15 | 5 | — |
| 34 | 124 | 14 | 6 | **P→** dispatch **123** (rem 7) |
| 35 | 123 | 6 | 4 | — |
| 36 | 123 | 5 | 5 | **P→** dispatch **129** (rem 6) |
| 37 | 129 | 5 | 5 | — |
| 38 | 129 | 4 | 6 | **P→** dispatch **126** (rem 10) |
| 39 | 126 | 9 | 4 | — |
| 40 | 126 | 8 | 5 | **P→** dispatch **125** (rem 9) |
| 41 | 125 | 8 | 5 | — |
| 42 | 125 | 7 | 6 | **P→** dispatch **130** (rem 5) |
| 43 | 130 | 4 | 7 | — |
| 44 | 130 | 3 | 8 | **P→** dispatch **124** (rem 14) |
| 45 | 124 | 13 | 7 | — |
| 46 | 124 | 12 | 8 | **P→** dispatch **123** (rem 5) |
| 47 | 123 | 4 | 6 | **IO** **123**; dispatch **129** (rem 4) |
| 48 | 129 | 3 | 7 | — |
| 49 | 129 | 2 | 8 | **P→** dispatch **126** (rem 8) |
| 50 | 126 | 7 | 6 | — |
| 51 | 126 | 6 | 7 | **P→** dispatch **125** (rem 7) |
| 52 | 125 | 6 | 7 | — |
| 53 | 125 | 5 | 8 | **P→** dispatch **130** (rem 3) |
| 54 | 130 | 2 | 9 | — |
| 55 | 130 | 1 | 10 | **P→** dispatch **124** (rem 12) |
| 56 | 124 | 11 | 9 | — |
| 57 | 124 | 10 | 10 | **P→** dispatch **129** (rem 2) |
| 58 | 129 | 1 | 9 | — |
| 59 | 129 | 0 | 10 | **DONE** (5/10); dispatch **126** (rem 6) |
| 60 | 126 | 5 | 8 | — |
| 61 | 126 | 4 | 9 | **P→** dispatch **123** (rem 4) |
| 62 | 123 | 3 | 7 | — |
| 63 | 123 | 2 | 8 | **P→** dispatch **125** (rem 5) |
| 64 | 125 | 4 | 9 | — |
| 65 | 125 | 3 | 10 | **P→** dispatch **130** (rem 1) |
| 66 | 130 | 0 | 11 | **DONE** (6/10); dispatch **124** (rem 10) |
| 67 | 124 | 9 | 11 | — |
| 68 | 124 | 8 | 12 | **P→** dispatch **126** (rem 4) |
| 69 | 126 | 3 | 10 | — |
| 70 | 126 | 2 | 11 | **P→** dispatch **123** (rem 2) |
| 71 | 123 | 1 | 9 | — |
| 72 | 123 | 0 | 10 | **DONE** (7/10); dispatch **125** (rem 3) |
| 73 | 125 | 2 | 11 | — |
| 74 | 125 | 1 | 12 | **P→** dispatch **124** (rem 8) |
| 75 | 124 | 7 | 13 | — |
| 76 | 124 | 6 | 14 | **P→** dispatch **126** (rem 2) |
| 77 | 126 | 1 | 12 | — |
| 78 | 126 | 0 | 13 | **DONE** (8/10); dispatch **125** (rem 1) |
| 79 | 125 | 0 | 13 | **DONE** (9/10); dispatch **124** (rem 6) |
| 80 | 124 | 5 | 15 | **IO** **124** (rem 5, `WAITING`) |
| 81 | 124 | 4 | 16 | **I/O wait tick** (`WAITING`); log `PREEMPTED` → **READY** (wakeup); dispatch **124** `RUNNING` rem 4 |
| 82 | 124 | 3 | 17 | — |
| 83 | 124 | 2 | 18 | **P→** dispatch **124** again (rem 2) — sole/head of ready queue |
| 84 | 124 | 1 | 19 | **IO** **124**; dispatch **124** `RUNNING` rem 1 (immediate re-dispatch in trace) |
| 85 | 124 | 0 | 20 | **DONE** (10/10) |

**t=0 (init only):** `scheduler_start`, `initial_arrivals`, first dispatch **123** `RUNNING` rem 10, cpu 0 (no CPU tick line at t=0).

---

## Compact “who ran CPU” strip (one character per tick)

Each digit is the **middle digit** of the PID (123→**3**, 130→**0**, …) for a quick visual scan of rotation. Dots mark steps where the trace shows **only** `WAITING` I/O countdown without a `RUNNING` CPU tick first (here: **t=81** only, before 124 resumes).

```
t:  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20
    3  3  0  0  1  4  4  6  6  7  9  9  2  2  5  5  8  8  3  0

t: 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
    0  4  4  6  7  9  9  5  5  8  0  0  4  4  3  3  9  9  6  6

t: 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60
    5  5  0  0  4  4  3  9  9  6  6  5  5  0  0  4  4  9  9  6

t: 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80
    6  3  3  5  5  0  4  4  6  6  3  3  5  5  4  4  6  6  5  4

t: 81 82 83 84 85
    .  4  4  4  4
```

At **t=81**, the first trace line is a **WAITING** tick for **124** (I/O burst countdown), not a CPU burst for another PID; the strip uses `.` for that first sub-step, then **124** is dispatched for **t=82–85**.

---

## I/O and termination checklist (from trace)

| t | Job | Event |
|---|-----|--------|
| 10 | 127 | CPU rem 2→1, then **I/O** (still rem 1 CPU after return) |
| 19 | 123 | **I/O** after 2-tick slice (rem 8→7) |
| 24 | 126 | **I/O** after 2-tick slice |
| 47 | 123 | **I/O** after 2-tick slice |
| 80 | 124 | **I/O** after 1 tick of a new slice (rem 6→5) |
| 81 | 124 | I/O completes (`WAITING` rem 5→4 in trace), **READY**, dispatch |
| 84 | 124 | **I/O** (rem 2→1), then immediate dispatch **124** |
| 5 | 131 | **DONE** |
| 14 | 132 | **DONE** |
| 25 | 127 | **DONE** |
| 30 | 128 | **DONE** |
| 59 | 129 | **DONE** |
| 66 | 130 | **DONE** |
| 72 | 123 | **DONE** |
| 78 | 126 | **DONE** |
| 79 | 125 | **DONE** |
| 85 | 124 | **DONE** |

---

## Summary statistics (from program output)

Copied from the same run for cross-check with the table above.

| Metric | Value |
|--------|------:|
| Total simulation time | 85 |
| Jobs | 10 |
| Shortest completion time | 5 (PID 131) |
| Longest completion time | 84 (PID 124) |
| Average completion time | 50.40 |
| Average time in ready queue | 45.10 |
| Average time sleeping on I/O | 3.70 |

---

## Round Robin checks this trace satisfies

1. **Fixed quantum:** Normal `RUNNING` bursts are **2 ticks** between dispatches of the *next* process, except when **I/O** or **DONE** cuts a burst short.
2. **Fair rotation:** After a preemption, a different PID runs next whenever another job is **READY** (e.g. **t=83** is the edge case where **124** is alone in the ready queue, so **124** follows **124**).
3. **I/O interleaving:** On I/O, the CPU is handed to the next dispatched job in the same timestep (e.g. **t=10** 127→129, **t=19** 123→130).

If you want this aligned to **your** `queue`/sort order (e.g. PID vs arrival tie-break), compare the **→ next** PID column against your `scheduler` policy implementation.
