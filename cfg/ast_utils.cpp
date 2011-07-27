using namespace std;

#include<string>
#include<sstream>
#include<vector>

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
      ss << "==";
      break;
    }
    case TK_LEQ: {
      ss << "=<";
      break;
    }
    case TK_NEQ: {
      ss << "!=";
      break;
    }
    case TK_GEQ: {
      ss << ">=";
      break;
    }
    case '>': {
      ss << ">";
      break;
    }
    case '<': {
      ss << "<";
      break;
    }
    default: {
      ss << (char) op;
      break;
    }
  }
  
  return ss.str();
}
