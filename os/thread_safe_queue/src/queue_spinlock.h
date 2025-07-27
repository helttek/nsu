#ifndef QUEUE_SPINLOCK_H
#define QUEUE_SPINLOCK_H

#ifndef __FITOS_QUEUE_H__
#define __FITOS_QUEUE_H__

#define _GNU_SOURCE
#include <sys/types.h>

typedef struct {
    int lock;
} spinlock_t;

typedef struct _QueueNode {
    int val;
    struct _QueueNode *next;
} qnode_t;

typedef struct _Queue {
    qnode_t *first;
    qnode_t *last;

    pthread_t qmonitor_tid;

    int count;
    int max_count;

    // queue statistics
    long add_attempts;
    long get_attempts;
    long add_count;
    long get_count;

    spinlock_t *lock;
} queue_t;

queue_t* queue_init(int max_count);
void queue_destroy(queue_t *q);
int queue_add(queue_t *q, int val);
int queue_get(queue_t *q, int *val);
void queue_print_stats(const queue_t *q);

#endif          // __FITOS_QUEUE_H__

#endif //QUEUE_SPINLOCK_H
