extern "C" {
  #include "ast.h"
}
  
using namespace std;

#include<iostream>
#include<string>
#include<vector>

#include "cfg_data.hpp"

ostream &operator<<( ostream &out, BasicBlock &block ) {
  out << block.name << ":" << endl;
  
  vector<CFG_Command*>::iterator it;
  for ( it = block.ops.begin(); it < block.ops.end(); it++ ) {
    out << "\t" << (*it)->str() << endl;
  }

  return out;
}

ostream &operator<<( ostream &out, CFG &cfg ) {
  out << "--" << cfg.name << endl;

  vector<BasicBlock*>::iterator it;
  
  for ( it = cfg.blocks.begin(); it < cfg.blocks.end(); it++ ) {
    out << **it;
  }

  return out;
}
