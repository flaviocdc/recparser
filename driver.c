#include <stdio.h>
#include <stdlib.h>
#include "decl.h"
#include "ast.h"

extern int yylineno;
extern FILE *outfile;
extern char *filename;

static int token;

static void match(int next) {
  if (token != next) {
    printf("missing token: %c\n", next);
    exit(0);
  }

  token = yylex();
}

static Exp* expr() {
  return NULL;
}

static Command *command() {
  Command *this;
  ALLOC(this, Command);

  switch (token) {
    case TK_IF: {
      this->tag = COMMAND_IF;
      
      token = yylex();
      match('(');

      this->u.cif.exp = expr();

      match(')');

      this->u.cif.comm = command();

      if (token == TK_ELSE) {
        token = yylex();
        this->u.cif.celse = command();
      }

      break;
    }

    case TK_WHILE: {
      this->tag = COMMAND_WHILE;

      token = yylex();
      match('(');

      this->u.cwhile.exp = expr();

      match(')');

      this->u.cwhile.comm=command();
      break;
    }
    
    case ';': {
      token = yylex();
      this->tag = COMMAND_BLOCK;
      break;
    }
   
    default: {
      printf("invalid command in line %i\n", yylineno);
      exit(0);
    }

  }  

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
