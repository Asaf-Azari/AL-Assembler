CFLAGS = -Wall -ansi -pedantic
prog:	main.c assembler.h
	gcc $(CFLAGS) main.c  -o prog 