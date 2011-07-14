#ifndef _SSA_HPP
#define _SSA_HPP

#include<map>

void bfs(BasicBlock* node, map<int, bool> &marks, int &n, vector<int> &rpo, string ident);
void rpo(CFG* cfg);

#endif
