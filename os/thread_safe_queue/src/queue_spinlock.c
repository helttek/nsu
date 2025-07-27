#include "queue_spinlock.h"
#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*
 *
 */

void spinlock_init(spinlock_t *spinlock) {
    spinlock->lock = 1;
}

void spinlock_lock(spinlock_t *s) {
    while (1) {
        int one = 1;
        if (atomic_compare_exchange_strong(&s->lock, &one, 0)) {
            break;
        }
    }
}

void spinlock_unlock(spinlock_t *s) {
    int zero = 0;
    atomic_compare_exchange_strong(&s->lock, &zero, 1);
}

void *qmonitor(void *arg) {
    queue_t *q = arg;

    printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        queue_print_stats(q);
        sleep(1);
    }
}

queue_t *queue_init(int max_count) {
    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    q->lock = malloc(sizeof(spinlock_t));
    if (!q->lock) {
        printf("Cannot allocate memory for spinlock\n");
        abort();
    }

    spinlock_init(q->lock);
    q->first     = NULL;
    q->last      = NULL;
    q->max_count = max_count;
    q->count     = 0;

    q->add_attempts = q->get_attempts = 0;
    q->add_count    = q->get_count    = 0;

    const int err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
    if (err) {
        printf("queue_init: pthread_create() failed: %s\n", strerror(err));
        abort();
    }
    return q;
}

void queue_destroy(queue_t *q) {
    if (!q) {
        return;
    }

    spinlock_lock(q->lock);

    pthread_cancel(q->qmonitor_tid);
    pthread_join(q->qmonitor_tid, NULL);

    while (q->first) {
        qnode_t *tmp = q->first;
        q->first     = q->first->next;
        free(tmp);
    }
    free(q->lock);
    free(q);
}

int queue_add(queue_t *q, int val) {
    spinlock_lock(q->lock);

    q->add_attempts++;

    assert(q->count <= q->max_count);
    if (q->count == q->max_count) {
        // printf("limit reached\n");
        spinlock_unlock(q->lock);
        return 0;
    }

    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new) {
        printf("Cannot allocate memory for new node\n");
        abort();
    }
    new->val  = val;
    new->next = NULL;

    if (!q->first) {
        q->first = q->last = new;
    } else {
        q->last->next = new;
        q->last       = q->last->next;
    }
    q->count++;
    q->add_count++;

    spinlock_unlock(q->lock);
    return 1;
}

int queue_get(queue_t *q, int *val) {
    spinlock_lock(q->lock);

    q->get_attempts++;

    assert(q->count >= 0);
    if (q->count == 0) {
        spinlock_unlock(q->lock);
        return 0;
    }

    qnode_t *tmp = q->first;

    *val     = tmp->val;
    q->first = q->first->next;

    free(tmp);

    q->count--;
    q->get_count++;

    spinlock_unlock(q->lock);
    return 1;
}

void queue_print_stats(const queue_t *q) {
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld "
           "%ld %ld)\n",
           q->count, q->add_attempts, q->get_attempts,
           q->add_attempts - q->get_attempts, q->add_count, q->get_count,
           q->add_count - q->get_count);
}
