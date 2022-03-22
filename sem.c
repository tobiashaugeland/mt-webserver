#include "sem.h"
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

struct SEM
{
    unsigned int count;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

SEM *sem_init(int init_val)
{
    errno = 0;
    SEM *sem = malloc(sizeof(SEM));
    sem->count = init_val;
    pthread_mutex_init(&sem->mutex, NULL);
    if (errno != 0)
    {
        perror("pthread_mutex_init");
        free(sem);
        return NULL;
    }
    pthread_cond_init(&sem->cond, NULL);
    if (errno != 0)
    {
        perror("pthread_cond_init");
        free(sem);
        return NULL;
    }

    return sem;
}

int sem_del(SEM *sem)
{
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->cond);
    free(sem);
    return 0;
}

void P(SEM *sem)
{
    pthread_mutex_lock(&sem->mutex);
    while (sem->count <= 0)
    {
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    (sem->count)--;
    pthread_mutex_unlock(&sem->mutex);
}

void V(SEM *sem)
{
    pthread_mutex_lock(&sem->mutex);
    (sem->count)++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
}