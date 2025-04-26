# Makefile for building UCTool
CC = g++
FLEX = flex
BISON = bison
CFLAGS = -Iinclude -std=c++17 -DYY_NO_UNISTD_H
LDFLAGS = -lfl

all: out/uctool

out/uctool: build/lex.yy.o build/lex-main.o build/parser.yy.o build/parser-main.o build/uctool-main.o
	$(CC) -o $@ $^ $(LDFLAGS)

build/lex.yy.o: src/lex.yy.c include/lexer.h src/parser.yy.h
	$(CC) $(CFLAGS) -c src/lex.yy.c -o $@

src/lex.yy.c: src/lexer.l include/lexer_utils.hpp src/parser.yy.h
	$(FLEX) -o src/lex.yy.c src/lexer.l

build/lex-main.o: src/lex-main.cpp include/lexer_utils.hpp include/lexer.h src/lex.yy.c
	$(CC) $(CFLAGS) -c src/lex-main.cpp -o $@

build/parser.yy.o: src/parser.yy.c src/parser.yy.h include/lexer_utils.hpp include/parser_utils.hpp include/lexer.h
	$(CC) $(CFLAGS) -c src/parser.yy.c -o $@

src/parser.yy.c src/parser.yy.h: src/parser.y include/lexer_utils.hpp include/parser_utils.hpp include/lexer.h
	$(BISON) -d -o src/parser.yy.c src/parser.y

build/parser-main.o: src/parser-main.cpp include/parser_utils.hpp include/lexer_utils.hpp include/lexer.h src/parser.yy.h
	$(CC) $(CFLAGS) -c src/parser-main.cpp -o $@

build/uctool-main.o: src/uctool-main.cpp
	$(CC) $(CFLAGS) -c src/uctool-main.cpp -o $@

clean:
	rm -f build/* src/lex.yy.c src/parser.yy.c src/parser.yy.h out/uctool