#ifndef _SSA_HPP
#define _SSA_HPP

#include<map>

void bfs(BasicBlock* node, map<int, bool> &marks, int &n, vector<int> &rpo);
vector<int> rpo(CFG* cfg);
BasicBlock* intersect(BasicBlock* left, BasicBlock* right);
void dom_tree(CFG* cfg);

#endif
