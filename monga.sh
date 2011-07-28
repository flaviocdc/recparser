#!/bin/sh

OUTPUT_DIR=tmp-comp
mkdir -p $OUTPUT_DIR

./cfg-llvm $1 > $OUTPUT_DIR/$1.llvm
llc $OUTPUT_DIR/$1.llvm
as -o $OUTPUT_DIR/$1.o $OUTPUT_DIR/$1.llvm.s

gcc -c runtime.c -o $OUTPUT_DIR/runtime.o
gcc -o $1.prog $OUTPUT_DIR/runtime.o $OUTPUT_DIR/$1.o

rm -rf $OUTPUT_DIR

