CUR_PROG = general

ifndef VERBOSE
.SILENT:
endif

CC = gcc

WARNINGS = -Wall -Wno-multichar
STANDARD =  
CFLAGS = $(STANDARD) $(WARNINGS) -lm

all: main main_byte_io main_random main_hash main_debug main_overloads main_timer main_strings_and_files
	echo Generals were built successfully

main: main.c general.h
	$(CC) $(CFLAGS) main.c -o general

main_byte_io: main_byte_io.c general_byte_io.h 
	$(CC) $(CFLAGS) main_byte_io.c -o general

main_random: main_random.c general_random.h
	$(CC) $(CFLAGS) main_random.c -o general

main_hash: main_hash.c general_hash.h
	$(CC) $(CFLAGS) main_hash.c -o general

main_debug: main_debug.c general_debug.h
	$(CC) $(CFLAGS) main_debug.c -o general

main_overloads: main_overloads.c general_overloads.h
	$(CC) $(CFLAGS) main_overloads.c -o general

main_timer: main_timer.c general_timer.h
	$(CC) $(CFLAGS) main_timer.c -o general

main_strings_and_files: main_strings_and_files.c general_strings_and_files.h
	$(CC) $(CFLAGS) main_strings_and_files.c -o general

run: all
	./$(CUR_PROG)
