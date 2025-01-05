#include <time.h>
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define THREAD_MAX_COUNT 2

int glob;

void *mythread(void *arg) {
  int local = -1;
  static char staticLocal;
  const char constLocal;
  printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(),
         gettid());
  //   printf("pthread id (pthread_self and gettid): [%lu, %d]\n",
  //   pthread_self(),
  //          gettid());
  printf("[local, static local, local const, glob] = [%p, %p, %p, "
         "%p]\n",
         &local, &staticLocal, &constLocal, &glob);
  printf("before: [loc, glob] = [%d, %d]\n", local, glob);
  local = gettid();
  glob = gettid();
  printf("after: [loc, glob] = [%d, %d]\n", local, glob);
  sleep(2000);
  return NULL;
}

int main() {
  pthread_t tid;
  int err[THREAD_MAX_COUNT];
  printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());
  for (int i = 0; i < THREAD_MAX_COUNT; ++i) {
    err[i] = pthread_create(&tid, NULL, mythread, NULL);
    if (err[i]) {
      printf("main: pthread_create() failed: %s\n", strerror(err[i]));
      return -1;
    }
  }
  sleep(2000);
  int ret;
  pthread_exit(&ret);
  return 0;
}
