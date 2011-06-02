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

class BasicBlock {
  string name;
  vector<Command*> ops;
  vector<BasicBlock*> succs;
  vector<BasicBlock*> preds;
  // phis
  // vars

  public: 
    BasicBlock() : name(),
                   ops(),
                   succs(),
                   preds() { };

    void set_name(string param_name) {
      name = param_name;
    }
};

class CFG {
  string name;
  vector<BasicBlock*> blocks;
  int counter;

  public:
    CFG(string param_name) : name(param_name), blocks(), counter(0) { };

    void add_block(BasicBlock* block) {
      counter++;
      block->set_name("B" + counter);
      blocks.push_back(block);
    }
};

void iterate_declrs(DeclrListNode* node) {

  while (node) {
    Declr* declr = node->declr;

    if (declr->tag == DECLR_FUNC && declr->u.func.block) {
      cout << "CFG para " << declr->u.func.name << endl;
      generate_cfg_func(declr);
    }

    node = node->next;
  }
}

void generate_cfg_func(Declr* declr) {
  Block* block = declr->u.func.block;

  if (block) {
    CFG* cfg = new CFG(declr->u.func.name);

    CommListNode *comm_node = block->comms;
    if (comm_node) {
      
    }
  }
}

void generate_cfg_comms(CommListNode* node) {
  
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
