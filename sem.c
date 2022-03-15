#include "sem.h"
#include <stdlib.h>
#include <pthread.h>

struct SEM
{
    unsigned int count;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

SEM *sem_init(int init_val)
{
    SEM *sem = malloc(sizeof(SEM));
    sem->count = init_val;
    pthread_mutex_init(&sem->mutex, NULL);
    pthread_cond_init(&sem->cond, NULL);
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
    if (sem->count <= 0)
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
    pthread_mutex_unlock(&sem->mutex);
    pthread_cond_signal(&sem->cond);
}