#include<iostream>
#include<vector>

#include "cfg_data.hpp"
#include "ssa_debug.hpp"

using namespace std;

void print_dom_tree(CFG* cfg) {
  cout << "== dom_tree(" << cfg->name << ") ==" << endl;
  
  for (int i = 0; i < cfg->block_list().size(); i++) {
    BasicBlock* block = cfg->block_list()[i];
    cout << block->index << ": ";
    for (vector<BasicBlock*>::iterator child = block->children.begin(); child != block->children.end(); child++) {
      cout << (*child)->str() << " ";
    }
    cout << endl;
  }
}

void print_dom_frontier(CFG* cfg) {
  cout << "== dom_frontier(" << cfg->name << ") ==" << endl;
  
  for (int i = 0; i < cfg->block_list().size(); i++) {
    BasicBlock* block = cfg->block_list()[i];
    cout << block->index << ": ";
    for (set<BasicBlock*>::iterator child = block->frontier.begin(); child != block->frontier.end(); child++) {
      cout << (*child)->str() << " ";
    }
    cout << endl;
  }
}

void print_dom_debug(vector<CFG*> cfgs) {
  for (int i = 0; i < cfgs.size(); i++) {
    print_dom_tree(cfgs[i]);
    print_dom_frontier(cfgs[i]);
  }
}
