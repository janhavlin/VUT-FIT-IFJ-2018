CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g

all: main
main: main.c dyn_arr.c ifj_error.c stack_list.c
	$(CC) $(CFLAGS) -o main main.c dyn_arr.c ifj_error.c stack_list.c -lm

clean:
	rm main


	
