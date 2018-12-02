CC = gcc
CFLAGS = -std=c99
TEST = sh

#takes all c files from directory
c_files = $(wildcard *.c)
test_script = tests/tests.sh
all: ifjcomp

ifjcomp: $(c_files)
	$(CC) $(CFLAGS) -o ifjcomp $(c_files) -lm
	
test: 
	$(TEST) $(test_script)
	
clean:
	rm -f main


	
