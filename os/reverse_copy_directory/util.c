#include "util.h"

void swap(char *a, char *b)
{
    char tmp;
    tmp = *b;
    *b = *a;
    *a = tmp;
}

int max(const int a, const int b)
{
    return (a > b) ? a : b;
}

int min(const int a, const int b)
{
    return (a < b) ? a : b;
}

void AllocFileMetaData(struct FileMetaData *fmd)
{
    fmd->name = malloc(MAX_FILE_NAME_SIZE);
    fmd->path = malloc(MAX_PATH_SIZE);
}

void FreeFileMetaData(struct FileMetaData *fmd)
{
    free(fmd->name);
    free(fmd->path);
}

char *ReverseString(char *str, const int N)
{
    for (int i = 0; i < max(N / 2, 1); i++)
    {
        swap(str + i, str + N - i - 1);
    }
    str[N] = '\0';
    return str;
}

char *ReverseBuffer(char *buf, const int N)
{
    for (int i = 0; i < N / 2; i++)
    {
        swap(buf + i, buf + N - i - 1);
    }
    return buf;
}

int GetDirName(const char *dirPath, const int dirPathSize, char *res)
{
    /*skip the case when '/' is the last character in the path (which is a legit
     * path)*/
    int i = dirPathSize - 1;
    if (dirPath[dirPathSize - 1] == '/')
    {
        --i;
    }
    while (dirPath[i] != '/' && i >= 0)
    {
        --i;
    }
    for (int j = 0; j < dirPathSize - i - 1; j++)
    {
        res[j] = dirPath[i + j + 1]; /*Directory name could be empty. For example:
                                        "/home/anton/nsu//"  */
    }
    if (dirPath[dirPathSize - 1] == '/')
    {
        res[dirPathSize - i - 2] = '\0';
    }
    else
    {
        res[dirPathSize - i - 1] = '\0';
    }
    return 1;
}

char *CreateReverseDirectory(struct FileMetaData *srcDir, struct FileMetaData *destDir)
{
    if (access(srcDir->path, F_OK) == -1)
    {
        fprintf(stderr, "Error: no such directory\n");
        return NULL;
    }
    if (!GetDirName(srcDir->path, srcDir->pathSize, destDir->name))
    {
        return NULL;
    }
    destDir->nameSize = strlen(destDir->name);
    ReverseString(destDir->name, destDir->nameSize);
    if (chdir(srcDir->path) == -1)
    {
        fprintf(stderr, "Error returned by chdir (%s). Terminating the program.\n", strerror(errno));
        return NULL;
    }
    if (chdir("..") == -1)
    {
        fprintf(stderr, "Error returned by chdir (%s). Terminating the program.\n", strerror(errno));
        return NULL;
    }
    if (getcwd(destDir->path, MAX_PATH_SIZE) == NULL)
    {
        fprintf(stderr, "'getcwd' returned error: %s\n", strerror(errno));
        return NULL;
    }
    strcat(destDir->path, "/");
    strcat(destDir->path, destDir->name);
    destDir->pathSize = strlen(destDir->path);
    if (access(destDir->path, F_OK) == -1)
    {
        mkdir(destDir->name, 0700);
    }
    return destDir->path;
}

int FileExists(const char *fileName)
{
    struct stat fileStat;
    if (access(fileName, F_OK) == 0)
    {
        if (stat(fileName, &fileStat) == -1)
        {
            fprintf(stderr, "Error: %s - %s\n", fileName, strerror(errno));
            return 1;
        }
        if (fileStat.st_size > 0)
        {
            fprintf(stderr, "Error: file '%s' already exists and is not empty.\n", fileName);
            return 1;
        }
    }
    return 0;
}

