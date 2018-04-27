CC=gcc
objects=main.o connection.o timelib.o
headers=macros.h connection.h timelib.h

server: $(objects)
		$(CC) -o server $(objects) -Wall -lrt -lpthread

main.o: main.c $(headers)
		$(CC) -c main.c -Wall

connection.o: connection.c
		$(CC) -c connection.c -Wall

timelib.o: timelib.c
		$(CC) -c timelib.c -Wall

clean:
		rm server $(objects)
