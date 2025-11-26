#include "parta.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

static void print_missing_args_error(void) {
    printf("ERROR: Missing arguments\n");
}

/**
 * Command-line driver for the CPU scheduler.
 *
 * Usage:
 *   ./parta_main fcfs <burst1> <burst2> ...
 *   ./parta_main rr <quantum> <burst1> <burst2> ...
 *
 * On success, prints:
 *   - The algorithm used
 *   - List of accepted processes and bursts
 *   - Average wait time (2 decimal places)
 *
 * On incorrect/missing arguments, prints an error and exits with status 1.
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_missing_args_error();
        return 1;
    }

    struct pcb* procs = NULL;

    if (strcmp(argv[1], "fcfs") == 0) {
        // Need at least one burst.
        if (argc < 3) {
            print_missing_args_error();
            return 1;
        }

        int plen = argc - 2;
        int* bursts = malloc(sizeof(int) * plen);
        if (bursts == NULL) {
            fprintf(stderr, "ERROR: Memory allocation failed\n");
            return 1;
        }

        for (int i = 0; i < plen; i++) {
            bursts[i] = atoi(argv[i + 2]);
        }

        procs = init_procs(bursts, plen);
        free(bursts);

        if (procs == NULL) {
            fprintf(stderr, "ERROR: Failed to initialize processes\n");
            return 1;
        }

        printf("Using FCFS\n\n");

        for (int i = 0; i < plen; i++) {
            printf("Accepted P%d: Burst %d\n", procs[i].pid, procs[i].burst_left);
        }

        int total_time = fcfs_run(procs, plen);
        (void)total_time; // total_time not printed but might be useful/debug

        double sum_wait = 0.0;
        for (int i = 0; i < plen; i++) {
            sum_wait += procs[i].wait;
        }
        double avg_wait = (plen > 0) ? (sum_wait / plen) : 0.0;

        printf("Average wait time: %.2f\n", avg_wait);

        free(procs);
        return 0;

    } else if (strcmp(argv[1], "rr") == 0) {
        // Need at least quantum + one burst.
        if (argc < 4) {
            print_missing_args_error();
            return 1;
        }

        int quantum = atoi(argv[2]);
        int plen = argc - 3;

        int* bursts = malloc(sizeof(int) * plen);
        if (bursts == NULL) {
            fprintf(stderr, "ERROR: Memory allocation failed\n");
            return 1;
        }

        for (int i = 0; i < plen; i++) {
            bursts[i] = atoi(argv[i + 3]);
        }

        procs = init_procs(bursts, plen);
        free(bursts);

        if (procs == NULL) {
            fprintf(stderr, "ERROR: Failed to initialize processes\n");
            return 1;
        }

        printf("Using RR(%d).\n\n", quantum);

        for (int i = 0; i < plen; i++) {
            printf("Accepted P%d: Burst %d\n", procs[i].pid, procs[i].burst_left);
        }

        int total_time = rr_run(procs, plen, quantum);
        (void)total_time;

        double sum_wait = 0.0;
        for (int i = 0; i < plen; i++) {
            sum_wait += procs[i].wait;
        }
        double avg_wait = (plen > 0) ? (sum_wait / plen) : 0.0;

        printf("Average wait time: %.2f\n", avg_wait);

        free(procs);
        return 0;

    } else {
        // Unknown algorithm – treat as incorrect usage.
        print_missing_args_error();
        return 1;
    }
}
