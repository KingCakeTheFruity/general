CUR_PROG = general

ifndef VERBOSE
.SILENT:
endif

CC = gcc

WARNINGS = -Wall -Wno-multichar
STANDARD =  
CFLAGS = $(STANDARD) $(WARNINGS) -lm

all: main

main: main.c general.h
	$(CC) $(CFLAGS) main.c -o out
	echo Generals were built successfully

run: all
	./$(CUR_PROG)
