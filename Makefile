OBJ = error_handler.c analyzator.c stack.c generator.c parser.c stackA.c skener.c symtable.c
CFLAGS=-I.
CC=gcc

compiler: $(OBJ)
	@$(CC) $(OBJ) -o $@ $(CFLAGS)

clean:
	rm *.o
