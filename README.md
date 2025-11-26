[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/LFoV51LH)
# Simple CPU Scheduler

## Introduction

In this assignment you will be writing a simple CPU schedule simulator.

To get started, first clone from GitHub. You may need to "close" the existing folder first.

### Build Instructions

To build this project run the `make` command in the terminal. You must run this *every time you change a file*.

### Running Unit Tests

To run the unit tests, see each part below.

### Commit Instructions

After completing Part A, first add the file called `parta.c` to staging:

    git add parta.c

Then do the actual commit:

    git commit -m 'Completed Part A'

Do the same after every function/file completed.

When you are ready to submit the assignment, run the following command:

    git push

## Instructions

There are two source files you need to complete. parta.c contains the scheduling code, and is
testing using the unit test file test_a.cpp. parta_main.c contains the command-line interface, and
should be done *after* parta.c is complete.

### parta.c

#### Init

    struct pcb* init_procs(int* bursts, int blen);

The first function you should complete is `init_procs`. This function takes an array of CPU bursts,
and returns an array of PCBs. The PCBs should be created in the heap, and each object contains the
PID, burst_left, and the total wait this process has experienced so far.

To run the unit tests for this part, run:

    ./test_parta_init

#### Print All

This is a helper function that are not tested against, but may serve useful. It
prints the current values of each PCB. 

    void printall(struct pcb* procs, int plen);

#### Run Processes

    void run_proc(struct pcb* procs, int plen, int current, int amount);

`run_proc` "runs" the current process by reducing its `burst_left` by amount, *and also* increases
the other processes `wait` by the same amount. For example, let's say we have burst 5, 8, 2. If
`current` is 0 and `amount` 4, then:

- PID 0 burst goes down by 4
- PID 1 wait goes up by 4
- PID 2 wait goes up by 4

To run the unit tests for this part, run:

    ./test_parta_run_proc

#### Next Process

    int rr_next(int prev, struct pcb* procs, int plen);

A helper function called `next` is provided to make it easier to develop the scheduler. These
functions return the next process to run. For example, let's say there are 2 processes total. If the
previous process was P0, the next is P1; if prev was P1, next is P0. If prev was P1 but P0 was
already completed (`burst_left` is 0), then next is P1 again. If all
the processes are complete (burst_left is 0), then return -1.

To run the unit tests for this part, run:

    ./test_parta_rr_next

#### Schedulers

    int fcfs_run(struct pcb* procs, int plen);
    int rr_run(struct pcb* procs, int plen, int quantum);
 
You are given an array of PCBs (and the time quantum for round-robin). Start from pid 0, and start
scheduling according to the scheduler algorithm. Keep track of the current time, and schedule
processes one by one. When all processes are done (`burst_left` is 0), then stop scheduling and
return the current time. The PCBs will be updated with each process' `wait`.

First-come-first-serve scheduling will start from index 0 and run each process until completion.

Round-robin scheduling will take an additional parameter (`quantum`). Each process will run until it
runs out of CPU burst or the time quantum (whichever comes first). 

Assume that all processes arrived that the same time, but in the order listed.

For example, if we have burst 5 and 8 and run FCFS:

    0      5      13
    +------+------+
    |   P0 |  P1  |
    +------+------+

will be the Gantt chart. P0 will return wait 0, P1 wait 5, and total time elapsed is 13.

If we have burst 5 and 8 and run Round-robin(4):

    0   4   8 9   13
    +---+---+-+---+
    | P0| P1|0| P1|
    +---+---+-+---+

will be the Gantt chart. P0 will return wait 4, P1 wait 5, and total time elapsed is 13.

To run the unit tests for this part, run:

    ./test_parta_fcfs
    ./test_parta_rr

### parta_main.c

You are to process the command-line arguments. The first argument is the algorithm to use: "fcfs" or
"rr". If using fcfs, then remaining values are the array of CPU bursts. If using rr, the immediate
next value is the time quantum, and the CPU bursts follow that.

After identifying the algorithm to use, print the current setting and then print the average wait
time (up to 2 decimal points). For example, if we want fcfs with 3 processes:

    $ ./parta_main fcfs 5 8 2
    Using FCFS

    Accepted P0: Burst 5
    Accepted P1: Burst 8
    Accepted P2: Burst 2
    Average wait time: 6.00

If we want round-robin with time quantum 2, with 3 processes:

    $ ./parta_main rr 2 5 8 2
    Using RR(2).

    Accepted P0: Burst 5
    Accepted P1: Burst 8
    Accepted P2: Burst 2
    Average wait time: 5.67

If the command-line arguments are not correctly provided, print a usage message and exit with status
1 immediately. For example:

    $ ./parta_main fcfs
    ERROR: Missing arguments

    $ ./parta_main rr 2
    ERROR: Missing arguments

You may use any function from stdlib.h, stdio.h, string.h, or ctype.h. For example, `strcmp` or `atoi`
can be used.

To test this part, run the following command in the terminal:

    bats tests/parta.bats
