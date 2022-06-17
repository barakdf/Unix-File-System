

all: test

test: main.o ufs.o myfile.o ufs_test.o
	gcc -o test main.o ufs.o myfile.o ufs_test.o

main.o: main.c ufs.h
	gcc -c main.c

ufs_test.o: ufs_test.c ufs.h
	gcc -c ufs_test.c

ufs.o: ufs.c ufs.h
	gcc -c ufs.c

myfile.o: myfile.c myfile.h
	gcc -c myfile.c

clean:
	rm -f *.o test