TARGET_EXEC := webserver

CC = gcc

all:
	$(CC) mtwwwd.c sem.c bbuffer.c -o $(TARGET_EXEC)

clean:
	rm -f *.o TARGET_EXEC

