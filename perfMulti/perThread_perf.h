#ifndef PER_THREAD_H
#define PER_THREAD_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* See feature_test_macros(7) */
#endif
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <asm/unistd.h>
#include <error.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/hw_breakpoint.h>
#include <linux/perf_event.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/*
#define EVENT1  0x3c    //UNHALTED_CORE_CYCLE
#define EVENT2 0xc0     //INSTRUCTION_RETIRED
#define EVENT3 0x82d0   //STORES
#define EVENT4 0x20d3  //REMOTE_FWD
#define EVENT5 0x10d3  //REMOTE_HITM
#define EVENT6 0x412e  //LLC MISSES
#define EVENT7 0x13c  //UNHALTED_REFERENCE_CYCLES
#define EVENT8 0x4f2e  //LLC_REFERENCES
*/

#define TIME_IN_MILLI_SEC                                                                          \
    250                // total measurement time per iteration is 4 times
                       // TIME_IN_MILLI_SEC(example 250 millisecond *4 = 1 second)
#define NUM_THREAD 200 // Total Threads being monitored
#define ITER 1         // Number of iterations
#define EVENTS 8       // static as per code , don't change

// Data structure to collect information per TID performance counters
struct perfThread {
    pid_t tid[NUM_THREAD];
    int index_tid;
    uint64_t event[NUM_THREAD][EVENTS];
};
struct perfThread THREADS;

// Data structure for reading counters
struct read_format {
    uint64_t nr;
    struct {
        uint64_t value;
        uint64_t id;

    } values[];
};

struct perf_stat {
    // values of event count
    uint64_t val[EVENTS];

    // buffer for reading couple events together
    char buf[EVENTS / 2][4096];
    // read format pointers for each buffer (group of events), each group contains
    // two events
    struct read_format *rf[EVENTS / 2];

    struct perf_event_attr pea; // perf_event attribute

    // file descriptor returned for each perf_event_open call
    int fd[EVENTS];

    // id of each event
    uint64_t id[EVENTS];
};

#if defined(__cplusplus)
extern "C" {
#endif

extern uint64_t EVENT[];

extern struct perf_stat *threads;

void setPerfAttr(struct perf_event_attr pea, uint64_t EVENT, int group_fd, int *fd, uint64_t *id,
                 int cpu, pid_t tid);

void start_event(int fd);

void stop_read_counters(struct read_format *rf, int fd, char *buf, int size, uint64_t *val1,
                        uint64_t *val2, uint64_t id1, uint64_t id2);

void initialize_events(void);

void count_event_perfMultiplex(pid_t tid[], int index_tid);

void displayTIDEvents(pid_t tid[], int index_tid);

int searchTID(int tid);

#if defined(__cplusplus)
};
#endif

#endif  /* PER_THREAD_H */
