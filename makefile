CFLAGS = -g -Wall -ansi -pedantic
OBJS = main.o first_pass.o asm_tables.o symbol_table.o second_pass.o mem_array.o util.o file_handling.o
UTILITY = util.c util.h
prog:	$(OBJS)
	gcc $(CFLAGS) $^ -o prog
main.o:	main.c file_handling.h asm_tables.h first_pass.h
	gcc $(CFLAGS) -c main.c 
asm_tables.o:	asm_tables.c asm_tables.h
	gcc $(CFLAGS) -c asm_tables.c
first_pass.o:	first_pass.c first_pass.h $(UTILITY)
	gcc $(CFLAGS) -c first_pass.c
second_pass.o:	second_pass.c first_pass.h second_pass.h $(UTILITY)
	gcc $(CFLAGS) -c second_pass.c
symbol_table.o:	symbol_table.c symbol_table.h
	gcc $(CFLAGS) -c symbol_table.c
mem_arr.o:	mem_array.c mem_array.h
	gcc $(CLFAGS) -c mem_array.c
util.o:	util.c util.h
	gcc $(CLFAGS) -c util.c
file_handling.o:	file_handling.c file_handling.h
	gcc $(CLFAGS) -c file_handling.c
clean:
	rm $(OBJS)
