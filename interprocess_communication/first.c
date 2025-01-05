#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SIZE 30

int main() {
  int fd = open("/dev/shm/shared_file", O_RDWR | O_CREAT, 0660);
  ftruncate(fd, 0);
  ftruncate(fd, SIZE);
  void *shared_memory =
      mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
  unsigned int *buf = (unsigned int *)shared_memory;
  unsigned int value = 0;
  for (int j = 0; j < 200000; j++) {
    for (unsigned int i = 0; i < SIZE / sizeof(unsigned int); i++) {
      buf[i] = value++;
      printf("%d\n", buf[i]);
      // fflush(stdout);
      sleep(1);
    }
  }
  int a;
  return EXIT_SUCCESS;
}
