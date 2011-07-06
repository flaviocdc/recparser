#ifndef _CFG_DATA_HPP
#define _CFG_DATA_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

struct CFG_Command {
  virtual string str() = 0;
  //friend ostream& operator<<(ostream& o, CFG_Command& m);
};

struct CFG_Member {
  virtual string str() = 0;
  //friend ostream& operator<<(ostream& o, CFG_Member& m);
};

struct CFG_NamedMember : public CFG_Member {
  string name;

  CFG_NamedMember(string paramName) : name(paramName) {};

  string str();
};

struct CFG_Var : public CFG_NamedMember {
  int index;

  CFG_Var(string paramName) : CFG_NamedMember(paramName), index(0) {}; 
  CFG_Var(string paramName, int paramIndex) : CFG_NamedMember(paramName), index(paramIndex) {}; 

  string str();
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
  CFG_Funcall(string paramName, vector<CFG_Member*> xParams) : CFG_NamedMember(paramName), params(xParams) {};

  string str();
};

struct CFG_Attr : public CFG_Command {
  CFG_Attr(CFG_Var* param_lv, CFG_Exp* param_rv) : lvalue(param_lv), rvalue(param_rv) {}; 

  CFG_Var* lvalue;
  CFG_Exp* rvalue;

  string str();
};

struct CFG_Return : public CFG_Command {
  CFG_Exp* exp;
};

#endif
