#include<vector>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<iostream>

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
        CFG_Attr* = create_cfg_attr(cmd);
      }
      case COMMAND_FUNCALL:
      case COMMAND_RET: {
                          block->add_op(cmd);
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

CFG_Var* create_cfg_attr(Command* cmd) {
  
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
