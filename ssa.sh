#/bin/sh

clang -O0 -S -emit-llvm -o $1.ll $1.c
llvm-as -f -o $1.bc $1.ll
opt -f -o $1.bco -mem2reg -dot-cfg-only $1.bc
dot -Tps *.dot > $1.ps
llvm-dis -f -o $1.ssa $1.bco
rm $1.bc $1.ll $1.bco *.dot

