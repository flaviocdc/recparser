
%{

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include "decl.h"

int alloc_sval = 0;

%}

%%

"while"		{ return TK_WHILE; }

"if"		{ return TK_IF; }

"else"		{ return TK_ELSE; }

"int"		{ return TK_TINT; }

"char"		{ return TK_TCHAR; }

"float"		{ return TK_TFLOAT; }

"return"	{ return TK_RETURN; }

"void"		{ return TK_TVOID; }

"..."    {return TK_MANY; }

[=]		{ return '='; }

[*]		{ return '*'; }

[/]		{ return '/'; }

[+]		{ return '+'; }

[-]		{ return '-'; }

[\^]   { return '^'; }

"=="		{ return TK_EQ; }

"!="		{ return TK_NEQ; }

"<="		{ return TK_LEQ; }

">="		{ return TK_GEQ; }

"&&"		{ return TK_AND; }

"||"		{ return TK_OR; }

[<]		{ return '<'; }

[>]		{ return '>'; }

[!]		{ return '!'; }

[(]		{ return '('; }

[)]		{ return ')'; }

[;]		{ return ';'; }

[,]		{ return ','; }

[\[]		{ return '['; }

[\]]		{ return ']'; }

[{]		{ return '{'; }

[}]		{ return '}'; }

[_a-zA-Z][_0-9a-zA-Z]*	{ 
			  if(alloc_sval) { free(yyval.sval); alloc_sval = 0; }
			  yyval.sval = (char*)malloc(yyleng + 1);
			  if(!yyval.sval) return ERR_MEM;
			  alloc_sval = 1;
			  strcpy(yyval.sval, yytext);
			  return TK_ID;
			}

[0-9]+  { 
	  long val;
	  if(alloc_sval) { free(yyval.sval); alloc_sval = 0; }
	  val = strtol(yytext, NULL, 10);
	  if(errno != ERANGE && val <= INT_MAX && val >= INT_MIN) { yyval.ival = (int)val; return TK_INT; } else return ERR_IRANGE;
	}

"0x"[0-9a-fA-F]+ { 
		   long val;
		   if(alloc_sval) { free(yyval.sval); alloc_sval = 0; }
		   val = strtol(yytext+2, NULL, 16);
		   if(errno != ERANGE && val <= INT_MAX && val >= INT_MIN) { yyval.ival = (int)val; return TK_INT; } else return ERR_IRANGE; 
		 }

[0-9]+"."[0-9]*([eE][+-]?[0-9]+)?  |
"."[0-9]+([eE][+-]?[0-9]+)?        |
[0-9]+([eE][+-]?[0-9]+)            {
				     if(alloc_sval) { free(yyval.sval); alloc_sval = 0; }
                                     yyval.fval = strtod(yytext, NULL);
				     if(errno != ERANGE) return TK_FLOAT; else return ERR_FRANGE;
                                   }

"\""([^\n\\\"]|"\\n"|"\\t"|"\\\\"|"\\\"")*"\""	{
						  int i, j;
						  i = 1; j = 0;
						  if(alloc_sval) { free(yyval.sval); alloc_sval = 0; }
						  yyval.sval = (char*)malloc(yyleng);
						  if(!yyval.sval) return ERR_MEM;
						  alloc_sval = 1;
						  while(i < (yyleng -1)) {
						    if(yytext[i]=='\\' && yytext[i+1]=='n') {
						      yyval.sval[j] = '\n';
						      i++;
						    } else if(yytext[i]=='\\' && yytext[i+1]=='t') {
						      yyval.sval[j] = '\t';
						      i++;
						    } else if(yytext[i]=='\\' && yytext[i+1]=='"') {
						      yyval.sval[j] = '\"';
						      i++;
						    } else if(yytext[i]=='\\' && yytext[i+1]=='\\') {
						      yyval.sval[j] = '\\';
						      i++;
						    } else yyval.sval[j] = yytext[i];
						    i++;
						    j++;
						  }
						  yyval.sval[j] = 0; 
						  return TK_STRING;
						}

"/*"([^*]|[*]+[^/*])*[*]*"*/"   ;

" "|\n|\t ;

.  { return ERR_SCAN; }

%%

int yywrap() { return 1; }

