#include <map>
#include "cfg_data.hpp"
#include "ssa_movs.hpp"

using namespace std;

void ssa_remove_movs(CFG* cfg) {
  map<string, CFG_Exp*> replace_map;

  for (vector<BasicBlock*>::iterator bit = cfg->blocks.begin(); bit < cfg->blocks.end(); bit++) {
    BasicBlock* block = (*bit);
    
    vector<CFG_Command*> new_ops;
    
    for (vector<CFG_Command*>::iterator it = block->ops.begin(); it < block->ops.end(); it++) {
      CFG_Command* cmd = (*it);
      bool skip_op = false;
      
      map<string, CFG_Exp*>::iterator lookup;
      
      skip_op = ssa_remove_movs_attr(cmd, replace_map);
      ssa_remove_movs_ret(cmd, replace_map);
      ssa_remove_movs_funcall(cmd, replace_map);
      
      if (!skip_op) {
        new_ops.push_back(cmd);
      }
    }
        
    block->ops = new_ops;
    
    ssa_remove_movs_phis(block, replace_map);
  }
}

bool ssa_remove_movs_attr(CFG_Command* cmd, map<string, CFG_Exp*> &replace_map) {
  map<string, CFG_Exp*>::iterator lookup;

  CFG_Attr* attr = dynamic_cast<CFG_Attr*>(cmd);
  bool skip_op = false;
  
  if (attr) {
    CFG_SimpleOp* simple = dynamic_cast<CFG_SimpleOp*>(attr->rvalue);
    if (simple) {
      
      lookup = replace_map.find(simple->str());
      
      if (lookup != replace_map.end()) {
        attr->rvalue = (*lookup).second;
      }
      
      CFG_Funcall* func = dynamic_cast<CFG_Funcall*>(simple->exp);
      if (!func) {
        skip_op = true;
        replace_map.insert(make_pair(attr->lvalue->str(), attr->rvalue));
      }
    }
    
    CFG_BinaryOp* binop = dynamic_cast<CFG_BinaryOp*>(attr->rvalue);
    if (binop) {
      lookup = replace_map.find(binop->e1->str());
      if (lookup != replace_map.end()) {
        binop->e1 = (*lookup).second;
      }
      
      lookup = replace_map.find(binop->e2->str());
      if (lookup != replace_map.end()) {
        binop->e2 = (*lookup).second;
      }
    }
  }
  
  return skip_op;
}

void ssa_remove_movs_ret(CFG_Command* cmd, map<string, CFG_Exp*> &replace_map) {
  map<string, CFG_Exp*>::iterator lookup;

  CFG_Return* ret = dynamic_cast<CFG_Return*>(cmd);
  if (ret) {
    lookup = replace_map.find(ret->retVal->str());
    if (lookup != replace_map.end()) {
      ret->retVal = (*lookup).second;
    }
  }
}

void ssa_remove_movs_funcall(CFG_Command* cmd, map<string, CFG_Exp*> &replace_map) {
  map<string, CFG_Exp*>::iterator lookup;

  CFG_FuncallCommand* func = dynamic_cast<CFG_FuncallCommand*>(cmd);
  if (func) {
    vector<CFG_Member*> new_params;
    for (vector<CFG_Member*>::iterator pit = func->params.begin(); pit < func->params.end(); pit++) {
      lookup = replace_map.find((*pit)->str());
      if (lookup != replace_map.end()) {
        new_params.push_back((*lookup).second);
      } else {
        new_params.push_back((*pit));
      }
    }
    func->params = new_params;
  }
}

void ssa_remove_movs_phis(BasicBlock* block, map<string, CFG_Exp*> &replace_map) {
  typedef set<pair<string, BasicBlock*> > pairs_set;
  typedef map<string, pairs_set> phis_map;
  
  phis_map new_phis_map;
  map<string, CFG_Exp*>::iterator lookup;
  
  for (vector<BasicBlock*>::iterator bit = block->succs.begin(); bit < block->succs.end(); bit++) {
    BasicBlock* succ_block = (*bit);
  
    for (phis_map::iterator it = succ_block->phis.begin(); it != succ_block->phis.end(); it++) {
      pairs_set pairs = (*it).second;
      pairs_set new_pairs;
      
      for (pairs_set::iterator pit = pairs.begin(); pit != pairs.end(); pit++) {
        pair<string, BasicBlock*> pair = (*pit);
        
        lookup = replace_map.find("%" + pair.first); // TODO HACK!
        if (lookup != replace_map.end()) {
          new_pairs.insert(make_pair((*lookup).second->str(), pair.second));
        } else {
          new_pairs.insert(pair);
        }
      }
      
      new_phis_map.insert(make_pair((*it).first, new_pairs));
    }
  
    succ_block->phis = new_phis_map;
  }
  
}

