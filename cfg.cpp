#include<vector>
#include<string>
#include<cstdio>
#include<cstdlib>

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
  /*
  self.name = name
  self.phis = {}
  self.ops = []
  self.succs = []
  self.preds = []
  self.vars = set()
  */
  string name;
  vector<Command> ops;
  vector<BasicBlock> succs;
  vector<BasicBlock> preds;

  public: 
    BasicBlock(string param_name) : name(param_name),
                                    ops(),
                                    succs(),
                                    preds() { };


};

void generate_cfg_func(Declr* declr) {
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
  //print_declrlist(0, declrs);
}
