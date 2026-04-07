#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>

// #include "queue_spinlock.h"
// #include "queue_mutex.h"
// #include "queue_cond_var.h"
#include "queue_semaphore.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"
#define READER_CPU 1
#define WRITER_CPU 2
#define QUEUE_SIZE 1000000
#define TIME_TO_CANCEL 10
#define USE_SCHED_YIELD 0
#define EMULATE_SELECT 0

void set_cpu(const int n) {
    cpu_set_t cpuset;
    const pthread_t tid = pthread_self();

    CPU_ZERO(&cpuset);
    CPU_SET(n, &cpuset);

    const int err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
    if (err) {
        printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
        return;
    }

    printf("set_cpu: set cpu %d\n", n);
}

void *reader(void *arg) {
    int expected = 0;
    queue_t *q   = arg;
    printf("reader [%d %d %d]\n", getpid(), getppid(), gettid());

    set_cpu(READER_CPU);

    while (1) {
        pthread_testcancel();

        int val      = -1;
        const int ok = queue_get(q, &val);
        if (!ok) {
            continue;
        }
        if (expected != val) {
            printf(RED "ERROR: get value is %d but expected - %d" NOCOLOR "\n", val, expected);
        }
        expected = val + 1;
    }
}

void *writer(void *arg) {
    int i      = 0;
    queue_t *q = arg;
    printf("writer [%d %d %d]\n", getpid(), getppid(), gettid());

    set_cpu(WRITER_CPU);

    while (1) {
#if EMULATE_SELECT
        usleep(1);
#endif

        pthread_testcancel();

        const int ok = queue_add(q, i);
        if (!ok) {
            continue;
        }
        i++;
    }
}

int main() {
    pthread_t rtid;
    pthread_t wtid;
    queue_t *q;

    printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

    q = queue_init(QUEUE_SIZE);

    int err = pthread_create(&rtid, NULL, reader, q);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        queue_destroy(q);
        return EXIT_FAILURE;
    }

#if USE_SCHED_YIELD
    sched_yield();
#endif

    err = pthread_create(&wtid, NULL, writer, q);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        queue_destroy(q);
        return EXIT_FAILURE;
    }

#ifndef TIME_TO_CANCEL
    printf("no finish defined\n");
    pthread_exit(NULL);
#else
    sleep(TIME_TO_CANCEL);
    pthread_cancel(rtid);
    pthread_cancel(wtid);
    // printf("cancels sent\n");
    pthread_join(rtid, NULL);
    pthread_join(wtid, NULL);
    // printf("finish here\n");
    // queue_destroy(q);
#endif
    queue_destroy(q);
    return EXIT_SUCCESS;
}
