using namespace std;

#include<string>
#include<sstream>

#define TK_EQ		1012
#define TK_LEQ	1013
#define TK_NEQ	1014
#define TK_GEQ	1015
#define TK_AND	1016
#define TK_OR		1017

string retrieve_operation_string(int op) {
  stringstream ss;
  
  switch(op)
  {
    case TK_EQ: {
      ss << "eq";
      break;
    }
    case TK_LEQ: {
      ss << "sle";
      break;
    }
    case TK_NEQ: {
      ss << "ne";
      break;
    }
    case TK_GEQ: {
      ss << "sge";
      break;
    }
    case '>': {
      ss << "sgt";
      break;
    }
    case '<': {
      ss << "slt";
      break;
    }
    default: {
      ss << (char) op;
      break;
    }
  }
  
  return ss.str();
}

