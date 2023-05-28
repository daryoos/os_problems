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
    char *data;
    int start;
    int end;
} thStruct;

void *thFunction(void *arg)
{
    thStruct *s = (thStruct *)arg;
    char c;
    int nrLines = 0;
    for (int i = s->start; i < s->end; i++)
    {
        if (s->data[i] == '\n')
        {
            nrLines++;
        }
    }
    return (void *)nrLines;
}

int main(int argc, char **argv)
{
    char pipeWr[] = "RD";
    char pipeRd[] = "WR";

    if (mkfifo(pipeWr, 0600) != 0)
    {
        perror("pipeWr");
        return -1;
    }
    int wrfd = open(pipeWr, O_RDWR);
    if (wrfd == -1)
    {
        perror("wrfd");
        return -1;
    }

    int rdfd = open(pipeRd, O_RDWR);
    if (rdfd == -1)
    {
        perror("rdfd");
        return -1;
    }

    char filePath[512];
    read(rdfd, filePath, sizeof(filePath));

    struct stat statbuf;
    int size = 0;
    int nrLines = 0;
    if (lstat(filePath, &statbuf) == 0)
    {
        if (S_ISREG(statbuf.st_mode))
        {
            int fd = open(filePath, O_RDWR);

            size = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);

            char fileValue[size];
            read(fd, fileValue, size);
            lseek(fd, 0, SEEK_SET);

            for (int i = 0; i < size / 2; i++)
            {
                char aux = fileValue[i];
                fileValue[i] = fileValue[size - i];
                fileValue[size - i] = aux;
            }

            write(fd, fileValue, size);
            lseek(fd, 0, SEEK_SET);

            int nrTh = 4;
            pthread_t tid[4];
            thStruct param[4];
            void *result[4];
            for (int i = 0; i < nrTh; i++)
            {
                strcpy(param[i].data, fileValue);
                param[i].start = i * (size / nrTh);
                param[i].end = (i + 1) * (size / nrTh);
                if (i == nrTh - 1)
                {
                    param[i].end += size % nrTh;
                }
                pthread_create(&tid[i], NULL, thFunction, &param[i]);
            }
            for (int i = 0; i < nrTh; i++)
            {
                pthread_join(tid[i], &result[i]);
                nrLines += (int)(long)result[i];
            }
            close(fd);
        }
    }

    int result;
    char fileName[100] = "metadata.txt";
    int fd = open("metadata.txt", O_CREAT | O_RDWR);
    if (fd == -1)
    {
        result = -1;
        write(wrfd, result, sizeof(int));
    }
    else
    {
        write(fd, &size, sizeof(int));
        write(fd, &nrLines, sizeof(int));

        result = 0;
        write(wrfd, &result, sizeof(int));
        write(wrfd, fileName, sizeof(fileName));
    }
    close(fd);
    close(rdfd);
    close(wrfd);
    return 0;
}