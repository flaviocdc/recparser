using namespace std;

#include<map>
#include<vector>
#include<iostream>
#include<algorithm>
#include<set>

#include "cfg_data.hpp"
#include "ssa.hpp"

template<class T>
inline ostream& operator<<(ostream& o, const vector<T>& v) {
  for (typename vector<T>::const_iterator i = v.begin(); i < v.end(); i++) {
    o << *i << " ";
  }

  return o;
}

inline ostream& operator<<(ostream& o, const vector<BasicBlock*>& v) {
  for (vector<BasicBlock*>::const_iterator i = v.begin(); i < v.end(); i++) {
    o << (*i)->str() << " ";
  }

  return o;
}

vector<int> rpo(CFG* cfg) {
  map<int, bool> marks;
  
  int size = cfg->block_list().size();
  
  int n = size - 1;
  vector<int> rpo(size, 0);
  
  bfs(cfg->block_list().at(0), marks, n, rpo);
  
  return rpo;  
}

void bfs(BasicBlock* node, map<int, bool> &marks, int &n, vector<int> &rpo) {
  marks[node->index] = true;
  for (vector<BasicBlock*>::iterator it = node->succs.begin(); it < node->succs.end(); it++) {
    int block_index = (*it)->index;
    
    if ( marks.count(block_index) == 0 ) {
      bfs(*it, marks, n, rpo);
    }
  }
  
  node->rpo = n;
  rpo[node->rpo] = node->index;
  
  n -= 1;
}

BasicBlock* intersect(BasicBlock* left, BasicBlock* right) {
  BasicBlock *finger1 = left, *finger2 = right;
  
  while (finger1->index != finger2->index) {
    while (finger1->rpo > finger2->rpo) {
      finger1 = finger1->idom;
    }
    while (finger2->rpo > finger1->rpo) {
      finger2 = finger2->idom;
    }
  }
  
  return finger1;
}

bool compare_blocks(BasicBlock* b1, BasicBlock* b2) {
  return b1->index < b2->index;
}
  
void dom_tree(CFG* cfg) {
  vector<int> nodes_rpo = rpo(cfg);
  vector<BasicBlock*> nodes = cfg->block_list();

  nodes[0]->idom = nodes[0];
  
  for (int i = 1; i < nodes.size(); i++) {
    int block_index = nodes_rpo[i];
    BasicBlock* node = nodes[block_index];
    if (node->preds.size() == 2 && node->preds[1]->idom != NULL) {
      node->idom = intersect(node->preds[0], node->preds[1]);
      node->idom->children.push_back(node);
    } else {
      node->idom = node->preds[0];
      node->idom->children.push_back(node);
    }  
  }
  
  for (vector<BasicBlock*>::iterator it = nodes.begin(); it < nodes.end(); it++) {
    sort((*it)->children.begin(), (*it)->children.end(), compare_blocks);
  }
}

void dom_frontier(CFG* cfg) {
  for (int i = 0; i < cfg->block_list().size(); i++) {
    BasicBlock* node = cfg->block_list()[i];
    
    if (node->preds.size() > 1) {
    
      for (int j = 0; j < node->preds.size(); j++) {
        BasicBlock* runner = node->preds[j];
        while (runner->index != node->idom->index) {
          runner->frontier.insert(node);
          runner = runner->idom;
        }
      }
      
    }   
  }
}


bool inspect_cfg_simple_op(vars_blocks_map &blocks_vars, string_set &globals, string_set &locals, CFG_Exp* exp) {
  CFG_SimpleOp *simple = dynamic_cast<CFG_SimpleOp*>(exp);
  if (simple) {
    CFG_Var *var = dynamic_cast<CFG_Var*>(simple->exp);
    
    if (var) {
      string name = var->str();
      if (blocks_vars.count(name) && locals.count(name) == 0) {
        cout << "Global found: " << name << endl;
        globals.insert(name);
      }
    }
  }
  
  return simple != NULL;
}

bool inspect_cfg_bin_op(vars_blocks_map &blocks_vars, string_set &globals, string_set &locals, CFG_Exp* exp) {
  CFG_BinaryOp *binop = dynamic_cast<CFG_BinaryOp*>(exp);

  if (binop) {
    inspect_cfg_simple_op(blocks_vars, globals, locals, dynamic_cast<CFG_Exp*>(binop->e1));
    inspect_cfg_simple_op(blocks_vars, globals, locals, dynamic_cast<CFG_Exp*>(binop->e2));
  }
}

void add_phis(CFG* cfg) {
  vars_blocks_map blocks_vars;
  
  string_set globals;
  
  cout << "Processing Globals..." << endl;
  for (int i = 0; i < cfg->block_list().size(); i++) {
    string_set locals;
    
    BasicBlock* block = cfg->block_list()[i];
    
    for (int j = 0; j < block->ops.size(); j++) {
      CFG_Command* op = block->ops[j];
      
      CFG_Attr* attr = dynamic_cast<CFG_Attr*>(op);
      if (attr) {
        
        inspect_cfg_simple_op(blocks_vars, globals, locals, attr->rvalue);
        inspect_cfg_bin_op(blocks_vars, globals, locals, attr->rvalue);
        
        locals.insert(attr->lvalue->str());
        blocks_vars.insert(pair<string, BasicBlock*>(attr->lvalue->str(), block));
      }
    }
  }
  
  for (string_set::iterator it = globals.begin(); it != globals.end(); it++) {
    string name = *it;
      
    vector<BasicBlock*> worklist;
    vector<BasicBlock*> aux = *get_blocks_for_var(blocks_vars, name);
    
    worklist.insert(worklist.end(), aux.begin(), aux.end());
    cout << "Worklist: " << worklist << endl;
    while (worklist.size() > 0) {
      BasicBlock* block = *worklist.begin();
      worklist.erase(worklist.begin());
      for (set<BasicBlock*>::iterator it = block->frontier.begin(); it != block->frontier.end(); it++) {
        cout << "Talvez phi" << endl;
      }
    }
  }
}

vector<BasicBlock*>* get_blocks_for_var(vars_blocks_map &blocks_vars, string name) {
  typedef vars_blocks_map::iterator vb_iter;
  
  vector<BasicBlock*> *list = new vector<BasicBlock*>;
  pair<vb_iter, vb_iter> temp = blocks_vars.equal_range(name);
  vb_iter it;
  
  for (it = temp.first; it != temp.second; ++it) {
    list->push_back((*it).second);
  }

  return list;
}
