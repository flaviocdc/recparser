extern "C" {
  #include "ast.h"
}
  
using namespace std;

#include<iostream>
#include<string>
#include<vector>

#include "cfg.hpp"

ostream &operator<<( ostream &out, BasicBlock &block ) {
  out << block.name << ":";
  
  vector<CFG_Command*>::iterator it;
    out << "To aqui!" << endl;
  for ( it = block.ops.begin(); it < block.ops.end(); it++ ) {
    out << "To aqui!" << endl;
    out << (*it)->str() << endl;
  }

  return out;
}

ostream &operator<<( ostream &out, CFG &cfg ) {
  out << "func: " << cfg.name << endl;

  vector<BasicBlock*>::iterator it;
  
  for ( it = cfg.blocks.begin(); it < cfg.blocks.end(); it++ ) {
    out << **it;
  }

  return out;
}
