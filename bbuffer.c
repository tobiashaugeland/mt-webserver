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
    SEM *remove_lock;
    SEM *insert_lock;
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
    bbuffer->remove_lock = sem_init(1);
    bbuffer->insert_lock = sem_init(1);
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

    // Locking for crital section
    P(bb->full);
    P(bb->remove_lock);

    item = bb->buffer[bb->remove];
    bb->remove = (bb->remove + 1) % bb->size;

    // Unlocking crital section
    V(bb->remove_lock);
    V(bb->empty);

    return item;
}

void bb_add(BNDBUF *bb, int fd)
{
    // Locking for crital section
    P(bb->empty);
    P(bb->insert_lock);

    bb->buffer[bb->insert] = fd;
    bb->insert = (bb->insert + 1) % bb->size;

    // Unlocking crital section
    V(bb->insert_lock);
    V(bb->full);
}