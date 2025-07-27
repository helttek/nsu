#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


// pid_t gettid(void) { return syscall(SYS_gettid); }

void sigint_handler(int sig) {
  printf("thread %d: got signal SIGINT (%d)\n", gettid(),
         sig); // not safe - don't use in prod
}

void *mythread0(void *arg) {
  sigset_t set;
  sigfillset(&set);
  pthread_sigmask(SIG_BLOCK, &set, NULL);
  printf("tid %d: blocking all signals\n", gettid());
  sleep(15000000);
  printf("time's up, exiting thread %d\n", gettid());
  return NULL;
}

void *mythread1(void *arg) {
  struct sigaction sa;
  sa.sa_handler = sigint_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction failed");
    pthread_exit(NULL);
  }
  printf("tid %d: awaiting SIGINT\n", gettid());
  sleep(10);
  printf("time's up exiting tid %d\n", gettid());
  return NULL;
}

void *mythread2(void *arg) {
  sigset_t set;
  int sig;
  sigemptyset(&set);
  sigaddset(&set, SIGQUIT);
  printf("thread %d: awaiting SIGQUIT\n", gettid());
  if (sigwait(&set, &sig) == 0) {
    printf("gettid() %d: SIGQUIT acquired (%d)\n", gettid(), sig);
  } else {
    perror("sigwait");
  }
  return NULL;
}

int main() {
  pthread_t ptid1, ptid2, ptid3;
  printf("pid: %d\n", getpid());

  // if (pthread_create(&ptid1, NULL, mythread0, NULL)) {
  //   fprintf(stderr, "Failed to create thread mythread0\n");
  //   return EXIT_FAILURE;
  // }
  // sleep(1);
  // printf("sending SIGINT\n");
  // if (pthread_kill(ptid1, SIGINT)) {
  //   perror("failed to produce SIGINT");
  //   return EXIT_FAILURE;
  // } else {
  //   printf("SIGINT sent successfully\n");
  // }

  // if (pthread_create(&ptid2, NULL, mythread1, NULL)) {
  //   fprintf(stderr, "Failed to create thread mythread0\n");
  //   return EXIT_FAILURE;
  // }

  if (pthread_create(&ptid3, NULL, mythread2, NULL)) {
    fprintf(stderr, "Failed to create thread mythread0\n");
    return EXIT_FAILURE;
  }
  sleep(1);
  // if (pthread_kill(ptid3, SIGQUIT)) {
  //   perror("failed to produce SIGQUIT");
  //   return EXIT_FAILURE;
  // }

  // pthread_join(ptid1, NULL);
  // pthread_join(ptid2, NULL);
  pthread_join(ptid3, NULL);

  pthread_exit(0);
  return 0;
}
