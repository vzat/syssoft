CC = gcc
objects = main.o timelib.o permissions.o backup.o transfer.o audit.o logger.o
headers = macros.h permissions.h backup.h transfer.h audit.h logger.h

daemon: $(objects)
		$(CC) -o daemon $(objects) -Wall -lrt

main.o: main.c $(headers)
		$(CC) -c main.c -Wall

timelib.o: timelib.c
		$(CC) -c timelib.c -Wall

permissions.o: permissions.c macros.h
		$(CC) -c permissions.c -Wall

backup.o: backup.c macros.h
		$(CC) -c backup.c -Wall

transfer.o: transfer.c macros.h
		$(CC) -c transfer.c -Wall

audit.o: audit.c macros.h timelib.h
		$(CC) -c audit.c -Wall

logger.o: logger.c macros.h timelib.h
		$(CC) -c logger.c -Wall

daemonClient: daemonClient.c macros.h
		$(CC) daemonClient.c -o daemonClient -Wall -lrt

clean:
		rm daemon $(objects)

cleancli:
		rm daemonClient
