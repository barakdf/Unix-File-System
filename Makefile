

all: test libufs.so libmylibc.so

test:  ufs.o myfile.o ufs_test.o
	gcc -o test ufs.o myfile.o ufs_test.o

libufs.so: ufs.o
	$(CC) --shared -fPIC -g ufs.o -o libmyfs.so

libmylibc.so: ufs.o myfile.o
	$(CC) --shared -fPIC -g  ufs.o myfile.o -o libmylibc.so


ufs_test.o: ufs_test.c ufs.h
	gcc -c ufs_test.c

ufs.o: ufs.c ufs.h
	gcc -c ufs.c -fPIC

myfile.o: myfile.c myfile.h
	gcc -c myfile.c -fPIC

clean:
	rm -f *.o *.so test