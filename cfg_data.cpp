extern "C" {
  #include "ast.h"
}

#include<vector>
#include<string>
#include<sstream>
#include "cfg_data.hpp"

void BasicBlock::add_op(CFG_Command* cmd) {
  if (!has_return) {
    ops.push_back(cmd);
  }
}

void BasicBlock::br(BasicBlock* block) {
  succs.push_back(block);
  block->preds.push_back(this);
  add_op(new CFG_Branch(block));
}

void BasicBlock::brc(BasicBlock *trueBlock, BasicBlock *falseBlock, CFG_Var* cond) {
  succs.push_back(trueBlock);
  succs.push_back(falseBlock);
  
  trueBlock->preds.push_back(this);
  falseBlock->preds.push_back(this);
  
  add_op(new CFG_ConditionalBranch(trueBlock, falseBlock, cond));
}

void BasicBlock::ret(CFG_Var* var) {
  add_op(new CFG_Return(var));
  has_return = true;
}

void CFG::add_block(BasicBlock* block) {
  counter++;

  stringstream ss;
  ss << "B" << counter;
  block->name = ss.str();

  blocks.push_back(block);
  
  if (blocks.size() == 1)
    working_block = block;
}

vector<BasicBlock*> CFG::block_list() {
  return blocks;
}
