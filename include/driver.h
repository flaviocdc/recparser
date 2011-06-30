#ifndef _DRIVER_H

#define _DRIVER_H

#include "ast.h"
#include "decl.h"

#define NEXT() token = yylex()
#define INSIDE_FUNC 1
#define NOT_INSIDE_FUNC 0

DeclrListNode *declr_list(int inside_function);

#endif
