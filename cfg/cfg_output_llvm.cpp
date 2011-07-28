#include <sstream>
#include <vector>

#include "cfg_data.hpp"
#include "ast_utils.hpp"

string CFG_NamedMember::str() {
  return name;
}

string CFG_Var::str() {
  stringstream ss;
  
  ss << "%";
  ss << CFG_NamedMember::str();
  if (index != -1)
    ss << "_" << index;
    
  return ss.str();
}

string CFG_Funcall::str() {
  stringstream ss;

  ss << "call i32 @" << CFG_NamedMember::str() << "(";
  for (vector<CFG_Member*>::iterator it = params.begin(); it != params.end(); ++it) {
    ss << "i32 " << (*it)->str() << ",";
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
  string str_op = retrieve_operation_string(op);

  if (str_op == "==") {
    ss << "icmp eq i32 " << e1->str() << ", " << e2->str();
  } else if (str_op == "<=") {
    ss << "icmp sle i32 " << e1->str() << ", " << e2->str();
  } else if (str_op == ">=") {
    ss << "icmp sge i32 " << e1->str() << ", " << e2->str();
  } else if (str_op == ">") {
    ss << "icmp sgt i32 " << e1->str() << ", " << e2->str();
  } else if (str_op == "<") {
    ss << "icmp slt i32 " << e1->str() << ", " << e2->str();
  } else if (str_op == "+") {
    ss << "add i32 " << e1->str() << ", " << e2->str();
  } else if (str_op == "-") {
    ss << "sub i32 " << e1->str() << ", " << e2->str();
  } else if (str_op == "*") {
    ss << "mul i32 " << e1->str() << ", " << e2->str();
  } else if (str_op == "/") {
    ss << "sdiv i32 " << e1->str() << ", " << e2->str();
  }
  
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
  return "ret i32 " + retVal->str();
}

string CFG_Branch::str() {
  stringstream ss;
  ss << "br label %" << target->str();
  return ss.str();
}

string CFG_ConditionalBranch::str() {
  stringstream ss;
  ss << "br i1 " << cond->str() << ", label %" << trueBlock->str() << ", label %" << falseBlock->str();
  return ss.str();
}

string CFG_FuncallCommand::str() {
  stringstream ss;
  
  ss << "call i32 @" << name << "(";
  for (vector<CFG_Member*>::iterator it = params.begin(); it != params.end(); ++it) {
    ss << "i32 " << (*it)->str() << ",";
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


string CFG_Phis::str() {
  typedef set<pair<string, BasicBlock*> > ms;
  typedef map<string, ms> mm;
  typedef mm::iterator mm_iter;
  stringstream ss;
  
  for (mm_iter it = block->phis.begin(); it != block->phis.end(); it++) {
    string var = (*it).first;
    ms aux = (*it).second;
    
    ss << "%" << var << " = phi i32 ";
    
    for (ms::iterator pairs = aux.begin(); pairs != aux.end(); pairs++) {
      pair<string, BasicBlock*> p = (*pairs);
      ss << "[" << p.first << "," << "%" << p.second->str() << "],";
    }
  }
  
  string out = ss.str();
  out = out.substr(0, out.size() - 1);

  return out;
}
