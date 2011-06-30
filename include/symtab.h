#ifndef _SYMTAB_H
#define _SYMTAB_H

#define ALLOC(p, t)	p = (t*)malloc(sizeof(t)); if(!p) { printf("Panic! Out of memory! Exiting...\n"); exit(0); }
#define ALLOCS(p, n)	p = (char*)malloc(n * sizeof(char)); if(!p) { printf("Panic! Out of memory! Exiting...\n"); exit(0); }

typedef struct _SymTabNode SymTabNode;
typedef struct _SymTab SymTab;

struct _SymTab {
  SymTabNode *first_sym;
  SymTab *prev;
};

struct _SymTabNode {
  char *name;
  Declr *symbol;
  SymTabNode *next;
};

SymTab *symtab_new(SymTab *prev);
void symtab_free(SymTab *st);
int symtab_add(SymTab *st, Declr *sym);
Declr *symtab_find(SymTab *st, char *name);

#endif
