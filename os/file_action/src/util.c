#include "util.h"

#include <stdio.h>
#include <string.h>

int argcmp(const char *s1, const char *s2)
{
    if (strlen(s1) < strlen(s2))
    {
        return 1;
    }
    int i = strlen(s1) - 1;
    int j = strlen(s2) - 1;
    for (; i >= 0 && j >= 0; --i, --j)
    {
        // printf("s1[%d]: %c\n", i, s1[i]);
        // printf("s2[%d]: %c\n", j, s2[j]);
        if (s1[i] != s2[j])
        {
            // printf("Warning: failed to identify operation %s with %s\n", s1, s2);
            return 1;
        }
    }
    if (s1[i] != '/')
    {
        return 1;
    }
    return 0;
}

int min(const int a, const int b) { return (a < b) ? a : b; }

int get_parent_path(const char *path, char *res)
{
    int i = strlen(path) - 1;
    for (; path[i] == '/'; --i)
    {
        if (i < 0)
        {
            return 0;
        }
    }
    strncpy(res, path, i + 1);
    return 1;
}