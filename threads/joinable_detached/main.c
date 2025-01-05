#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define _GNU_SOURCE

void *mythread1(void *args) { return (void *)42; }

void *mythread2(void *args) {
  char *hello = malloc(sizeof(char) * 15);
  char hell[15] = "hello, world";
  strcpy(hello, hell);
  return hello;
}

void *mythread3(void *args) {
  // e
  // if (pthread_detach(pthread_self())) {
  //   fprintf(stderr, "detach failed in mythread3\n");
  //   return 0;
  // }
  printf("ptid: %d ", gettid());
  /* sleep(60); */
  return 0;
}

int main() {
  pthread_t tid;
  int err;
  void **retval = malloc(sizeof(int));
  if (retval == NULL) {
    fprintf(stderr, "malloc failed in main\n");
    return EXIT_FAILURE;
  }

  // b
  pthread_create(&tid, NULL, mythread1, NULL);
  pthread_join(tid, retval);
  printf("retval: %d\n", *(int *)retval);

  // c
  pthread_create(&tid, NULL, mythread2, NULL);
  pthread_join(tid, retval);
  printf("retval: %s\n", (char *)(*retval));
  free(*retval);

  size_t sum = 0;

  pthread_attr_t attr;
  if (pthread_attr_init(&attr)) {
    fprintf(stderr, "thread_attr_init failed in main\n");
    return 0;
  }
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  while (1) {
    // d -- test with and without join
    int err = pthread_create(&tid, NULL, mythread3, NULL);
    if (err) {
      fprintf(stderr, "pthread_create failed in main\n");
      printf("%s\n", strerror(err));
      break;
    }
    // f -- disable join and pthread_create without attributes
    // if (pthread_create(&tid, &attr, mythread3, NULL)) {
    //   fprintf(stderr, "pthread_create failed in main\n");
    //   break;
    // }
    ++sum;
    printf("sum: %ld\n", sum);
    // if (pthread_join(tid, NULL)) {
    //   fprintf(stderr, "pthread_join failed in main\n");
    //   break;
    // }
  }

  if (pthread_attr_destroy(&attr) != 0) {
    perror("pthread_attr_destroy");
    exit(EXIT_FAILURE);
  }

  return 0;
}
