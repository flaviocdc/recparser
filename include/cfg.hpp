#ifndef _CFG_HPP

#define _CFG_HPP

extern "C" {
  #include "ast.h"
}

#include<vector>
#include<string>
#include<sstream>
#include "cfg_data.hpp"

using namespace std;

void iterate_declrs(DeclrListNode* node);
CFG* generate_cfg(Declr* declr);
void generate_cfg_comms(CFG* cfg, CommListNode* node);
string new_temp_var();

CFG_Attr* create_cfg_attr(CFG* cfg, Command* cmd);
CFG_Return* create_cfg_return(CFG* cfg, Command* cmd);

CFG_Exp* create_cfg_exp(CFG* cfg, Exp* ast_exp);
CFG_Attr* create_temp_cfg_attr(BasicBlock* block, CFG_Exp* exp);

CFG_Var* create_short_circuit_and(CFG* cfg, Exp* ast_exp);

#endif
