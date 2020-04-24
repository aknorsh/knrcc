#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./knrcc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"

assert 47 "5 +6 *7;"
assert 15 "5*(9-6);"
assert 4 "(3+5) /2;"

assert 10 "-10 +20;"
assert 10 "+10    ;"
assert 12 "-2*-3*+(-6+2)/-2;"

assert 1 "(0<1)      * ((1<1)+1)  * ((1<0)+1) ;"
assert 1 "(0<=1)     * (1<=1)     * ((1<=0)+1);"
assert 1 "((0>1)+1)  * ((1>1)+1)  * (1>0)     ;"
assert 1 "((0>=1)+1) * (1>=1)     * (1>=0)    ;"
assert 1 "((0==1)+1) * (1==1)     * ((1==0)+1);"
assert 1 "(0!=1)     * ((1!=1)+1) * (1!=0)    ;"

assert 3 "0;-1;1;2;3; "
assert 3 "a=1;a+2*3-4;"
assert 3 "n=2;n=n+1;n;"
assert 3 "y=1;z=2;y+z;"
assert 3 "f_=1;b2=2;f_+b2;"

assert 4 "return 4; return 5; return 6;"
assert 4 "one=1;two=2;return one*2+two;"

echo OK