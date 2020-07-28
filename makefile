CFLAGS = -g -Wall -ansi -pedantic
OBJS = main.o first_pass.o asm_tables.o symbol_table.o
prog:	$(OBJS)
	gcc $(CFLAGS) $^ -o prog
main.o:	main.c assembler.h asm_tables.h first_pass.h
	gcc $(CFLAGS) -c main.c 
asm_tables.o:	asm_tables.c asm_tables.h
	gcc $(CFLAGS) -c asm_tables.c
first_pass.o:	first_pass.c first_pass.h
	gcc $(CFLAGS) -c first_pass.c
symbol_table.o:	symbol_table.c symbol_table.h
	gcc $(CFLAGS) -c symbol_table.c
clean:
	rm $(OBJS)
