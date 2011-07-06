CC=gcc
CFLAGS=-Iinclude/ -ggdb

CXX=g++
CXX_FLAGS=-Iinclude/ -ggdb

DEPS = lex.yy.c
OUT = out

_C_OBJ = ast_pretty_printer.o driver.o  lex.yy.o symtab.o type_checker.o
C_OBJ = $(patsubst %,$(OUT)/%,$(_C_OBJ))

_CXX_OBJ = cfg.o cfg_printer.o cfg_data.o cfg_impl.o
CXX_OBJ = $(patsubst %,$(OUT)/%,$(_CXX_OBJ))

all: compiler cfg

# Compilador
compiler: $(C_OBJ) out/compiler.o
	$(CC) -o $@ $^ $(CFLAGS)

# CFG
cfg: $(CXX_OBJ) $(C_OBJ)
	$(CXX) -o $@ $^ $(CXX_FLAGS)

# Gerandor o lexer
lex.yy.c: monga.l
	lex monga.l

# Compilando arquivos C
$(OUT)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compilando arquivos C++
$(OUT)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)

clean:
	rm -f $(OUT)/*.o
	rm -f compiler cfg
	rm -f lex.yy.c

.PHONY: clean
