#include<vector>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<iostream>

#include "cfg_data.hpp"
#include "cfg.hpp"

extern "C" {
  #include "decl.h"
  #include "ast.h"
  #include "driver.h"

  int yylex (void);
  void yyrestart (FILE *input_file);
}

extern int yylineno;
extern FILE *outfile;
extern char *filename;
extern int token;

int temp_counter = 0;

using namespace std;

void iterate_declrs(DeclrListNode* node) {

  while (node) {
    Declr* declr = node->declr;

    if (declr->tag == DECLR_FUNC && declr->u.func.block) {
      CFG *cfg = generate_cfg(declr);

      cout << *cfg << endl;
    }

    node = node->next;
  }
}

CFG* generate_cfg(Declr* declr) {
  Block* block = declr->u.func.block;

  if (block) {
    CFG* cfg = new CFG(declr->u.func.name);

    if (block->comms) {
      BasicBlock* bb = create_basic_working_block(cfg);
      
      generate_cfg_comms(cfg, block);
    }

    return cfg;
  }

  // TODO funcao sem corpo (prototipo)
  return NULL;
}

void generate_cfg_comms(CFG* cfg, Block* block) {
  CommListNode* node = block->comms;

  while(node) {
    Command* cmd = node->comm;
  
    generate_cfg_comm(cfg, cmd);    

    node = node->next;
  }
}

void generate_cfg_comm(CFG* cfg, Command* cmd) {
  switch(cmd->tag) {
    case COMMAND_ATTR: {
      CFG_Attr* cfg_attr = create_cfg_attr(cfg, cmd);
      cfg->working_block->add_op(cfg_attr);
      break;
    }
    case COMMAND_FUNCALL: {
      break;
    }
    case COMMAND_RET: {
      CFG_Return* cfg_ret = create_cfg_return(cfg, cmd);
      cfg->working_block->add_op(cfg_ret);
      break;
    }
    case COMMAND_IF: {
      Command *cmd_if = cmd->u.cif.comm;
      Command *cmd_else = cmd->u.cif.celse;
      
      BasicBlock* top = cfg->working_block;
      BasicBlock* blk_final = create_basic_block(cfg);
      
      BasicBlock* blk_if = create_basic_working_block(cfg);
      generate_cfg_comm(cfg, cmd_if);
      cfg->working_block->br(blk_final);
      
      BasicBlock* blk_else = NULL;
      if (cmd_else) {
        blk_else = create_basic_working_block(cfg);
        generate_cfg_comm(cfg, cmd_else);
        cfg->working_block->br(blk_final);
      }
    
      CFG_Attr* cond = create_temp_cfg_attr(top, create_cfg_exp(cfg, cmd->u.cif.exp));
      CFG_ConditionalBranch* brc;
      if (blk_else) {
        brc = new CFG_ConditionalBranch(blk_if, blk_else, cond->lvalue);
      } else {
        brc = new CFG_ConditionalBranch(blk_if, blk_final, cond->lvalue);
      }
      top->add_op(brc);
      
      cfg->working_block = blk_final;
    
      break;
    }
    case COMMAND_BLOCK: {
      generate_cfg_comms(cfg, cmd->u.block);
      break;
    }
    default: {
     cout << "Caso nao tratado no CFG: " << cmd->tag << endl;
     break;
   }
  }
}

CFG_Attr* create_cfg_attr(CFG* cfg, Command* cmd) {
  Var* ast_var = cmd->u.attr.lvalue;
  Exp* ast_exp = cmd->u.attr.rvalue;

  CFG_Var* cfg_var = new CFG_Var(ast_var->name);
  CFG_Exp* cfg_exp = create_cfg_exp(cfg, ast_exp);
  
  CFG_Attr* cfg_attr = new CFG_Attr(cfg_var, cfg_exp);
  
  return cfg_attr;
}

CFG_Exp* create_cfg_exp(CFG* cfg, Exp* ast_exp) {
  CFG_Exp* cfg_exp;

  switch (ast_exp->tag) {
    case EXP_INT: {
      CFG_Literal<int>* literal = new CFG_Literal<int>(ast_exp->u.ival);
      cfg_exp = new CFG_SimpleOp(literal);
      break;
    }
    case EXP_VAR: {
      CFG_Var* cfg_var = new CFG_Var(ast_exp->u.var->name);
      cfg_exp = new CFG_SimpleOp(cfg_var);
      break;
    }
    case EXP_FUNCALL: {
      CFG_Funcall* cfg_funcall = new CFG_Funcall(ast_exp->u.funcall.name);
      
      ExpListNode* expl = ast_exp->u.funcall.expl;
      BasicBlock* block = cfg->working_block;
      
      while (expl) {
        CFG_Attr* temp = create_temp_cfg_attr(cfg->working_block, create_cfg_exp(cfg, expl->exp));
        
        cfg_funcall->params.push_back(temp->lvalue);
        expl = expl->next;
      }
      
      cfg_exp = new CFG_SimpleOp(cfg_funcall);
      break;
    }
    case EXP_BINOP: {
      int op = ast_exp->u.binop.op;
      
      if (op == TK_AND) {
        CFG_Var* var = create_short_circuit_and(cfg, ast_exp);
        cfg_exp = new CFG_SimpleOp(var);
        break;
      }
      if (op == TK_OR) {
        CFG_Var* var = create_short_circuit_or(cfg, ast_exp);
        cfg_exp = new CFG_SimpleOp(var);
        break;
      }

      CFG_Exp* left_exp = create_cfg_exp(cfg, ast_exp->u.binop.e1);
      CFG_Attr* left_attr = create_temp_cfg_attr(cfg->working_block, left_exp);
      
      CFG_Exp* right_exp = create_cfg_exp(cfg, ast_exp->u.binop.e2);
      CFG_Attr* right_attr = create_temp_cfg_attr(cfg->working_block, right_exp);
            
      cfg_exp = new CFG_BinaryOp(left_attr->lvalue, op, right_attr->lvalue);
      break;
    }
    case EXP_NEG: {
      CFG_Exp* exp = create_cfg_exp(cfg, ast_exp->u.exp);
      CFG_Attr* attr = create_temp_cfg_attr(cfg->working_block, exp);
      
      CFG_Literal<int>* zero_literal = new CFG_Literal<int>(0);
      
      cfg_exp = new CFG_BinaryOp(zero_literal, '-', attr->lvalue);
      break;
    }

  }
  
  return cfg_exp;
}

