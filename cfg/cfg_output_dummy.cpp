#include <sstream>
#include <vector>

#include "cfg_data.hpp"
#include "ast_utils.hpp"

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
  for (vector<CFG_Member*>::iterator it = params.begin(); it != params.end(); ++it) {
    ss << (*it)->str() << ",";
  }
  
  string out = ss.str();
  
  if (params.size() > 0) {
    out = out.substr(0, out.size()-1);
  }
    
  return out + ")";
}

string CFG_SimpleOp::str() {
  return exp->str();
}

string CFG_BinaryOp::str() {
  stringstream ss;
  
  ss << e1->str() << " " + retrieve_operation_string(op) << " " << e2->str();
  
  return ss.str();
}

string CFG_LogicalNotOp::str() {
  return "not " + exp->str();
}

string CFG_String::str() {
  return value;
}

string CFG_Attr::str() {
  stringstream ss;
  
  ss << lvalue->str() << " = " << rvalue->str();
  
  return ss.str();
}

string CFG_Return::str() {
  return "ret " + retVal->str();
}

string CFG_Branch::str() {
  stringstream ss;
  ss << "br " << target->str();
  return ss.str();
}

string CFG_ConditionalBranch::str() {
  stringstream ss;
  ss << "brc " << cond->str() << " " << trueBlock->str() << " " << falseBlock->str();
  return ss.str();
}

string CFG_FuncallCommand::str() {
  stringstream ss;
  
  ss << name << "(";
  for (vector<CFG_Member*>::iterator it = params.begin(); it != params.end(); ++it) {
    ss << (*it)->str() << ",";
  }
  
  string out = ss.str();
  
  if (params.size() > 0) {
    out = out.substr(0, out.size()-1);
  }
    
  return out + ")";
}

string BasicBlock::str() {
  stringstream ss;
  ss << "B" << index;
  return ss.str();
}
