#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <pthread.h>

typedef struct {
    void (*run)(void *);

    void *arg;
} t_task;

typedef struct {
    t_task *tasks;
    size_t size;
    size_t capacity;
    int first;
    int last;
    int stopped;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    pthread_t *threads;
    size_t num_threads;
} t_thread_pool;

t_thread_pool *thread_pool_create(size_t num_threads, size_t max_num_tasks);

int thread_pool_submit_task(t_thread_pool *pool, void (*run)(void *),
                            void *arg);

void thread_pool_destroy(t_thread_pool *pool);

#endif // THREAD_POOL_H
