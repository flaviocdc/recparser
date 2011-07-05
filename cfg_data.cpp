#include <string>

using namespace std;

struct CFG_Command {

  virtual string str();
  friend ostream& operator<<(ostream& o, CFG_Command& m);
};

struct CFG_Member {
};

struct CFG_NamedMember : public CFG_Member {
  string name;

  CFG_NamedMember(string paramName) : name(paramName) {};
};

struct CFG_Var : public CFG_NamedMember {
  int index;

  CFG_Var(string paramName) : CFG_NamedMember(name), index(0) {}; 
  CFG_Var(string paramName, int paramIndex) : CFG_NamedMember(name), index(paramIndex) {}; 
};

template<class T>
struct CFG_Literal : public CFG_Member {
  T value;
};

struct CFG_Exp : public CFG_Member {
  CFG_Exp *e1, CFG_Exp *e2;
};

struct CFG_Attr : public CFG_Command {
  CFG_Var* left;
  CFG_Exp* right;
};


