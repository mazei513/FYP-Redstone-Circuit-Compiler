CC=g++
CFLAGS=-std=c++11 -Wl,--stack,16777216 -Os
LFLAG=-c

all: ccompiler clean

ccompiler: nbtOps.o interpreter.o toVer.o main.o
	$(CC) $(CFLAGS) nbtOps.o interpreter.o toVer.o main.o -o ccompiler
	
main.o: main.cpp
	$(CC) $(CFLAGS) $(LFLAG) main.cpp

interpreter.o: interpreter.cpp
	$(CC) $(CFLAGS) $(LFLAG) interpreter.cpp
	
nbtOps: nbtOps.o
	$(CC) $(CFLAGS) nbtOps.o -o nbtOps
	
nbtOps.o: nbtOps.cpp
	$(CC) $(CFLAGS) $(LFLAG) nbtOps.cpp
	
toVer: toVer.o
	$(CC) $(CFLAGS) toVer.o -o toVer
	
toVer.o: toVer.cpp
	$(CC) $(CFLAGS) $(LFLAG) toVer.cpp
	
clean:
	rm *o
	
clean_all:
	rm *o *.exe