CFG_Return* create_cfg_return(CFG* cfg, Command* cmd) {
  CFG_Exp* exp_ret = create_cfg_exp(cfg, cmd->u.ret);
  CFG_Attr* cfg_ret_attr = create_temp_cfg_attr(cfg->working_block, exp_ret);
  
  return new CFG_Return(cfg_ret_attr->lvalue);
}

CFG_Var* create_short_circuit_and(CFG* cfg, Exp* ast_exp) {
  BasicBlock* cond_bb = create_basic_block(cfg);
  
  cfg->working_block->br(cond_bb);
  cfg->working_block = cond_bb;
  
  CFG_Attr* left = create_temp_cfg_attr(cond_bb, create_cfg_exp(cfg, ast_exp->u.binop.e1));
  BasicBlock* trueBlock = create_basic_block(cfg);
  BasicBlock* falseBlock = create_basic_block(cfg);
  
  CFG_ConditionalBranch* brc = new CFG_ConditionalBranch(trueBlock, falseBlock, left->lvalue);
  cond_bb->add_op(brc);
  
  cfg->working_block = trueBlock;
  
  CFG_Attr* temp = new CFG_Attr(left->lvalue, create_cfg_exp(cfg, ast_exp->u.binop.e2));
  trueBlock->add_op(temp);
  trueBlock->br(falseBlock);
  
  cfg->working_block = falseBlock;
  
  cfg->working_block = falseBlock;
  
  return left->lvalue;
}

CFG_Var* create_short_circuit_or(CFG* cfg, Exp* ast_exp) {
  BasicBlock* cond_bb = create_basic_block(cfg);
  
  cfg->working_block->br(cond_bb);
  cfg->working_block = cond_bb;
  
  CFG_Attr* left = create_temp_cfg_attr(cond_bb, create_cfg_exp(cfg, ast_exp->u.binop.e1));
  BasicBlock* trueBlock = create_basic_block(cfg);
  BasicBlock* falseBlock = create_basic_block(cfg);
  
  CFG_ConditionalBranch* brc = new CFG_ConditionalBranch(falseBlock, trueBlock, left->lvalue);
  cond_bb->add_op(brc);
  
  cfg->working_block = trueBlock;
  
  CFG_Attr* temp = new CFG_Attr(left->lvalue, create_cfg_exp(cfg, ast_exp->u.binop.e2));
  trueBlock->add_op(temp);
  trueBlock->br(falseBlock);
  
  cfg->working_block = falseBlock;
  
  cfg->working_block = falseBlock;
  
  return left->lvalue;
}

CFG_Attr* create_temp_cfg_attr(BasicBlock* block, CFG_Exp* exp) {
  CFG_Var* var = new CFG_Var(new_temp_var());
  CFG_Attr* attr = new CFG_Attr(var, exp);
  
  block->add_op(attr);
  
  return attr;
}

string new_temp_var() {
  stringstream ss;
  ss << "t" << temp_counter;
  
  temp_counter++;
  
  return ss.str();
}

BasicBlock *create_basic_block(CFG* cfg) {
  BasicBlock *block = new BasicBlock;
  cfg->add_block(block);
  
  return block;
}

void set_working_block(CFG* cfg, BasicBlock* block) {
  cfg->working_block = block;
}

BasicBlock *create_basic_working_block(CFG* cfg) {
  BasicBlock* block = create_basic_block(cfg);
  set_working_block(cfg, block);
  
  return block;
}

int main(int argc, char **argv) {
  FILE *f;
  DeclrListNode *declrs;

  if(argc > 1) {
    f = fopen(argv[1], "r");
    filename = argv[1];
  } else {
    f = stdin;
    filename = "stdin";
  }
  if(!f) {
    fprintf(stderr, "Cannot open file %s. Exiting...", filename);
    exit(0);
  }
  yyrestart(f);
  yylineno = 1;
  outfile = stdout;
  filename = "stdout";

  token = yylex();

  declrs = declr_list(0);

  print_declrlist(0, declrs);
  iterate_declrs(declrs);
}
