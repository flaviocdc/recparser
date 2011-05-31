#!/bin/bash

OUT=out
CXX=g++
CC=gcc

$CC -c ast_pretty_printer.c -o $OUT/ast_pretty_printer.o
$CC -c driver.c -o $OUT/driver.o
$CC -c lex.yy.c -o $OUT/lex.yy.o

#$CC -o driver $OUT/*.o

$CXX -c cfg.cpp -o $OUT/cfg.o
$CXX -o driver $OUT/*.o
