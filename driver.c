#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decl.h"
#include "ast.h"

#define NEXT() token = yylex();
#define NO_BINOP -1
#define NO_UNOP 0

extern int yylineno;
extern FILE *outfile;
extern char *filename;

static int token;
static CommListNode *commandl();
static Exp* simple();

static void match(int next) {
  if (token != next) {
    printf("expected '%c' but was '%c'\n", next, token);
    exit(0);
  }

  NEXT();
}

static struct { int left; int right; } 
pri[] = {
  { 3, 3 }, /* + - */
  { 4, 4 }, /* * / */
  { 2, 2 }, /* == < > */
  { 5, 4 } /* ^ */
};

static int binop(int token) {
  switch (token) {
    case '+' : 
    case '-' : 
      return 0;
    case '*' :
    case '/' :
      return 1;
    case '^' : return 3;
    case '<' :
    case '>' : 
      return 2;
    case TK_EQ : return 2;
    default : return NO_BINOP;
  }
}

static int unop(int token) {
  switch (token) {
    case '-' : return 4;
    case '!' : return 2;
    default : return NO_UNOP;
  }
}

static Exp* expr(int level) {
  Exp *exp1 = simple();

  int op = binop(token);

  while (op != NO_BINOP && pri[op].left > level) {
    Exp *new, *exp2;

    int token_op = token;

    NEXT();
    exp2 = expr(pri[op].right);
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

static ExpListNode* funcall_params() {
  ExpListNode *root, *node, *prev;
          
  ALLOC(root, ExpListNode);
  root->exp = expr(0);
  root->next = NULL;

  prev = root;

  while (token == ',') {
    NEXT();
  
    ALLOC(node, ExpListNode);
    prev->next = node;

    node->exp = expr(0);

    prev = node;
  }

  return root;
}

static Exp* simple() {
  Exp *exp;

  switch (token) {
    case TK_ID: {
      char *name;

      ALLOC(exp, Exp);
      ALLOCS(name, strlen(yyval.sval) + 1);

      strcpy(name, yyval.sval);

      NEXT();

      // chamada de funcao
      if (token == '(') {
        NEXT();

        exp->tag = EXP_FUNCALL;
        exp->u.funcall.name = name;

        // funcao sem parametro
        if (token == ')') {
          exp->u.funcall.expl = NULL;
        } else {
          exp->u.funcall.expl = funcall_params();
        }

        match(')');

      } else {
        Var *var;
        ALLOC(var, Var);

        var->name = name;

        exp->tag = EXP_VAR;
        exp->u.var = var;
      }

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
    case '-': {
      NEXT();

      ALLOC(exp, Exp);

      exp->tag = EXP_NEG;
      exp->u.exp = expr(unop('-'));

      break;
    }
    case '!': {
      NEXT();

      ALLOC(exp, Exp);

      exp->tag = EXP_LNEG;
      exp->u.exp = expr(unop('!'));

      break;
    }
    default:
      printf("Expressao invalida %d\n", token);
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
