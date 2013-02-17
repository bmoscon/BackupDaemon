# Makefile for INI parse test program
# 
# Feb 2013 - Bryant Moscon


ini_test: ini_test.o ini_parse.o hash_set.o
	gcc -o ini_test ini_test.o ini_parse.o hash_set.o

ini_test.o: ini_test.c
	gcc -c -g ini_test.c

ini_parse.o: ../src/ini_parse.c
	gcc -c -g ../src/ini_parse.c

hash_set.o: ../src/hash_set.c
	gcc -c -g ../src/hash_set.c

clean:
	rm ini_test *.o
