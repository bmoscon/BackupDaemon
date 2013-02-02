# Makefile for INI parse test program
# 
# Feb 2013 - Bryant Moscon


ini_test: ini_test.o ini_parse.o
	gcc -o ini_test ini_test.o ini_parse.o

ini_test.o: ini_test.c
	gcc -c ini_test.c

ini_parse.o: ../src/ini_parse.c
	gcc -c ../src/ini_parse.c

clean:
	rm ini_test ini_test.o ini_parse.o
