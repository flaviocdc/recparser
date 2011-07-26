using namespace std;

#include<map>
#include<vector>
#include<iostream>
#include<algorithm>
#include<set>
#include<cstdlib>

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
      string name = var->name;
      if (blocks_vars.count(name) && locals.count(name) == 0) {
        //cout << "Global found: " << name << endl;
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
  
  //cout << "Processing Globals..." << endl;
  for (int i = 0; i < cfg->block_list().size(); i++) {
    string_set locals;
    
    BasicBlock* block = cfg->block_list()[i];
    
    for (int j = 0; j < block->ops.size(); j++) {
      CFG_Command* op = block->ops[j];
      
      CFG_Attr* attr = dynamic_cast<CFG_Attr*>(op);
      if (attr) {
        
        inspect_cfg_simple_op(blocks_vars, globals, locals, attr->rvalue);
        inspect_cfg_bin_op(blocks_vars, globals, locals, attr->rvalue);
        
        locals.insert(attr->lvalue->name);
        blocks_vars.insert(make_pair(attr->lvalue->name, block));
      }
    }
  }
  
  for (string_set::iterator it = globals.begin(); it != globals.end(); it++) {
    string name = *it;
      
    vector<BasicBlock*> worklist;
    vector<BasicBlock*> aux = *get_blocks_for_var(blocks_vars, name);
    
    worklist.insert(worklist.end(), aux.begin(), aux.end());
    
    while (worklist.size() > 0) {
      // pop the block
      BasicBlock* block = *worklist.begin();
      worklist.erase(worklist.begin());
      
      for (set<BasicBlock*>::iterator it = block->frontier.begin(); it != block->frontier.end(); ++it) {
        BasicBlock* frontier = (*it);
        if (frontier->add_phi(name)) {
          worklist.push_back(frontier);
        }
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

void ssa_rename(CFG* cfg) {
  map<string, int> counter;
  multimap<string, int> stack;
  
  vector<BasicBlock*> blocks = cfg->block_list();
  
  for (vector<BasicBlock*>::iterator it = blocks.begin(); it < blocks.end(); it++) {
    BasicBlock* block = (*it);
    
    for (set<string>::iterator vars = block->vars.begin(); vars != block->vars.end(); vars++) {
      string name = (*vars);
      if (counter.count(name) == 0) {
        counter.insert(make_pair(name, 0));
      }
    }
  }
  
  rename(blocks[0], counter, stack);
}

void rename(BasicBlock* block, map<string, int> &counter, multimap<string, int> &stack) {
  typedef multimap<string, pair<string, BasicBlock*> >::iterator phis_iter;
  
  string new_var(""), current_var("");
  
  for (phis_iter it = block->phis.begin(); it != block->phis.end();) {
    string var = (*it).first;
    cout << "var == " << var << endl;

    if (var != current_var) {
      stringstream ss;
      ss << var << "_" << new_name(var, counter, stack);
      new_var = ss.str();
      cout << "new_var == " << new_var << endl;
      current_var = var;
    }
    
    pair<string, BasicBlock*> pair = (*it).second;
    
    block->phis.erase(it++);
    block->phis.insert(make_pair(new_var, pair));
  }
  
  for (vector<CFG_Command*>::iterator it = block->ops.begin(); it < block->ops.end(); it++) {
    CFG_Command *op = (*it);
    CFG_Attr* attr = dynamic_cast<CFG_Attr*>(op);
    if (attr) {
      CFG_Var* cfg_var = attr->lvalue;
      cfg_var->index = new_name(cfg_var->name, counter, stack);
      
      rename_simple_op(attr->rvalue, stack);
      rename_binary_op(attr->rvalue, stack);
    }
  }
  
  for (vector<BasicBlock*>::iterator it = block->succs.begin(); it < block->succs.end(); it++) {
    
  }
  
  for (vector<BasicBlock*>::iterator it = block->children.begin(); it < block->succs.end(); it++) {
    rename((*it), counter, stack);    
  }
  
  for (vector<CFG_Command*>::iterator it = block->ops.begin(); it < block->ops.end(); it++) {
    
  }
}

void rename_binary_op(CFG_Exp* &exp, multimap<string, int> &stack) {
  CFG_BinaryOp* binop = dynamic_cast<CFG_BinaryOp*>(exp);
  if (binop) {
    CFG_Exp* e1 = dynamic_cast<CFG_Exp*>(binop->e1);
    if (e1) rename_simple_op(e1, stack);
    
    CFG_Exp* e2 = dynamic_cast<CFG_Exp*>(binop->e2);
    if (e2) rename_simple_op(e2, stack);  
  }
}

void rename_simple_op(CFG_Exp* &exp, multimap<string, int> &stack) {
  typedef multimap<string, int>::iterator stack_iter;

  CFG_SimpleOp* simple = dynamic_cast<CFG_SimpleOp*>(exp);
  if (simple) {
    CFG_Var* source = dynamic_cast<CFG_Var*>(simple->exp);
    
    if (source) {
      pair<stack_iter, stack_iter> range = stack.equal_range(source->name);
      int index = (*range.second).second;
      source->index = index;       
    }
  }
}

int new_name(string var_name, map<string, int> &counter, multimap<string, int> &stack) {
  int i = counter[var_name];
  counter[var_name] += 1;
  
  stack.insert(make_pair(var_name, i));
  
  return i;
}
