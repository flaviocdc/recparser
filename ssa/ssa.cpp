using namespace std;

#include<map>
#include<vector>
#include<iostream>
#include "cfg_data.hpp"
#include "ssa.hpp"

template<class T>
inline ostream& operator<<(ostream& o, const vector<T>& v) {
  for (typename vector<T>::const_iterator i = v.begin(); i != v.end(); ++i) {
    o << *i << " ";
  }

  return o;
}

void rpo(CFG* cfg) {
  map<int, bool> marks;
  
  int size = cfg->block_list().size();
  
  int n = size - 1;
  vector<int> rpo(size, 0);
  
  bfs(cfg->block_list().at(0), marks, n, rpo);
  
  cout << rpo << endl;
}

void bfs(BasicBlock* node, map<int, bool> &marks, int &n, vector<int> &rpo) {
  marks[node->index] = true;
  
  for (vector<BasicBlock*>::iterator it = node->succs.begin(); it != node->succs.end(); it++) {
    int block_index = (*it)->index;
    
    if ( marks.count(block_index) == 0 ) {
      bfs(*it, marks, n, rpo);
    }
  }
  
  node->rpo = n;
  rpo[n] = node->index;
  
  n -= 1;
}
