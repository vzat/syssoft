CC = gcc
objects = main.o areaOfRectangle.o areaOfCircle.o area_of_triangle.o areaOfSquare.o
headers = areaOfRectangle.h areaOfCircle.h area_of_triangle.h areaOfSquare.h

myprog: $(objects)
				$(CC) -g -o areaProg $(objects) -lm

main.o: main.c $(headers)
				$(CC) -g -c main.c

areaOfRectangle.o: areaOfRectangle.c
				$(CC) -g -c areaOfRectangle.c

areaOfSquare.o: areaOfSquare.c areaOfRectangle.h
				$(CC) -g -c areaOfSquare.c

areaOfCircle.o: areaOfCircle.c
				$(CC) -g -c areaOfCircle.c

area_of_triangle.o: area_of_triangle.c
				$(CC) -g -c area_of_triangle.c -lm

clean:
				rm areaProg $(objects)
