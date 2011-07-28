#include <map>
#include <typeinfo>
#include "cfg_data.hpp"

using namespace std;

void ssa_remove_movs(CFG* cfg) {
  map<string, CFG_Exp*> replace_map;

  for (vector<BasicBlock*>::iterator bit = cfg->blocks.begin(); bit < cfg->blocks.end(); bit++) {
    BasicBlock* block = (*bit);
    
    vector<CFG_Command*> new_ops;
    
    for (vector<CFG_Command*>::iterator it = block->ops.begin(); it < block->ops.end(); it++) {
      CFG_Command* cmd = (*it);
      CFG_Attr* attr = dynamic_cast<CFG_Attr*>(cmd);
      
      map<string, CFG_Exp*>::iterator lookup;
      bool skip_op = false;
      
      if (attr) {
      
        CFG_SimpleOp* simple = dynamic_cast<CFG_SimpleOp*>(attr->rvalue);
        if (simple) {
          lookup = replace_map.find(simple->str());
          
          if (lookup != replace_map.end()) {
            attr->rvalue = (*lookup).second;
          }
          
          skip_op = true;
          replace_map.insert(make_pair(attr->lvalue->str(), attr->rvalue));
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
      
      if (!skip_op) {
        new_ops.push_back(cmd);
      }
    }
    
    block->ops = new_ops;
  }
}
