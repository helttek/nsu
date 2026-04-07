#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int globalVar = 222;

void PrintStuff(size_t addr, int value)
{
    printf("local var addr: %p\n", addr);
    printf("global var addr: %p\n", &globalVar);
    printf("local var value: %d\n", value);
    printf("global var value: %d\n", globalVar);
}

int main()
{
    int localVar = 111;
    PrintStuff((size_t)(&localVar), localVar); /*local var is on the stack, global in data section, */
    __pid_t pid = getpid();
    printf("pid: %d\n", pid);
    sleep(10);
    __pid_t newPid = fork();
    if (newPid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (newPid == 0)
    {
        printf("---------------------------------------\n");
        puts("in the child process: \n");
        printf("pid: %d\n", getpid());
        sleep(10);
        printf("ppid: %d\n", getppid());
        PrintStuff((size_t)(&localVar), localVar);
        ++localVar;
        ++globalVar;
        printf("local var value (after change): %d\n", localVar);
        printf("global var value (after change): %d\n", globalVar);
        printf("---------------------------------------\n");
        sleep(600);
        _exit(5);
    }
    else
    {
        printf("var values in parent process: %d %d\n", localVar, globalVar);
        sleep(10);
        int stat_lock;
        do
        {
            if (wait(&stat_lock) == -1)
            {
                fprintf(stderr, "Couldn't recover the killed process' exit signal.\n");
                return EXIT_FAILURE;
            }
            if (__WIFEXITED(stat_lock))
            {
                printf("exited, status = %d\n", __WEXITSTATUS(stat_lock));
            }
            else if (__WIFSIGNALED(stat_lock))
            {
                printf("killed by signal %d\n", __WTERMSIG(stat_lock));
            }
            else if (__WIFSTOPPED(stat_lock))
            {
                printf("stopped by signal %d\n", __WSTOPSIG(stat_lock));
            }
            else if (__WIFCONTINUED(stat_lock))
            {
                printf("continued\n");
            }
        } while (!WIFEXITED(stat_lock) && !WIFSIGNALED(stat_lock));
        sleep(600);
        exit(EXIT_SUCCESS);
    }
}