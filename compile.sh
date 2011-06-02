#!/bin/bash

OUT=out

if [ -z $CXX ]; then
  CXX=g++
fi

if [ -z $CC ]; then
  CC=gcc
fi

C_FLAGS="-ggdb"
CXX_FLAGS="-ggdb"

$CC $C_FLAGS -c ast_pretty_printer.c -o $OUT/ast_pretty_printer.o
$CC $C_FLAGS -c driver.c -o $OUT/driver.o
$CC $C_FLAGS -c lex.yy.c -o $OUT/lex.yy.o

#$CC -o driver $OUT/*.o

$CXX $CXX_FLAGS -c cfg.cpp -o $OUT/cfg.o
$CXX $CXX_FLAGS -c cfg_printer.cpp -o $OUT/cfg_printer.o
$CXX $CXX_FLAGS -o driver $OUT/*.o
