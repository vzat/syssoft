CC=gcc
objects=main.o
headers=macros.h

client: $(objects)
		$(CC) -o client $(objects) -Wall -lrt

main.o: main.c $(headers)
		$(CC) -c main.c -Wall

clean:
		rm client $(objects)
