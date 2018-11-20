CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra
CPFLAGS=-std=c++11 -pedantic -Wall

test_object_files=Tscanner.o Tifj_error.o
test_c_files=Tscanner.c Tifj_error.c

all: Tparser

#---------RUNNABLE---------#

Tparser: Tparser.o libhtab.a
	$(CC) -o Tparser -static Tparser.o -L. -lhtab

#-------RUNNABLE-END-------#
#--------LIBRARIES---------#	
	
libhtab.a: modules.o
	ar rcs libhtab.a $(test_object_files)

#------LIBRARIES-END-------#	
#---------MODULES----------#

Tparser.o: Tparser.c
	$(CC) $(CFLAGS) -c Tparser.c

modules.o: $(test_c_files)
	$(CC) $(CFLAGS) -c $(test_c_files)

#-------MODULES-END--------#

zip:
	zip xkarpi06.zip *.c *.cc *.h Makefile
	
clean:
	rm -f Tparser *.o libhtab.a