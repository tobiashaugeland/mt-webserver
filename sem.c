#include "sem.h"

struct SEM
{
    unsigned int count;
};

SEM *sem_init(int init_val)
{
    struct SEM *sem = malloc(sizeof(struct SEM));
    sem->count = init_val;
    return sem;
}

int sem_del(SEM *sem)
{
    free(sem);
    return 0;
}

void P(SEM *sem)
{
    while ((*sem).count == 0);
    sem->count--;
}

void V(SEM *sem){
    sem -> count ++;
}