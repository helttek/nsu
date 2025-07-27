#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define SIZE 30

int main(int argc, char const *argv[])
{
    int fd = open("/dev/shm/shared_file", O_RDWR | O_CREAT, 0660);
    void *shared_memory = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    unsigned int *buf = (unsigned int *)shared_memory;
    unsigned int value = 0;
    for (int j = 0; j < 200000; j++)
    {
        for (unsigned int i = 0; i < SIZE / sizeof(unsigned int); i++)
        {
            if ((i > 0) && (buf[i] - 1 != buf[i - 1]))
            {
                fprintf(stderr, "not continuous series at %d\n", i);
                // printf("\n buf[%d]=%d buf[%d]=%d\n", i - 1, buf[i - 1], i, buf[i]);
            }
            // printf("%d\n", buf[i]);
            fflush(stdout);
            // sleep(1);
            // if (i == SIZE / sizeof(unsigned int) - 1)
            // {
            //     i = 0;
            // }
        }
        // printf("---------------------------------------\n");
    }

    return 0;
}
