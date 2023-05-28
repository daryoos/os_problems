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
    pid_t pid2 = fork();
    int pipeFd[2];
    char fileName[512];
    int len;

    if (pipe(pipeFd) != 0)
    {
        perror("could not create pipe");
        return -1;
    }

    if (pid2 == 0)
    {
        // copil
        close(pipeFd[1]); // close write end
        read(pipeFd[0], fileName, sizeof(fileName));
        printf("%s    ", fileName);
        read(pipeFd[0], &len, sizeof(len));
        printf("%d\n", len);
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

        while ((entry = readdir(dir)) != NULL)
        {
            strcpy(filePath, path);
            strcat(filePath, "/");
            strcat(filePath, entry->d_name);

            strcpy(fileName, entry->d_name);
            write(pipeFd[1], fileName, sizeof(fileName));

            fd = open(filePath, O_RDONLY);
            if (fd == -1)
            {
                perror("fd not found");
                return -1;
            }

            len = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);

            write(pipeFd[1], &len, sizeof(len));
            close(fd);
        }

        close(pipeFd[1]);
        closedir(dir);
    }
    return 0;
}