#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symtab.h"

SymTab *symtab_new(SymTab *prev) {
  SymTab *new;
  ALLOC(new, SymTab);
  new->prev = prev;
  new->first_sym = NULL;
  return new;
}

void symtab_free(SymTab *st) {
  free(st);
}

Declr *symtab_find_one(SymTab *st, char *name) {
  SymTabNode *stn;
  stn = st->first_sym;
  while(stn) {
    if(strcmp(stn->name, name) == 0) return stn->symbol;
    stn = stn->next;
  }
  return NULL;
}

int symtab_add(SymTab *st, Declr *sym) {
  char *name;
  SymTabNode *stn;
  if(sym->tag == DECLR_VAR)
    name = sym->u.name;
  else
    name = sym->u.func.name;
  if(symtab_find_one(st, name)) return 0;
  ALLOC(stn, SymTabNode);
  stn->name = name;
  stn->symbol = sym;
  stn->next = st->first_sym;
  st->first_sym = stn;
  return 1;
}

Declr *symtab_find(SymTab *st, char* name) {
  Declr *d;
  if(!st) return NULL;
  if(d = symtab_find_one(st, name))
    return d;
  else
    return symtab_find(st->prev, name);
}

