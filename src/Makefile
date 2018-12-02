CC = gcc
CFLAGS = -std=c99
TEST = sh

all_c_files = code_gen.c code_gen_list.c dyn_arr.c ifj_error.c main.c parser.c psa.c s_table.c scanner.c stack.c stack_list.c type_conv.c
test_script = tests/tests.sh
all: main

main: $(all_c_files)
	$(CC) $(CFLAGS) -o main $(all_c_files) -lm
	
test: 
	$(TEST) $(test_script)

clean:
	rm -f main


	
