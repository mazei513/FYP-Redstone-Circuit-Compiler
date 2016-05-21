CC=g++
CFLAGS=-std=c++11 -c

all: ccompiler

ccompiler: main.o
	$(CC) main.o -o ccompiler

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp
	
clean:
	rm *o ccompiler