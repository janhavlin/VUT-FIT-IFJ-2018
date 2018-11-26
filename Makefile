TST_FILES = tests/main.c tests/tests.c tests/tests.h
SRC_FILES = src/scanner.c src/scanner.h src/dyn_arr.c src/dyn_arr.h src/type_conv.c src/type_conv.h src/ifj_error.c src/ifj_error.h src/s_table.c src/s_table.h src/parser.c src/parser.h

unit-tests: $(TST_FILES) $(SRC_FILES) 
	gcc $(TST_FILES) $(SRC_FILES) -o tst -lm
	./tst

code-gen: src/code_gen.c src/code_gen.h src/type_conv.c src/type_conv.h src/dyn_arr.c src/dyn_arr.h src/ifj_error.c src/ifj_error.h
	gcc src/code_gen.c src/code_gen.h src/type_conv.c src/type_conv.h src/dyn_arr.c src/dyn_arr.h src/ifj_error.c src/ifj_error.h -o gen -lm
	./gen