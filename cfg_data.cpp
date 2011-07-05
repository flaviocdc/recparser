#include <sstream>
#include <vector>
#include "cfg_data.hpp"

string CFG_NamedMember::str() {
  return name;
}

string CFG_Var::str() {
  stringstream ss;
  
  ss << CFG_NamedMember::str() << "_" << index;

  return ss.str();
}

string CFG_Funcall::str() {
  stringstream ss;

  ss << CFG_NamedMember::str() << "(";
  for (vector<CFG_Exp*>::iterator it = params.begin(); it != params.end(); ++it) {
    ss << (*it)->str() << ",";
  }

  string out = ss.str();

  out = out.substr(0, out.size() - 1) + ")";

  return out;
}

string CFG_SimpleOp::str() {
  return exp->str();
}

string CFG_Attr::str() {
  stringstream ss;
  
  ss << left->str() << "=" << right->str();
  
  return ss.str();
}
