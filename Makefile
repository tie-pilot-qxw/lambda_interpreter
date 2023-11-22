lexer.h: lang.l
	flex lang.l

lexer.c: lang.l
	flex lang.l

parser.c: lang.y
	bison -o parser.c -d -v lang.y

parser.h: lang.y
	bison -o parser.c -d -v lang.y

lib.o: lib.c lib.h
	gcc -c -g lib.c

lang.o: lang.c lang.h lib.h
	gcc -c -g lang.c

parser.o: parser.c parser.h lexer.h lang.h
	gcc -c -g parser.c

lexer.o: lexer.c lexer.h parser.h lang.h
	gcc -c -g lexer.c

check.o: check.cpp check.hpp lang.h
	gcc -c -g check.cpp

main.o: main.cpp lexer.h parser.h lang.h check.hpp
	gcc -c -g main.cpp

main: lang.o parser.o lexer.o lib.o main.o check.o
	gcc -g lang.o parser.o lexer.o lib.o main.o check.o -lstdc++ -o main

all: main

clean:
	rm -f lexer.h lexer.c parser.h parser.c *.o main

%.c: %.y

%.c: %.l

.DEFAULT_GOAL := all

