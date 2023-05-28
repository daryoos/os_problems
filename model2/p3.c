#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <dirent.h>

typedef struct
{
    char fileName[512];
} thStruct;

void *thFunction(void *arg)
{
    thStruct *s = (thStruct *)arg;
    int fd = open(s->fileName, O_RDWR);
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char *data = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    for (int i = 0; i < size; i++)
    {
        if (data >= '0' && data <= '9')
        {
            data = 'X';
        }
    }
    munmap(data, size);
    close(fd);
    return NULL;
}

int main(int argc, char **argv)
{
    int n = argc - 1;
    char fileName[n][512];
    for (int i = 0; i < n; i++)
    {
        strcpy(fileName[i], argv[i + 1]);
    }

    pthread_t tid[n];
    thStruct *param[n];
    for (int i = 0; i < n; i++)
    {
        strcpy(param[i]->fileName, fileName[i]);
        pthread_create(&tid[i], NULL, thFunction, param[i]);
    }
    for (int i = 0; i < n; i++)
    {
        pthread_join(&tid[i], NULL);
    }
    return 0;
}