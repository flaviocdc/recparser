#ifndef _CFG_DATA_HPP
#define _CFG_DATA_HPP

#include <string>
#include <vector>

using namespace std;

struct CFG_Command {

  virtual string str();
  //friend ostream& operator<<(ostream& o, CFG_Command& m);
};

struct CFG_Member {
  virtual string str() { };
  //friend ostream& operator<<(ostream& o, CFG_Member& m);
};

struct CFG_NamedMember : public CFG_Member {
  string name;

  CFG_NamedMember(string paramName) : name(paramName) {};

  string str();
};

struct CFG_Var : public CFG_NamedMember {
  int index;

  CFG_Var(string paramName) : CFG_NamedMember(name), index(0) {}; 
  CFG_Var(string paramName, int paramIndex) : CFG_NamedMember(paramName), index(paramIndex) {}; 

  string str();
};

template<class T>
struct CFG_Literal : public CFG_Member {
  T value;

  string str() {
    stringstream ss;
    ss << value;

    return ss.str();
  }
};

struct CFG_Exp : public CFG_Member {
  
  CFG_Exp *e1;
  CFG_Exp *e2;
  
  string str();
};

struct CFG_Funcall : public CFG_NamedMember {
  vector<CFG_Exp*> params;
  
  CFG_Funcall(string paramName) : CFG_NamedMember(paramName), params() {};
  CFG_Funcall(string paramName, vector<CFG_Exp*> xParams) : CFG_NamedMember(paramName), params(xParams) {};

  string str();
};

struct CFG_Attr : public CFG_Command {
  CFG_Attr(CFG_Var* lhs, CFG_Exp* rhs) : left(lhs), right(rhs) {}; 

  CFG_Var* left;
  CFG_Exp* right;

  string str();
};

struct CFG_Return : public CFG_Command {
  CFG_Exp* exp;
};

#endif
