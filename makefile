all: lab1

lab1: lab1.c functions.o
	gcc -g functions.o lab1.c -o lab1

functions: functions.c functions.h
	gcc -g -c functions.c

clean:
	rm -f *.o lab1
