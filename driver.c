#include <stdio.h>
#include <stdlib.h>
#include "decl.h"
#include "ast.h"

extern int yylineno;
extern FILE *outfile;
extern char *filename;

static int token;

static Command *command() {
  Command *this;
  ALLOC(this, Command);
  this->tag = COMMAND_RET;
  this->line = yylineno;
  this->u.ret = NULL;
  token = yylex();
  return this;
}

static CommListNode *commandl() {
  CommListNode *first, *curr;
  if(token) {
    ALLOC(first, CommListNode);
    first->comm = command();
    first->next = NULL;
  }
  curr = first;
  while(token) {
    CommListNode *next;
    ALLOC(next, CommListNode);
	next->comm = command();
	next->next = NULL;
    curr->next = next;
    curr = next;
  }
  return first;
}

int main(int argc, char **argv) {
  FILE *f;
  CommListNode* commands;
  if(argc > 1) {
    f = fopen(argv[1], "r");
    filename = argv[1];
  } else {
    f = stdin;
    filename = "stdin";
  }
  if(!f) {
    fprintf(stderr, "Cannot open file %s. Exiting...", filename);
    exit(0);
  }
  yyrestart(f);
  yylineno = 1;
  outfile = stdout;
  filename = "stdout";
  token = yylex();
  commands = commandl();
  print_commlist(0, commands);
}
