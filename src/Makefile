#	file name:		Makefile
#	project:		VUT-FIT-IFJ-2018
#	created:		02.12.2018
#	last modified:	05.12.2018
#	
#	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
#	modifications:	Jan Havlín     xhavli47@stud.fit.vutbr.cz
#	
#	description:	Main				

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


	
