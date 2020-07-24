CFLAGS = -g -Wall -ansi -pedantic
prog:	main.c assembler.h asm_tables.o
	gcc $(CFLAGS) main.c asm_tables.o -o prog
asm_tables.o:	asm_tables.c asm_tables.h
	gcc $(CFLAGS) -c asm_tables.c