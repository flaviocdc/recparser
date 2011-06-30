#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decl.h"
#include "ast.h"
#include "driver.h"

#define EXTRACT_NAME(var) ALLOCS(var, strlen(yyval.sval) + 1);  strcpy(var, yyval.sval);
#define SYNTAX_ERROR(format, args...) printf(format , ## args); exit(1);
#define LAST_NODE(node) while(node->next) node = node->next;

#define NO_BINOP -1
#define NO_UNOP 0

extern int yylineno;
extern FILE *outfile;
extern char *filename;

int token;

static CommListNode *commandl();
static Exp* simple();
static Block *block();
static int is_type(int token);

Type tvoid = { TK_TVOID, 0, 0, NULL };

static void match(int next) {
  if (token != next) {
  
    if (next >= 0 && next <= 255) {
      SYNTAX_ERROR("expected '%c' but was '%c' on line %d\n", next, token, yylineno);
    } else {
      SYNTAX_ERROR("expected '%d' but was '%d' on line %d\n", next, token, yylineno);
    }

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
    Exp *new_exp, *exp2;

    int token_op = token;

    NEXT();
    exp2 = expr(pri[op].right);
    op = binop(token);

    ALLOC(new_exp, Exp);
    new_exp->tag = EXP_BINOP;
    new_exp->u.binop.op = token_op;
    new_exp->u.binop.e1 = exp1;
    new_exp->u.binop.e2 = exp2;

    exp1 = new_exp;
  }

  return exp1;
}

static Var* create_var(char* name, ExpListNode* list) {
  Var *var;
  ALLOC(var, Var);
  
  var->name = name;
  var->idxs = list;
  
  return var;
}

static ExpListNode* parse_exps(char separator, char match_next) {
  ExpListNode *head, *node, *prev;
          
  ALLOC(head, ExpListNode);
  head->exp = expr(0);
  head->next = NULL;

  prev = head;
  
  if (match_next != '\0') {
    match(match_next);
  }

  while (token == separator) {
    NEXT();
  
    ALLOC(node, ExpListNode);
    node->exp = expr(0);
    node->next = NULL;
    
    prev->next = node;

    prev = node;
    
    if (match_next != '\0') {
      match(match_next);
    }
  }

  return head;
}

static ExpListNode* funcall_params() {
  return parse_exps(',', '\0');
}

static ExpListNode* parse_arrays() {
  return parse_exps('[', ']');
}

