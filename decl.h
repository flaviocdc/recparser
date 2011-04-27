#ifndef _DECL_H

#define _DECL_H

#define TK_INT		1000
#define TK_FLOAT	1001
#define TK_STRING	1002
#define TK_ID		1003
#define TK_WHILE	1004
#define TK_IF		1005
#define TK_ELSE		1006
#define TK_TINT		1007
#define TK_TFLOAT	1008
#define TK_TCHAR	1009
#define TK_RETURN	1010
#define TK_TVOID	1011
#define TK_EQ		1012
#define TK_LEQ		1013
#define TK_NEQ		1014
#define TK_GEQ		1015
#define TK_AND		1016
#define TK_OR		1017
#define TK_MANY         276

#define ERR_SCAN	-1001
#define ERR_MEM		-1002
#define ERR_IRANGE	-1003
#define ERR_FRANGE	-1004

union {
  int ival;
  double fval;
  char *sval;
} yyval;

#endif

