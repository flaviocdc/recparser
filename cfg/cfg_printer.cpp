extern "C" {
  #include "ast.h"
}
  
using namespace std;

#include<iostream>
#include<string>
#include<vector>

#include "cfg_data.hpp"

ostream &operator<<( ostream &out, BasicBlock &block ) {
  out << block.str() << ":" << endl;
  
  vector<CFG_Command*>::iterator it;
  for ( it = block.ops.begin(); it < block.ops.end(); it++ ) {
    out << "  " << (*it)->str() << endl;
  }

  return out;
}

ostream &operator<<( ostream &out, CFG &cfg ) {
  out << "define i32 @" << cfg.name << "(";
  
  //cout << "params addr: " << &cfg.params << endl;
  //cout << "cfg.params.size() == " << cfg.params.size() << endl;
  /*for (int i = 0; i < cfg.params.size(); i++) {
    out << "i32 %" << cfg.params[i];
    if (i != cfg.params.size() - 1)
      out << ",";
  }*/
  for (vector<string>::iterator sit = cfg.params.begin(); sit < cfg.params.end(); sit++) {
    out << "i32 %" << (*sit);
   // if (i != cfg.params.size() - 1)
      out << ",";
  }
  
  out << ")" << endl;

  vector<BasicBlock*>::iterator it;
  
  for ( it = cfg.blocks.begin(); it < cfg.blocks.end(); it++ ) {
    out << **it;
  }

  return out;
}
