extern "C" {
  #include "ast.h"
}
  
using namespace std;

#include<iostream>
#include<string>
#include<vector>

#include "cfg_data.hpp"

ostream &operator<<( ostream &out, BasicBlock &block ) {
  if (block.ops.size() == 1)
    // bloco possui apenas uma operacao que eh o "placeholder" de um possivel phi.
    return out;

  out << block.str() << ":" << endl;
  
  vector<CFG_Command*>::iterator it;
  for ( it = block.ops.begin(); it < block.ops.end(); it++ ) {
    string op = (*it)->str();
    if (!op.empty())
      out << "  " << op << endl;
  }

  return out;
}

ostream &operator<<( ostream &out, CFG &cfg ) {
  out << "define i32 @" << cfg.name << "(";
  
  for (int i = 0; i < cfg.params.size(); i++) {
    out << "i32 %" << cfg.params[i] << "_0" ;
    if (i != cfg.params.size() - 1)
      out << ",";
  }
  out << ")" << endl;

  out << "{" << endl;
  vector<BasicBlock*>::iterator it;
  
  for ( it = cfg.blocks.begin(); it < cfg.blocks.end(); it++ ) {
    out << **it;
  }

  out << "}" << endl;
  return out;
}
