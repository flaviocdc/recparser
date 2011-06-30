CC=gcc
CFLAGS=-Iinclude/ -ggdb

CXX=g++
CXX_FLAGS=-Iinclude/ -ggdb

DEPS = lex.yy.c
OUT = out

_C_OBJ = ast_pretty_printer.o  compiler.o  driver.o  lex.yy.o
C_OBJ = $(patsubst %,$(OUT)/%,$(_C_OBJ))


compiler: $(C_OBJ)
	gcc -o $@ $^ $(CFLAGS)

lex.yy.c: monga.l
	lex monga.l

$(OUT)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OUT)/*.o
	rm -f compiler

.PHONY: clean
