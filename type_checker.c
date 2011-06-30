#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "decl.h"
#include "symtab.h"

void check_prog();
void check_declrlist(SymTab *st, DeclrListNode* dln);
void check_declr(SymTab *st, Declr* d);
void check_paramlist(SymTab *st, DeclrListNode* pln, int line);
void check_block(SymTab *st, Block* b);
void check_commlist(SymTab *st, CommListNode* cln);
void check_command(SymTab *st, Command* c);
void check_var(SymTab *st, Var* v);
void check_exp(SymTab *st, Exp* e);

Type *return_type;

extern DeclrListNode *ast;

Type tint = { TK_TINT, 0, 0, NULL };
Type tstring = { TK_TCHAR, 0, 1, NULL};
Type tfloat = { TK_TFLOAT, 0, 0, NULL};


void check_array_empty(Type *t) {
  if(t->dimensions) {
    IntListNode *size = t->sizes;
    while(size) {
      if(size->n) print_error("array size cannot be specified here", t->line);
      size = size->next;
    }
  }
}

void check_array_sizes(Type *t) {
  if(t->dimensions) {
    IntListNode *size = t->sizes;
    while(size && size->n) {
      size = size->next;
    }
    while(size) {
      if(size->n) print_error("cannot declare an array inside an empty array",
      							t->line);
      size = size->next;
    }
  }
}

void check_type_int(Exp *e) {
  if(e->type->dimensions || e->type->type != TK_TINT)
    print_error("expression is not an integer", e->line);
}

void insert_conv(Exp **e, int type) {
  Exp *conv;
  if((*e)->tag == EXP_CONV) {
    (*e)->u.conv.type = type;
  } else {
    ALLOC(conv, Exp);
    conv->tag = EXP_CONV;
    conv->u.conv.type = type;
    conv->u.conv.exp = *e;
    if(type == TK_TINT)
      conv->type = &tint;
    else
      conv->type = &tfloat;
    *e = conv;
  }
}

void check_type_compatible(Type *type, Exp **right, int line) {

  if((type->dimensions != (*right)->type->dimensions)){
    print_error("expression is of an incompatible type", line);  
  } 	
  else if ((*right)->type->type == TK_TVOID){
 	print_error("expression cannot be of type void", line);
  }
  else {
    switch(type->type) {
      case TK_TCHAR: {
        if((*right)->type->type == TK_TINT)
	  	  insert_conv(right, TK_TCHAR);
        else if((*right)->type->type == TK_TFLOAT)
          print_error("cannot convert a float to a char", line);
        break;
      }
      case TK_TINT: {
        if((*right)->type->type == TK_TCHAR)
	  	  insert_conv(right, TK_TINT);
        else if((*right)->type->type == TK_TFLOAT)
          print_error("cannot convert a float to an int", line);
		break;
      }
      default: {
        if((*right)->type->type == TK_TINT || (*right)->type->type == TK_TCHAR)
          insert_conv(right, TK_TFLOAT);
      }    		
  	}
  }	
}

void check_prog(DeclrListNode* ast) {
  SymTab *global;
  global = symtab_new(NULL);
  check_declrlist(global, ast);
  symtab_free(global);
}

void check_declrlist(SymTab *st, DeclrListNode* dln) {
  while(dln) {
    check_declr(st, dln->declr);
    dln = dln->next;
  }
}

