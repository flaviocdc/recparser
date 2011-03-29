#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decl.h"
#include "ast.h"

FILE* outfile;
char *filename;

void print_error(const char *msg, int line) {
  fprintf(stderr, "%s(%i): %s\n", filename, line, msg);
  exit(0);
}

void print_ident(int ident) {
  int i;
  for(i = 0; i < ident; i++) fprintf(outfile, " ");
}

void print_string(char* s) {
  int i;
  fprintf(outfile, "\"");
  for(i = 0; i < strlen(s); i++)
    switch(s[i]) {
      case '\"': fprintf(outfile, "\\\""); break;
      case '\\': fprintf(outfile, "\\\\"); break;
      case '\t': fprintf(outfile, "\\t"); break;
      case '\n': fprintf(outfile, "\\n"); break;
      default: fprintf(outfile, "%c", s[i]);
    }
  fprintf(outfile, "\"");
}

void print_declrlist(int ident, DeclrListNode* dln)
{
  if(dln) {
    print_declr(ident, dln->declr);
    fprintf(outfile, "\n");
    print_declrlist(ident, dln->next);
  }
}

void print_declr(int ident, Declr* d) {
  print_ident(ident);
  print_type(d->type);
  switch(d->tag) {
    case DECLR_VAR:
      {
	fprintf(outfile, " ");
	fprintf(outfile, "%s",d->u.name);
	fprintf(outfile, ";");
        break;
      }
    case DECLR_FUNC:
      {
	fprintf(outfile, " %s(", d->u.func.name);
	print_paramlist(d->u.func.params);
	fprintf(outfile, ")");
	if(d->u.func.block) {
	  fprintf(outfile, "\n");
	  print_block(ident, d->u.func.block);
	} else fprintf(outfile, ";");
        break;
      }
  }
}

void print_type(Type* t) {
  switch(t->type) {
    case TK_TVOID: fprintf(outfile, "void"); break;
    case TK_TINT: fprintf(outfile, "int"); break;
    case TK_TFLOAT: fprintf(outfile, "float"); break;
    case TK_TCHAR: fprintf(outfile, "char"); break;
  }
  if(t->dimensions) {
    IntListNode* iln = t->sizes;
    while(iln) {
      if(iln->n)    
        fprintf(outfile, "[%i]", iln->n);
      else
	fprintf(outfile, "[]");
      iln = iln->next;
    }
  }
}

void print_strlist(StrListNode* sln) {
  fputs(sln->name, outfile);
  if(sln->next) {
    fprintf(outfile, ", ");
    print_strlist(sln->next);
  }
}

void print_paramlist(DeclrListNode* pln) {
  if(!pln) return;
  print_param(pln->declr);
  if(pln->next) {
    fprintf(outfile, ", ");
    print_paramlist(pln->next);
  }
}

void print_block(int ident, Block* b) {
  if(!b) { fprintf(outfile, ";\n"); return; }
  print_ident(ident);
  fprintf(outfile, "{\n");
  print_declrlist(ident + 2, b->declrs);
  print_commlist(ident + 2, b->comms);
  print_ident(ident);
  fprintf(outfile, "}\n");
}

void print_param(Declr* p) {
  if(!p)
    fprintf(outfile, "...");
  else {
    print_type(p->type);
    fprintf(outfile, " %s", p->u.name);
  }
}

void print_commlist(int ident, CommListNode* cln) {
  if(cln) {
    print_command(ident, cln->comm);
    /*fprintf(outfile, "\n");*/
    print_commlist(ident, cln->next);
   }
}

void print_command(int ident, Command* c) {
  print_ident(ident);
  switch(c->tag) {
    case COMMAND_IF:
      {
	fprintf(outfile, "if(");
	print_exp(c->u.cif.exp);
	fprintf(outfile, ") ");
	print_command(ident, c->u.cif.comm);
	if(c->u.cif.celse) {
  	  fprintf(outfile, "else ");
	  print_command(ident, c->u.cif.celse);
	}
	break;
      }
    case COMMAND_WHILE:
      {
	fprintf(outfile, "while(");
	print_exp(c->u.cwhile.exp);
	fprintf(outfile, ") ");
	print_command(ident, c->u.cwhile.comm);
	break;
      }
    case COMMAND_ATTR:
      {
	print_var(c->u.attr.lvalue);
	fprintf(outfile, " = ");
	print_exp(c->u.attr.rvalue);
	fprintf(outfile, ";\n");
	break;
      }
    case COMMAND_RET:
      {
	fprintf(outfile, "return ");
	if(c->u.ret)
          print_exp(c->u.ret);
	fprintf(outfile, ";\n");
	break;
      }
    case COMMAND_FUNCALL:
      {
	print_exp(c->u.funcall);
	fprintf(outfile, ";\n");
	break;
      }
    case COMMAND_BLOCK:
      {
	print_block(ident, c->u.block);
	break;
      }
  }
}

void print_var(Var* v) {
  fprintf(outfile, "%s", v->name);
  if(v->idxs) {
    ExpListNode* idx = v->idxs;
    while(idx) {
      fprintf(outfile, "[");
      print_exp(idx->exp);
      fprintf(outfile, "]");
      idx = idx->next;
    }
  }
}

void print_exp(Exp* e) {
  switch(e->tag) {
    case EXP_INT:
      fprintf(outfile, "%i", e->u.ival);
      break;
    case EXP_FLOAT:
      fprintf(outfile, "%f", e->u.fval);
      break;
    case EXP_STRING:
      print_string(e->u.sval);
      break;
    case EXP_VAR:
      print_var(e->u.var);
      break;
    case EXP_BINOP:
      {
	fprintf(outfile, "(");
	print_exp(e->u.binop.e1);
	switch(e->u.binop.op) {
	  case TK_EQ: fprintf(outfile, "=="); break;
	  case TK_LEQ: fprintf(outfile, "<="); break;
	  case TK_GEQ: fprintf(outfile, ">="); break;
	  case TK_NEQ: fprintf(outfile, "!="); break;
	  case TK_AND: fprintf(outfile, "&&"); break;
	  case TK_OR: fprintf(outfile, "||"); break;
          default: fprintf(outfile, "%c", e->u.binop.op);
	}
	print_exp(e->u.binop.e2);
	fprintf(outfile, ")");
        break;
      }
    case EXP_NEG:
      {
	fprintf(outfile, "-");
	print_exp(e->u.exp);
	break;
      }
    case EXP_LNEG:
      {
	fprintf(outfile, "!");
	print_exp(e->u.exp);
	break;
      }
     case EXP_FUNCALL:
      {
	fprintf(outfile, "%s(", e->u.funcall.name);
	print_explist(e->u.funcall.expl);
	fprintf(outfile, ")");
      }
  }
}

void print_explist(ExpListNode* eln) {
  if(!eln) return;
  print_exp(eln->exp);
  if(eln->next) {
    fprintf(outfile, ", ");
    print_explist(eln->next);
  }  
}
