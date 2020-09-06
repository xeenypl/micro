BINARY = mic
OBJECT = compile.o lexer.o memory.o file.o

MAIN = src/main.c

CC = gcc
CCFLAGS = -Wall -Wextra -pedantic -O3

PREFIX = /usr/local

all: $(BINARY)

$(BINARY): $(MAIN) $(OBJECT)
	$(CC) $(CCFLAGS) -o $@ $< $(OBJECT)

%.o: src/%.c
	$(CC) $(CCFLAGS) -o $@ -c $<

install: $(BINARY)
	cp $(BINARY) $(PREFIX)/bin/

uninstall:
	rm $(PREFIX)/bin/$(BINARY)

clean:
	rm $(OBJECT) $(BINARY)

.PHONY: all clean
