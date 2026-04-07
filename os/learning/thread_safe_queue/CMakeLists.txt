cmake_minimum_required(VERSION 3.30)
project(queue C)

set(CMAKE_C_STANDARD 23)

add_executable(spinlock0 src/queue-example.c src/queue_spinlock.c src/queue_spinlock.h)
add_executable(spinlock1 src/queue-threads.c src/queue_spinlock.c src/queue_spinlock.h)
add_executable(mutex0 src/queue-example.c src/queue_mutex.c src/queue_mutex.h)
add_executable(mutex1 src/queue-threads.c src/queue_mutex.c src/queue_mutex.h)
add_executable(cond_var0 src/queue_cond_var.c src/queue_cond_var.h src/queue-example.c)
add_executable(cond_var1 src/queue_cond_var.c src/queue_cond_var.h src/queue-threads.c)
add_executable(semaphore0 src/queue_semaphore.c src/queue_semaphore.h src/queue-example.c)
add_executable(semaphore1 src/queue_semaphore.c src/queue_semaphore.h src/queue-threads.c)
