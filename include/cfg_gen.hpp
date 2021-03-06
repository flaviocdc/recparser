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
void print_globals();
void print_cfgs();

CFG* generate_cfg(Declr* declr);
string new_temp_var();

void generate_cfg_comms(CFG* cfg, Block* block);
void generate_cfg_comm(CFG* cfg, Command* cmd);

CFG_Attr* create_cfg_attr(CFG* cfg, Command* cmd);
void create_cfg_if(CFG* cfg, Command* cmd);
void create_cfg_while(CFG* cfg, Command* cmd);
void create_cfg_funcall(CFG* cfg, Command* cmd);

CFG_Exp* create_cfg_exp(CFG* cfg, Exp* ast_exp);
CFG_Attr* create_temp_cfg_attr(CFG* cfg, CFG_Exp* exp);

CFG_Var* create_short_circuit_and(CFG* cfg, Exp* ast_exp);
CFG_Var* create_short_circuit_or(CFG* cfg, Exp* ast_exp);

BasicBlock *create_basic_block(CFG* cfg);
BasicBlock *create_basic_working_block(CFG* cfg);
void set_working_block(CFG* cfg, BasicBlock* block);

#endif
