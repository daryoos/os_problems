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

int main(int argc, char **argv)
{
    char pipeWr[] = "WR";
    char pipeRd[] = "RD";
    if (mkfifo(pipeWr, 0600) != 0)
    {
        perror("mkinfo");
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
    strcpy(filePath, argv[1]);

    write(wrfd, filePath, sizeof(filePath));

    int result;
    read(rdfd, &result, sizeof(int));

    char fileName[100];
    read(rdfd, fileName, sizeof(fileName));

    int fd = open(filePath, O_RDWR);
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char fileValue[size];
    read(fd, fileValue, size);
    printf("%s\n", fileValue);

    close(fd);

    fd = open(fileName, O_RDWR);
    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    read(fd, fileValue, size);
    printf("%s\n", fileValue);

    close(fd);

    close(wrfd);
    close(rdfd);
    return 0;
}