void check_declr(SymTab *st, Declr* d) {
  switch(d->tag) {
    case DECLR_VAR:
      {
        if(st->prev)
	  check_array_sizes(d->type);
        else
          check_array_empty(d->type);
	if(!symtab_add(st, d))
	  print_error("double declaration of variable", d->line);
	break;
      }
    case DECLR_FUNC:
      {
	check_array_empty(d->type);
	if(d->u.func.block) {
	  Declr *proto;
	  SymTab *param;
	  
	  proto = symtab_find(st, d->u.func.name);
	  if(!proto) {
	    symtab_add(st, d);
	    proto = d;
	  } else if(proto->u.func.block) {
	    print_error("double declaration of function", d->line);
	  } else {
	    proto->u.func.block = d->u.func.block;
	  }
	  return_type = d->type;
	  param = symtab_new(st);
	  check_paramlist(param, proto->u.func.params, d->line);
	  check_block(param, proto->u.func.block);
	  symtab_free(param);
	}
	else {
	  if(!symtab_add(st, d))
	    print_error("double declaration of function prototype", d->line);
	}
	break;
      }
    default: print_error("bug in compiler!", 0);
  }
}

void check_paramlist(SymTab *st, DeclrListNode* pln, int line) {
  while(pln) {
    if(pln->declr == NULL) {
      if(pln->next != NULL)
        print_error("... must be the last parameter of a function", line);
      break;
    } else {
      check_array_empty(pln->declr->type);
      if(!symtab_add(st, pln->declr)) print_error("parameter declared twice", line);
      pln = pln->next;
    }
  }
}

void check_block(SymTab *st, Block* b) {
  if(!b) return;
  SymTab *local_scope;
  local_scope = symtab_new(st);
  check_declrlist(local_scope, b->declrs);
  check_commlist(local_scope, b->comms);
  symtab_free(local_scope);
}

void check_commlist(SymTab *st, CommListNode* cln) {
  while(cln) {
    check_command(st, cln->comm);
    cln = cln->next;
  }
}

void check_command(SymTab *st, Command* c) {
  if(!c) return;
  switch(c->tag) {
    case COMMAND_IF:
      {
	check_exp(st, c->u.cif.exp);
	check_type_int(c->u.cif.exp);
	check_command(st, c->u.cif.comm);
	check_command(st, c->u.cif.celse);
	break;
      }
    case COMMAND_WHILE:
      {
	check_exp(st, c->u.cwhile.exp);
	check_type_int(c->u.cwhile.exp);
	check_command(st, c->u.cwhile.comm);
        break;
      }
    case COMMAND_ATTR:
      {
	check_var(st, c->u.attr.lvalue);
	check_exp(st, c->u.attr.rvalue);
	check_type_compatible(c->u.attr.lvalue->type, &(c->u.attr.rvalue), c->line);
	break;
      }
    case COMMAND_RET:
      {
	if(c->u.ret) {
          check_exp(st, c->u.ret);
          check_type_compatible(return_type, &(c->u.ret), c->line);
	}
	break;
      }
    case COMMAND_FUNCALL:
      {
	check_exp(st, c->u.funcall);
	break;
      }
    case COMMAND_BLOCK:
      {
	check_block(st, c->u.block);
	break;
      }
  }
}

void check_var(SymTab *st, Var* v) {
  Declr *d;
  int dim;
  ExpListNode *eln;
  d = symtab_find(st, v->name);
  if(!d) print_error("variable is not declared" , v->line);
  if(d->tag == DECLR_FUNC) print_error("illegal use of a function", v->line);
  v->var = d;
  dim = d->type->dimensions;
  eln = v->idxs;
  while(eln) {
    if(!dim) print_error("tried to index a value that is not an array", v->line);
    check_exp(st, eln->exp);
    check_type_int(eln->exp);
    dim--;
    eln = eln->next;
  }
  ALLOC(v->type, Type);
  v->type->type = d->type->type;
  v->type->dimensions = dim;
  v->type->sizes = NULL;
}

void check_type_relational(Exp *e) {
  Type *t1, *t2;
  t1 = (e->u.binop.e1)->type;
  t2 = (e->u.binop.e2)->type;
  if(t1->dimensions != 0 || t2->dimensions !=0)
    print_error("cannot compare an array", e->line);
  if(t1->type == TK_TVOID || t2->type == TK_TVOID)
    print_error("cannot compare a void value", e->line);
  if(t1->type == TK_TINT && t2->type == TK_TFLOAT)
    insert_conv(&(e->u.binop.e1), TK_FLOAT);
  else if(t1->type == TK_TFLOAT && t2->type == TK_TINT)
    insert_conv(&(e->u.binop.e2), TK_FLOAT);
  e->type = &tint;
}

