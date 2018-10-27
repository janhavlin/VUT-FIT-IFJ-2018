TST_FILES = tests/main.c tests/tests.c tests/tests.h
SRC_FILES = src/scanner.c src/scanner.h src/dyn_arr.c src/dyn_arr.h src/type_conv.c src/type_conv.h

unit-tests: $(TST_FILES) $(SRC_FILES) 
	gcc $(TST_FILES) $(SRC_FILES) -o tst -lm
	./tst