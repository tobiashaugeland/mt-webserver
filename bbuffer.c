#include <stdlib.h>
#include "bbuffer.h"
#include "sem.h"

struct BNDBUF{
    int insert;
    int remove;
    SEM *full;
    SEM *empty;
    int size;
    int buffer[];
};

BNDBUF * bb_init(unsigned int size){
    BNDBUF *bbuffer = malloc(sizeof(*bbuffer) + size*sizeof(int));
    bbuffer -> insert = 0;
    bbuffer -> remove = 0;
    bbuffer -> full = sem_init(0);
    bbuffer -> empty = sem_init(size);
    bbuffer -> size = size;

}

void bb_del(BNDBUF *bb){
    sem_del(bb->full);
    sem_del(bb->empty);
    free(bb);
}

int bb_get(BNDBUF *bb){
    int item;
    P(bb->empty);
    item = bb->buffer[bb->remove];
    bb->remove = (bb->remove + 1) % bb->size;
    V(bb->full);
    return item;
}

void bb_add(BNDBUF *bb, int fd){
    P(bb->full);
    bb->buffer[bb->insert] = fd;
    bb->insert = (bb->insert + 1) % bb->size;
    V(bb->empty);
}