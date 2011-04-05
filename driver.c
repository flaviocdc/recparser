#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decl.h"
#include "ast.h"

#define NEXT() token = yylex();
#define NO_BINOP 4

extern int yylineno;
extern FILE *outfile;
extern char *filename;

static int token;
static CommListNode *commandl();
static Exp* simple();

static void match(int next) {
  if (token != next) {
    printf("missing token: %c\n", next);
    exit(0);
  }

  NEXT();
}

static int pri[] = {
  1, /* + */
  1, /* - */
  2, /* * */
  2, /* / */
  0
};

static int binop(int token) {
  switch (token) {
    case '+' : return 0;
    case '-' : return 1;
    case '*' : return 2;
    case '/' : return 3;
    default : return NO_BINOP;
  }
}

static Exp* expr(int level) {
  Exp *exp1 = simple();

  int op = binop(token);

  while (op != NO_BINOP && pri[op] > level) {
    Exp *new, *exp2;

    int token_op = token;

    NEXT();
    exp2 = expr(pri[op]);
    op = binop(token);

    ALLOC(new, Exp);
    new->tag = EXP_BINOP;
    new->u.binop.op = token_op;
    new->u.binop.e1 = exp1;
    new->u.binop.e2 = exp2;

    exp1 = new;
  }

  return exp1;
}

static Exp* simple() {
  Exp *exp;

  switch (token) {
    case TK_ID: {
      Var *var;
      char *name;

      ALLOC(exp, Exp);
      ALLOC(var, Var);
      ALLOCS(name, strlen(yyval.sval) + 1);

      strcpy(name, yyval.sval);
      var->name = name;

      exp->tag = EXP_VAR;
      exp->u.var = var;

      NEXT();

      break;
    }
    case TK_INT: {
      ALLOC(exp, Exp);

      exp->tag = EXP_INT;
      exp->u.ival = yyval.ival;

      NEXT();

      break;
    }
    case '(': {
      NEXT();

      exp = expr(0);

      match(')');

      break;
    }
    default:
      printf("Expressao invalida\n");
      exit(1);
      break;
  }

  return exp;
}

static Command *command() {
  Command *this;
  ALLOC(this, Command);

  switch (token) {
    case TK_IF: {
      this->tag = COMMAND_IF;
      
      NEXT();
      match('(');

      this->u.cif.exp = expr(0);

      match(')');

      this->u.cif.comm = command();

      if (token == TK_ELSE) {
        NEXT();
        this->u.cif.celse = command();
      }

      break;
    }

    case TK_WHILE: {
      this->tag = COMMAND_WHILE;

      NEXT();
      match('(');

      this->u.cwhile.exp = expr(0);

      match(')');

      this->u.cwhile.comm=command();
      break;
    }

    case TK_RETURN: {
      this->tag = COMMAND_RET;

      NEXT();

      if (token != ';') {
        this->u.ret = expr(0);
      }
      
      match(';');

      break;
    }

    case TK_ID: {
      char *name;

      ALLOCS(name, strlen(yyval.sval) + 1);
      strcpy(name, yyval.sval);

      NEXT();

      if (token == '=') { /* Attr */
        this->tag = COMMAND_ATTR;
        ALLOC(this->u.attr.lvalue, Var);
        this->u.attr.lvalue->name = name;

        NEXT();

        this->u.attr.rvalue = expr(0);

        match(';');
      } else if (token == '(') {
        this->tag = COMMAND_FUNCALL;
        ALLOC(this->u.funcall, Exp);
        this->u.funcall->tag = EXP_FUNCALL;
        this->u.funcall->u.funcall.name = name;

        NEXT();

        match(')'); match(';');
      } else {
        printf("invalid command, funcall or attr");
      }

      break;
    }
    
    case '{': {
      this->tag = COMMAND_BLOCK;

      NEXT();

      if (token != '}') {
        ALLOC(this->u.block, Block);
        this->u.block->comms = commandl();
      }

      match('}');

      break;
    }
    
    case ';': {
      NEXT();
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
  while(token && token != '}') {
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
  NEXT();
  commands = commandl();
  print_commlist(0, commands);
}
