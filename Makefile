TST_FILES = tests/main.c tests/tests.c tests/tests.h
SRC_FILES = src/scanner.c src/scanner.h src/dyn_arr.c src/dyn_arr.h src/type_conv.c src/type_conv.h src/ifj_error.c src/ifj_error.h src/s_table.c src/s_table.h src/parser.c src/parser.h src/psa.c src/psa.h src/stack_list.c src/stack_list.h src/code_gen.c src/code_gen.h src/code_gen_list.c src/code_gen_list.h
CFLAGS= -std=c99 -pedantic -Wall -Wextra -g

unit-tests: $(TST_FILES) $(SRC_FILES) 
	gcc $(TST_FILES) $(SRC_FILES) -o tst -lm
	./tst

code-gen: src/code_gen.c src/code_gen.h src/code_gen_list.c src/code_gen_list.h src/type_conv.c src/type_conv.h src/dyn_arr.c src/dyn_arr.h src/ifj_error.c src/ifj_error.h
	gcc src/code_gen.c src/code_gen.h src/code_gen_list.c src/code_gen_list.h src/type_conv.c src/type_conv.h src/dyn_arr.c src/dyn_arr.h src/ifj_error.c src/ifj_error.h -o gen -lm
	./gen

ifj: src/main.c src/main.h $(SRC_FILES) 
	gcc $(CFLAGS) src/main.c src/main.h $(SRC_FILES) -o ifj18 -lm

zip:
	cd src && zip xvavri10 *.c *.h Makefile dokumentace.pdf rozsireni rozdeleni tests/tests.sh tests/test_scanner tests/test_parser tests/test_type_comp
	