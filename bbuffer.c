#include <stdlib.h>
#include "bbuffer.h"
#include "sem.h"
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

struct BNDBUF
{
    int insert;
    int remove;
    SEM *full;
    SEM *empty;
    pthread_mutex_t read_mutex;
    pthread_mutex_t write_mutex;
    int size;
    int buffer[];
};

BNDBUF *bb_init(unsigned int size)
{
    errno = 0;
    BNDBUF *bbuffer = malloc(sizeof(*bbuffer) + size * sizeof(int));
    if (errno != 0)
    {
        perror("malloc");
        return NULL; // Should the program maybe exit here?
    }

    bbuffer->insert = 0;
    bbuffer->remove = 0;
    bbuffer->full = sem_init(0);
    bbuffer->empty = sem_init(size);
    bbuffer->size = size;

    pthread_mutex_init(&bbuffer->read_mutex, NULL);
    if (errno != 0)
    {
        perror("pthread_mutex_init");
        return NULL;
    }

    pthread_mutex_init(&bbuffer->write_mutex, NULL);
    if (errno != 0)
    {
        perror("pthread_mutex_init");
        return NULL;
    }

    return bbuffer;
}

void bb_del(BNDBUF *bb)
{
    sem_del(bb->full);
    sem_del(bb->empty);
    free(bb);
}

int bb_get(BNDBUF *bb)
{
    int item;

    // Locking for crital section
    P(bb->full);
    pthread_mutex_lock(&bb->read_mutex);

    item = bb->buffer[bb->remove];
    bb->remove = (bb->remove + 1) % bb->size;

    // Unlocking crital section
    pthread_mutex_unlock(&bb->read_mutex);
    V(bb->empty);

    return item;
}

void bb_add(BNDBUF *bb, int fd)
{
    // Locking for crital section
    P(bb->empty);
    pthread_mutex_lock(&bb->write_mutex);

    bb->buffer[bb->insert] = fd;
    bb->insert = (bb->insert + 1) % bb->size;

    // Unlocking crital section
    pthread_mutex_unlock(&bb->write_mutex);
    V(bb->full);
}