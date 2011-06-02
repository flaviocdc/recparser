#ifndef _CFG_HPP

#define _CFG_HPP

extern "C" {
  #include "ast.h"
}

#include<vector>
#include<string>
#include <sstream>

using namespace std;

class BasicBlock {
  public:

    string name;
    vector<Command*> ops;
    vector<BasicBlock*> succs;
    vector<BasicBlock*> preds;
    // phis
    // vars

    BasicBlock() : name(),
                   ops(),
                   succs(),
                   preds() { };

    void add_op(Command* cmd) {
      ops.push_back(cmd);
    }

    friend ostream &operator<<( ostream &out, BasicBlock &block );
};

class CFG {
  public:
    string name;
    vector<BasicBlock*> blocks;
    int counter;

    CFG(string param_name) : name(param_name), blocks(), counter(0) { };

    void add_block(BasicBlock* block) {
      counter++;

      ostringstream o;
      o << "B" << counter;
      block->name = o.str();

      blocks.push_back(block);
    }

    vector<BasicBlock*> block_list() {
      return blocks;
    }

    friend ostream &operator<<( ostream &out, CFG &cfg );
};

void iterate_declrs(DeclrListNode* node);
CFG* generate_cfg(Declr* declr);
void generate_cfg_comms(CFG* cfg, CommListNode* node);

#endif
