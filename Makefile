
CC = g++
CFLAGS = -g -std=c++11

disassem: main.o sicXe.o symbol.o literal.o
	$(CC) $(CFLAGS) -o disassem -g main.o sicXe.o symbol.o literal.o

main.o: main.cpp sicXe.h symbol.h literal.h
	$(CC) $(CFLAGS) -c main.cpp
sicXe.o: sicXe.h
symbol.o: symbol.h
literal.o: literal.h
clean:
	-rm disassem out.lst main.o sicXe.o symbol.o literal.o