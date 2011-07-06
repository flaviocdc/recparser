extern "C" {
  #include "ast.h"
}

#include<vector>
#include<string>
#include<sstream>
#include "cfg.hpp"
#include "cfg_data.hpp"

void BasicBlock::add_op(CFG_Command* cmd) {
  ops.push_back(cmd);
  //cout << "adding " << cmd->str() << " to " << this->str() << endl;
}

void BasicBlock::br(BasicBlock* block) {
  succs.push_back(block);
  block->preds.push_back(this);
  ops.push_back(new CFG_Branch(block));
}

void BasicBlock::brc(BasicBlock *trueBlock, BasicBlock *falseBlock) {
  succs.push_back(trueBlock);
  succs.push_back(falseBlock);
  
  trueBlock->preds.push_back(this);
  falseBlock->preds.push_back(this);
  
  ops.push_back(new CFG_ConditionalBranch(trueBlock, falseBlock, NULL));
}

string BasicBlock::str() {
  return name;
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
