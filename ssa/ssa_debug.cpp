#include<iostream>
#include<vector>

#include "cfg_data.hpp"

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

void print_dom_trees(vector<CFG*> cfgs) {
  for (int i = 0; i < cfgs.size(); i++) {
    print_dom_tree(cfgs[i]);
  }
}
