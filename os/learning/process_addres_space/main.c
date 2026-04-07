#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 100

extern char **environ;

/*
1) where is uninitialized global var stored
2) why is it unsafe to use an adress of a local variable after scope
3) does free overwrites the allocated memory
4) why is garbage displayed after printing freed buffer?
5) where are environmental variables stored?



1) bss section because it's global and global vars are automatically initialized to zero
2) потому что можно эта переменная уже может быть использована
3) нет
4) потому что эта память может быть перезаписана
5) 
*/


int globVarUnInit;
int globVarInit = 222;
const int globConst = 333;

void PrintVarAdr()
{
    int localVar;
    static int staticLocalVar;
    const int localConst = 111;
    printf("local var: %p\n", &localVar);
    printf("static local var: %p\n", &staticLocalVar);
    printf("local const: %p\n", &localConst);
    printf("global var uninitialized: %p\n", &globVarUnInit);
    printf("global var initialized: %p\n", &globVarInit);
    printf("global const: %p\n", &globConst);
}

int *PrintInitVar()
{
    int var = 0;
    printf("created and initialized local variable: %p\n", &var);
    return &var;
}

void BufferWork()
{
    char str[] = "hello, world";
    char *buf1 = malloc(BUF_SIZE);
    strcpy(buf1, str);
    printf("buf1 (before free): %s\n", buf1);
    free(buf1);
    printf("buf1 (after free): %s\n", buf1);
    char *buf2 = malloc(BUF_SIZE);
    strcpy(buf1, str);
    strcpy(buf2, str);
    printf("buf2: %s\n", buf2);
    char *tmp = buf2 + BUF_SIZE / 2;
    free(tmp);
    printf("half of buf2: %s\n", buf2);
    sleep(10);
}

void EnvVarWork()
{
    setenv("NEW_ENV_VAR", "some var value", 1);
    char *NEW_ENV_VAR = getenv("NEW_ENV_VAR");
    printf("envinment variable value: %s\n", NEW_ENV_VAR);
    setenv("NEW_ENV_VAR", "new var value", 1);
    NEW_ENV_VAR = getenv("NEW_ENV_VAR");
    printf("new envinment variable value: %s\n", NEW_ENV_VAR);
}

int main(int argc, char const *argv[])
{
    printf("pid: %d\n", getpid());
    printf("----------------------------------------------------------------\n");
    PrintVarAdr();
    printf("----------------------------------------------------------------\n");
    int *p = PrintInitVar();
    printf("returned pointer: %p\n", p);
    printf("----------------------------------------------------------------\n");
    // BufferWork();
    printf("----------------------------------------------------------------\n");
    // exported env var is called NEW_ENV_VAR
    EnvVarWork();
    sleep(10);
    return 0;
}
