#/bin/sh

OUTPUT_DIR=tmp
mkdir -p $OUTPUT_DIR
rm -rf $OUTPUT_DIR/$1.*

clang -O0 -S -emit-llvm -o $1.ll $1.c
llvm-as -f -o $1.bc $1.ll
opt -f -o $1.bco -mem2reg -dot-cfg-only -dot-dom-only $1.bc

for i in `ls *.dot`; do
  dot -Tpng $i > $OUTPUT_DIR/$1.$i.png
done

llvm-dis -f -o $OUTPUT_DIR/$1.ssa $1.bco
rm $1.bc $1.ll $1.bco *.dot

