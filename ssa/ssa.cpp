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
  
  int size = cfg->blocks.size();
  
  int n = size - 1;
  vector<int> rpo(size, 0);
  
  bfs(cfg->blocks.at(0), marks, n, rpo);
  
  return rpo;  
}

void bfs(BasicBlock* node, map<int, bool> &marks, int &n, vector<int> &rpo) {
  marks[node->index] = true;
  for (vector<BasicBlock*>::iterator it = node->succs.begin(); it < node->succs.end(); it++) {
    int block_index = (*it)->index;
    
    if (marks.count(block_index) == 0 ) {
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
  vector<BasicBlock*> nodes = cfg->blocks;

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
  for (int i = 0; i < cfg->blocks.size(); i++) {
    BasicBlock* node = cfg->blocks[i];
    
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
  for (int i = 0; i < cfg->blocks.size(); i++) {
    string_set locals;
    
    BasicBlock* block = cfg->blocks[i];
    
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
  stacks_map stack;
  
  vector<BasicBlock*> blocks = cfg->blocks;
  
  for (vector<BasicBlock*>::iterator it = blocks.begin(); it < blocks.end(); it++) {
    BasicBlock* block = (*it);
    
    for (set<string>::iterator vars = block->vars.begin(); vars != block->vars.end(); vars++) {
      string name = (*vars);
      //cout << "init : " << name << endl;
      if (counter.count(name) == 0) {
        counter.insert(make_pair(name, 0));
      }
      if (stack.count(name) == 0) {
        stack.insert(make_pair(name, deque<int>()));
      }
    }
  }
  
  rename(blocks[0], counter, stack);
}

void rename(BasicBlock* block, map<string, int> &counter, stacks_map &stack) {
  typedef phis_map::iterator phis_iter;
  
  //cout << "rename(" << block->str() << ")" << endl;
  
  phis_map new_phis;
  for (phis_iter it = block->phis.begin(); it != block->phis.end(); it++) {
    string var = (*it).first;
    //cout << "tratando " << var << " = phi " << endl;
    set<pair<string, BasicBlock*> > aux = (*it).second;
    
    stringstream ss;
    ss << var << "_" << new_name(var, counter, stack);
    //cout << "antigo phi " << var << " novo phi " << ss.str() << endl;
    new_phis.insert(make_pair(ss.str(), aux));    
  }
  block->phis = new_phis;
  
  for (vector<CFG_Command*>::iterator it = block->ops.begin(); it < block->ops.end(); it++) {
    CFG_Command *op = (*it);
    CFG_Attr* attr = dynamic_cast<CFG_Attr*>(op);
    if (attr) {
      CFG_Var* cfg_var = attr->lvalue;
      //cout << "- rename attr: " << cfg_var->name << " with index " << cfg_var->index << endl;
      cfg_var->index = new_name(cfg_var->name, counter, stack);
      //cout << "- new index is: " << cfg_var->index << " - ref = " << cfg_var << endl;
      
      rename_simple_op(attr->rvalue, stack);
      rename_binary_op(attr->rvalue, stack);
    }
  }
  
  for (vector<BasicBlock*>::iterator it = block->succs.begin(); it < block->succs.end(); it++) {
    BasicBlock* succ = (*it);
    //cout << block->str() << " - Verificando sucessor: " << succ->str() << endl;
    
    for (phis_map::iterator phis = succ->phis.begin(); phis != succ->phis.end();) {
      typedef set<pair<string, BasicBlock*> > phi_pairs;
      
      phi_pairs pairs = (*phis).second;
      phi_pairs new_pairs;
      
      for (phi_pairs::iterator phi_pair = pairs.begin(); phi_pair != pairs.end(); phi_pair++) {
        pair<string, BasicBlock*> aux = (*phi_pair);
        //cout << "- phi[" << aux.first << "," << aux.second->str() << "]" << endl;
        if (aux.second->index == block->index) {
          //cout << "- block index match!" << endl;
          if (stack[aux.first].empty()) {
            stack[aux.first].push_back(0);
            counter[aux.first] += 1;
          }
          
          aux = make_pair(ssa_name(aux.first, stack), aux.second);
        }
        
        new_pairs.insert(aux);
      }
      
      string var = (*phis).first;
      
      succ->phis.erase(phis++);
      succ->phis.insert(make_pair(var, new_pairs));
    }
  }
  
  for (vector<BasicBlock*>::iterator it = block->children.begin(); it < block->children.end(); it++) {
    rename((*it), counter, stack);    
  }
  
  for (vector<CFG_Command*>::iterator it = block->ops.begin(); it < block->ops.end(); it++) {
    CFG_Attr* attr = dynamic_cast<CFG_Attr*>((*it));
    if (attr) {
      stack[attr->lvalue->name].pop_back();
      //cout << "popped from stack[" << attr->lvalue->name << "]" << endl;
    }
  }
  
  for (phis_map::iterator it = block->phis.begin(); it != block->phis.end(); it++) {
    string var = (*it).first;
    string old_name = var.substr(0, var.find_first_of('_'));
    
    stack[old_name].pop_back();
    //cout << "popped from stack[" << old_name << "]" << endl;
  }
}

void rename_binary_op(CFG_Exp* &exp, stacks_map &stack) {
  CFG_BinaryOp* binop = dynamic_cast<CFG_BinaryOp*>(exp);
  if (binop) {
    CFG_Exp* e1 = dynamic_cast<CFG_Exp*>(binop->e1);
    if (e1) rename_simple_op(e1, stack);
    
    CFG_Exp* e2 = dynamic_cast<CFG_Exp*>(binop->e2);
    if (e2) rename_simple_op(e2, stack);  
  }
}

void rename_simple_op(CFG_Exp* &exp, stacks_map &stack) {
  typedef stacks_map::iterator stack_iter;

  CFG_SimpleOp* simple = dynamic_cast<CFG_SimpleOp*>(exp);
  if (simple) {
    CFG_Var* source = dynamic_cast<CFG_Var*>(simple->exp);
    
    if (source) {
      if (stack[source->name].empty()) {
        stack[source->name].push_back(0);
        //counter[source->name] += 1
      }
      
      int index = stack[source->name].back();      
      //cout << "- alterando '" << source->name << "->index' ("<< source << ") de " << source->index << " para " << index << endl;
      source->index = index;       
    }
  }
}

int new_name(string var_name, map<string, int> &counter, stacks_map &stack) {
  int i = counter[var_name];
  counter[var_name] += 1;

  stack[var_name].push_back(i);
  //cout << "new_name(" << var_name << ") == " << i << endl;
  return i;
}

string ssa_name(string var_name, stacks_map &stack) {
  stringstream ss;
  
  ss << var_name << "_" << stack[var_name].back();
  
  return ss.str();  
}
