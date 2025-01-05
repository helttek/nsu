#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
int main()
{
    DIR *dirStream = opendir(".");
    struct dirent *file = readdir(dirStream);
    struct stat fileStat;
    stat(file->d_name, &fileStat);
    while (file != NULL)
    {
        if (S_ISREG(fileStat.st_mode))
        {
            printf("%s is regular\n", file->d_name);
        }
        file = readdir(dirStream);
        stat(file->d_name, &fileStat);
        // printf("%s\n", file->d_name);
    }
    stat("Makefile", &fileStat);
    if (S_ISREG(fileStat.st_mode))
    {
        printf("all good\n");
    }
    return 0;
}