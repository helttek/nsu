#include <dirent.h>
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#define MAX_FILE_NAME_SIZE 255
#define MAX_PATH_SIZE 4096000

struct FileMetaData
{
    char *path;
    int pathSize;
    char *name;
    int nameSize;
};

char *CreateReverseDirectory(struct FileMetaData *srcDir, struct FileMetaData *destDir);

int ReverseCopyFiles(const char *srcDirPath, const char *destDirName);

void AllocFileMetaData(struct FileMetaData *fmd);

void FreeFileMetaData(struct FileMetaData *fmd);