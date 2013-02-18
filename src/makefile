# Makefile for backupd
# 
# Feb 2013 - Bryant Moscon


backupd: backupd.o ini_parse.o hash_set.o
	gcc -o backupd backupd.o ini_parse.o hash_set.o

ini_parse.o: ini_parse.c
	gcc -c ini_parse.c

hash_set.o: hash_set.c
	gcc -c hash_set.c

backupd.o: backupd.c
	gcc -c backupd.c

clean:
	rm backupd *.o
