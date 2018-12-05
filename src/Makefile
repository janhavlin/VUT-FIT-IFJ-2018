CC = gcc
CFLAGS = -std=c99
TEST = sh

#takes all c files from directory
c_files = $(wildcard *.c *.h)
test_script = tests.sh
all: ifjcomp

ifjcomp: $(c_files)
	$(CC) $(CFLAGS) -o ifjcomp $(c_files) -lm
	
test: 
	cd ../tests && $(TEST) $(test_script)
	
clean:
	rm -f main


	
