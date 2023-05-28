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
    sem_t logSem;
    sem_init(&logSem, 1, 0);

    pid_t pid = fork();

    int fileLen[100];
    int pipeFd[2];
    if (pipeFd != 0)
    {
        perror("pipe failed");
        return -1;
    }

    if (pid == 0)
    {
        // copil
        close(pipeFd[1]); // close write end
        int totalSize = 0;
        int i = 0;
        read(pipeFd[0], &fileLen[i++], sizeof(int));
        sem_wait(&logSem);
        for (int j = 0; j < i; j++)
        {
            totalSize += fileLen[i];
        }
        printf("%d", totalSize);

        close(pipeFd[0]);
    }
    else
    {
        // parinte
        close(pipeFd[0]); // close read end

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
        int fd;
        while ((entry = readdir(dir) != NULL))
        {
            strcpy(filePath, path);
            strcat(filePath, "/");
            strcat(filePath, entry->d_name);

            fd = open(filePath, O_RDONLY);
            if (fd == -1)
            {
                perror("fd not found");
                return -1;
            }

            int fileLen2 = lseek(fd, 0, SEEK_END);
            if (fileLen > 0)
            {
                write(pipeFd[1], &fileLen2, sizeof(int));
            }
            close(fd);
        }
        sem_post(&logSem);
        close(pipeFd[1]);
        closedir(dir);
    }
    sem_destroy(&logSem);
    wait(NULL);
    return 0;
}