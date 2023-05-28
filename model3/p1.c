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
    int tid;
} thStruct;

int counter = 0;
sem_t sem0;
sem_t sem1;
sem_t sem2;
sem_t semct;

void *thFunction(void *arg)
{
    thStruct *s = (thStruct *)arg;

    while (counter < 100)
    {
        if (s->tid == 0)
        {
            sem_wait(&sem0);
            sem_wait(&semct);
            if (counter == 100)
            {
                sem_post(&sem0);
                sem_post(&sem1);
                sem_post(&sem2);
                sem_post(&semct);
                break;
            }
            counter++;
            printf("Th[%d]: %d -> %d\n", s->tid, counter - 1, counter);
            sem_post(&semct);
            sem_post(&sem2);
        }
        else if (s->tid == 1)
        {
            sem_wait(&sem1);
            sem_wait(&semct);
            if (counter == 100)
            {
                sem_post(&sem0);
                sem_post(&sem1);
                sem_post(&sem2);
                sem_post(&semct);
                break;
            }
            counter++;
            printf("Th[%d]: %d -> %d\n", s->tid, counter - 1, counter);
            sem_post(&semct);
            sem_post(&sem0);
        }
        else if (s->tid == 2)
        {
            sem_wait(&sem2);
            sem_wait(&semct);
            if (counter == 100)
            {
                sem_post(&sem0);
                sem_post(&sem1);
                sem_post(&sem2);
                sem_post(&semct);
                break;
            }
            counter++;
            printf("Th[%d]: %d -> %d\n", s->tid, counter - 1, counter);
            sem_post(&semct);
            sem_post(&sem1);
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    sem_init(&sem0, 0, 0);
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 1);
    sem_init(&semct, 0, 1);

    int nrTh = 3;
    pthread_t tid[nrTh];
    thStruct param[nrTh];

    for (int i = 0; i < nrTh; i++)
    {
        param[i].tid = i;
        pthread_create(&tid[i], NULL, thFunction, &param[i]);
    }
    for (int i = 0; i < nrTh; i++)
    {
        pthread_join(tid[i], NULL);
    }

    return 0;
}