int CopyFile(const char *srcDirPath, const char *destDirName, char *srcFileName, char *buf, const int maxBufSize)
{
    FILE *srcFile = fopen(srcFileName, "rb");
    if (srcFile == NULL)
    {
        fprintf(stderr, "Couldn't open file %s.\n", srcFileName);
        fclose(srcFile);
        return 0;
    }
    struct stat fileStat;
    if (stat(srcFileName, &fileStat) == -1)
    {
        fprintf(stderr, "Can not copy the permissions of a file %s. Proceeding to copy the file without copying the permissions.\n", srcFileName);
    }
    buf[0] = '.', buf[1] = '.', buf[2] = '/', buf[3] = '\0';
    strcat(buf, destDirName);
    if (chdir(buf) == -1)
    {
        fprintf(stderr, "Error: couldn't enter directory %s\n", buf);
        fclose(srcFile);
        return 0;
    }
    strcpy(buf, srcFileName);
    ReverseString(buf, strlen(srcFileName));
    if (FileExists(buf))
    {
        if (chdir(srcDirPath) == -1)
        {
            fprintf(stderr, "Error: couldn't enter directory %s\n", buf);
            fclose(srcFile);
            return 0;
        }
        fclose(srcFile);
        return 0;
    }
    FILE *destFile = fopen(buf, "wb");
    if (destFile == NULL)
    {
        if (chdir(srcDirPath) == -1)
        {
            fprintf(stderr, "Error: couldn't enter directory %s\n", buf);
            fclose(srcFile);
            return 0;
        }
        fprintf(stderr, "Couldn't open file %s.\n", srcFileName);
        fclose(srcFile);
        return 0;
    }
    if (chmod(buf, fileStat.st_mode) == -1)
    {
        fprintf(stderr, "Can not copy the permissions of a file, error code (%s). Proceeding to copy the file without copying the permissions.\n", strerror(errno));
    }
    size_t bufSize = 0, i = 0, accum = 0;
    if (maxBufSize > fileStat.st_size)
    {
        if (fseek(srcFile, 0, SEEK_SET))
        {
            if (chdir(srcDirPath) == -1)
            {
                fprintf(stderr, "Error: couldn't enter directory %s\n", buf);
                fclose(srcFile);
                return 0;
            }
            fprintf(stderr, "Couldn't copy file\n");
            fclose(srcFile);
            fclose(destFile);
            return 0;
        }
        bufSize = fread(buf, 1, maxBufSize, srcFile);
        // printf("\n%ld %ld\n", bufSize, fileStat.st_size);
        ReverseBuffer(buf, bufSize);
        fwrite(buf, 1, bufSize, destFile);
    }
    else
    {
        while ((long int)(i * maxBufSize) < fileStat.st_size)
        {
            if (fseek(srcFile, -((i + 1) * maxBufSize), SEEK_END) == -1)
            {
                fseek(srcFile, 0, SEEK_SET);
            }
            bufSize = fread(buf, 1, min(maxBufSize, fileStat.st_size - i * maxBufSize), srcFile);
            accum += bufSize;
            // printf("%ld %ld\n", bufSize, fileStat.st_size);
            // for (size_t k = 0; k < bufSize; k++)
            // {
            //     printf("%c", buf[k]);
            // }
            // printf("\n-------------------------------------------------\n");
            ReverseBuffer(buf, bufSize);
            fwrite(buf, 1, bufSize, destFile);
            while ((int)accum != min(maxBufSize, fileStat.st_size - i * maxBufSize))
            {
                bufSize = fread(buf, 1, (min(maxBufSize, fileStat.st_size - i * maxBufSize) - accum), srcFile);
                ReverseBuffer(buf, bufSize);
                fwrite(buf, 1, bufSize, destFile);
                accum += bufSize;
            }
            ++i;
            accum = 0;
        }
    }
    if (chdir(srcDirPath) == -1)
    {
        fprintf(stderr, "Error: couldn't enter directory %s\n", buf);
        fclose(destFile);
        fclose(srcFile);
        return 0;
    }
    fclose(destFile);
    fclose(srcFile);
    return 1;
}

int ReverseCopyFiles(const char *srcDirPath, const char *destDirName)
{
    if (chdir(srcDirPath) == -1)
    {
        fprintf(stderr, "Error returned by chdir (%s). Terminating the program.\n", strerror(errno));
        return 0;
    }
    DIR *dirStream = opendir(srcDirPath);
    if (dirStream == NULL)
    {
        fprintf(stderr, "Error: opendir returned error (%s).\n", strerror(errno));
        return 0;
    }
    // const int destDirPathSize = strlen(destDirPath);
    char *buf = malloc(MAX_PATH_SIZE);
    int i = 0;
    struct dirent *file;
    struct stat fileStat;
    while ((file = readdir(dirStream)) != NULL)
    {
        errno = 0;
        // printf("%s\n", file->d_name);
        if (stat(file->d_name, &fileStat) == -1)
        {
            fprintf(stderr, "Error occured during stat function call (%s). Skipping file '%s'.\n", strerror(errno), file->d_name);
            continue;
        }
        if (S_ISREG(fileStat.st_mode))
        {
            // printf("%s\n", file->d_name);
            if (!CopyFile(srcDirPath, destDirName, file->d_name, buf, MAX_PATH_SIZE))
            {
                fprintf(stderr, "Failed to copy '%s'. Skipping this file.\n", file->d_name);
                continue;
            }
            ++i;
        }
    }
    printf("Successfully copied %d files.\n", i);
    free(buf);
    return 1;
}
