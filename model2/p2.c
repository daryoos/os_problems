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
    sem_t logSem;
    char filePath[512];
} thStruct;

int totalLines = 0;

void *threadFunction(void *arg)
{
    thStruct *s = (thStruct *)arg;

    int fd = open(s->filePath, O_RDONLY);

    int len = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char c;
    for (int i = 0; i < len; i++)
    {
        read(fd, &c, 1);
        if (c == '\n')
        {
            sem_wait(&(s->logSem));
            totalLines++;
            sem_post(&(s->logSem));
        }
    }

    close(fd);
}

int main(int argc, char **argv)
{
    char path[512];
    strcpy(path, argv[1]);

    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("dir not found");
        return -1;
    }

    struct dirent *entry;
    char filePath[512];
    pthread_t tid[100];
    thStruct *param[100];
    int nrTh = 0;
    sem_t logSem;
    sem_init(&logSem, 0, 1);
    while ((entry = readdir(dir)) != NULL)
    {
        strcpy(filePath, path);
        strcpy(filePath, "/");
        strcpy(filePath, entry->d_name);

        param[nrTh]->logSem = logSem;
        strcpy(param[nrTh]->filePath, filePath);
        pthread_create(&tid[nrTh], NULL, threadFunction, param[nrTh]);
        nrTh++;
    }

    for (int i = 0; i < nrTh; i++)
    {
        pthread_join(tid[i], NULL);
    }
    printf("%d", totalLines);

    sem_destroy(&logSem);
    closedir(dir);
    return 0;
}