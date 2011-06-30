#ifndef _AST_H
#define _AST_H

#define DECLR_VAR 	1000
#define DECLR_FUNC	1001

#define COMMAND_IF	2000
#define COMMAND_WHILE	2001
#define COMMAND_ATTR	2002
#define COMMAND_RET	2003
#define COMMAND_FUNCALL	2004
#define COMMAND_BLOCK	2005

#define EXP_INT		3000
#define EXP_FLOAT	3001
#define EXP_STRING	3002
#define EXP_VAR		3003
#define EXP_BINOP	3004
#define EXP_NEG		3005
#define EXP_LNEG	3006
#define EXP_FUNCALL	3007
#define EXP_CONV	3008

#define ALLOC(p, t)	p = (t*)malloc(sizeof(t)); if(!p) { printf("Panic! Out of memory! Exiting...\n"); exit(0); }
#define ALLOCS(p, n)	p = (char*)malloc(n * sizeof(char)); if(!p) { printf("Panic! Out of memory! Exiting...\n"); exit(0); }

typedef struct _Type Type;
typedef struct _IntListNode IntListNode;
typedef struct _Block Block;
typedef struct _Declr Declr;
typedef struct _StrListNode StrListNode;
typedef struct _DeclrListNode DeclrListNode;
typedef struct _CommListNode CommListNode;
typedef struct _ExpListNode ExpListNode;
typedef struct _Command Command;
typedef struct _Exp Exp;
typedef struct _Var Var;

struct _Declr {
  int tag;
  int line;
  Type* type;
  union {
    char* name;
    struct {
      char* name;
      DeclrListNode* params;
      Block* block;
    } func;    
  } u;
};

struct _StrListNode {
  StrListNode* next;
  char* name;
};

struct _Type {
  int type;
  int line;
  int dimensions;
  IntListNode* sizes;
};

struct _IntListNode {
  IntListNode* next;
  int n;
};

struct _Block {
  DeclrListNode* declrs;
  CommListNode* comms;
};

struct _DeclrListNode {
  DeclrListNode* next;
  Declr* declr;
};

struct _CommListNode {
  CommListNode* next;
  Command* comm;
};

struct _Command {
  int tag;
  int line;
  union {
    struct {
      Exp* exp;
      Command* comm;
      Command* celse;
    } cif;
    struct {
      Exp* exp;
      Command* comm;
    } cwhile;
    struct {
      Var* lvalue;
      Exp* rvalue;
    } attr;
    Exp* ret;
    Exp* funcall;
    Block* block;
  } u;
};

struct _Var {
  char* name;
  int line;
  ExpListNode* idxs;
  Declr *var;
  Type *type;
};

struct _ExpListNode {
  ExpListNode* next;
  Exp* exp;
};

struct _Exp {
  int tag;
  int line;
  Type *type;
  union {
    int ival;
    double fval;
    char* sval;
    Var* var;
    Exp* exp;
    struct {
      int op;
      Exp* e1;
      Exp* e2;
    } binop;
    struct {
      char* name;
      ExpListNode* expl;
      Declr *func;
    } funcall;
    struct {
      int type;
      Exp *exp;
    } conv;
  } u;
};

void print_declrlist(int ident, DeclrListNode* dln);
void print_declr(int ident, Declr* d);
void print_type(Type* t);
void print_strlist(StrListNode* sln);
void print_paramlist(DeclrListNode* pln);
void print_block(int ident, Block* b);
void print_param(Declr* p);
void print_commlist(int ident, CommListNode* cln);
void print_command(int ident, Command* c);
void print_var(Var* v);
void print_exp(Exp* e);
void print_explist(ExpListNode* eln);

void print_error(const char *msg, int line);

#endif
