#ifndef _CFG_DATA_HPP
#define _CFG_DATA_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <set>

using namespace std;

struct CFG_Command {
  virtual string str() = 0;
};

struct CFG_Member {
  virtual string str() = 0;
};

struct CFG_NamedMember : public CFG_Member {
  string name;

  CFG_NamedMember(string paramName) : name(paramName) {};

  virtual string str() = 0;
};

struct CFG_Var : public CFG_NamedMember {
  int index;

  CFG_Var(string paramName) : CFG_NamedMember(paramName), index(0) {}; 
  CFG_Var(string paramName, int paramIndex) : CFG_NamedMember(paramName), index(paramIndex) {}; 

  string str();
};

class BasicBlock {
  public:

    int index;
    vector<CFG_Command*> ops;
    vector<BasicBlock*> succs;
    vector<BasicBlock*> preds;
    set<string> vars;
    
    vector<BasicBlock*> children;
    BasicBlock* idom;
    int rpo;
    set<BasicBlock*> frontier;
    
    bool has_return;
    
    // phis

    BasicBlock() : index(0),
                   ops(),
                   succs(),
                   preds(),
                   children(),
                   idom(NULL),
                   frontier(),
                   vars(),
                   has_return(false) { };

    void add_op(CFG_Command* cmd);   
    void add_var(CFG_Var* var); 
    void br(BasicBlock* block);
    void brc(BasicBlock *trueBlock, BasicBlock *falseBlock, CFG_Var* cond);
    void ret(CFG_Var* var);

    string str();
    friend ostream &operator<<( ostream &out, BasicBlock &block );
};

class CFG {
  public:
    string name;
    vector<BasicBlock*> blocks;
    BasicBlock *working_block;
    
    int counter;

    CFG(string param_name) : name(param_name), blocks(), counter(0) { };

    void add_block(BasicBlock* block);
    vector<BasicBlock*> block_list();

    friend ostream &operator<<( ostream &out, CFG &cfg );
};

template<class T>
struct CFG_Literal : public CFG_Member {
  T value;

  CFG_Literal(T paramValue) : value(paramValue) {};

  string str() {
    stringstream ss;
    ss << value;

    return ss.str();
  }
};

struct CFG_Exp : public CFG_Member {
  virtual string str() = 0;
};

struct CFG_SimpleOp : public CFG_Exp {
  CFG_Member *exp;

  CFG_SimpleOp(CFG_Member* paramExp) : exp(paramExp) {};

  string str();
};

struct CFG_BinaryOp : public CFG_Exp {
  CFG_Member *e1;
  CFG_Member *e2;
  int op;
  
  CFG_BinaryOp(CFG_Member *left, int paramOp, CFG_Member *right) : e1(left), op(paramOp), e2(right) {};
  
  string str();
};

struct CFG_Funcall : public CFG_NamedMember {
  vector<CFG_Member*> params;
  
  CFG_Funcall(string paramName) : CFG_NamedMember(paramName), params() {};

  string str();
};

struct CFG_LogicalNotOp : public CFG_Exp {
  CFG_Member *exp;

  CFG_LogicalNotOp(CFG_Member* paramExp) : exp(paramExp) {};

  string str();
};

struct CFG_String : public CFG_Exp {
  string value;
  
  CFG_String(string paramValue) : value(paramValue) {};
  
  string str();
};

struct CFG_Attr : public CFG_Command {
  CFG_Attr(CFG_Var* param_lv, CFG_Exp* param_rv) : lvalue(param_lv), rvalue(param_rv) {}; 

  CFG_Var* lvalue;
  CFG_Exp* rvalue;

  string str();
};

struct CFG_Return : public CFG_Command {
  CFG_Member* retVal;
  
  CFG_Return(CFG_Member* paramRetVal) : retVal(paramRetVal) {};
  
  string str();
};

struct CFG_Branch : public CFG_Command {
  BasicBlock* target;
  
  CFG_Branch(BasicBlock* paramTarget) : target(paramTarget) {};
  
  string str();
};

struct CFG_ConditionalBranch : public CFG_Command {
  BasicBlock *trueBlock;
  BasicBlock *falseBlock;
  CFG_Var *cond;
  
  CFG_ConditionalBranch(BasicBlock *paramTrueBlock, BasicBlock *paramFalseBlock, CFG_Var *paramCond) : 
      trueBlock(paramTrueBlock), falseBlock(paramFalseBlock), cond(paramCond) {};
  
  string str();
};

struct CFG_FuncallCommand : public CFG_Command {
  string name;
  vector<CFG_Member*> params;
  
  CFG_FuncallCommand(string paramName) : name(paramName), params() {};

  string str();
};

#endif
