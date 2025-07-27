#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define _GNU_SOURCE
#define STR_SIZE 25

struct args_struct {
  int d;
  char *p;
};

struct heap_args_struct {
  int d;
  char p[STR_SIZE];
};

void *mythread1(void *args) {
  printf("passed arguments: [%d, %s]\n", ((struct args_struct *)args)->d,
         ((struct args_struct *)args)->p);
  return NULL;
}

void *mythread2(void *args) {
  printf("passed arguments: [%d, %s]\n", ((struct heap_args_struct *)args)->d,
         ((struct heap_args_struct *)args)->p);
  /* free(args); */
  return NULL;
}

int main() {
  // 1
  struct args_struct args;
  args.d = 1;
  char *str = "some string";
  args.p = str;
  pthread_t tid;
  pthread_create(&tid, NULL, mythread1, &args);
  pthread_join(tid, NULL);

  // 2
  struct heap_args_struct *heapArgs =
      malloc(sizeof(struct heap_args_struct) * 1);
  if (!heapArgs) {
    fprintf(stderr, "malloc failed in main\n");
    return EXIT_FAILURE;
  }
  heapArgs->d = 2;
  //----------------------------------------
  struct heap_args_struct has;
  has.d = 3;
  strcpy(has.p, "random string");
  //----------------------------------------
  strcpy(heapArgs->p, "another string");
  pthread_attr_t attr;
  if (pthread_attr_init(&attr)) {
    fprintf(stderr, "thread_attr_init failed in main\n");
    return EXIT_FAILURE;
  }
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
    fprintf(stderr, "pthread_attr_setdetachstate failed in main\n");
    return EXIT_FAILURE;
  }
  if (pthread_create(&tid, &attr, mythread2, &has)) {
    fprintf(stdout, "pthread_create failed in main\n");
    return EXIT_FAILURE;
  }
  if (pthread_attr_destroy(&attr) != 0) {
    perror("pthread_attr_destroy");
    exit(EXIT_FAILURE);
  }
  pthread_exit(0);
  return EXIT_SUCCESS;
}
