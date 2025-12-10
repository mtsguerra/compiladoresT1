CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
TARGET = ada_compiler
OBJS = main.o lexer.o parser.o ast.o semantic.o symbol_table.o mips_codegen.o register_alloc.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c lexer.h parser.h ast.h semantic.h symbol_table.h mips_codegen.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c parser.h lexer.h ast.h
	$(CC) $(CFLAGS) -c parser.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

semantic.o: semantic.c semantic.h ast.h symbol_table.h
	$(CC) $(CFLAGS) -c semantic.c

symbol_table.o: symbol_table.c symbol_table.h
	$(CC) $(CFLAGS) -c symbol_table.c

mips_codegen.o: mips_codegen.c mips_codegen.h ast.h symbol_table.h register_alloc.h
	$(CC) $(CFLAGS) -c mips_codegen.c

register_alloc.o: register_alloc.c register_alloc.h
	$(CC) $(CFLAGS) -c register_alloc.c

clean:
	rm -f $(TARGET) $(OBJS) output.asm

test: $(TARGET)
	./$(TARGET) ../testes/testeBasico.ada

.PHONY: all clean test