static Exp* simple() {
  Exp *exp;

  switch (token) {
    case TK_ID: {
      char *name;

      ALLOC(exp, Exp);
      EXTRACT_NAME(name);

      NEXT();

      // chamada de funcao
      switch (token) {
        case '(': {
          NEXT();

          exp->tag = EXP_FUNCALL;
          exp->u.funcall.name = name;

          if (token == ')') {
            // funcao sem parametro
            exp->u.funcall.expl = NULL;
          } else {
            // funcao com parametros
            exp->u.funcall.expl = funcall_params();
          }

          match(')');
          break;
        }
        case '[': {
          NEXT();
        
          exp->tag = EXP_VAR;
          exp->u.var = create_var(name, parse_arrays());
          break;
        }
        default: {
          exp->tag = EXP_VAR;        
          exp->u.var = create_var(name, NULL);
        }
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
    case TK_STRING: {
      NEXT();

      ALLOC(exp, Exp);

      exp->tag = EXP_STRING;
      exp->u.sval = yyval.sval;
      
      break;
    }
    default:
      SYNTAX_ERROR("Invalid expression %d\n", token);
      break;
  }

  return exp;
}

static Command *command() {
  Command *cmd;
  ALLOC(cmd, Command);

  switch (token) {
    case TK_IF: {
      cmd->tag = COMMAND_IF;
      
      NEXT();
      match('(');

      cmd->u.cif.exp = expr(0);

      match(')');

      cmd->u.cif.comm = command();

      if (token == TK_ELSE) {
        NEXT();
        cmd->u.cif.celse = command();
      }

      break;
    }

    case TK_WHILE: {
      cmd->tag = COMMAND_WHILE;

      NEXT();
      match('(');

      cmd->u.cwhile.exp = expr(0);

      match(')');

      cmd->u.cwhile.comm=command();
      break;
    }

    case TK_RETURN: {
      cmd->tag = COMMAND_RET;

      NEXT();

      if (token != ';') {
        cmd->u.ret = expr(0);
      }
      
      match(';');

      break;
    }

    case TK_ID: {
      char *name;

      EXTRACT_NAME(name);

      NEXT();
      
      ExpListNode* idxs = NULL;
      if (token == '[') {
        NEXT();
        
        idxs = parse_arrays();
        
        if (token != '=') {
          match('=');
        }
      }

      if (token == '=') { /* Attr */
        cmd->tag = COMMAND_ATTR;
        cmd->u.attr.lvalue = create_var(name, NULL);
        
        if (idxs)
          cmd->u.attr.lvalue->idxs = idxs;

        NEXT();
        
        cmd->u.attr.rvalue = expr(0);

        match(';');
      } else if (token == '(') {
        cmd->tag = COMMAND_FUNCALL;
        ALLOC(cmd->u.funcall, Exp);
        cmd->u.funcall->tag = EXP_FUNCALL;
        cmd->u.funcall->u.funcall.name = name;

        NEXT();
        if (token != ')') {
          cmd->u.funcall->u.funcall.expl = funcall_params();
        }

        match(')'); match(';');
      } else {
        printf("invalid command, funcall or attr");
      }

      break;
    }
    
    case '{': {
      cmd->tag = COMMAND_BLOCK;

      NEXT();

      if (token != '}') {
        cmd->u.block = block();
      }

      match('}');

      break;
    }
    
    case ';': {
      NEXT();
      cmd->tag = COMMAND_BLOCK;
      break;
    }

    default: {
      SYNTAX_ERROR("invalid command in line %i\n", yylineno);
    }

  }  

  return cmd;
}

static Type* parse_type() {
  Type* type;

  ALLOC(type, Type);
  
  switch (token) {
    case TK_TVOID:
    case TK_TINT:    
    case TK_TCHAR: {
      type->dimensions = 0;
      type->type = token;
      type->sizes = NULL;
      break; 
    }
    default:
      SYNTAX_ERROR("Declaration error (invalid type)\n");
  }

  NEXT();

  IntListNode *curr;

  if (token == '[') {
    
    while (token == '[') {
      NEXT();

      type->dimensions = type->dimensions + 1;

      ALLOC(curr, IntListNode);
      curr->next = NULL;

      if (token == TK_INT) {
        curr->n = yyval.ival;
        NEXT();
      } else {
        curr->n = 0;
      }

      match(']');
      if (type->sizes) {
        IntListNode* last = type->sizes;

        LAST_NODE(last);

        last->next = curr;
      } else {
        type->sizes = curr;
      }
    }

  }

  return type;
}

static Block *block() {
  Block *block;
  ALLOC(block, Block);

  block->declrs = NULL;
  
  if (token != '}') {
    block->declrs = declr_list(INSIDE_FUNC);
    block->comms = commandl();
  } else {
    block->comms = NULL;
  }

  return block;
}

static Declr *declr(DeclrListNode *head, int inside_func) {
  Type *type;
  type = parse_type();

  char* name;
  Declr* declr;

  head->next = NULL;

  if (token != TK_ID) {
    SYNTAX_ERROR("Expected TK_ID but found: %d\n", token);
  }

  EXTRACT_NAME(name);
  NEXT();
  ALLOC(declr, Declr);

  switch (token) {
    case '(': {
      
      if (inside_func) {
        SYNTAX_ERROR("Nesting function declaration is not allowed\n");
      }

      declr->tag = DECLR_FUNC;
      declr->u.func.name = name;

      NEXT();

      if (is_type(token)) { // parametro de funcao
        DeclrListNode *hparm, *cparm, *iparm;
        Declr *declr_parm;
        Type *type_parm;
        
        ALLOC(hparm, DeclrListNode);
        ALLOC(declr_parm, Declr);

        type_parm = parse_type();
        
        EXTRACT_NAME(name);

        declr_parm->type = type_parm;
        declr_parm->u.name = name;

        hparm->declr = declr_parm;
        hparm->next = NULL;

        NEXT();

        iparm = hparm;
        while (token == ',') { 
          NEXT();
            
          if (token == TK_MANY) {
            ALLOC(iparm->next, DeclrListNode);

            iparm->next->declr = NULL;
            iparm->next->next = NULL;

            NEXT();

            break;
          }

          if (!is_type(token)) {
            SYNTAX_ERROR("expected a type but found %d line %d", token, yylineno);
          }

          type_parm = parse_type();
          NEXT();
          EXTRACT_NAME(name);
          
          ALLOC(cparm, DeclrListNode);
          ALLOC(declr_parm, Declr);

          declr_parm->type = type;
          declr_parm->u.name = name;

          cparm->declr = declr_parm;
          cparm->next = NULL;
          iparm->next = cparm;
          iparm = cparm;
        }

        declr->u.func.params = hparm;
      }
      else if (token == TK_MANY) {
        // se for varargs, precisa ser o ultimo
        NEXT();
        //match(')');

        ALLOC(declr->u.func.params, DeclrListNode);
      }
      else {
        declr->u.func.params = NULL;
      }

      match(')');

      if (token == '{') {
        match('{');
        declr->u.func.block = block();
        match('}');
      } else if (token == ';') {
        declr->u.func.block = NULL;
        match(';');
      } else { 
        SYNTAX_ERROR("Invalid function declaration: %s\n", name);
      }

      break;
    }
    case ';': {
      declr->tag = DECLR_VAR;
      declr->u.name = name;

      NEXT();

      break;
    }
    case ',': {
      declr->tag = DECLR_VAR;
      declr->u.name = name;
      
      NEXT();
      
      if (token != TK_ID) {
        SYNTAX_ERROR("Was expecting TK_ID but found: %d\n", token);
      }

      DeclrListNode *new_node, *it;
      Declr *new_declr;

      it = head;
      LAST_NODE(it);

      while (1) {
        ALLOC(new_node, DeclrListNode);
        ALLOC(new_declr, Declr);

        EXTRACT_NAME(name);
        new_declr->u.name = name;
        new_declr->tag = DECLR_VAR;
        new_declr->type = type;

        new_node->declr = new_declr;
        new_node->next = NULL;

        it->next = new_node;

        NEXT();
        
        if (token == ';') break;
        if (token == ',') {
          it = new_node; 
          NEXT();
        }
      }
      
      NEXT();
      break;
    }
    default: {
      SYNTAX_ERROR("Invalid declaration.\n");
    }
  }

  declr->type = type;

  return declr;
}

static int is_type(int token) {
  return token == TK_TINT || token == TK_TCHAR || token == TK_TVOID;
}

DeclrListNode *declr_list(int inside_function) {
  DeclrListNode *first, *curr;
  if (token && is_type(token)) {
    ALLOC(first, DeclrListNode);
    
    first->declr = declr(first, inside_function);
  } else if (inside_function) {
    // funcoes podem nao ter variaveis declaradas dentro dela
    return NULL;
  }
  
  curr = first;
  LAST_NODE(curr);
  
  while (token && is_type(token)) {
    DeclrListNode *new_node;
    ALLOC(new_node, DeclrListNode);

    new_node->declr = declr(new_node, inside_function);

    curr->next = new_node;
    LAST_NODE(new_node);
    curr = new_node;
  }

  return first;
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