void check_type_arith(Exp *e) {
  Type *t1, *t2;
  t1 = (e->u.binop.e1)->type;
  t2 = (e->u.binop.e2)->type;
  if(t1->dimensions != 0 || t2->dimensions !=0)
    print_error("cannot use an array in expression", e->line);
  if(t1->type == TK_TVOID || t2->type == TK_TVOID)
    print_error("cannot use a void value in expression", e->line);
  if(t1->type == TK_TINT && t2->type == TK_TFLOAT)
    insert_conv(&(e->u.binop.e1), TK_TFLOAT);
  else if(t1->type == TK_TFLOAT && t2->type == TK_TINT)
    insert_conv(&(e->u.binop.e2), TK_TFLOAT);
  e->type = (e->u.binop.e1)->type;
}

void check_exp(SymTab *st, Exp* e) {
  switch(e->tag) {
    case EXP_INT:
      e->type = &tint; 
      break;
    case EXP_FLOAT:
      e->type = &tfloat;
      break;
    case EXP_STRING:
      e->type = &tstring;
      break;
    case EXP_VAR:
      check_var(st, e->u.var);
      e->type = (e->u.var)->type;
      break;
    case EXP_BINOP:
      {
	check_exp(st, e->u.binop.e1);
	if((e->u.binop.e1)->type->type == TK_TCHAR)
          insert_conv(&(e->u.binop.e1), TK_TINT);
	check_exp(st, e->u.binop.e2);
	if((e->u.binop.e2)->type->type == TK_TCHAR)
          insert_conv(&(e->u.binop.e2), TK_TINT);
	switch(e->u.binop.op) {
	  case TK_EQ:
	  case TK_LEQ:
	  case TK_GEQ:
	  case TK_NEQ:
          case '<':
	  case '>': {
	    check_type_relational(e);
	    break;
	  }
	  case TK_AND:
	  case TK_OR: {
            check_type_int(e->u.binop.e1);
	    check_type_int(e->u.binop.e2);
	    e->type = &tint;
	    break;
	  }
	  case '+':
	  case '-':
	  case '*':
	  case '/': {
	    check_type_arith(e);
	    break;
	  }  
          default: print_error("Bug in the compiler!", e->line);
	}
        break;
      }
    case EXP_NEG:
      {
	check_exp(st, e->u.exp);
	if((e->u.exp)->type->type == TK_TCHAR)
          insert_conv(&(e->u.exp), TK_TINT);
        if((e->u.exp)->type->dimensions != 0)
          print_error("cannot negate an array", e->line);
	e->type = (e->u.exp)->type;
	break;
      }
    case EXP_LNEG:
      {
	check_exp(st, e->u.exp);
	check_type_int(e->u.exp);
	e->type = &tint;
	break;
      }
     case EXP_FUNCALL:
      {
	Declr *func;
	ExpListNode *eln;
	DeclrListNode *pln;
	func = symtab_find(st, e->u.funcall.name);
	if(!func) print_error("calling undeclared function", e->line);
        eln = e->u.funcall.expl;
	pln = func->u.func.params;
	while(eln) {
	  if(!pln) print_error("excess arguments in function call", e->line);
          check_exp(st, eln->exp);
	  if(pln->declr != NULL) {
  	    check_type_compatible(pln->declr->type, &(eln->exp), e->line);
	    pln = pln->next;
	  }
	  eln = eln->next;
	}
	if(pln && pln->declr) print_error("missing arguments in function call", e->line);
	e->u.funcall.func = func;
	e->type = func->type;
	break;
      }
  }
}
