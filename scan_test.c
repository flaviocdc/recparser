#include <stdio.h>
#include "decl.h"

FILE *tmpinput = NULL;

int setup_test(char *input) {
  if(tmpinput) fclose(tmpinput);
  tmpinput = tmpfile();
  fputs(input, tmpinput);
  fflush(tmpinput);
  rewind(tmpinput);
  yyrestart(tmpinput);
}

int test_comment1() {
  setup_test("/*/");
  return yylex() == '/' && yylex() == '*' && yylex() == '/';
}

int test_comment2() {
  setup_test("/**/");
  return yylex() == 0;
}

int test_comment3() {
  setup_test("/***/");
  return yylex() == 0;
}

int test_comment4() {
  setup_test("/**\n*/");
  return yylex() == 0;
}

int test_comment5() {
  setup_test("/**/***/");
  return yylex() == '*' && yylex() == '*' && yylex() == '*' && yylex() == '/';
}

int test_comment6() {
  setup_test("/*/**/");
  return yylex() == 0;
}

int test_comment7() {
  setup_test("/*\"foo/*\"*/");
  return yylex() == 0;
}

int test_string1() {
  setup_test("\"\"");
  return yylex() == TK_STRING && strcmp(yyval.sval, "") == 0;
}

int test_string2() {
  setup_test("\"\"\"");
  return yylex() == TK_STRING && strcmp(yyval.sval, "") == 0 && yylex() == ERR_SCAN;
}

int test_string3() {
  setup_test("\"\"\"\"");
  return yylex() == TK_STRING && strcmp(yyval.sval, "") == 0 && yylex() == TK_STRING &&
	  strcmp(yyval.sval, "") == 0;
}

int test_string4() {
  setup_test("\"\\\"");
  return yylex() == ERR_SCAN;
}

int test_string5() {
  setup_test("\"\\a\"");
  return yylex() == ERR_SCAN;
}

int test_string6() {
  setup_test("\"\\\\a\"");
  return yylex() == TK_STRING && strcmp(yyval.sval, "\\a") == 0;
}

int test_string7() {
  setup_test("\"this\\tis a\\ntest for \\\"string\\\"!\"");
  return yylex() == TK_STRING && strcmp(yyval.sval, "this\tis a\ntest for \"string\"!") == 0;
}

int test_string8() {
  setup_test("\"\"a\"");
  return yylex() == TK_STRING && strcmp(yyval.sval, "") == 0 && yylex() == TK_ID && strcmp(yyval.sval, "a") == 0 &&
	  yylex() == ERR_SCAN;
}

int test_string9() {
  setup_test("\"foo/*bar*/\"");
  return yylex() == TK_STRING && strcmp(yyval.sval, "foo/*bar*/") == 0;
}

int test_reserved() {
  setup_test("char else float if int return void while");
  return yylex() == TK_TCHAR && yylex() == TK_ELSE && yylex() == TK_TFLOAT && yylex() == TK_IF &&
	  yylex() == TK_TINT && yylex() == TK_RETURN && yylex() == TK_TVOID && yylex() == TK_WHILE;
}

int test_id1() {
  setup_test("_ _foo _12foo whilefoo wh12kc w_foo __");
  return yylex() == TK_ID && strcmp(yyval.sval, "_") == 0 &&
	  yylex() == TK_ID && strcmp(yyval.sval, "_foo") == 0 &&
	  yylex() == TK_ID && strcmp(yyval.sval, "_12foo") == 0 &&
	  yylex() == TK_ID && strcmp(yyval.sval, "whilefoo") == 0 &&
	  yylex() == TK_ID && strcmp(yyval.sval, "wh12kc") == 0 &&
	  yylex() == TK_ID && strcmp(yyval.sval, "w_foo") == 0 &&
	  yylex() == TK_ID && strcmp(yyval.sval, "__") == 0;
}

int test_id2() {
  setup_test("foo_bar= abc!234 k;x");
  return yylex() == TK_ID && strcmp(yyval.sval, "foo_bar") == 0 &&
	  yylex() == '=' && yylex() == TK_ID && strcmp(yyval.sval, "abc") == 0 &&
	  yylex() == '!' && yylex() == TK_INT && yyval.ival == 234 &&
	  yylex() == TK_ID  && strcmp(yyval.sval, "k") == 0 && yylex() == ';' &&
	  yylex() == TK_ID && strcmp(yyval.sval, "x") == 0;
}

int test_op() {
  setup_test("+ - / * = == != <= >= && || ! < > , ; () [] {}");
  return yylex() == '+' && yylex() == '-' && yylex() == '/' && yylex() == '*' && yylex() == '=' &&
	  yylex() == TK_EQ && yylex() == TK_NEQ && yylex() == TK_LEQ && yylex() == TK_GEQ &&
	  yylex() == TK_AND && yylex() == TK_OR && yylex() == '!' && yylex() == '<' &&
	  yylex() == '>' && yylex() == ',' && yylex() == ';' && yylex() == '(' &&
	  yylex() == ')' && yylex() == '[' && yylex() == ']' && yylex() == '{' && yylex() == '}';
}

int test_number1(){
  setup_test("1234 2.3 0 2.3424e-1 23874e34 .89 0x2F");
  return yylex() == TK_INT && yyval.ival == 1234 && yylex() == TK_FLOAT && yyval.fval == 2.3 &&
          yylex() == TK_INT && yyval.ival == 0 && yylex() == TK_FLOAT && yyval.fval == 2.3424e-1 &&
	  yylex() == TK_FLOAT && yyval.fval == 23874e34 && yylex() == TK_FLOAT && yyval.fval == 0.89 &&
          yylex() == TK_INT && yyval.ival == 0x2f; 
	
}

int test_number2(){
  setup_test("12e 34 2.e 34. 2F 41231231234");
  return yylex() == TK_INT && yyval.ival == 12 && yylex() == TK_ID && strcmp(yyval.sval, "e") == 0 &&
          yylex() == TK_INT && yyval.ival == 34 && yylex() == TK_FLOAT && yyval.fval == 2.0 && 
          yylex() == TK_ID && strcmp(yyval.sval, "e") == 0 && yylex() == TK_FLOAT && yyval.fval == 34.0 &&
          yylex() == TK_INT && yyval.ival == 2 && yylex() == TK_ID && strcmp(yyval.sval, "F") == 0 && yylex() == ERR_IRANGE; 
	
}

int test_number3(){
  setup_test("0.45e+2 0xA4B5e 0.0 23.e-1");
  return yylex() == TK_FLOAT && yyval.fval == 0.45e2 && yylex() == TK_INT && yyval.ival == 0xa4b5e && 
	  yylex() == TK_FLOAT && yyval.fval == 0.0 && yylex() == TK_FLOAT && yyval.fval == 23.0e-1; 
}

typedef int (*test_case)(void);

test_case tests[] = { test_comment1,
		       test_comment2,
		       test_comment3,
		       test_comment4,
		       test_comment5,
		       test_comment6,
		       test_comment7,
		       test_string1,
		       test_string2,
		       test_string3,
		       test_string4,
		       test_string5,
		       test_string6,
		       test_string7,
		       test_string8,
		       test_string9,
		       test_reserved,
		       test_id1,
		       test_id2,
		       test_op,
                       test_number1,
                       test_number2,
                       test_number3,
		       NULL };

int main(void) {
  int i, pass;
  pass = 1;
  for(i = 0; tests[i] != NULL; i++) {
    if(!(*tests[i])()) {
      pass = 0;
      printf("Failed!!! Test no. %i\n", i+1);
    }
  }
  if(pass) printf("Ok\n");
}

