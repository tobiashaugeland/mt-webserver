TARGET_EXEC := mtwwwd

CC = gcc

all:
	$(CC) mtwwwd.c sem.c bbuffer.c -g -pthread -o $(TARGET_EXEC)

clean:
	rm $(TARGET_EXEC)