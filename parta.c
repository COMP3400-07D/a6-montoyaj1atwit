#include "parta.h"
#include <stdlib.h>
#include <stdio.h>

/** /
 * Initialize an array of PCBs on the heap from an array of CPU bursts.
 * Each PCB gets:
 *   - pid = its index in the array
 *   - burst_left = bursts[i]
 *   - wait = 0
 *
 * Returns a pointer to the allocated PCB array (caller must free).
 */
struct pcb* init_procs(int* bursts, int blen) {
    if (blen <= 0 || bursts == NULL) {
        return NULL;
    }

    struct pcb* procs = malloc(sizeof(struct pcb) * blen);
    if (procs == NULL) {
        return NULL;
    }

    for (int i = 0; i < blen; i++) {
        procs[i].pid = i;
        procs[i].burst_left = bursts[i];
        procs[i].wait = 0;
    }

    return procs;
}

/**
 * Print all PCBs in a simple human-readable format.
 * This is only a helper for debugging; not used by the tests.
 */
void printall(struct pcb* procs, int plen) {
    if (procs == NULL || plen <= 0) {
        return;
    }

    for (int i = 0; i < plen; i++) {
        printf("P%d: burst_left=%d wait=%d\n",
               procs[i].pid, procs[i].burst_left, procs[i].wait);
    }
}

/**
 * "Run" the current process for a given amount of time.
 *  - The current process' burst_left is reduced by run_time, where
 *      run_time = min(amount, burst_left of current).
 *  - Every *other* process with burst_left > 0 has its wait increased
 *    by run_time.
 */
void run_proc(struct pcb* procs, int plen, int current, int amount) {
    if (procs == NULL || plen <= 0) return;
    if (current < 0 || current >= plen) return;
    if (amount <= 0) return;

    int available = procs[current].burst_left;
    if (available <= 0) {
        // Current already finished; nothing happens.
        return;
    }

    int run_time = amount;
    if (available < run_time) {
        run_time = available;
    }

    // Decrease current burst.
    procs[current].burst_left -= run_time;

    // Everyone else with remaining burst waits.
    for (int i = 0; i < plen; i++) {
        if (i == current) continue;
        if (procs[i].burst_left > 0) {
            procs[i].wait += run_time;
        }
    }
}

/**
 * Run all processes using First-Come-First-Serve (FCFS).
 * Start from pid 0 and run each process until completion.
 * Uses run_proc to update burst_left and wait fields.
 *
 * Returns the total time elapsed when all processes are complete.
 */
int fcfs_run(struct pcb* procs, int plen) {
    if (procs == NULL || plen <= 0) {
        return 0;
    }

    int total_time = 0;

    for (int i = 0; i < plen; i++) {
        int remaining = procs[i].burst_left;
        if (remaining <= 0) {
            continue;
        }

        run_proc(procs, plen, i, remaining);
        total_time += remaining;
    }

    return total_time;
}

/**
 * Helper for Round-Robin: given the index of the current process,
 * return the index of the next process to run in RR order.
 *
 * Rules:
 *  - If all processes have burst_left == 0, return -1.
 *  - Otherwise, starting from (current + 1) % plen, scan forward
 *    circularly until finding a process with burst_left > 0.
 *  - It is possible that the "next" process is the same as current,
 *    if it is the only remaining process.
 */
int rr_next(int current, struct pcb* procs, int plen) {
    if (procs == NULL || plen <= 0) {
        return -1;
    }

    // First check if any process still has work.
    int has_work = 0;
    for (int i = 0; i < plen; i++) {
        if (procs[i].burst_left > 0) {
            has_work = 1;
            break;
        }
    }
    if (!has_work) {
        return -1;
    }

    int start = ((current + 1) % plen);

    // Scan at most plen entries circularly.
    for (int offset = 0; offset < plen; offset++) {
        int idx = (start + offset) % plen;
        if (procs[idx].burst_left > 0) {
            return idx;
        }
    }

    // Should not get here because we already checked has_work.
    return -1;
}

/**
 * Run all processes using Round-Robin scheduling with the given quantum.
 * Start from pid 0, always moving to the next runnable process in RR order.
 * Each step:
 *   - Run the current process for min(quantum, burst_left[current]).
 *   - Update waits using run_proc.
 *   - Use rr_next to choose the next process.
 *
 * Returns the total time elapsed when all processes are complete.
 */
int rr_run(struct pcb* procs, int plen, int quantum) {
    if (procs == NULL || plen <= 0 || quantum <= 0) {
        return 0;
    }

    int total_time = 0;

    // Find first process with work (starting from 0).
    int current = -1;
    for (int i = 0; i < plen; i++) {
        if (procs[i].burst_left > 0) {
            current = i;
            break;
        }
    }
    if (current == -1) {
        // Nothing to run.
        return 0;
    }

    while (1) {
        int remaining = procs[current].burst_left;
        if (remaining > 0) {
            int run_time = (remaining < quantum) ? remaining : quantum;
            run_proc(procs, plen, current, run_time);
            total_time += run_time;
        }

        int next = rr_next(current, procs, plen);
        if (next == -1) {
            break;  // all done
        }
        current = next;
    }

    return total_time;
}
