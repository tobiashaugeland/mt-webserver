#include <stdlib.h>
#include "bbuffer.h"
#include "sem.h"

struct BNDBUF{
    int insert;
    int remove;
    SEM *full;
    SEM *empty;
    int buffer[];
};

BNDBUF * bb_init(unsigned int size){
    BNDBUF *bbuffer = malloc(sizeof(*bbuffer) + size*sizeof(int));
    bbuffer -> insert = 0;
    bbuffer -> remove = 0;
    bbuffer -> full = sem_init(0);
    bbuffer -> empty = sem_init(size);

}