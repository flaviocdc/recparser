#ifndef _SSA_HPP
#define _SSA_HPP

#include<map>
#include<deque>

void bfs(BasicBlock* node, map<int, bool> &marks, int &n, vector<int> &rpo);
vector<int> rpo(CFG* cfg);
BasicBlock* intersect(BasicBlock* left, BasicBlock* right);

void dom_tree(CFG* cfg);
void dom_frontier(CFG* cfg);

typedef multimap<string, BasicBlock*> vars_blocks_map;
typedef set<string> string_set;

bool inspect_cfg_simple_op(vars_blocks_map &blocks_vars, string_set &globals, string_set &locals, CFG_Exp* exp);
bool inspect_cfg_bin_op(vars_blocks_map &blocks_vars, string_set &globals, string_set &locals, CFG_Exp* exp);

void add_phis(CFG* cfg);
vector<BasicBlock*>* get_blocks_for_var(vars_blocks_map &blocks_vars, string name);

void ssa_rename(CFG* cfg);

typedef map<string, deque<int> > stacks_map;

void rename(BasicBlock* block, map<string, int> &counter, stacks_map &stack);
int new_name(string var_name, map<string, int> &counter, stacks_map &stack);

void rename_simple_op(CFG_Exp* &exp, stacks_map &stack);
void rename_binary_op(CFG_Exp* &exp, stacks_map &stack);

#endif
