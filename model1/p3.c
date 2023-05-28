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
    int n;
    char **string;
    char *entryName;
    char *filePath;
} th_struct;

void *thread_function(void *arg)
{
    th_struct *s = (th_struct *)arg;
    int fd;
    int fileLen;
    fd = open(s->filePath, O_RDONLY);
    if (fd == -1)
    {
        perror("fd not found");
        return -1;
    }

    fileLen = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char fileValue[fileLen + 1];
    read(fd, fileValue, fileLen);
    fileValue[fileLen] = '\0';

    char result[512];
    for (int i = 0; i < s->n; i++)
    {
        if (strstr(fileValue, s->string[i]) != NULL)
        {
            strcpy(result, s->entryName);
            strcat(result, "   ");
            strcat(result, s->string[i]);
        }
    }
    close(fd);

    return (void *)result;
}

int main(int argc, char **argv)
{
    char path[512];
    strcpy(path, argv[1]);
    int n = argc - 2;
    char string[n][512];
    for (int i = 0; i < n; i++)
    {
        strcpy(string[i], argv[i + 2]);
    }

    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("dir not found");
        return -1;
    }

    struct dirent *entry;
    char filePath[512];
    pthread_t tid[512];
    int tCt = 0;
    th_struct param;
    strcpy(param.entryName, entry->d_name);
    strcpy(param.n, n);
    for (int i = 0; i < n; i++)
    {
        strcpy(param.string[i], string[i]);
    }
    while ((entry = readdir(dir)) != NULL)
    {
        strcpy(filePath, path);
        strcat(filePath, "/");
        strcat(filePath, entry->d_name);

        strcpy(param.filePath, filePath);
        pthread_create(&tid[tCt++], NULL, thread_function, &param);
    }

    void *result;
    for (int i = 0; i < tCt; i++)
    {
        thread_join(tid[i], &result);
        printf("%s\n", (char *)result);
    }

    closedir(dir);
    return 0;
}