

all: test

test: main.o ufs.o
	gcc -o test main.o ufs.o

main.o: main.c ufs.h
	gcc -c main.c

ufs.o: ufs.c ufs.h
	gcc -c ufs.c


clean:
	rm -f *.o test