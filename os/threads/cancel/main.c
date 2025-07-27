#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define _GNU_SOURCE
#define STR_SIZE 25

void *mythread0(void *arg) {
  int i = 0;
  char str[STR_SIZE];
  strcpy(str, "str0");
  while (1) {
    printf("%s\n", str);
    ++i;
    str[3] = i + '0';
  }
  return NULL;
}

// size_t i = 0;

void *mythread1(void *arg) {
  size_t i = 0;
  // pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  while (1) {
    ++i;
    // test_cancel();
    if (i == 1000) {
      sleep(1);
    }
  }
  return NULL;
}

void handler(void *data) { free(data); }

// void cancel(tid) {
//   cancel_thread = true;
// }
// 
//  bool cancel_thread;
//
// bool test_cancel(tid) {
//   if (cancel_thread) {
//     kill_thread();
//   }
// }

void *mythread2(void *arg) {
  int i = 0;
  char *hw = malloc(20);
  pthread_cleanup_push(handler, hw);
  strcpy(hw, "hello, world");
  while (1) {
    printf("%d. %s\n", ++i, hw);
  }
  pthread_cleanup_pop(handler);
  return NULL;
}
// while(1);
int main() {
  pthread_t tid;
  // pthread_create(&tid, NULL, mythread0, NULL);
  // sleep(1);
  // pthread_cancel(tid);
  // pthread_join(tid, NULL);

  // pthread_create(&tid, NULL, mythread1, NULL);
  // sleep(1);
  // pthread_cancel(tid);
  // pthread_join(tid, NULL);

  pthread_create(&tid, NULL, mythread2, NULL);
  pthread_cancel(tid);
  pthread_join(tid, NULL);

  return 0;
}
