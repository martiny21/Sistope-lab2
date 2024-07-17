all:padre
#all: lab1

#lab1: lab1.c functions.o
#	gcc -g functions.o lab1.c -o lab1

padre: padre.c functions broker worker
	gcc -g functions.o padre.c -o padre

broker: broker.c functions
	gcc -g functions.o broker.c -o broker

worker: worker.c
	gcc -g worker.c -o worker

functions: functions.c functions.h
	gcc -g -c functions.c

clean:
	rm -f *.o lab1 broker worker padre
