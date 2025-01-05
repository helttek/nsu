#include "thread_pool.h"

#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include "logger.h"

static void *do_work(void *arg) {
    t_thread_pool *pool = arg;
    while (1) {
        pthread_mutex_lock(&pool->mutex);

        while (pool->size == 0 && !pool->stopped) {
            pthread_cond_wait(&pool->not_empty, &pool->mutex);
        }

        if (pool->stopped) {
            //gets out of loop when the pool needs to be destroyed
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(NULL);
        }

        const t_task task = pool->tasks[pool->first];
        pool->first = (pool->first + 1) % pool->capacity;
        pool->size--;

        pthread_cond_signal(&pool->not_full);
        pthread_mutex_unlock(&pool->mutex);

        if (task.run != NULL) {
            if (task.arg != NULL) {
                task.run(task.arg);
            }
        }
    }
}

t_thread_pool *thread_pool_create(const size_t num_threads,
                                  const size_t max_num_tasks) {
    t_thread_pool *p_pool = malloc(sizeof(t_thread_pool));
    if (!p_pool) {
        log_debug("Failed to allocate thread pool", strerror(errno));
        return NULL;
    }

    p_pool->tasks = calloc(max_num_tasks, sizeof(t_task));
    if (!p_pool->tasks) {
        log_debug("Failed to allocate thread pool tasks", strerror(errno));
        free(p_pool);
        return NULL;
    }

    p_pool->threads = malloc(sizeof(pthread_t) * num_threads);
    if (!p_pool->threads) {
        log_debug("Failed to allocate thread pool threads array", strerror(errno));
        free(p_pool->tasks);
        free(p_pool);
        return NULL;
    }

    p_pool->capacity = max_num_tasks;
    p_pool->num_threads = num_threads;
    p_pool->size = 0;
    p_pool->first = 0;
    p_pool->last = 0;
    p_pool->stopped = 0;
    if (pthread_mutex_init(&p_pool->mutex, NULL)) {
        log_debug("Failed to initialize mutex",NULL);
        return NULL;
    }
    if (pthread_cond_init(&p_pool->not_empty, NULL)) {
        log_debug("Failed to initialize condition variable",NULL);
        return NULL;
    }
    if (pthread_cond_init(&p_pool->not_full, NULL)) {
        log_debug("Failed to initialize condition variable",NULL);
        return NULL;
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&p_pool->threads[i], NULL, do_work, p_pool)) {
            log_debug("Failed to create thread pool thread", NULL);
            free(p_pool->tasks);
            free(p_pool->threads);
            free(p_pool);
            return NULL;
        }
    }

    log_info("Thread pool successfully created", NULL);

    return p_pool;
}

int thread_pool_submit_task(t_thread_pool *pool, void (*run)(void *),
                            void *arg) {
    if (pool->stopped) {
        log_debug("Thread pool stopped - can't submit a new task", NULL);
        return -1;
    }

    pthread_mutex_lock(&pool->mutex);

    while (pool->size == pool->capacity &&
           !pool->stopped) {
        pthread_cond_wait(&pool->not_full, &pool->mutex);
    }

    if (pool->stopped) {
        pthread_mutex_unlock(&pool->mutex);
        return -1;
    }

    pool->tasks[pool->last].run = run;
    pool->tasks[pool->last].arg = arg;
    pool->last = (pool->last + 1) % pool->capacity;
    pool->size++;

    pthread_cond_signal(&pool->not_empty);
    pthread_mutex_unlock(&pool->mutex);

    log_debug("Task added to thread pool", NULL);

    return 0;
}

void thread_pool_destroy(t_thread_pool *pool) {
    pool->stopped = 1;

    // tell waiting threads to stop waiting
    pthread_cond_broadcast(&pool->not_empty);
    pthread_cond_broadcast(&pool->not_full);

    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    free(pool->tasks);
    free(pool->threads);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->not_empty);
    pthread_cond_destroy(&pool->not_full);
    free(pool);

    log_info("Thread pool successfully destroyed", NULL);
}
