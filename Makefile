CC=gcc
CFLAGS=-Iinclude/ -ggdb

CXX=g++
CXX_FLAGS=-Iinclude/ -ggdb

AST_DIR=ast
CFG_DIR=cfg
SSA_DIR=ssa

DEPS = $(AST_DIR)/lex.yy.c
OUT = out

_C_OBJ = ast_pretty_printer.o driver.o lex.yy.o symtab.o type_checker.o
C_OBJ = $(patsubst %,$(OUT)/%,$(_C_OBJ))

_CXX_OBJ = cfg_gen.o cfg_printer.o cfg_data.o ast_utils.o ssa.o ssa_debug.o
CXX_OBJ = $(patsubst %,$(OUT)/%,$(_CXX_OBJ))

all: compiler cfg-dummy cfg-llvm

# Compilador
compiler: $(C_OBJ) out/compiler.o
	$(CC) -o $@ $^ $(CFLAGS)

# CFG
cfg-dummy: $(CXX_OBJ) $(C_OBJ) out/cfg_output_dummy.o
	$(CXX) -o $@ $^ $(CXX_FLAGS)
	
cfg-llvm: $(CXX_OBJ) $(C_OBJ) out/cfg_output_llvm.o
	$(CXX) -o $@ $^ $(CXX_FLAGS)

# Gerandor o lexer
$(AST_DIR)/lex.yy.c: monga.l
	lex -o $@ $<

# Compilando arquivos do gerador de AST
$(OUT)/%.o: $(AST_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compilando arquivos do gerador de CFG
$(OUT)/%.o: $(CFG_DIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)

# Compilando arquivos do gerador de SSA
$(OUT)/%.o: $(SSA_DIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)

clean:
	rm -f $(OUT)/*.o
	rm -f compiler cfg-dummy cfg-llvm
	rm -f $(AST_DIR)/lex.yy.c

.PHONY: clean
