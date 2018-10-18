TST_FILES = tests/main.c tests/tests.c tests/tests.h
SRC_FILES = src/scanner.c src/scanner.h

u-tests: $(TST_FILES) $(SRC_FILES) 
	gcc $(TST_FILES) $(SRC_FILES) -o tst