all:padre
#all: lab1

#lab1: lab1.c functions.o
#	gcc -g functions.o lab1.c -o lab1

padre: padre.c functions.o broker worker
	gcc -g functions.o padre.c -o padre

broker: broker.c fbroker worker functions.o
	gcc -g functions.o fbroker.o broker.c -o broker

worker: worker.c fworker functions.o
	gcc -g functions.o fworker.o worker.c -o worker

fbroker: fbroker.c fbroker.h functions.o
	gcc -g -c fbroker.c

fworker: fworker.c fworker.h functions.o
	gcc -g -c fworker.c

functions: functions.c functions.h
	gcc -g -c functions.c

clean:
	rm -f *.o lab1 broker worker padre
