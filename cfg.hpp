#ifndef _CFG_HPP

#define _CFG_HPP

extern "C" {
  #include "ast.h"
}

void iterate_declrs(DeclrListNode* node);
void generate_cfg_func(Declr* declr);

#endif
