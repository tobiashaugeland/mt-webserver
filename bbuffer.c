#include <stdlib.h>
#include "bbuffer.h"
#include "sem.h"
#include <stdio.h>
#include <pthread.h>

struct BNDBUF
{
    int insert;
    int remove;
    SEM *full;
    SEM *empty;
    int size;
    int buffer[];
};

BNDBUF *bb_init(unsigned int size)
{
    BNDBUF *bbuffer = malloc(sizeof(*bbuffer) + size * sizeof(int));
    bbuffer->insert = 0;
    bbuffer->remove = 0;
    bbuffer->full = sem_init(0);
    bbuffer->empty = sem_init(size);
    bbuffer->size = size;
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
    pthread_t tid = pthread_self();
    printf("starting P in thread %ld\n", tid);
    P(bb->full);
    item = bb->buffer[bb->remove];
    printf("got %d at %d\n", item, bb->remove);
    bb->remove = (bb->remove + 1) % bb->size;
    V(bb->empty);
    return item;
}

void bb_add(BNDBUF *bb, int fd)
{
    P(bb->empty);
    bb->buffer[bb->insert] = fd;
    printf("added %d at %d\n", fd, (bb->insert));
    bb->insert = (bb->insert + 1) % bb->size;
    V(bb->full);
}