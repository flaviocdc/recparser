#include<vector>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<iostream>

#include "cfg_data.hpp"
#include "cfg.hpp"

extern "C" {
  #include "decl.h"
  #include "ast.h"
  #include "driver.h"

  int yylex (void);
  void yyrestart (FILE *input_file);
}

extern int yylineno;
extern FILE *outfile;
extern char *filename;
extern int token;

using namespace std;

void iterate_declrs(DeclrListNode* node) {

  while (node) {
    Declr* declr = node->declr;

    if (declr->tag == DECLR_FUNC && declr->u.func.block) {
      CFG *cfg = generate_cfg(declr);

      cout << *cfg << endl;
    }

    node = node->next;
  }
}

CFG* generate_cfg(Declr* declr) {
  Block* block = declr->u.func.block;

  if (block) {
    CFG* cfg = new CFG(declr->u.func.name);

    CommListNode *comm_node = block->comms;
    if (comm_node) {
      generate_cfg_comms(cfg, comm_node);
    }

    return cfg;
  }

  return NULL;
}

void generate_cfg_comms(CFG* cfg, CommListNode* node) {
  BasicBlock* block = new BasicBlock;
  cfg->add_block(block);

  while(node) {
    Command* cmd = node->comm;
    switch(cmd->tag) {
      case COMMAND_ATTR: {
        CFG_Attr* cfg_attr = create_cfg_attr(block, cmd);
      }
      case COMMAND_FUNCALL:
      case COMMAND_RET: {
        //block->add_op(cmd);
        break;
      }
      default: {
       cout << "Caso nao tratado no CFG: " << cmd->tag << endl;
       break;
     }
    }

    node = node->next;
  }
}

CFG_Attr* create_cfg_attr(BasicBlock *block, Command* cmd) {
  Var* ast_var = cmd->u.attr.lvalue;
  Exp* ast_exp = cmd->u.attr.rvalue;

  CFG_Var* cfg_var = new CFG_Var(ast_var->name);
  CFG_Exp* cfg_exp = create_cfg_exp(block, ast_exp);
  
  CFG_Attr* cfg_attr = new CFG_Attr(cfg_var, cfg_exp);
  block->ops.push_back(cfg_attr);
  
  return cfg_attr;
}

CFG_Exp* create_cfg_exp(BasicBlock *block, Exp* ast_exp) {
  CFG_Exp* cfg_exp;

  switch (ast_exp->tag) {
    case EXP_INT: {
      CFG_Literal<int>* literal = new CFG_Literal<int>(ast_exp->u.ival);
      cfg_exp = new CFG_SimpleOp(literal);
      break;
    }

  }
}

int main(int argc, char **argv) {
  FILE *f;
  DeclrListNode *declrs;

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

  declrs = declr_list(0);

  iterate_declrs(declrs);
  //print_declrlist(0, declrs);
}
