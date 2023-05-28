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
    char string[512];
    strcpy(string, argv[1]);
    char path[512];
    strcpy(path, argv[2]);

    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("dir not found");
        return -1;
    }

    int len = strlen(string);
    char filePath[512];
    int fd;
    char buf;
    int fileLen;
    struct dirent *entry;
    int lineNr = 0;
    char line[512];
    while ((entry = readdir(dir)) != NULL)
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

        fileLen = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

        int il = 0;
        for (int i = 0; i < fileLen; i++)
        {
            read(fd, buf, 1);
            if (buf == '\n')
            {
                lineNr++;
                line[il] = '\0';
                il = 0;

                if (strstr(line, string) != NULL)
                {
                    printf("%d\n", lineNr);
                }
            }
            else
            {
                line[il++] = buf;
            }
        }
        close(fd);
    }
    closedir(dir);
    return 0;
}