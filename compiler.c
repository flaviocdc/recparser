#include <stdio.h>
#include <stdlib.h>
#include "decl.h"
#include "driver.h"

extern int yylineno;
extern FILE *outfile;
extern char *filename;
extern int token;

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
  NEXT();

  declrs = declr_list(NOT_INSIDE_FUNC);
  print_declrlist(0, declrs);
}
