#!/bin/bash
clang -o driver ast_pretty_printer.c driver.c lex.yy.c
