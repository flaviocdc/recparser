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
  
  bfs(cfg->block_list().at(0), marks, n, rpo, " ");
  
  cout << rpo << endl;
}

void bfs(BasicBlock* node, map<int, bool> &marks, int &n, vector<int> &rpo, string ident) {
  //cout << ident << "Visitando " << node->str() << endl;
  marks[node->name] = true;
  
  //cout << ident << "Entrando no loop de sucss" << endl;
  for (vector<BasicBlock*>::iterator it = node->succs.begin(); it != node->succs.end(); it++) {
    int block_index = (*it)->name;
    
    //cout << ident << (*it)->str() << " has been marked? " << (marks.count(block_index) != 0) << endl;
    if ( marks.count(block_index) == 0 ) {
      bfs(*it, marks, n, rpo, ident + " ");
    }
  }
  
  node->rpo = n;
  //cout << ident << "node.rpo = " << node->str() << "." << node->rpo << endl;
  rpo[node->rpo] = node->name;
  n -= 1;
  //cout << ident << "'n' == " << n << endl;
}
